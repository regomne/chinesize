//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		GraphFilter系プログラム( Direct3D9 )
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

#ifndef DX_NON_DIRECT3D9

// インクルード ---------------------------------------------------------------
#include "DxGraphicsFilterD3D9.h"
#include "DxGraphicsWin.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxShader_PS_D3D9.h"
#include "../DxSystem.h"
#include "../DxLog.h"
#include "../DxModel.h"
#include "../DxBaseFunc.h"
#include "../DxGraphics.h"
#include "../DxASyncLoad.h"

#ifndef DX_NON_FILTER

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

struct GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9
{
	int						UserSetVertexShaderHandle ;
	int						UserSetPixelShaderHandle ;
	int						UserSetTextureGraphHandle[ 8 ] ;
	int						TextureNum ;
} ;

// データ定義 -----------------------------------------------------------------


static int GraphFilterPassNum_Table[DX_GRAPH_FILTER_NUM] =
{
		1,		// DX_GRAPH_FILTER_MONO
		2,		// DX_GRAPH_FILTER_GAUSS
		1,		// DX_GRAPH_FILTER_DOWN_SCALE
		1,		// DX_GRAPH_FILTER_BRIGHT_CLIP
		1,		// DX_GRAPH_FILTER_BRIGHT_SCALE
		2,		// DX_GRAPH_FILTER_HSB
		1,		// DX_GRAPH_FILTER_INVERT
		1,		// DX_GRAPH_FILTER_LEVEL
		1,		// DX_GRAPH_FILTER_TWO_COLOR
		1,		// DX_GRAPH_FILTER_GRADIENT_MAP
		1,		// DX_GRAPH_FILTER_PREMUL_ALPHA
		1,		// DX_GRAPH_FILTER_INTERP_ALPHA
		1,		// DX_GRAPH_FILTER_YUV_TO_RGB
		1,		// DX_GRAPH_FILTER_Y2UV1_TO_RGB
		1,		// DX_GRAPH_FILTER_YUV_TO_RGB_RRA
		1,		// DX_GRAPH_FILTER_Y2UV1_TO_RGB_RRA
		1,		// DX_GRAPH_FILTER_BICUBIC_SCALE
		1,		// DX_GRAPH_FILTER_LANCZOS3_SCALE
		1,		// DX_GRAPH_FILTER_PMA_BRIGHT_CLIP
		1,		// DX_GRAPH_FILTER_PMA_BRIGHT_SCALE
		1,		// DX_GRAPH_FILTER_PMA_HSB
		1,		// DX_GRAPH_FILTER_PMA_INVERT
		1,		// DX_GRAPH_FILTER_PMA_LEVEL
		1,		// DX_GRAPH_FILTER_PMA_TWO_COLOR
		1,		// DX_GRAPH_FILTER_PMA_GRADIENT_MAP
};
static int GraphFilterUseWorkScreen_Table[DX_GRAPH_FILTER_NUM] =
{
		0,		// DX_GRAPH_FILTER_MONO
		1,		// DX_GRAPH_FILTER_GAUSS
		0,		// DX_GRAPH_FILTER_DOWN_SCALE
		0,		// DX_GRAPH_FILTER_BRIGHT_CLIP
		0,		// DX_GRAPH_FILTER_BRIGHT_SCALE
		0,		// DX_GRAPH_FILTER_HSB
		0,		// DX_GRAPH_FILTER_INVERT
		0,		// DX_GRAPH_FILTER_LEVEL
		0,		// DX_GRAPH_FILTER_TWOCOLOR
		0,		// DX_GRAPH_FILTER_GRADIENT_MAP
		0,		// DX_GRAPH_FILTER_PREMUL_ALPHA
		0,		// DX_GRAPH_FILTER_INTERP_ALPHA
		0,		// DX_GRAPH_FILTER_YUV_TO_RGB
		0,		// DX_GRAPH_FILTER_Y2UV1_TO_RGB
		0,		// DX_GRAPH_FILTER_YUV_TO_RGB_RRA
		0,		// DX_GRAPH_FILTER_Y2UV1_TO_RGB_RRA
		0,		// DX_GRAPH_FILTER_BICUBIC_SCALE
		0,		// DX_GRAPH_FILTER_LANCZOS3_SCALE
		0,		// DX_GRAPH_FILTER_PMA_BRIGHT_CLIP
		0,		// DX_GRAPH_FILTER_PMA_BRIGHT_SCALE
		0,		// DX_GRAPH_FILTER_PMA_HSB
		0,		// DX_GRAPH_FILTER_PMA_INVERT
		0,		// DX_GRAPH_FILTER_PMA_LEVEL
		0,		// DX_GRAPH_FILTER_PMA_TWO_COLOR
		0,		// DX_GRAPH_FILTER_PMA_GRADIENT_MAP
};

static int GraphBlendPassNum_Table[DX_GRAPH_BLEND_NUM] =
{
		1,		// DX_GRAPH_BLEND_NORMAL
		1,		// DX_GRAPH_BLEND_RGBA_SELECT_MIX
		1,		// DX_GRAPH_BLEND_MULTIPLE
		1,		// DX_GRAPH_BLEND_DIFFERENCE
		1,		// DX_GRAPH_BLEND_ADD
		1,		// DX_GRAPH_BLEND_SCREEN
		1,		// DX_GRAPH_BLEND_OVERLAY
		1,		// DX_GRAPH_BLEND_DODGE
		1,		// DX_GRAPH_BLEND_BURN
		1,		// DX_GRAPH_BLEND_DARKEN
		1,		// DX_GRAPH_BLEND_LIGHTEN
		1,		// DX_GRAPH_BLEND_SOFTLIGHT
		1,		// DX_GRAPH_BLEND_HARDLIGHT
		1,		// DX_GRAPH_BLEND_EXCLUSION
		1,		// DX_GRAPH_BLEND_NORMAL_ALPHACH
		1,		// DX_GRAPH_BLEND_ADD_ALPHACH
		1,		// DX_GRAPH_BLEND_MULTIPLE_A_ONLY
		1,		// DX_GRAPH_BLEND_PMA_NORMAL
		1,		// DX_GRAPH_BLEND_PMA_RGBA_SELECT_MIX
		1,		// DX_GRAPH_BLEND_PMA_MULTIPLE
		1,		// DX_GRAPH_BLEND_PMA_DIFFERENCE
		1,		// DX_GRAPH_BLEND_PMA_ADD
		1,		// DX_GRAPH_BLEND_PMA_SCREEN
		1,		// DX_GRAPH_BLEND_PMA_OVERLAY
		1,		// DX_GRAPH_BLEND_PMA_DODGE
		1,		// DX_GRAPH_BLEND_PMA_BURN
		1,		// DX_GRAPH_BLEND_PMA_DARKEN
		1,		// DX_GRAPH_BLEND_PMA_LIGHTEN
		1,		// DX_GRAPH_BLEND_PMA_SOFTLIGHT
		1,		// DX_GRAPH_BLEND_PMA_HARDLIGHT
		1,		// DX_GRAPH_BLEND_PMA_EXCLUSION
		1,		// DX_GRAPH_BLEND_PMA_NORMAL_ALPHACH
		1,		// DX_GRAPH_BLEND_PMA_ADD_ALPHACH
		1,		// DX_GRAPH_BLEND_PMA_MULTIPLE_A_ONLY
};
static int GraphBlendUseWorkScreen_Table[DX_GRAPH_BLEND_NUM] =
{
		0,		// DX_GRAPH_BLEND_NORMAL
		0,		// DX_GRAPH_BLEND_RGBA_SELECT_MIX
		0,		// DX_GRAPH_BLEND_MULTIPLE
		0,		// DX_GRAPH_BLEND_DIFFERENCE
		0,		// DX_GRAPH_BLEND_ADD
		0,		// DX_GRAPH_BLEND_SCREEN
		0,		// DX_GRAPH_BLEND_OVERLAY
		0,		// DX_GRAPH_BLEND_DODGE
		0,		// DX_GRAPH_BLEND_BURN
		0,		// DX_GRAPH_BLEND_DARKEN
		0,		// DX_GRAPH_BLEND_LIGHTEN
		0,		// DX_GRAPH_BLEND_SOFTLIGHT
		0,		// DX_GRAPH_BLEND_HARDLIGHT
		0,		// DX_GRAPH_BLEND_EXCLUSION
		0,		// DX_GRAPH_BLEND_NORMAL_ALPHACH
		0,		// DX_GRAPH_BLEND_ADD_ALPHACH
		0,		// DX_GRAPH_BLEND_MULTIPLE_A_ONLY
		0,		// DX_GRAPH_BLEND_PMA_NORMAL
		0,		// DX_GRAPH_BLEND_PMA_RGBA_SELECT_MIX
		0,		// DX_GRAPH_BLEND_PMA_MULTIPLE
		0,		// DX_GRAPH_BLEND_PMA_DIFFERENCE
		0,		// DX_GRAPH_BLEND_PMA_ADD
		0,		// DX_GRAPH_BLEND_PMA_SCREEN
		0,		// DX_GRAPH_BLEND_PMA_OVERLAY
		0,		// DX_GRAPH_BLEND_PMA_DODGE
		0,		// DX_GRAPH_BLEND_PMA_BURN
		0,		// DX_GRAPH_BLEND_PMA_DARKEN
		0,		// DX_GRAPH_BLEND_PMA_LIGHTEN
		0,		// DX_GRAPH_BLEND_PMA_SOFTLIGHT
		0,		// DX_GRAPH_BLEND_PMA_HARDLIGHT
		0,		// DX_GRAPH_BLEND_PMA_EXCLUSION
		0,		// DX_GRAPH_BLEND_PMA_NORMAL_ALPHACH
		0,		// DX_GRAPH_BLEND_PMA_ADD_ALPHACH
		0,		// DX_GRAPH_BLEND_PMA_MULTIPLE_A_ONLY
};

GRAPHFILTER_SYSTEMIFNO_DIRET3D9 GraphFilterSystemInfoD3D9 ;

// 関数宣言 -------------------------------------------------------------------

static int	Direct3D9_GraphFilter_GetWorkTexture( int IsFloatType, int ChannelBitDepth, DWORD TexSizeW, DWORD TexSizeH, DWORD HandleIndex ) ;		// フィルター作業用のテクスチャハンドルを取得する

static int  Direct3D9_MemLoadShaderCode( const char *ShaderName, bool IsVertexShader ) ;	// 指定名の事前用意シェーダーを読み込む

extern void Direct3D9_GraphFilter_PushBaseDrawParam( GRAPHFILTER_DRAWPARAMTEMP_DIRECT3D9 *DrawParamTemp ) ;
extern void Direct3D9_GraphFilter_PopBaseDrawParam(  GRAPHFILTER_DRAWPARAMTEMP_DIRECT3D9 *DrawParamTemp ) ;
static void Direct3D9_GraphFilter_PushShaderParam(   GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 *ShaderParamTemp, int TextureNum ) ;
static void Direct3D9_GraphFilter_PopShaderParam(    GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 *ShaderParamTemp ) ;

static int  Direct3D9_BasePolygonDraw( GRAPHFILTER_INFO *Info, int ScaleDivNum = 1, VERTEX_TEX8_2D *Texcoord8Vertex = NULL ) ;

// プログラム -----------------------------------------------------------------

// フィルター作業用のテクスチャハンドルを取得する
static int	Direct3D9_GraphFilter_GetWorkTexture( int IsFloatType, int ChannelBitDepth, DWORD TexSizeW, DWORD TexSizeH, DWORD HandleIndex )
{
	IMAGEDATA *BaseImage ;
	DWORD       NPowW ;
	DWORD       NPowH ;
	int         WorkTexSizeW ;
	int         WorkTexSizeH ;
	int         BitDepth = ( IsFloatType && ChannelBitDepth == 32 ) ? 1 : 0 ;

	for( NPowW = 0 ; ( DWORD )( 1 << NPowW ) < TexSizeW ; NPowW ++ ){}
	for( NPowH = 0 ; ( DWORD )( 1 << NPowH ) < TexSizeH ; NPowH ++ ){}
	WorkTexSizeW = ( int )( 1 << NPowW ) ;
	WorkTexSizeH = ( int )( 1 << NPowH ) ;

	if( GRAPHCHK( GraphFilterSystemInfoD3D9.WorkDrawValidGrHandle[ IsFloatType ][ BitDepth ][ NPowW ][ NPowH ][ HandleIndex ], BaseImage ) )
	{
		SETUP_GRAPHHANDLE_GPARAM GParam ;

		Graphics_Image_InitSetupGraphHandleGParam_Normal_DrawValid_NoneZBuffer( &GParam, 32, TRUE ) ;
		GParam.DrawValidFloatTypeGraphCreateFlag = IsFloatType ;
		if( IsFloatType )
		{
			GParam.CreateDrawValidGraphChannelNum = 4 ;
			GParam.CreateImageChannelBitDepth = ChannelBitDepth ;
		}

		GraphFilterSystemInfoD3D9.WorkDrawValidGrHandle[ IsFloatType ][ BitDepth ][ NPowW ][ NPowH ][ HandleIndex ] = Graphics_Image_MakeGraph_UseGParam( &GParam, WorkTexSizeW, WorkTexSizeH, FALSE, FALSE, 0, FALSE ) ;
		if( GraphFilterSystemInfoD3D9.WorkDrawValidGrHandle[ IsFloatType ][ BitDepth ][ NPowW ][ NPowH ][ HandleIndex ] < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x5c\x4f\x6d\x69\x28\x75\xcf\x63\x3b\x75\xef\x53\xfd\x80\xc6\x30\xaf\x30\xb9\x30\xc1\x30\xe3\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x53\x00\x69\x00\x7a\x00\x65\x00\x3a\x00\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x00"/*@ L"作業用描画可能テクスチャの作成に失敗しました Size:%dx%d" @*/, WorkTexSizeW, WorkTexSizeH )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterSystemInfoD3D9.WorkDrawValidGrHandle[ IsFloatType ][ BitDepth ][ NPowW ][ NPowH ][ HandleIndex ], &GraphFilterSystemInfoD3D9.WorkDrawValidGrHandle[ IsFloatType ][ BitDepth ][ NPowW ][ NPowH ][ HandleIndex ] ) ;
	}

	return GraphFilterSystemInfoD3D9.WorkDrawValidGrHandle[ IsFloatType ][ BitDepth ][ NPowW ][ NPowH ][ HandleIndex ] ;
}

// 指定名の事前用意シェーダーを読み込む
static int Direct3D9_MemLoadShaderCode( const char *ShaderName, bool IsVertexShader )
{
	int Addr, Size ;
	BYTE *FileImage ;
	int Ret ;

	if( GD3D9.ShaderCode.Base.BaseShaderInitializeFlag == FALSE ) return -1 ;

	if( DXA_GetFileInfo( &GD3D9.ShaderCode.Base.FilterShaderBinDxa, DX_CHARCODEFORMAT_ASCII, ShaderName, &Addr, &Size ) < 0 ) return -1 ;

	FileImage = ( BYTE * )DXA_GetFileImage( &GD3D9.ShaderCode.Base.FilterShaderBinDxa ) + Addr ;

	Ret = Graphics_Shader_CreateHandle_UseGParam( IsVertexShader, FileImage, Size, FALSE, FALSE ) ;

	return Ret ;
}

extern void Direct3D9_GraphFilter_PushBaseDrawParam( GRAPHFILTER_DRAWPARAMTEMP_DIRECT3D9 *DrawParamTemp )
{
	int i ;

	for( i = 0 ; i < GSYS.HardInfo.RenderTargetNum ; i ++ )
	{
		DrawParamTemp->TargetSurface[ i ] = GD3D9.Device.State.TargetSurface[ i ] ;
	}
	for( i = 1 ; i < GSYS.HardInfo.RenderTargetNum ; i ++ )
	{
		if( GD3D9.Device.State.TargetSurface[ i ] != NULL )
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( NULL, i ) ;
		}
	}

	DrawParamTemp->UseZBufferFlag3D = GSYS.DrawSetting.EnableZBufferFlag3D ;
	DrawParamTemp->BlendMode        = GSYS.DrawSetting.BlendMode ;
	DrawParamTemp->BlendParam       = GSYS.DrawSetting.BlendParam ;
	DrawParamTemp->DrawMode         = GSYS.DrawSetting.DrawMode ;
	DrawParamTemp->AlphaTestMode    = GSYS.DrawSetting.AlphaTestMode ;
	DrawParamTemp->AlphaTestParam   = GSYS.DrawSetting.AlphaTestParam ;
	DrawParamTemp->FogEnable        = GSYS.DrawSetting.FogEnable ;
	DrawParamTemp->LightEnable      = GSYS.Light.ProcessDisable ? FALSE : TRUE ;
	
	NS_SetUseZBuffer3D( FALSE ) ;
	NS_SetDrawBlendMode( DX_BLENDMODE_SRCCOLOR, 255 ) ;
	NS_SetDrawMode( DX_DRAWMODE_BILINEAR ) ;
	NS_SetDrawAlphaTest( DX_CMP_ALWAYS, 0 ) ;
	NS_SetFogEnable( FALSE ) ;
	NS_SetUseLighting( FALSE ) ;
	if( Graphics_Hardware_CheckValid_PF() != 0 )
	{
		Direct3DDevice9_GetDepthStencilSurface( &DrawParamTemp->DepthStencilSurfaece ) ;
		Direct3DDevice9_SetDepthStencilSurface( NULL ) ;
	}
}

extern void Direct3D9_GraphFilter_PopBaseDrawParam( GRAPHFILTER_DRAWPARAMTEMP_DIRECT3D9 *DrawParamTemp )
{
	int i ;

	if( Graphics_Hardware_CheckValid_PF() != 0 )
	{
		Direct3DDevice9_SetDepthStencilSurface( DrawParamTemp->DepthStencilSurfaece ) ;
		Direct3D9_ObjectRelease( DrawParamTemp->DepthStencilSurfaece ) ;
		DrawParamTemp->DepthStencilSurfaece = NULL ;
	}
	for( i = 0 ; i < GSYS.HardInfo.RenderTargetNum ; i ++ )
	{
		if( DrawParamTemp->TargetSurface[ i ] != NULL )
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( DrawParamTemp->TargetSurface[ i ], i ) ;
		}
	}

	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = TRUE ;
	Graphics_D3D9_DeviceState_SetViewportEasy( GSYS.DrawSetting.DrawArea.left, GSYS.DrawSetting.DrawArea.top, GSYS.DrawSetting.DrawArea.right, GSYS.DrawSetting.DrawArea.bottom ) ;
	GD3D9.Device.DrawSetting.CancelSettingEqualCheck = FALSE ;

	NS_SetUseLighting(   DrawParamTemp->LightEnable ) ;
	NS_SetFogEnable(     DrawParamTemp->FogEnable ) ;
	NS_SetDrawAlphaTest( DrawParamTemp->AlphaTestMode, DrawParamTemp->AlphaTestParam ) ;
	NS_SetDrawMode(      DrawParamTemp->DrawMode ) ;
	NS_SetDrawBlendMode( DrawParamTemp->BlendMode, DrawParamTemp->BlendParam ) ;
	NS_SetUseZBuffer3D(  DrawParamTemp->UseZBufferFlag3D ) ;
}

static void Direct3D9_GraphFilter_PushShaderParam( GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 *ShaderParamTemp, int TextureNum )
{
	ShaderParamTemp->UserSetVertexShaderHandle = GSYS.DrawSetting.UserShaderRenderInfo.SetVertexShaderHandle ;
	ShaderParamTemp->UserSetPixelShaderHandle = GSYS.DrawSetting.UserShaderRenderInfo.SetPixelShaderHandle ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetUseState( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTSET_MASK_LIB | DX_SHADERCONSTANTSET_MASK_LIB_SUB ) ;
	if( TextureNum != 0 )
	{
		_MEMCPY( ShaderParamTemp->UserSetTextureGraphHandle, GSYS.DrawSetting.UserShaderRenderInfo.SetTextureGraphHandle, sizeof( int ) * TextureNum ) ;
	}
	ShaderParamTemp->TextureNum = TextureNum ;
}

static void Direct3D9_GraphFilter_PopShaderParam( GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 *ShaderParamTemp )
{
	int i ;

	for( i = 0 ; i < ShaderParamTemp->TextureNum ; i ++ )
	{
		NS_SetUseTextureToShader( i, ShaderParamTemp->UserSetTextureGraphHandle[ i ] == 0 ? -1 : ShaderParamTemp->UserSetTextureGraphHandle[ i ] ) ;
	}
	NS_SetUseVertexShader( ShaderParamTemp->UserSetVertexShaderHandle ) ;
	NS_SetUsePixelShader( ShaderParamTemp->UserSetPixelShaderHandle ) ;
}





extern int	GraphFilter_D3D9_Mono_PF(        GRAPHFILTER_INFO *Info, float Cb, float Cr )
{
	static const char *PsoFileName[ 1 ] =
	{
		"Mono.pso",
	};
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 2 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( GraphFilterShaderHandle.MonoPS < 0 )
	{
		GraphFilterShaderHandle.MonoPS = Direct3D9_MemLoadShaderCode( PsoFileName[ 0 ], false ) ;
		if( GraphFilterShaderHandle.MonoPS < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ 0 ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.MonoPS, &GraphFilterShaderHandle.MonoPS ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	ParamF4[ 0 ].x = 0.29900f ;
	ParamF4[ 0 ].y = 0.58700f ;
	ParamF4[ 0 ].z = 0.11400f ;
	ParamF4[ 0 ].w = 0.0f ;
	ParamF4[ 1 ].x = 1.40200f * Cr ;
	ParamF4[ 1 ].y = 0.34414f * Cb - 0.71414f * Cr ;
	ParamF4[ 1 ].z = 1.77200f * Cb ;
	ParamF4[ 1 ].w = 0.0f ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 2, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.MonoPS ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 2 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Gauss_PF( GRAPHFILTER_INFO *Info, int PixelWidth, float /*Param*/, float *Table )
{
	static const char *PsoFileName[] =
	{
		"Gauss_08Pixel_PS.pso",
		"Gauss_16Pixel_PS.pso",
		"Gauss_32Pixel_PS.pso"
	} ;
	IMAGEDATA               *SrcImage = NULL ;
	SHADOWMAPDATA           *SrcShadowMap = NULL ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	int                      UseShader = 0 ;
	FLOAT4                   ParamF4[ 21 ] ;
	int                      TexWidth ;
	int                      TexHeight ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	if( GRAPHCHK(     Info->SrcGrHandle, SrcImage    ) &&
		SHADOWMAPCHK( Info->SrcGrHandle, SrcShadowMap ) )
		return -1 ;

	// PixelWidth によってシェーダーを変更
	switch( PixelWidth )
	{
	case 8  : UseShader = 0 ; break ;
	case 16 : UseShader = 1 ; break ;
	case 32 : UseShader = 2 ; break ;
	}

/*	if( Param < 0.0000001f )
	{
		IMAGEDATA *DestImage = NULL ;
		SHADOWMAPDATA *DestShadowMap = NULL ;

		if( GRAPHCHK(     Info->DestGrHandle, DestImage    ) &&
			SHADOWMAPCHK( Info->DestGrHandle, DestShadowMap ) )
			return -1 ;

		if( DestShadowMap )
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( DestShadowMap->PF->D3D9.Surface ) ;
			NS_DrawGraph( 0, 0, Info->SrcGrHandle, FALSE ) ;
		}
		else
		{
			Graphics_D3D9_DeviceState_SetRenderTarget( Graphics_Image_CheckMultiSampleDrawValid( Info->DestGrHandle ) ?
										DestImage->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface :
										DestImage->Hard.Draw[ 0 ].Tex->PF->D3D9.Surface[ 0 ] ) ;
			NS_DrawGraph( 0, 0, Info->SrcGrHandle, FALSE ) ;
			if( Graphics_Image_CheckMultiSampleDrawValid( Info->DestGrHandle ) )
			{
				Graphics_D3D9_EndScene() ;
				Graphics_D3D9_UpdateDrawTexture( DestImage->Hard.Draw[ 0 ].Tex, 0 ) ;
			}
		}
	}
	else */
	{
		// 使用するシェーダーのセットアップ
		if( GraphFilterShaderHandle.Gauss_PS[ UseShader ] < 0 )
		{
			GraphFilterShaderHandle.Gauss_PS[ UseShader ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
			if( GraphFilterShaderHandle.Gauss_PS[ UseShader ] < 0 )
			{
				char PathUTF16LE[ 128 ] ;

				ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
				return -1 ;
			}
			NS_SetDeleteHandleFlag( GraphFilterShaderHandle.Gauss_PS[ UseShader ], &GraphFilterShaderHandle.Gauss_PS[ UseShader ] ) ;
		}

		NS_GetGraphTextureSize( Info->SrcGrHandle, &TexWidth, &TexHeight ) ;

		Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

		ParamF4[  0 ].x = ( float )TexWidth ;
		ParamF4[  0 ].y = ( float )TexHeight ;
		ParamF4[  1 ].x = Table[ 0 ] ;
		ParamF4[  1 ].y = Table[ 1 ] ;
		ParamF4[  1 ].z = Table[ 2 ] ;
		ParamF4[  1 ].w = Table[ 3 ] ;
		ParamF4[  2 ].x = Table[ 4 ] ;
		ParamF4[  2 ].y = Table[ 5 ] ;
		ParamF4[  2 ].z = Table[ 6 ] ;
		ParamF4[  2 ].w = Table[ 7 ] ;
		ParamF4[  3 ].x = Table[ 8 ] ;
		ParamF4[  3 ].y = Table[ 9 ] ;
		ParamF4[  3 ].z = Table[ 10 ] ;
		ParamF4[  3 ].w = Table[ 11 ] ;
		ParamF4[  4 ].x = Table[ 12 ] ;
		ParamF4[  4 ].y = Table[ 13 ] ;
		ParamF4[  4 ].z = Table[ 14 ] ;
		ParamF4[  4 ].w = Table[ 15 ] ;

		// パスによって処理を分岐
		switch( Info->Pass )
		{
		case 0 :
			ParamF4[  5 ].x =  0.0f / ParamF4[  0 ].x ; ParamF4[  5 ].y =  0.0f ; ParamF4[  5 ].z =  0.0f ; ParamF4[  5 ].w =  0.0f ;
			ParamF4[  6 ].x =  1.5f / ParamF4[  0 ].x ; ParamF4[  6 ].y =  0.0f ; ParamF4[  6 ].z =  0.0f ; ParamF4[  6 ].w =  0.0f ;
			ParamF4[  7 ].x =  3.5f / ParamF4[  0 ].x ; ParamF4[  7 ].y =  0.0f ; ParamF4[  7 ].z =  0.0f ; ParamF4[  7 ].w =  0.0f ;
			ParamF4[  8 ].x =  5.5f / ParamF4[  0 ].x ; ParamF4[  8 ].y =  0.0f ; ParamF4[  8 ].z =  0.0f ; ParamF4[  8 ].w =  0.0f ;
			ParamF4[  9 ].x =  7.5f / ParamF4[  0 ].x ; ParamF4[  9 ].y =  0.0f ; ParamF4[  9 ].z =  0.0f ; ParamF4[  9 ].w =  0.0f ;
			ParamF4[ 10 ].x =  9.5f / ParamF4[  0 ].x ; ParamF4[ 10 ].y =  0.0f ; ParamF4[ 10 ].z =  0.0f ; ParamF4[ 10 ].w =  0.0f ;
			ParamF4[ 11 ].x = 11.5f / ParamF4[  0 ].x ; ParamF4[ 11 ].y =  0.0f ; ParamF4[ 11 ].z =  0.0f ; ParamF4[ 11 ].w =  0.0f ;
			ParamF4[ 12 ].x = 13.5f / ParamF4[  0 ].x ; ParamF4[ 12 ].y =  0.0f ; ParamF4[ 12 ].z =  0.0f ; ParamF4[ 12 ].w =  0.0f ;
			ParamF4[ 13 ].x = 15.5f / ParamF4[  0 ].x ; ParamF4[ 13 ].y =  0.0f ; ParamF4[ 13 ].z =  0.0f ; ParamF4[ 13 ].w =  0.0f ;
			ParamF4[ 14 ].x = 17.5f / ParamF4[  0 ].x ; ParamF4[ 14 ].y =  0.0f ; ParamF4[ 14 ].z =  0.0f ; ParamF4[ 14 ].w =  0.0f ;
			ParamF4[ 15 ].x = 19.5f / ParamF4[  0 ].x ; ParamF4[ 15 ].y =  0.0f ; ParamF4[ 15 ].z =  0.0f ; ParamF4[ 15 ].w =  0.0f ;
			ParamF4[ 16 ].x = 21.5f / ParamF4[  0 ].x ; ParamF4[ 16 ].y =  0.0f ; ParamF4[ 16 ].z =  0.0f ; ParamF4[ 16 ].w =  0.0f ;
			ParamF4[ 17 ].x = 23.5f / ParamF4[  0 ].x ; ParamF4[ 17 ].y =  0.0f ; ParamF4[ 17 ].z =  0.0f ; ParamF4[ 17 ].w =  0.0f ;
			ParamF4[ 18 ].x = 25.5f / ParamF4[  0 ].x ; ParamF4[ 18 ].y =  0.0f ; ParamF4[ 18 ].z =  0.0f ; ParamF4[ 18 ].w =  0.0f ;
			ParamF4[ 19 ].x = 27.5f / ParamF4[  0 ].x ; ParamF4[ 19 ].y =  0.0f ; ParamF4[ 19 ].z =  0.0f ; ParamF4[ 19 ].w =  0.0f ;
			ParamF4[ 20 ].x = 29.5f / ParamF4[  0 ].x ; ParamF4[ 20 ].y =  0.0f ; ParamF4[ 20 ].z =  0.0f ; ParamF4[ 20 ].w =  0.0f ;
			break ;

		case 1 :
			ParamF4[  5 ].y =  0.0f / ParamF4[  0 ].y ; ParamF4[  5 ].x =  0.0f ; ParamF4[  5 ].z =  0.0f ; ParamF4[  5 ].w =  0.0f ;
			ParamF4[  6 ].y =  1.5f / ParamF4[  0 ].y ; ParamF4[  6 ].x =  0.0f ; ParamF4[  6 ].z =  0.0f ; ParamF4[  6 ].w =  0.0f ;
			ParamF4[  7 ].y =  3.5f / ParamF4[  0 ].y ; ParamF4[  7 ].x =  0.0f ; ParamF4[  7 ].z =  0.0f ; ParamF4[  7 ].w =  0.0f ;
			ParamF4[  8 ].y =  5.5f / ParamF4[  0 ].y ; ParamF4[  8 ].x =  0.0f ; ParamF4[  8 ].z =  0.0f ; ParamF4[  8 ].w =  0.0f ;
			ParamF4[  9 ].y =  7.5f / ParamF4[  0 ].y ; ParamF4[  9 ].x =  0.0f ; ParamF4[  9 ].z =  0.0f ; ParamF4[  9 ].w =  0.0f ;
			ParamF4[ 10 ].y =  9.5f / ParamF4[  0 ].y ; ParamF4[ 10 ].x =  0.0f ; ParamF4[ 10 ].z =  0.0f ; ParamF4[ 10 ].w =  0.0f ;
			ParamF4[ 11 ].y = 11.5f / ParamF4[  0 ].y ; ParamF4[ 11 ].x =  0.0f ; ParamF4[ 11 ].z =  0.0f ; ParamF4[ 11 ].w =  0.0f ;
			ParamF4[ 12 ].y = 13.5f / ParamF4[  0 ].y ; ParamF4[ 12 ].x =  0.0f ; ParamF4[ 12 ].z =  0.0f ; ParamF4[ 12 ].w =  0.0f ;
			ParamF4[ 13 ].y = 15.5f / ParamF4[  0 ].y ; ParamF4[ 13 ].x =  0.0f ; ParamF4[ 13 ].z =  0.0f ; ParamF4[ 13 ].w =  0.0f ;
			ParamF4[ 14 ].y = 17.5f / ParamF4[  0 ].y ; ParamF4[ 14 ].x =  0.0f ; ParamF4[ 14 ].z =  0.0f ; ParamF4[ 14 ].w =  0.0f ;
			ParamF4[ 15 ].y = 19.5f / ParamF4[  0 ].y ; ParamF4[ 15 ].x =  0.0f ; ParamF4[ 15 ].z =  0.0f ; ParamF4[ 15 ].w =  0.0f ;
			ParamF4[ 16 ].y = 21.5f / ParamF4[  0 ].y ; ParamF4[ 16 ].x =  0.0f ; ParamF4[ 16 ].z =  0.0f ; ParamF4[ 16 ].w =  0.0f ;
			ParamF4[ 17 ].y = 23.5f / ParamF4[  0 ].y ; ParamF4[ 17 ].x =  0.0f ; ParamF4[ 17 ].z =  0.0f ; ParamF4[ 17 ].w =  0.0f ;
			ParamF4[ 18 ].y = 25.5f / ParamF4[  0 ].y ; ParamF4[ 18 ].x =  0.0f ; ParamF4[ 18 ].z =  0.0f ; ParamF4[ 18 ].w =  0.0f ;
			ParamF4[ 19 ].y = 27.5f / ParamF4[  0 ].y ; ParamF4[ 19 ].x =  0.0f ; ParamF4[ 19 ].z =  0.0f ; ParamF4[ 19 ].w =  0.0f ;
			ParamF4[ 20 ].y = 29.5f / ParamF4[  0 ].y ; ParamF4[ 20 ].x =  0.0f ; ParamF4[ 20 ].z =  0.0f ; ParamF4[ 20 ].w =  0.0f ;
			break ;
		}

		Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 21, FALSE ) ;

		NS_SetUsePixelShader( GraphFilterShaderHandle.Gauss_PS[ UseShader ] ) ;

		Direct3D9_BasePolygonDraw( Info ) ;

		Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

		Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 21 ) ;

		if( Info->Pass == 0 )
		{
			VERTEX2D Vert[ 4 ] ;
			float Width ;
			float Height ;
			int TextureWidth ;
			int TextureHeight ;

			GetGraphTextureSize( Info->DestGrHandle, &TextureWidth, &TextureHeight ) ;
			Width  = ( float )( Info->SrcX2 - Info->SrcX1 ) ;
			Height = ( float )( Info->SrcY2 - Info->SrcY1 ) ;
			Vert[ 0 ].pos.x = Info->DestX			- 0.5f ;	Vert[ 0 ].pos.y = Info->DestY + Height	- 0.5f ;	Vert[ 0 ].pos.z = 0.0f ;	Vert[ 0 ].rhw = 1.0f ;
			Vert[ 1 ].pos.x = Info->DestX + Width	- 0.5f ;	Vert[ 1 ].pos.y = Info->DestY + Height	- 0.5f ;	Vert[ 1 ].pos.z = 0.0f ;	Vert[ 1 ].rhw = 1.0f ;
			Vert[ 2 ].pos.x = Info->DestX			- 0.5f ;	Vert[ 2 ].pos.y = TextureHeight			- 0.5f ;	Vert[ 2 ].pos.z = 0.0f ;	Vert[ 2 ].rhw = 1.0f ;
			Vert[ 3 ].pos.x = Info->DestX + Width	- 0.5f ;	Vert[ 3 ].pos.y = TextureHeight			- 0.5f ;	Vert[ 3 ].pos.z = 0.0f ;	Vert[ 3 ].rhw = 1.0f ;
			Vert[ 0 ].u = Info->DestX             / ( float )TextureWidth ;	Vert[ 0 ].v = ( float )( Info->DestY + Height - 1 ) / TextureHeight ;
			Vert[ 1 ].u = ( Info->DestX + Width ) / ( float )TextureWidth ;	Vert[ 1 ].v = Vert[ 0 ].v ;
			Vert[ 2 ].u = Vert[ 0 ].u ;										Vert[ 2 ].v = Vert[ 0 ].v ;
			Vert[ 3 ].u = Vert[ 1 ].u ;										Vert[ 3 ].v = Vert[ 0 ].v ;
			Vert[ 0 ].dif = GetColorU8( 255,255,255,255 ) ;
			Vert[ 1 ].dif = Vert[ 0 ].dif ;
			Vert[ 2 ].dif = Vert[ 0 ].dif ;
			Vert[ 3 ].dif = Vert[ 0 ].dif ;
			NS_DrawPrimitive2D( Vert, 4, DX_PRIMTYPE_TRIANGLESTRIP, Info->DestGrHandle, FALSE ) ;
		}
	}

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Down_Scale_PF(  GRAPHFILTER_INFO *Info, int DivNum )
{
	static const char *PsoFileName[ 3 ] =
	{
		"DownScaleX2.pso",
		"DownScaleX4.pso",
		"DownScaleX8.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	IMAGEDATA *SrcImage ;
	int UseShader = 0 ;
	int UseConstNum = 0 ;
	FLOAT4 ParamF4[ 16 ] ;
	float TextureW ;
	float TextureH ;
	int i ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	if( GRAPHCHK( Info->SrcGrHandle, SrcImage ) )
		return -1 ;

	// 使用するシェーダーのセットアップ
	switch( DivNum )
	{
	case 2 : UseShader = 0 ; break ;
	case 4 : UseShader = 1 ; break ;
	case 8 : UseShader = 2 ; break ;
	}
	if( GraphFilterShaderHandle.DownScalePS[ UseShader ] < 0 )
	{
		GraphFilterShaderHandle.DownScalePS[ UseShader ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.DownScalePS[ UseShader ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.DownScalePS[ UseShader ], &GraphFilterShaderHandle.DownScalePS[ UseShader ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	switch( DivNum )
	{
	case 2 :
		ParamF4[  0 ].x = 0.0f ; ParamF4[  0 ].y = 0.0f ; ParamF4[  0 ].z = 0.0f ; ParamF4[  0 ].w = 0.0f ;
		UseConstNum = 1 ;
		break ;

	case 4 :
		ParamF4[  0 ].x = -1.0f ; ParamF4[  0 ].y = -1.0f ; ParamF4[  0 ].z = 0.0f ; ParamF4[  0 ].w = 0.0f ;
		ParamF4[  1 ].x =  1.0f ; ParamF4[  1 ].y = -1.0f ; ParamF4[  1 ].z = 0.0f ; ParamF4[  1 ].w = 0.0f ;
		ParamF4[  2 ].x = -1.0f ; ParamF4[  2 ].y =  1.0f ; ParamF4[  2 ].z = 0.0f ; ParamF4[  2 ].w = 0.0f ;
		ParamF4[  3 ].x =  1.0f ; ParamF4[  3 ].y =  1.0f ; ParamF4[  3 ].z = 0.0f ; ParamF4[  3 ].w = 0.0f ;
		UseConstNum = 4 ;
		break ;

	case 8 :
		ParamF4[  0 ].x = -3.0f ; ParamF4[  0 ].y = -3.0f ; ParamF4[  0 ].z = 0.0f ; ParamF4[  0 ].w = 0.0f ;
		ParamF4[  1 ].x = -1.0f ; ParamF4[  1 ].y = -3.0f ; ParamF4[  1 ].z = 0.0f ; ParamF4[  1 ].w = 0.0f ;
		ParamF4[  2 ].x =  1.0f ; ParamF4[  2 ].y = -3.0f ; ParamF4[  2 ].z = 0.0f ; ParamF4[  2 ].w = 0.0f ;
		ParamF4[  3 ].x =  3.0f ; ParamF4[  3 ].y = -3.0f ; ParamF4[  3 ].z = 0.0f ; ParamF4[  3 ].w = 0.0f ;
		ParamF4[  4 ].x = -3.0f ; ParamF4[  4 ].y = -1.0f ; ParamF4[  4 ].z = 0.0f ; ParamF4[  4 ].w = 0.0f ;
		ParamF4[  5 ].x = -1.0f ; ParamF4[  5 ].y = -1.0f ; ParamF4[  5 ].z = 0.0f ; ParamF4[  5 ].w = 0.0f ;
		ParamF4[  6 ].x =  1.0f ; ParamF4[  6 ].y = -1.0f ; ParamF4[  6 ].z = 0.0f ; ParamF4[  6 ].w = 0.0f ;
		ParamF4[  7 ].x =  3.0f ; ParamF4[  7 ].y = -1.0f ; ParamF4[  7 ].z = 0.0f ; ParamF4[  7 ].w = 0.0f ;
		ParamF4[  8 ].x = -3.0f ; ParamF4[  8 ].y =  1.0f ; ParamF4[  8 ].z = 0.0f ; ParamF4[  8 ].w = 0.0f ;
		ParamF4[  9 ].x = -1.0f ; ParamF4[  9 ].y =  1.0f ; ParamF4[  9 ].z = 0.0f ; ParamF4[  9 ].w = 0.0f ;
		ParamF4[ 10 ].x =  1.0f ; ParamF4[ 10 ].y =  1.0f ; ParamF4[ 10 ].z = 0.0f ; ParamF4[ 10 ].w = 0.0f ;
		ParamF4[ 11 ].x =  3.0f ; ParamF4[ 11 ].y =  1.0f ; ParamF4[ 11 ].z = 0.0f ; ParamF4[ 11 ].w = 0.0f ;
		ParamF4[ 12 ].x = -3.0f ; ParamF4[ 12 ].y =  3.0f ; ParamF4[ 12 ].z = 0.0f ; ParamF4[ 12 ].w = 0.0f ;
		ParamF4[ 13 ].x = -1.0f ; ParamF4[ 13 ].y =  3.0f ; ParamF4[ 13 ].z = 0.0f ; ParamF4[ 13 ].w = 0.0f ;
		ParamF4[ 14 ].x =  1.0f ; ParamF4[ 14 ].y =  3.0f ; ParamF4[ 14 ].z = 0.0f ; ParamF4[ 14 ].w = 0.0f ;
		ParamF4[ 15 ].x =  3.0f ; ParamF4[ 15 ].y =  3.0f ; ParamF4[ 15 ].z = 0.0f ; ParamF4[ 15 ].w = 0.0f ;
		UseConstNum = 16 ;
		break ;
	}
	TextureW = ( float )SrcImage->Orig->Hard.Tex[ 0 ].TexWidth ;
	TextureH = ( float )SrcImage->Orig->Hard.Tex[ 0 ].TexHeight ;
	for( i = 0 ; i < UseConstNum ; i ++ )
	{
		ParamF4[ i ].x /= TextureW ;
		ParamF4[ i ].y /= TextureH ;
	}
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, UseConstNum, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.DownScalePS[ UseShader ] ) ;

	Direct3D9_BasePolygonDraw( Info, DivNum ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, UseConstNum ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Bright_Clip_PF( GRAPHFILTER_INFO *Info, int CmpType, float CmpParam, int ClipFillFlag, COLOR_F *ClipFillColor, int IsPMA )
{
	static const char *PsoFileName[ 2 ][ 2 ][ 2 ] =
	{
		"BrightClipLess.pso",
		"BrightClipLess_PMA.pso",
		"BrightClipFillLess.pso",
		"BrightClipFillLess_PMA.pso",
		"BrightClipGreater.pso",
		"BrightClipGreater_PMA.pso",
		"BrightClipFillGreater.pso",
		"BrightClipFillGreater_PMA.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	int UseShader = 0 ;
	int UseFill ;
	FLOAT4 ParamF4[ 3 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseFill = ClipFillFlag == TRUE ? 1 : 0 ;
	switch( CmpType )
	{
	case DX_CMP_GREATER :
		UseShader = 1 ;
		break ;

	case DX_CMP_LESS :
		UseShader = 0 ;
		break ;
	}
	if( GraphFilterShaderHandle.BrightClipPS[ UseShader ][ UseFill ][ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.BrightClipPS[ UseShader ][ UseFill ][ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ][ UseFill ][ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.BrightClipPS[ UseShader ][ UseFill ][ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ][ UseFill ][ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.BrightClipPS[ UseShader ][ UseFill ][ IsPMA ], &GraphFilterShaderHandle.BrightClipPS[ UseShader ][ UseFill ][ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	ParamF4[  0 ].x = CmpParam ;
	ParamF4[  0 ].y = CmpParam ;
	ParamF4[  0 ].z = CmpParam ;
	ParamF4[  0 ].w = CmpParam ;
	ParamF4[  1 ].x = 0.299f ;
	ParamF4[  1 ].y = 0.587f ;
	ParamF4[  1 ].z = 0.114f ;
	ParamF4[  1 ].w = 0.0f ;
	if( ClipFillColor != NULL )
	{
		ParamF4[  2 ].x = ClipFillColor->r ;
		ParamF4[  2 ].y = ClipFillColor->g ;
		ParamF4[  2 ].z = ClipFillColor->b ;
		ParamF4[  2 ].w = ClipFillColor->a ;
	}
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 3, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.BrightClipPS[ UseShader ][ UseFill ][ IsPMA ] ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 3 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Bright_Scale_PF( GRAPHFILTER_INFO *Info, int BrightMin, int BrightMax, int IsPMA )
{
	static const char *PsoFileName[ 2 ] =
	{
		"BrightnessScale.pso",
		"BrightnessScale_PMA.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 1 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( GraphFilterShaderHandle.BrightScalePS[ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.BrightScalePS[ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.BrightScalePS[ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.BrightScalePS[ IsPMA ], &GraphFilterShaderHandle.BrightScalePS[ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	ParamF4[ 0 ].x = BrightMin / 255.0f ;
	ParamF4[ 0 ].y = 255.0f / ( float )( BrightMax - BrightMin ) ;
	ParamF4[ 0 ].z = 0.0f ;
	ParamF4[ 0 ].w = 0.0f ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 1, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.BrightScalePS[ IsPMA ] ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 1 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_HSB_PF(         GRAPHFILTER_INFO *Info, int HueType, float Hue, float Saturation, float Bright, int IsPMA )
{
	static const char *PsoFileName[ 3 ][ 2 ] =
	{
		"Hsb_RGB_TO_HSB.pso",
		"Hsb_RGB_TO_HSB_PMA.pso",
		"Hsb_HSB_TO_RGB.pso",
		"Hsb_HSB_TO_RGB_PMA.pso",
		"Hsb_HSB_HLOCK.pso",
		"Hsb_HSB_HLOCK_PMA.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	int UseShader ;
	FLOAT4 ParamF4[ 3 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( HueType == 1 )
	{
		UseShader = 2 ;
	}
	else
	{
		UseShader = Info->Pass ;
	}
	if( GraphFilterShaderHandle.HsbPS[ UseShader ][ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.HsbPS[ UseShader ][ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ][ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.HsbPS[ UseShader ][ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ][ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.HsbPS[ UseShader ][ IsPMA ], &GraphFilterShaderHandle.HsbPS[ UseShader ][ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	ParamF4[ 0 ].x = Hue ;
	ParamF4[ 0 ].y = Saturation > 0.0f ? Saturation * 5.0f : Saturation ;
	ParamF4[ 0 ].z = Bright ;
	ParamF4[ 0 ].w = 0.0f ;
	ParamF4[ 1 ].x = 0.5f ;
	ParamF4[ 1 ].y = 0.5f ;
	ParamF4[ 1 ].z = 0.5f ;
	ParamF4[ 1 ].w = 0.5f ;
	ParamF4[ 2 ].x = 1.0f ;
	ParamF4[ 2 ].y = 1.0f ;
	ParamF4[ 2 ].z = 1.0f ;
	ParamF4[ 2 ].w = 1.0f ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 3, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.HsbPS[ UseShader ][ IsPMA ] ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 3 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Invert_PF(      GRAPHFILTER_INFO *Info, int IsPMA )
{
	static const char *PsoFileName[ 2 ] =
	{
		"Invert.pso",
		"Invert_PMA.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( GraphFilterShaderHandle.InvertPS[ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.InvertPS[ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.InvertPS[ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.InvertPS[ IsPMA ], &GraphFilterShaderHandle.InvertPS[ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.InvertPS[ IsPMA ] ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Level_PF(       GRAPHFILTER_INFO *Info, float Min, float Max, float /*Gamma*/, float AfterMin, float AfterMax, int IsPMA )
{
	static const char *PsoFileName[ 2 ] =
	{
		"Level.pso",
		"Level_PMA.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 1 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	// NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( GraphFilterShaderHandle.LevelPS[ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.LevelPS[ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.LevelPS[ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.LevelPS[ IsPMA ], &GraphFilterShaderHandle.LevelPS[ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	ParamF4[ 0 ].x = Min ;
	ParamF4[ 0 ].y = 1.0f / ( Max - Min ) ;
	ParamF4[ 0 ].z = AfterMin ;
	ParamF4[ 0 ].w = AfterMax ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 1, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.LevelPS[ IsPMA ] ) ;

	NS_SetUseTextureToShader( 1, GraphFilterShaderHandle.GammaTex ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 1 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_TwoColor_PF(    GRAPHFILTER_INFO *Info, float Threshold, COLOR_F *LowColor, COLOR_F *HighColor, int IsPMA )
{
	Threshold /= 255.0f ;

	static const char *PsoFileName[ 2 ] =
	{
		"TwoColor.pso",
		"TwoColor_PMA.pso",
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 3 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( GraphFilterShaderHandle.TwoColorPS[ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.TwoColorPS[ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.TwoColorPS[ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.TwoColorPS[ IsPMA ], &GraphFilterShaderHandle.TwoColorPS[ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	ParamF4[ 0 ].x = 0.29900f ;
	ParamF4[ 0 ].y = 0.58700f ;
	ParamF4[ 0 ].z = 0.11400f ;
	ParamF4[ 0 ].w = Threshold ;
	ParamF4[ 1 ].x = LowColor->r ;
	ParamF4[ 1 ].y = LowColor->g ;
	ParamF4[ 1 ].z = LowColor->b ;
	ParamF4[ 1 ].w = LowColor->a ;
	ParamF4[ 2 ].x = HighColor->r ;
	ParamF4[ 2 ].y = HighColor->g ;
	ParamF4[ 2 ].z = HighColor->b ;
	ParamF4[ 2 ].w = HighColor->a ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 3, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.TwoColorPS[ IsPMA ] ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 3 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_GradientMap_PF( GRAPHFILTER_INFO *Info, int MapGrHandle, int Reverse, int IsPMA )
{
	static const char *PsoFileName[ 2 ][ 2 ] =
	{
		"GradientMap.pso",
		"GradientMap_PMA.pso",
		"GradientMapRev.pso",
		"GradientMapRev_PMA.pso",
	} ;
	int UseShader ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 1 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = Reverse ? 1 : 0 ;
	if( GraphFilterShaderHandle.GradientMapPS[ UseShader ][ IsPMA ] < 0 )
	{
		GraphFilterShaderHandle.GradientMapPS[ UseShader ][ IsPMA ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ][ IsPMA ], false ) ;
		if( GraphFilterShaderHandle.GradientMapPS[ UseShader ][ IsPMA ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ][ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.GradientMapPS[ UseShader ][ IsPMA ], &GraphFilterShaderHandle.GradientMapPS[ UseShader ][ IsPMA ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 2 ) ;

	ParamF4[ 0 ].x = 0.29900f ;
	ParamF4[ 0 ].y = 0.58700f ;
	ParamF4[ 0 ].z = 0.11400f ;
	ParamF4[ 0 ].w = 0.0f ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 1, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.GradientMapPS[ UseShader ][ IsPMA ] ) ;

	NS_SetUseTextureToShader( 1, MapGrHandle ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 1 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_PremulAlpha_PF( GRAPHFILTER_INFO *Info )
{
	static const char *PsoFileName[ 1 ] =
	{
		"PremultipliedAlpha.pso",
	} ;
	int UseShader ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = 0 ;
	if( GraphFilterShaderHandle.PreMulAlphaPS < 0 )
	{
		GraphFilterShaderHandle.PreMulAlphaPS = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.PreMulAlphaPS < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.PreMulAlphaPS, &GraphFilterShaderHandle.PreMulAlphaPS ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.PreMulAlphaPS ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_InterpAlpha_PF( GRAPHFILTER_INFO *Info )
{
	static const char *PsoFileName[ 1 ] =
	{
		"InterpolatedAlpha.pso",
	} ;
	int UseShader ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = 0 ;
	if( GraphFilterShaderHandle.InterpAlphaPS < 0 )
	{
		GraphFilterShaderHandle.InterpAlphaPS = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.InterpAlphaPS < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.InterpAlphaPS, &GraphFilterShaderHandle.InterpAlphaPS ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.InterpAlphaPS ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_YUVtoRGB_PF(     GRAPHFILTER_INFO *Info, int UVGrHandle )
{
	static const char *PsoFileName[ 4 ] =
	{
		"Y2uv2ToRgb.pso",
		"Y2uv1ToRgb.pso",

		"Y2uv2ToRgb_RRA.pso",
		"Y2uv1ToRgb_RRA.pso",
	} ;
	int UseShader ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 1 ] ;
	int RRAFlag = ( Info->FilterOrBlendType == DX_GRAPH_FILTER_YUV_TO_RGB_RRA || Info->FilterOrBlendType == DX_GRAPH_FILTER_Y2UV1_TO_RGB_RRA ) ? TRUE : FALSE ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = ( UVGrHandle < 0 ? 0 : 1 ) + ( RRAFlag ? 2 : 0 ) ;
	if( GraphFilterShaderHandle.YUVtoRGBPS[ UseShader ] < 0 )
	{
		GraphFilterShaderHandle.YUVtoRGBPS[ UseShader ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.YUVtoRGBPS[ UseShader ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.YUVtoRGBPS[ UseShader ], &GraphFilterShaderHandle.YUVtoRGBPS[ UseShader ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 2 ) ;

	if( UVGrHandle >= 0 || RRAFlag )
	{
		int YImgWidth, YImgHeight ;
		int YTexWidth, YTexHeight ;
		float YU, YV ;

		NS_GetGraphSize(        Info->SrcGrHandle, &YImgWidth,  &YImgHeight ) ;
		NS_GetGraphTextureSize( Info->SrcGrHandle, &YTexWidth,  &YTexHeight ) ;
		YU  = ( float )YImgWidth  / YTexWidth ;
		YV  = ( float )YImgHeight / YTexHeight ;

		if( UVGrHandle >= 0 )
		{
			int UVImgWidth, UVImgHeight ;
			int UVTexWidth, UVTexHeight ;
			float UVU, UVV ;

			NS_GetGraphSize(        UVGrHandle, &UVImgWidth, &UVImgHeight ) ;
			NS_GetGraphTextureSize( UVGrHandle, &UVTexWidth, &UVTexHeight ) ;
			UVU = ( float )UVImgWidth  / UVTexWidth ;
			UVV = ( float )UVImgHeight / UVTexHeight ;

			ParamF4[ 0 ].x = UVU / YU ;
			ParamF4[ 0 ].y = UVV / YV ;
			ParamF4[ 0 ].z = YU  / 2.0f ;
			ParamF4[ 0 ].w = UVU / 2.0f ;
		}
		else
		{
			ParamF4[ 0 ].x = 0.0f ;
			ParamF4[ 0 ].y = 0.0f ;
			ParamF4[ 0 ].z = YU  / 2.0f ;
			ParamF4[ 0 ].w = 0.0f ;
		}

		Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 1, FALSE ) ;
	}

	NS_SetUsePixelShader( GraphFilterShaderHandle.YUVtoRGBPS[ UseShader ] ) ;

	NS_SetUseTextureToShader( 1, UVGrHandle ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	if( UVGrHandle >= 0 )
	{
		Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 1 ) ;
	}

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_BicubicScale_PF( GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY )
{
	static const char *PsoFileName[ 1 ] =
	{
		"Bicubic.pso",
	} ;
	int UseShader ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 2 ] ;
	VERTEX_TEX8_2D VertexTex8[ 4 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = 0 ;
	if( GraphFilterShaderHandle.BicubicPS < 0 )
	{
		GraphFilterShaderHandle.BicubicPS = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.BicubicPS < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.BicubicPS, &GraphFilterShaderHandle.BicubicPS ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	int TexW, TexH ;
	NS_GetGraphTextureSize( Info->SrcGrHandle, &TexW, &TexH ) ;

	ParamF4[ 0 ].x = ( FLOAT )TexW ;
	ParamF4[ 0 ].y = ( FLOAT )TexH ;
	ParamF4[ 0 ].z = 0.0f ;
	ParamF4[ 0 ].w = 0.0f ;

	ParamF4[ 1 ].x = 2.0f / ( FLOAT )TexW ;
	ParamF4[ 1 ].y = 0.0f ;
	ParamF4[ 1 ].z = 0.0f ;
	ParamF4[ 1 ].w = 0.0f ;

	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 2, FALSE ) ;

	float du = 1.0f / TexW ;
	float dv = 1.0f / TexH ;

	VertexTex8[ 0 ].pos.x = -0.5f ;
	VertexTex8[ 0 ].pos.y = -0.5f ;
	VertexTex8[ 0 ].pos.z = 0.0f ;
	VertexTex8[ 0 ].rhw = 1.0f ;
	VertexTex8[ 0 ].u0 = 0.0f - 1.5f * du ;	VertexTex8[ 0 ].v0 = 0.0f - 1.5f * dv ;
	VertexTex8[ 0 ].u1 = 0.0f - 1.5f * du ;	VertexTex8[ 0 ].v1 = 0.0f - 0.5f * dv ;
	VertexTex8[ 0 ].u2 = 0.0f - 1.5f * du ;	VertexTex8[ 0 ].v2 = 0.0f + 0.5f * dv ;
	VertexTex8[ 0 ].u3 = 0.0f - 1.5f * du ;	VertexTex8[ 0 ].v3 = 0.0f + 1.5f * dv ;
	VertexTex8[ 0 ].u4 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v4 = 0.0f - 1.5f * dv ;
	VertexTex8[ 0 ].u5 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v5 = 0.0f - 0.5f * dv ;
	VertexTex8[ 0 ].u6 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v6 = 0.0f + 0.5f * dv ;
	VertexTex8[ 0 ].u7 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v7 = 0.0f + 1.5f * dv ;

	VertexTex8[ 1 ].pos.x = ( FLOAT )DestSizeX - 0.5f ;
	VertexTex8[ 1 ].pos.y = -0.5f ;
	VertexTex8[ 1 ].pos.z = 0.0f ;
	VertexTex8[ 1 ].rhw = 1.0f ;
	VertexTex8[ 1 ].u0 = 1.0f - 1.5f * du ;	VertexTex8[ 1 ].v0 = 0.0f - 1.5f * dv ;
	VertexTex8[ 1 ].u1 = 1.0f - 1.5f * du ;	VertexTex8[ 1 ].v1 = 0.0f - 0.5f * dv ;
	VertexTex8[ 1 ].u2 = 1.0f - 1.5f * du ;	VertexTex8[ 1 ].v2 = 0.0f + 0.5f * dv ;
	VertexTex8[ 1 ].u3 = 1.0f - 1.5f * du ;	VertexTex8[ 1 ].v3 = 0.0f + 1.5f * dv ;
	VertexTex8[ 1 ].u4 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v4 = 0.0f - 1.5f * dv ;
	VertexTex8[ 1 ].u5 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v5 = 0.0f - 0.5f * dv ;
	VertexTex8[ 1 ].u6 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v6 = 0.0f + 0.5f * dv ;
	VertexTex8[ 1 ].u7 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v7 = 0.0f + 1.5f * dv ;

	VertexTex8[ 2 ].pos.x = -0.5f ;
	VertexTex8[ 2 ].pos.y = ( FLOAT )DestSizeY - 0.5f ;
	VertexTex8[ 2 ].pos.z = 0.0f ;
	VertexTex8[ 2 ].rhw = 1.0f ;
	VertexTex8[ 2 ].u0 = 0.0f - 1.5f * du ;	VertexTex8[ 2 ].v0 = 1.0f - 1.5f * dv ;
	VertexTex8[ 2 ].u1 = 0.0f - 1.5f * du ;	VertexTex8[ 2 ].v1 = 1.0f - 0.5f * dv ;
	VertexTex8[ 2 ].u2 = 0.0f - 1.5f * du ;	VertexTex8[ 2 ].v2 = 1.0f + 0.5f * dv ;
	VertexTex8[ 2 ].u3 = 0.0f - 1.5f * du ;	VertexTex8[ 2 ].v3 = 1.0f + 1.5f * dv ;
	VertexTex8[ 2 ].u4 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v4 = 1.0f - 1.5f * dv ;
	VertexTex8[ 2 ].u5 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v5 = 1.0f - 0.5f * dv ;
	VertexTex8[ 2 ].u6 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v6 = 1.0f + 0.5f * dv ;
	VertexTex8[ 2 ].u7 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v7 = 1.0f + 1.5f * dv ;

	VertexTex8[ 3 ].pos.x = ( FLOAT )DestSizeX - 0.5f ;
	VertexTex8[ 3 ].pos.y = ( FLOAT )DestSizeY - 0.5f ;
	VertexTex8[ 3 ].pos.z = 0.0f ;
	VertexTex8[ 3 ].rhw = 1.0f ;
	VertexTex8[ 3 ].u0 = 1.0f - 1.5f * du ;	VertexTex8[ 3 ].v0 = 1.0f - 1.5f * dv ;
	VertexTex8[ 3 ].u1 = 1.0f - 1.5f * du ;	VertexTex8[ 3 ].v1 = 1.0f - 0.5f * dv ;
	VertexTex8[ 3 ].u2 = 1.0f - 1.5f * du ;	VertexTex8[ 3 ].v2 = 1.0f + 0.5f * dv ;
	VertexTex8[ 3 ].u3 = 1.0f - 1.5f * du ;	VertexTex8[ 3 ].v3 = 1.0f + 1.5f * dv ;
	VertexTex8[ 3 ].u4 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v4 = 1.0f - 1.5f * dv ;
	VertexTex8[ 3 ].u5 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v5 = 1.0f - 0.5f * dv ;
	VertexTex8[ 3 ].u6 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v6 = 1.0f + 0.5f * dv ;
	VertexTex8[ 3 ].u7 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v7 = 1.0f + 1.5f * dv ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.BicubicPS ) ;

	Direct3D9_BasePolygonDraw( Info, 1, VertexTex8 ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 2 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphFilter_D3D9_Lanczos3Scale_PF( GRAPHFILTER_INFO *Info, int DestSizeX, int DestSizeY )
{
	static const char *PsoFileName[ 1 ] =
	{
		"Lanczos3.pso",
	} ;
	int UseShader ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 2 ] ;
	VERTEX_TEX8_2D VertexTex8[ 4 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = 0 ;
	if( GraphFilterShaderHandle.Lanczos3PS < 0 )
	{
		GraphFilterShaderHandle.Lanczos3PS = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.Lanczos3PS < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.Lanczos3PS, &GraphFilterShaderHandle.Lanczos3PS ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 1 ) ;

	int TexW, TexH ;
	NS_GetGraphTextureSize( Info->SrcGrHandle, &TexW, &TexH ) ;

	ParamF4[ 0 ].x = ( FLOAT )TexW ;
	ParamF4[ 0 ].y = ( FLOAT )TexH ;
	ParamF4[ 0 ].z = 0.0f ;
	ParamF4[ 0 ].w = 0.0f ;

	ParamF4[ 1 ].x = 1.0f / ( FLOAT )TexW ;
	ParamF4[ 1 ].y = 0.0f ;
	ParamF4[ 1 ].z = 0.0f ;
	ParamF4[ 1 ].w = 0.0f ;

	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 2, FALSE ) ;

	float du = 1.0f / TexW ;
	float dv = 1.0f / TexH ;

	VertexTex8[ 0 ].pos.x = -0.5f ;
	VertexTex8[ 0 ].pos.y = -0.5f ;
	VertexTex8[ 0 ].pos.z = 0.0f ;
	VertexTex8[ 0 ].rhw = 1.0f ;
	VertexTex8[ 0 ].u0 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v0 = 0.0f - 2.5f * dv ;
	VertexTex8[ 0 ].u1 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v1 = 0.0f - 1.5f * dv ;
	VertexTex8[ 0 ].u2 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v2 = 0.0f - 0.5f * dv ;
	VertexTex8[ 0 ].u3 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v3 = 0.0f + 0.5f * dv ;
	VertexTex8[ 0 ].u4 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v4 = 0.0f + 1.5f * dv ;
	VertexTex8[ 0 ].u5 = 0.0f - 0.5f * du ;	VertexTex8[ 0 ].v5 = 0.0f + 2.5f * dv ;

	VertexTex8[ 1 ].pos.x = ( FLOAT )DestSizeX - 0.5f ;
	VertexTex8[ 1 ].pos.y = -0.5f ;
	VertexTex8[ 1 ].pos.z = 0.0f ;
	VertexTex8[ 1 ].rhw = 1.0f ;
	VertexTex8[ 1 ].u0 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v0 = 0.0f - 2.5f * dv ;
	VertexTex8[ 1 ].u1 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v1 = 0.0f - 1.5f * dv ;
	VertexTex8[ 1 ].u2 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v2 = 0.0f - 0.5f * dv ;
	VertexTex8[ 1 ].u3 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v3 = 0.0f + 0.5f * dv ;
	VertexTex8[ 1 ].u4 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v4 = 0.0f + 1.5f * dv ;
	VertexTex8[ 1 ].u5 = 1.0f - 0.5f * du ;	VertexTex8[ 1 ].v5 = 0.0f + 2.5f * dv ;

	VertexTex8[ 2 ].pos.x = -0.5f ;
	VertexTex8[ 2 ].pos.y = ( FLOAT )DestSizeY - 0.5f ;
	VertexTex8[ 2 ].pos.z = 0.0f ;
	VertexTex8[ 2 ].rhw = 1.0f ;
	VertexTex8[ 2 ].u0 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v0 = 1.0f - 2.5f * dv ;
	VertexTex8[ 2 ].u1 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v1 = 1.0f - 1.5f * dv ;
	VertexTex8[ 2 ].u2 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v2 = 1.0f - 0.5f * dv ;
	VertexTex8[ 2 ].u3 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v3 = 1.0f + 0.5f * dv ;
	VertexTex8[ 2 ].u4 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v4 = 1.0f + 1.5f * dv ;
	VertexTex8[ 2 ].u5 = 0.0f - 0.5f * du ;	VertexTex8[ 2 ].v5 = 1.0f + 2.5f * dv ;

	VertexTex8[ 3 ].pos.x = ( FLOAT )DestSizeX - 0.5f ;
	VertexTex8[ 3 ].pos.y = ( FLOAT )DestSizeY - 0.5f ;
	VertexTex8[ 3 ].pos.z = 0.0f ;
	VertexTex8[ 3 ].rhw = 1.0f ;
	VertexTex8[ 3 ].u0 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v0 = 1.0f - 2.5f * dv ;
	VertexTex8[ 3 ].u1 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v1 = 1.0f - 1.5f * dv ;
	VertexTex8[ 3 ].u2 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v2 = 1.0f - 0.5f * dv ;
	VertexTex8[ 3 ].u3 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v3 = 1.0f + 0.5f * dv ;
	VertexTex8[ 3 ].u4 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v4 = 1.0f + 1.5f * dv ;
	VertexTex8[ 3 ].u5 = 1.0f - 0.5f * du ;	VertexTex8[ 3 ].v5 = 1.0f + 2.5f * dv ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.Lanczos3PS ) ;

	Direct3D9_BasePolygonDraw( Info, 1, VertexTex8 ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 2 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphBlend_D3D9_Basic_PF(           GRAPHFILTER_INFO *Info, int /*IsPMA*/ )
{
	static const char *PsoFileName[ DX_GRAPH_BLEND_NUM ] =
	{
		"BasBF_Normal.pso",					// DX_GRAPH_BLEND_NORMAL
		NULL,								// DX_GRAPH_BLEND_RGBA_SELECT_MIX
		"BasBF_Multiple.pso",				// DX_GRAPH_BLEND_MULTIPLE
		"BasBF_Difference.pso",				// DX_GRAPH_BLEND_DIFFERENCE
		"BasBF_Add.pso",					// DX_GRAPH_BLEND_ADD
		"BasBF_Screen.pso",					// DX_GRAPH_BLEND_SCREEN
		"BasBF_Overlay.pso",				// DX_GRAPH_BLEND_OVERLAY
		"BasBF_Dodge.pso",					// DX_GRAPH_BLEND_DODGE
		"BasBF_Burn.pso",					// DX_GRAPH_BLEND_BURN
		"BasBF_Darken.pso",					// DX_GRAPH_BLEND_DARKEN
		"BasBF_Lighten.pso",				// DX_GRAPH_BLEND_LIGHTEN
		"BasBF_SoftLight.pso",				// DX_GRAPH_BLEND_SOFTLIGHT
		"BasBF_HardLight.pso",				// DX_GRAPH_BLEND_HARDLIGHT
		"BasBF_Exclusion.pso",				// DX_GRAPH_BLEND_EXCLUSION
		"BasBF_Normal_AlphaCh.pso",			// DX_GRAPH_BLEND_NORMAL_ALPHACH
		"BasBF_Add_AlphaCh.pso",			// DX_GRAPH_BLEND_ADD_ALPHACH
		"BasBF_Multiple_AOnly.pso",			// DX_GRAPH_BLEND_MULTIPLE_A_ONLY
		"BasBF_Normal_PMA.pso",				// DX_GRAPH_BLEND_PMA_NORMAL
		NULL,								// DX_GRAPH_BLEND_PMA_RGBA_SELECT_MIX
		"BasBF_Multiple_PMA.pso",			// DX_GRAPH_BLEND_PMA_MULTIPLE
		"BasBF_Difference_PMA.pso",			// DX_GRAPH_BLEND_PMA_DIFFERENCE
		"BasBF_Add_PMA.pso",				// DX_GRAPH_BLEND_PMA_ADD
		"BasBF_Screen_PMA.pso",				// DX_GRAPH_BLEND_PMA_SCREEN
		"BasBF_Overlay_PMA.pso",			// DX_GRAPH_BLEND_PMA_OVERLAY
		"BasBF_Dodge_PMA.pso",				// DX_GRAPH_BLEND_PMA_DODGE
		"BasBF_Burn_PMA.pso",				// DX_GRAPH_BLEND_PMA_BURN
		"BasBF_Darken_PMA.pso",				// DX_GRAPH_BLEND_PMA_DARKEN
		"BasBF_Lighten_PMA.pso",			// DX_GRAPH_BLEND_PMA_LIGHTEN
		"BasBF_SoftLight_PMA.pso",			// DX_GRAPH_BLEND_PMA_SOFTLIGHT
		"BasBF_HardLight_PMA.pso",			// DX_GRAPH_BLEND_PMA_HARDLIGHT
		"BasBF_Exclusion_PMA.pso",			// DX_GRAPH_BLEND_PMA_EXCLUSION
		"BasBF_Normal_AlphaCh_PMA.pso",		// DX_GRAPH_BLEND_PMA_NORMAL_ALPHACH
		"BasBF_Add_AlphaCh_PMA.pso",		// DX_GRAPH_BLEND_PMA_ADD_ALPHACH
		"BasBF_Multiple_AOnly_PMA.pso",		// DX_GRAPH_BLEND_PMA_MULTIPLE_A_ONLY
	} ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	int UseShader ;
	FLOAT4 ParamF4[ 2 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

//	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	UseShader = Info->FilterOrBlendType ;
	if( GraphFilterShaderHandle.BasicBlendPS[ UseShader ] < 0 )
	{
		GraphFilterShaderHandle.BasicBlendPS[ UseShader ] = Direct3D9_MemLoadShaderCode( PsoFileName[ UseShader ], false ) ;
		if( GraphFilterShaderHandle.BasicBlendPS[ UseShader ] < 0 )
		{
			char PathUTF16LE[ 128 ] ;

			ConvString( PsoFileName[ UseShader ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
			return -1 ;
		}
		NS_SetDeleteHandleFlag( GraphFilterShaderHandle.BasicBlendPS[ UseShader ], &GraphFilterShaderHandle.BasicBlendPS[ UseShader ] ) ;
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 2 ) ;
	
	ParamF4[ 0 ].x = Info->BlendRatio ;
	ParamF4[ 0 ].y = Info->BlendRatio ;
	ParamF4[ 0 ].z = Info->BlendRatio ;
	ParamF4[ 0 ].w = Info->BlendRatio ;
	ParamF4[ 1 ].x = 1.0f ;
	ParamF4[ 1 ].y = 1.0f ;
	ParamF4[ 1 ].z = 1.0f ;
	ParamF4[ 1 ].w = 1.0f ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 2, FALSE ) ;

	NS_SetUsePixelShader( GraphFilterShaderHandle.BasicBlendPS[ UseShader ] ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 2 ) ;

	// 正常終了
	return 0 ;
}

extern int	GraphBlend_D3D9_RGBA_Select_Mix_PF( GRAPHFILTER_INFO *Info, int SelectR, int SelectG, int SelectB, int SelectA, int IsPMA )
{
	static const char *PsoFileName[ 2 ] =
	{
		"RGBAMix_Base.pso",
		"RGBAMix_Base_PMA.pso",
	} ;
	static const char *CharTable[ 4 ] =
	{
		"R", "G", "B", "A"
	};
	int *PixelShaderHandle ;
	GRAPHFILTER_SHADERPARAMTEMP_DIRECT3D9 ShaderParamTemp ;
	FLOAT4 ParamF4[ 2 ] ;
	char FileName[ 64 ] ;
	int SrcBlendReverse = FALSE ;
	int HandleTemp ;
	int SrcOnlyFlag = FALSE ;
	char PathUTF16LE[ 128 ] ;

	if( Graphics_Hardware_CheckValid_PF() == 0 )
	{
		return -1 ;
	}

	NS_SetDrawMode( DX_DRAWMODE_NEAREST ) ;

	// 使用するシェーダーのセットアップ
	if( SelectR >= DX_RGBA_SELECT_SRC_R && SelectR <= DX_RGBA_SELECT_SRC_A &&
		SelectG >= DX_RGBA_SELECT_SRC_R && SelectG <= DX_RGBA_SELECT_SRC_A &&
		SelectB >= DX_RGBA_SELECT_SRC_R && SelectB <= DX_RGBA_SELECT_SRC_A &&
		SelectA >= DX_RGBA_SELECT_SRC_R && SelectA <= DX_RGBA_SELECT_SRC_A )
	{
		SrcOnlyFlag = TRUE ;
	}
	else
	if( SelectR >= DX_RGBA_SELECT_BLEND_R && SelectR <= DX_RGBA_SELECT_BLEND_A &&
		SelectG >= DX_RGBA_SELECT_BLEND_R && SelectG <= DX_RGBA_SELECT_BLEND_A &&
		SelectB >= DX_RGBA_SELECT_BLEND_R && SelectB <= DX_RGBA_SELECT_BLEND_A &&
		SelectA >= DX_RGBA_SELECT_BLEND_R && SelectA <= DX_RGBA_SELECT_BLEND_A )
	{
		SelectR -= DX_RGBA_SELECT_BLEND_R ;
		SelectG -= DX_RGBA_SELECT_BLEND_R ;
		SelectB -= DX_RGBA_SELECT_BLEND_R ;
		SelectA -= DX_RGBA_SELECT_BLEND_R ;

		SrcOnlyFlag = TRUE ;
		SrcBlendReverse = TRUE ;

		HandleTemp = Info->BlendGrHandle ;
		Info->BlendGrHandle = Info->SrcGrHandle ;
		Info->SrcGrHandle = HandleTemp ;
	}

	if( SrcOnlyFlag )
	{
		PixelShaderHandle = &GraphFilterShaderHandle.RgbaMixS[ SelectR ][ SelectG ][ SelectB ][ SelectA ][ IsPMA ] ;
		if( *PixelShaderHandle < 0 )
		{
			if( GD3D9.ShaderCode.Base.RGBAMixS_ShaderPackImage != NULL )
			{
				*PixelShaderHandle = NS_LoadPixelShaderFromMem( GD3D9.ShaderCode.Base.RGBAMixS_ShaderAddress[ SelectR ][ SelectG ][ SelectB ][ SelectA ][ IsPMA ], GD3D9.ShaderCode.Base.RGBAMixS_ShaderSize[ SelectR ][ SelectG ][ SelectB ][ SelectA ][ IsPMA ] ) ;
				if( *PixelShaderHandle < 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x47\x00\x42\x00\x41\x00\x4d\x00\x69\x00\x78\x00\x53\x00\x20\x00\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"RGBAMixS フィルター用シェーダーの作成に失敗しました" @*/ )) ;
					return -1 ;
				}
				NS_SetDeleteHandleFlag( *PixelShaderHandle, PixelShaderHandle ) ;
			}
		}
	}
	else
	if( SelectR == SelectG && SelectR == SelectB && SelectR != DX_RGBA_SELECT_BLEND_A && SelectR != DX_RGBA_SELECT_SRC_A )
	{
		if( SelectR >= DX_RGBA_SELECT_BLEND_R && SelectR <= DX_RGBA_SELECT_BLEND_A )
		{
			SelectR -= DX_RGBA_SELECT_BLEND_R ;
			SelectG -= DX_RGBA_SELECT_BLEND_R ;
			SelectB -= DX_RGBA_SELECT_BLEND_R ;
			SelectA += DX_RGBA_SELECT_BLEND_R ;

			SrcBlendReverse = TRUE ;

			HandleTemp = Info->BlendGrHandle ;
			Info->BlendGrHandle = Info->SrcGrHandle ;
			Info->SrcGrHandle = HandleTemp ;
		}

		PixelShaderHandle = &GraphFilterShaderHandle.RgbaMixSRRRB[ SelectR ][ SelectA - DX_RGBA_SELECT_BLEND_R ][ IsPMA ] ;
		if( *PixelShaderHandle < 0 )
		{
			_SNPRINTF( FileName, sizeof( FileName ), "RGBAMix_S%sS%sS%sB%s%s.pso", CharTable[ SelectR ], CharTable[ SelectG ], CharTable[ SelectB ], CharTable[ SelectA - DX_RGBA_SELECT_BLEND_R ], IsPMA ? "_PMA" : "" ) ;
			*PixelShaderHandle = Direct3D9_MemLoadShaderCode( FileName, false ) ;
			if( *PixelShaderHandle < 0 )
			{
				ConvString( FileName, -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x47\x00\x42\x00\x41\x00\x4d\x00\x69\x00\x78\x00\x53\x00\x20\x00\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"RGBAMixS フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
				return -1 ;
			}
			NS_SetDeleteHandleFlag( *PixelShaderHandle, PixelShaderHandle ) ;
		}
	}
	else
	if( ( SelectR == DX_RGBA_SELECT_SRC_R   && SelectG == DX_RGBA_SELECT_SRC_G   && SelectB == DX_RGBA_SELECT_SRC_B   ) ||
		( SelectR == DX_RGBA_SELECT_BLEND_R && SelectG == DX_RGBA_SELECT_BLEND_G && SelectB == DX_RGBA_SELECT_BLEND_B ) )
	{
		if( SelectR == DX_RGBA_SELECT_BLEND_R )
		{
			SelectR -= DX_RGBA_SELECT_BLEND_R ;
			SelectG -= DX_RGBA_SELECT_BLEND_R ;
			SelectB -= DX_RGBA_SELECT_BLEND_R ;
			SelectA += DX_RGBA_SELECT_BLEND_R ;

			SrcBlendReverse = TRUE ;

			HandleTemp = Info->BlendGrHandle ;
			Info->BlendGrHandle = Info->SrcGrHandle ;
			Info->SrcGrHandle = HandleTemp ;
		}

		PixelShaderHandle = &GraphFilterShaderHandle.RgbaMixSRGBB[ SelectA - DX_RGBA_SELECT_BLEND_R ][ IsPMA ] ;
		if( *PixelShaderHandle < 0 )
		{
			_SNPRINTF( FileName, sizeof( FileName ), "RGBAMix_S%sS%sS%sB%s%s.pso", CharTable[ SelectR ], CharTable[ SelectG ], CharTable[ SelectB ], CharTable[ SelectA - DX_RGBA_SELECT_BLEND_R ], IsPMA ? "_PMA" : "" ) ;
			*PixelShaderHandle = Direct3D9_MemLoadShaderCode( FileName, false ) ;
			if( *PixelShaderHandle < 0 )
			{
				ConvString( FileName, -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\x52\x00\x47\x00\x42\x00\x41\x00\x4d\x00\x69\x00\x78\x00\x53\x00\x20\x00\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"RGBAMixS フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
				return -1 ;
			}
			NS_SetDeleteHandleFlag( *PixelShaderHandle, PixelShaderHandle ) ;
		}
	}
	else
	{
		PixelShaderHandle = &GraphFilterShaderHandle.RgbaMixBasePS[ IsPMA ] ;
		if( *PixelShaderHandle < 0 )
		{
			*PixelShaderHandle = Direct3D9_MemLoadShaderCode( PsoFileName[ IsPMA ], false ) ;
			if( *PixelShaderHandle < 0 )
			{
				ConvString( PsoFileName[ IsPMA ], -1, DX_CHARCODEFORMAT_ASCII, ( char * )PathUTF16LE, sizeof( PathUTF16LE ), DX_CHARCODEFORMAT_UTF16LE ) ;
				DXST_LOGFILEFMT_ADDUTF16LE(( "\xd5\x30\xa3\x30\xeb\x30\xbf\x30\xfc\x30\x28\x75\xb7\x30\xa7\x30\xfc\x30\xc0\x30\xfc\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x25\x00\x73\x00\x00"/*@ L"フィルター用シェーダーの作成に失敗しました %s" @*/, PathUTF16LE )) ;
				return -1 ;
			}
			NS_SetDeleteHandleFlag( *PixelShaderHandle, PixelShaderHandle ) ;
		}
	}

	Direct3D9_GraphFilter_PushShaderParam( &ShaderParamTemp, 2 ) ;

	ParamF4[ 0 ].x = Info->BlendRatio ;
	ParamF4[ 0 ].y = Info->BlendRatio ;
	ParamF4[ 0 ].z = Info->BlendRatio ;
	ParamF4[ 0 ].w = Info->BlendRatio ;
	ParamF4[ 1 ].x = ( float )SelectR + 0.5f ;
	ParamF4[ 1 ].y = ( float )SelectG + 0.5f ;
	ParamF4[ 1 ].z = ( float )SelectB + 0.5f ;
	ParamF4[ 1 ].w = ( float )SelectA + 0.5f ;
	Graphics_D3D9_ShaderConstant_InfoSet_SetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, ParamF4, 2, FALSE ) ;

	NS_SetUsePixelShader( *PixelShaderHandle ) ;

	Direct3D9_BasePolygonDraw( Info ) ;

	Direct3D9_GraphFilter_PopShaderParam( &ShaderParamTemp ) ;

	if( SrcBlendReverse )
	{
		HandleTemp = Info->BlendGrHandle ;
		Info->BlendGrHandle = Info->SrcGrHandle ;
		Info->SrcGrHandle = HandleTemp ;
	}

	Graphics_D3D9_ShaderConstant_InfoSet_ResetParam( &GD3D9.Device.Shader.ShaderConstantInfo, DX_SHADERCONSTANTTYPE_PS_FLOAT, DX_SHADERCONSTANTSET_LIB_SUB, 0, 2 ) ;

	// 正常終了
	return 0 ;
}

















static int Direct3D9_BasePolygonDraw( GRAPHFILTER_INFO *Info, int ScaleDivNum, VERTEX_TEX8_2D *Texcoord8Vertex )
{
	VERTEX2DSHADER Vert[ 4 ] ;
	IMAGEDATA *SrcImage = NULL ;
	IMAGEDATA *BlendImage = NULL ;
	IMAGEDATA *DestImage = NULL ;
	SHADOWMAPDATA *SrcShadowMap = NULL ;
	SHADOWMAPDATA *DestShadowMap = NULL ;
	float SrcX1F, SrcX2F, SrcY1F, SrcY2F ;
	float SrcX1TexF, SrcX2TexF, SrcY1TexF, SrcY2TexF ;
	int SrcWidth = 0, SrcHeight = 0 ;
	float DestX1F, DestX2F, DestY1F, DestY2F ;
	float BlendX1TexF, BlendX2TexF, BlendY1TexF, BlendY2TexF ;
	int SrcTexWidth, SrcTexHeight ;

	if( GRAPHCHK(     Info->SrcGrHandle, SrcImage    ) &&
		SHADOWMAPCHK( Info->SrcGrHandle, SrcShadowMap ) )
		return -1 ;

	if( GRAPHCHK(     Info->DestGrHandle, DestImage    ) &&
		SHADOWMAPCHK( Info->DestGrHandle, DestShadowMap ) )
		return -1 ;

	if( DestShadowMap != NULL )
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( DestShadowMap->PF->D3D9.Surface ) ;
	}
	else
	{
		Graphics_D3D9_DeviceState_SetRenderTarget( Graphics_Image_CheckMultiSampleDrawValid( Info->DestGrHandle ) ?
									DestImage->Hard.Draw[ 0 ].Tex->PF->D3D9.RenderTargetSurface :
									DestImage->Hard.Draw[ 0 ].Tex->PF->D3D9.Surface[ 0 ] ) ;
	}

	if( Texcoord8Vertex == NULL )
	{
		SrcX1F = ( float )Info->SrcX1 ;
		SrcX2F = ( float )Info->SrcX2 ;
		SrcY1F = ( float )Info->SrcY1 ;
		SrcY2F = ( float )Info->SrcY2 ;
		SrcWidth  = Info->SrcX2 - Info->SrcX1 ;
		SrcHeight = Info->SrcY2 - Info->SrcY1 ;

		if( ScaleDivNum != 1 )
		{
			DestX1F = ( float )  Info->DestX                             - 0.5f ;
			DestX2F = ( float )( Info->DestX + SrcWidth  / ScaleDivNum ) - 0.5f ;
			DestY1F = ( float )  Info->DestY                             - 0.5f ;
			DestY2F = ( float )( Info->DestY + SrcHeight / ScaleDivNum ) - 0.5f ;
		}
		else
		{
			DestX1F = ( float )  Info->DestX               - 0.5f ;
			DestX2F = ( float )( Info->DestX + SrcWidth  ) - 0.5f ;
			DestY1F = ( float )  Info->DestY               - 0.5f ;
			DestY2F = ( float )( Info->DestY + SrcHeight ) - 0.5f ;
		}

		NS_GetGraphTextureSize( Info->SrcGrHandle, &SrcTexWidth, &SrcTexHeight ) ;
		SrcX1TexF = SrcX1F / SrcTexWidth ;
		SrcX2TexF = SrcX2F / SrcTexWidth ;
		SrcY1TexF = SrcY1F / SrcTexHeight ;
		SrcY2TexF = SrcY2F / SrcTexHeight ;

		Vert[ 0 ].pos = VGet( DestX1F, DestY1F, 0.0f ) ;
		Vert[ 1 ].pos = VGet( DestX2F, DestY1F, 0.0f ) ;
		Vert[ 2 ].pos = VGet( DestX1F, DestY2F, 0.0f ) ;
		Vert[ 3 ].pos = VGet( DestX2F, DestY2F, 0.0f ) ;
		Vert[ 0 ].u = SrcX1TexF ; Vert[ 0 ].v = SrcY1TexF ;
		Vert[ 1 ].u = SrcX2TexF ; Vert[ 1 ].v = SrcY1TexF ;
		Vert[ 2 ].u = SrcX1TexF ; Vert[ 2 ].v = SrcY2TexF ;
		Vert[ 3 ].u = SrcX2TexF ; Vert[ 3 ].v = SrcY2TexF ;
		Vert[ 0 ].rhw = 1.0f ;
		Vert[ 1 ].rhw = 1.0f ;
		Vert[ 2 ].rhw = 1.0f ;
		Vert[ 3 ].rhw = 1.0f ;
	}

	if( Info->IsBlend )
	{
		if( GRAPHCHK( Info->BlendGrHandle, BlendImage ) )
			return -1 ;

		if( Texcoord8Vertex == NULL )
		{
			if( Info->BlendPosEnable )
			{
				BlendX1TexF = ( float )  Info->BlendX               / BlendImage->Orig->Hard.Tex[ 0 ].TexWidth ;
				BlendX2TexF = ( float )( Info->BlendX + SrcWidth )  / BlendImage->Orig->Hard.Tex[ 0 ].TexWidth ;
				BlendY1TexF = ( float )  Info->BlendY               / BlendImage->Orig->Hard.Tex[ 0 ].TexHeight ;
				BlendY2TexF = ( float )( Info->BlendY + SrcHeight ) / BlendImage->Orig->Hard.Tex[ 0 ].TexHeight ;
			}
			else
			{
				BlendX1TexF = BlendImage->Hard.Draw[ 0 ].Vertex[ 0 ].u ;
				BlendX2TexF = BlendImage->Hard.Draw[ 0 ].Vertex[ 1 ].u ;
				BlendY1TexF = BlendImage->Hard.Draw[ 0 ].Vertex[ 0 ].v ;
				BlendY2TexF = BlendImage->Hard.Draw[ 0 ].Vertex[ 2 ].v ;
			}
			Vert[ 0 ].su = BlendX1TexF ; Vert[ 0 ].sv = BlendY1TexF ;
			Vert[ 1 ].su = BlendX2TexF ; Vert[ 1 ].sv = BlendY1TexF ;
			Vert[ 2 ].su = BlendX1TexF ; Vert[ 2 ].sv = BlendY2TexF ;
			Vert[ 3 ].su = BlendX2TexF ; Vert[ 3 ].sv = BlendY2TexF ;
		}

		NS_SetUseTextureToShader( 1, Info->BlendGrHandle ) ;
	}

	NS_SetUseTextureToShader( 0, Info->SrcGrHandle ) ;

	// 描画待機している描画物を描画
	DRAWSTOCKINFO

	// 描画の準備
	Graphics_D3D9_BeginScene() ;
	Graphics_D3D9_DrawPreparationToShader( 0, FALSE, TRUE, TRUE ) ;

	// 描画
	if( Texcoord8Vertex != NULL )
	{
		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_TEX8_2D ) ;
		Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )DX_PRIMTYPE_TRIANGLESTRIP, GETPRIMNUM( DX_PRIMTYPE_TRIANGLESTRIP, 4 ), Texcoord8Vertex, sizeof( VERTEX_TEX8_2D ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}
	else
	{
		Graphics_D3D9_DeviceState_SetFVF( VERTEXFVF_SHADER_2D ) ;
		Direct3DDevice9_DrawPrimitiveUP( ( D_D3DPRIMITIVETYPE )DX_PRIMTYPE_TRIANGLESTRIP, GETPRIMNUM( DX_PRIMTYPE_TRIANGLESTRIP, 4 ), Vert, sizeof( VERTEX2DSHADER ) ) ;
		GSYS.PerformanceInfo.NowFrameDrawCallCount ++ ;
	}

	if( Graphics_Image_CheckMultiSampleDrawValid( Info->DestGrHandle ) )
	{
		Graphics_D3D9_EndScene() ;
		Graphics_D3D9_UpdateDrawTexture( DestImage->Hard.Draw[ 0 ].Tex, 0 ) ;
	}

	// 終了
	return 0 ;
}




















extern int GraphFilter_D3D9_RectBltBase_Timing0_PF( GRAPHFILTER_INFO *Info, GRAPHFILTER_PARAM *Param )
{
	Info->PassNum = -1 ;
	if( Info->IsBlend )
	{
		// パスの数を取得
		if( Info->PassNum == -1 )
		{
			Info->PassNum = GraphBlendPassNum_Table[ Info->FilterOrBlendType ] ;
		}

		// 作業用テクスチャを使用するかどうかを取得
		Info->UseWorkScreen = GraphBlendUseWorkScreen_Table[ Info->FilterOrBlendType ] ;
	}
	else
	{
		switch( Info->FilterOrBlendType )
		{
		case DX_GRAPH_FILTER_MONO :
			break ;

		case DX_GRAPH_FILTER_GAUSS :
			break ;

		case DX_GRAPH_FILTER_DOWN_SCALE :
			break ;

		case DX_GRAPH_FILTER_BRIGHT_CLIP :
			break ;

		case DX_GRAPH_FILTER_HSB :
			if( Param->Hsb_HueType == 1 )
			{
				Info->PassNum = 1 ;
			}
			break ;

		case DX_GRAPH_FILTER_LEVEL :
			break ;

		case DX_GRAPH_FILTER_TWO_COLOR :
			break ;

		case DX_GRAPH_FILTER_GRADIENT_MAP :
			if( Info->TargetScreenImage != NULL && Param->GM_MapGrHandle == GSYS.DrawSetting.TargetScreen[ 0 ] )
			{
				Graphics_D3D9_EndScene() ;
				Graphics_D3D9_UpdateDrawTexture( &Info->TargetScreenImage->Orig->Hard.Tex[ 0 ], GSYS.DrawSetting.TargetScreenSurface[ 0 ] ) ;
			}
			break ;

		case DX_GRAPH_FILTER_PREMUL_ALPHA :
			break ;

		case DX_GRAPH_FILTER_INTERP_ALPHA :
			break ;
		}

		// パスの数を取得
		if( Info->PassNum == -1 )
		{
			Info->PassNum = GraphFilterPassNum_Table[ Info->FilterOrBlendType ] ;
		}

		// 作業用テクスチャを使用するかどうかを取得
		Info->UseWorkScreen = GraphFilterUseWorkScreen_Table[ Info->FilterOrBlendType ] ;
	}
	if( Info->SrcX1 != Info->DestX ||
		Info->SrcY1 != Info->DestY ||
		GD3D9.Device.Caps.ValidRenderTargetInputTexture == FALSE )
	{
		Info->UseWorkScreen = 1 ;
	}

	// シェーダーが使えない場合は必ず１パスで終わる
	if( GSYS.HardInfo.UseShader == FALSE )
	{
		Info->PassNum = 1 ;
	}

	if( GD3D9.Device.DrawInfo.BeginSceneFlag == FALSE )
	{
		Graphics_D3D9_BeginScene() ;
	}

	// パラメータの保存
	Direct3D9_GraphFilter_PushBaseDrawParam( &GraphFilterSystemInfoD3D9.DrawParamTemp ) ;

	return 0 ;
}

extern int GraphFilter_D3D9_RectBltBase_Timing1_PF( void )
{
	// パラメータの復帰
	Direct3D9_GraphFilter_PopBaseDrawParam( &GraphFilterSystemInfoD3D9.DrawParamTemp ) ;

	return 0 ;
}

extern int GraphFilter_D3D9_DestGraphSetup_PF( GRAPHFILTER_INFO *Info, int *UseSrcGrHandle, int *UseDestGrHandle )
{
	SHADOWMAPDATA *SrcShadowMap = NULL ;
	SHADOWMAPDATA *DestShadowMap = NULL ;
	IMAGEDATA     *SrcImage = NULL ;
	IMAGEDATA     *DestImage = NULL ;
	IMAGEDATA     *TempSrcImage = NULL ;
	int TexSizeW ;
	int TexSizeH ;
	int SrcTexFloatType ;
	int SrcTexChannelBitDepth ;
	int DestTexFloatType ;
	int DestTexChannelBitDepth ;
	int SrcEqDestOrDestNonDrawValidHandle ;
	int AlwaysUseDestWorkHandle ;
	int UseDestWorkHandleIndex ;
	int UseSrcWorkHandleIndex ;
	int SrcGraphDivFlag ;

	if( !GRAPHCHK( Info->SrcGrHandle, SrcImage ) )
	{
		if( SrcImage->Orig->FormatDesc.PlatformTextureFormat >= DX_TEXTUREFORMAT_DIRECT3D9_R16F &&
			SrcImage->Orig->FormatDesc.PlatformTextureFormat <= DX_TEXTUREFORMAT_DIRECT3D9_A32B32G32R32F )
		{
			SrcTexFloatType = TRUE ;
			SrcTexChannelBitDepth = 
				SrcImage->Orig->FormatDesc.PlatformTextureFormat >= DX_TEXTUREFORMAT_DIRECT3D9_R16F &&
				SrcImage->Orig->FormatDesc.PlatformTextureFormat <= DX_TEXTUREFORMAT_DIRECT3D9_A16B16G16R16F ? 16 : 32 ;
		}
		else
		{
			SrcTexFloatType = SrcImage->Orig->FormatDesc.FloatTypeFlag ;
			SrcTexChannelBitDepth = SrcImage->Orig->FormatDesc.ChannelBitDepth ;
		}
	}
	else
	if( !SHADOWMAPCHK( Info->SrcGrHandle, SrcShadowMap ) )
	{
		SrcTexFloatType = SrcShadowMap->TexFormat_Float ;
		SrcTexChannelBitDepth = 16 ;
	}
	else
	{
		return -1 ;
	}

	if( !GRAPHCHK( Info->DestGrHandle, DestImage ) )
	{
		if( DestImage->Orig->FormatDesc.PlatformTextureFormat >= DX_TEXTUREFORMAT_DIRECT3D9_R16F &&
			DestImage->Orig->FormatDesc.PlatformTextureFormat <= DX_TEXTUREFORMAT_DIRECT3D9_A32B32G32R32F )
		{
			DestTexFloatType = TRUE ;
			DestTexChannelBitDepth = 
				DestImage->Orig->FormatDesc.PlatformTextureFormat >= DX_TEXTUREFORMAT_DIRECT3D9_R16F &&
				DestImage->Orig->FormatDesc.PlatformTextureFormat <= DX_TEXTUREFORMAT_DIRECT3D9_A16B16G16R16F ? 16 : 32 ;
		}
		else
		{
			DestTexFloatType = DestImage->Orig->FormatDesc.FloatTypeFlag ;
			DestTexChannelBitDepth = DestImage->Orig->FormatDesc.ChannelBitDepth ;
		}
	}
	else
	if( !SHADOWMAPCHK( Info->DestGrHandle, DestShadowMap ) )
	{
		DestTexFloatType = DestShadowMap->TexFormat_Float ;
		DestTexChannelBitDepth = 16 ;
	}
	else
	{
		return -1 ;
	}

	UseSrcWorkHandleIndex = 0 ;

	// シェーダーが使えない場合は作業用画像は使用しない
	if( GSYS.HardInfo.UseShader == FALSE )
	{
		*UseDestGrHandle = Info->DestGrHandle ;
	}
	else
	{
		if( SrcShadowMap == NULL &&
			( ( Info->UseWorkScreen == 1 && Info->SrcGrHandle == Info->DestGrHandle ) ||
			  DestImage->Orig->FormatDesc.DrawValidFlag == FALSE ) )
		{
			SrcEqDestOrDestNonDrawValidHandle = TRUE ;
		}
		else
		{
			SrcEqDestOrDestNonDrawValidHandle = FALSE ;
		}

		if( Info->UseWorkScreen == 1 &&
			( ( Info->Pass % 2 == 0 && Info->PassNum % 2 == 0 ) ||
			  ( Info->Pass % 2 == 1 && Info->PassNum % 2 == 1 ) ) )
		{
			AlwaysUseDestWorkHandle = TRUE ;
		}
		else
		{
			AlwaysUseDestWorkHandle = FALSE ;
		}

		if(
			SrcShadowMap == NULL &&
			Info->Pass == 0 &&
			(
			  SrcImage->Hard.DrawNum != 1 ||
			  SrcImage->Hard.Draw[ 0 ].UsePosXI != 0 ||
			  SrcImage->Hard.Draw[ 0 ].UsePosYI != 0
			)
		  )
		{
			SrcGraphDivFlag = TRUE ;
		}
		else
		{
			SrcGraphDivFlag = FALSE ;
		}

		if( SrcEqDestOrDestNonDrawValidHandle || AlwaysUseDestWorkHandle )
		{
			if( AlwaysUseDestWorkHandle == FALSE && SrcEqDestOrDestNonDrawValidHandle == TRUE )
			{
				UseDestWorkHandleIndex = 1 ;
				UseSrcWorkHandleIndex = 0 ;
			}
			else
			{
				UseDestWorkHandleIndex = 0 ;
				UseSrcWorkHandleIndex = 1 ;
			}

			if( SrcGraphDivFlag )
			{
				TexSizeW = SrcImage->WidthI ;
				TexSizeH = SrcImage->HeightI ;
			}
			else
			{
				NS_GetGraphTextureSize( Info->SrcGrHandle, &TexSizeW, &TexSizeH ) ;
			}

			*UseDestGrHandle = Direct3D9_GraphFilter_GetWorkTexture( DestTexFloatType, DestTexChannelBitDepth, ( DWORD )TexSizeW, ( DWORD )TexSizeH, ( DWORD )UseDestWorkHandleIndex ) ;
			if( *UseDestGrHandle < 0 )
				return -1 ;
		}
		else
		{
			*UseDestGrHandle = Info->DestGrHandle ;
		}

		if( SrcGraphDivFlag )
		{
			VERTEX_2D Vert[ 4 ] ;
			IMAGEDATA_HARD_VERT *TexVect ;
			int i ;

			TexSizeW = SrcImage->WidthI ;
			TexSizeH = SrcImage->HeightI ;

			*UseSrcGrHandle = Direct3D9_GraphFilter_GetWorkTexture( SrcTexFloatType, SrcTexChannelBitDepth, ( DWORD )TexSizeW, ( DWORD )TexSizeH, ( DWORD )UseSrcWorkHandleIndex ) ;
			if( *UseSrcGrHandle < 0 )
				return -1 ;

			GRAPHCHK( *UseSrcGrHandle, TempSrcImage ) ;
			Graphics_D3D9_DeviceState_SetRenderTarget( TempSrcImage->Hard.Draw[ 0 ].Tex->PF->D3D9.Surface[ 0 ] ) ;

			Vert[ 0 ].rhw = 1.0f ;
			Vert[ 1 ].rhw = 1.0f ;
			Vert[ 2 ].rhw = 1.0f ;
			Vert[ 3 ].rhw = 1.0f ;
			Vert[ 0 ].color = 0xffffffff ;
			Vert[ 1 ].color = 0xffffffff ;
			Vert[ 2 ].color = 0xffffffff ;
			Vert[ 3 ].color = 0xffffffff ;
			for( i = 0 ; i < SrcImage->Hard.DrawNum ; i ++ )
			{
				TexVect = SrcImage->Hard.Draw[ i ].Vertex ;

				Vert[ 0 ].pos.x = TexVect[ 0 ].x - 0.5f ;
				Vert[ 0 ].pos.y = TexVect[ 0 ].y - 0.5f ;
				Vert[ 0 ].pos.z = 0.0f ;
				Vert[ 0 ].u = TexVect[ 0 ].u ;
				Vert[ 0 ].v = TexVect[ 0 ].v ;
				Vert[ 1 ].pos.x = TexVect[ 1 ].x - 0.5f ;
				Vert[ 1 ].pos.y = TexVect[ 1 ].y - 0.5f ;
				Vert[ 1 ].pos.z = 0.0f ;
				Vert[ 1 ].u = TexVect[ 1 ].u ;
				Vert[ 1 ].v = TexVect[ 1 ].v ;
				Vert[ 2 ].pos.x = TexVect[ 2 ].x - 0.5f ;
				Vert[ 2 ].pos.y = TexVect[ 2 ].y - 0.5f ;
				Vert[ 2 ].u = TexVect[ 2 ].u ;
				Vert[ 2 ].v = TexVect[ 2 ].v ;
				Vert[ 2 ].pos.z = 0.0f ;
				Vert[ 3 ].pos.x = TexVect[ 3 ].x - 0.5f ;
				Vert[ 3 ].pos.y = TexVect[ 3 ].y - 0.5f ;
				Vert[ 3 ].pos.z = 0.0f ;
				Vert[ 3 ].u = TexVect[ 3 ].u ;
				Vert[ 3 ].v = TexVect[ 3 ].v ;
				Graphics_D3D9_DrawPrimitive2D( Vert, 4, DX_PRIMTYPE_TRIANGLESTRIP, SrcImage, TRUE, FALSE, FALSE, FALSE, FALSE, i, FALSE ) ;
			}
		}
	}

	return 0 ;
}

extern int GraphFilter_D3D9_DestGraphUpdate_PF( GRAPHFILTER_INFO *Info, int UseDestGrHandle, int FilterResult )
{
	if( FilterResult >= 0 )
	{
		if( GSYS.HardInfo.UseShader == TRUE )
		{
			if( ( Info->UseWorkScreen == 1 && Info->SrcGrHandle == Info->DestGrHandle ) ||
				UseDestGrHandle != Info->DestGrHandle )
			{
				Graphics_Image_GetDrawScreenGraphBase(
					UseDestGrHandle,
					0,
					0,
					Info->DestX,
					Info->DestY,
					Info->DestX + ( Info->SrcX2 - Info->SrcX1 ) / Info->SrcDivNum,
					Info->DestY + ( Info->SrcY2 - Info->SrcY1 ) / Info->SrcDivNum,
					Info->DestX,
					Info->DestY,
					Info->DestGrHandle
				) ;
			}
		}
	}

	return 0 ;
}





#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_FILTER

#endif // DX_NON_DIRECT3D9

#endif // DX_NON_GRAPHICS
