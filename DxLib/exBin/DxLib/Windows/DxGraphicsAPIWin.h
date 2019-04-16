// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		描画ＡＰＩプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXGRAPHICSAPIWIN_H__
#define __DXGRAPHICSAPIWIN_H__

#include "../DxCompileConfig.h"

#ifndef DX_NON_GRAPHICS

// インクルード ------------------------------------------------------------------
#include "DxDirectX.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

struct GRAPHICSAPIINFO_WIN
{
	D_IDirectDraw7			*DirectDraw7Object ;					// ＤｉｒｅｃｔＤｒａｗインターフェイス
	LONGLONG				DirectDraw7_VSyncWaitTime ;				// 前回ＶＳＹＮＣ待ちをしてから次にチェックするまでに待つ時間
	LONGLONG				DirectDraw7_VSyncTime ;					// 前回ＶＳＹＮＣ待ちをした時間

#ifndef DX_NON_DIRECT3D9
	HINSTANCE				Direct3D9DLL ;							// Ｄｉｒｅｃｔ３Ｄ９．ＤＬＬ
	D_IDirect3D9			*Direct3D9Object ;						// Ｄｉｒｅｃｔ３Ｄ９インターフェイス
	D_IDirect3D9Ex			*Direct3D9ExObject ;					// Ｄｉｒｅｃｔ３Ｄ９Ｅｘインターフェイス
	D_IDirect3DDevice9		*Direct3DDevice9Object ;				// Ｄｉｒｅｃｔ３ＤＤｅｖｉｃｅ９インターフェイス
	D_IDirect3DDevice9Ex	*Direct3DDevice9ExObject ;				// Ｄｉｒｅｃｔ３ＤＤｅｖｉｃｅ９Ｅｘインターフェイス
	D_IDirect3DSwapChain9	*Direct3DSwapChain9Object ;				// Ｄｉｒｅｃｔ３ＤＳｗａｐＣｈａｉｎ９インターフェイス
#endif // DX_NON_DIRECT3D9

#ifndef DX_NON_DIRECT3D11
	HINSTANCE				Direct3D11DLL ;							// Ｄｉｒｅｃｔ３Ｄ１１．ＤＬＬ
	HINSTANCE				DXGIDLL ;								// ｄｘｇｉ．ｄｌｌ
	D_IDXGIDevice1			*DXGIDevice1Object ;					// ＤＸＧＩＤｅｖｉｃｅ１インターフェイス
	D_IDXGIAdapter			*DXGIAdapterObject ;					// ＤＸＧＩＡｄａｐｔｅｒインターフェイス
	D_IDXGIFactory			*DXGIFactoryObject ;					// ＤＸＧＩＦａｃｔｏｒｙインターフェイス
	D_IDXGIFactory1			*DXGIFactory1Object ;					// ＤＸＧＩＦａｃｔｏｒｙ１インターフェイス
	D_ID3D11Device			*D3D11DeviceObject ;					// Ｄ３Ｄ１１Ｄｅｖｉｃｅインターフェイス
	D_ID3D11DeviceContext	*D3D11DeviceContext ;					// Ｄ３Ｄ１１ＤｅｖｉｃｅＣｏｎｔｅｘｔインターフェイス
	D_ID3D11DeviceContext	*D3D11DeferredContext ;					// Ｄ３Ｄ１１ＤｅｖｉｃｅＣｏｎｔｅｘｔインターフェイス( DeferredContext )
	int						D3D11BufferNum ;
	int						D3D11Texture1DNum ;
	int						D3D11Texture2DNum ;
	int						D3D11Texture3DNum ;
	int						D3D11ShaderResourceViewNum ;
	int						D3D11RenderTargetViewNum ;
	int						D3D11DepthStencilViewNum ;
	int						D3D11InputLayoutNum ;
	int						D3D11VertexShaderNum ;
	int						D3D11PixelShaderNum ;
	int						D3D11BlendStateNum ;
	int						D3D11DepthStencilStateNum ;
	int						D3D11RasterizerStateNum ;
	int						D3D11SamplerStateNum ;
#endif // DX_NON_DIRECT3D11
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern GRAPHICSAPIINFO_WIN GAPIWin ;

// 関数プロトタイプ宣言-----------------------------------------------------------



extern	int		DirectDraw7_Create						( void ) ;
extern	int		DirectDraw7_Release						( void ) ;
extern	void *	DirectDraw7_GetObject					( void ) ;
extern	int		DirectDraw7_GetVideoMemorySize			( int *TotalSize, int *FreeSize ) ;
extern	void	DirectDraw7_WaitVSyncInitialize			( void ) ;
extern	void	DirectDraw7_LocalWaitVSync				( void ) ;
extern	int		DirectDraw7_WaitVSync					( int SyncNum ) ;




#ifndef DX_NON_DIRECT3D11

extern	ULONG	Direct3D11_ObjectRelease_ASync					( void *pObject, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_Buffer_ASync					( D_ID3D11Buffer *pBuffer, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_Texture1D_ASync				( D_ID3D11Texture1D *pTexture1D, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_Texture2D_ASync				( D_ID3D11Texture2D *pTexture2D, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_Texture3D_ASync				( D_ID3D11Texture3D *pTexture3D, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_ShaderResourceView_ASync		( D_ID3D11ShaderResourceView *pSRView, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_RenderTargetView_ASync		( D_ID3D11RenderTargetView *pRTView, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_DepthStencilView_ASync		( D_ID3D11DepthStencilView *pDepthStencilView, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_InputLayout_ASync			( D_ID3D11InputLayout *pInputLayout, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_VertexShader_ASync			( D_ID3D11VertexShader *pVertexShader, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_PixelShader_ASync			( D_ID3D11PixelShader *pPixelShader, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_BlendState_ASync				( D_ID3D11BlendState *pBlendState, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_DepthStencilState_ASync		( D_ID3D11DepthStencilState *pDepthStencilState, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_RasterizerState_ASync		( D_ID3D11RasterizerState *pRasterizerState, int ASyncThread = FALSE ) ;
extern	ULONG	Direct3D11_Release_SamplerState_ASync			( D_ID3D11SamplerState *pSamplerState, int ASyncThread = FALSE ) ;

extern	HRESULT	D3D11Device_CreateBuffer_ASync					( const D_D3D11_BUFFER_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Buffer **ppBuffer, int ASyncThread = FALSE ) ;
extern	HRESULT D3D11Device_CreateTexture1D_ASync				( const D_D3D11_TEXTURE1D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture1D **ppTexture1D, int ASyncThread = FALSE ) ;
extern	HRESULT D3D11Device_CreateTexture2D_ASync				( const D_D3D11_TEXTURE2D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture2D **ppTexture2D, int ASyncThread = FALSE ) ;
extern	HRESULT D3D11Device_CreateTexture3D_ASync				( const D_D3D11_TEXTURE3D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture3D **ppTexture3D, int ASyncThread = FALSE ) ;
extern	HRESULT D3D11Device_CreateShaderResourceView_ASync		( D_ID3D11Resource *pResource, const D_D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, D_ID3D11ShaderResourceView **ppSRView, int ASyncThread = FALSE ) ;
extern	HRESULT D3D11Device_CreateRenderTargetView_ASync		( D_ID3D11Resource *pResource, const D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc, D_ID3D11RenderTargetView **ppRTView, int ASyncThread = FALSE ) ;
extern	HRESULT	D3D11Device_CreateDepthStencilView_ASync		( D_ID3D11Resource *pResource, const D_D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, D_ID3D11DepthStencilView **ppDepthStencilView, int ASyncThread = FALSE ) ;
extern	HRESULT	D3D11Device_CreateVertexShader_ASync			( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11VertexShader **ppVertexShader, int ASyncThread = FALSE ) ;
extern	HRESULT	D3D11Device_CreatePixelShader_ASync				( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11PixelShader **ppPixelShader, int ASyncThread = FALSE ) ;
extern	long	D3D11Device_CheckMultisampleQualityLevels_ASync	( D_DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels, int ASyncThread = FALSE ) ;
extern	int		D3D11Device_CheckMultiSampleParam_ASync			( D_DXGI_FORMAT Format, UINT *Samples, UINT *Quality, int SamplesFailedBreak, int ASyncThread = FALSE ) ;

extern	HRESULT	D3D11DeviceContext_Map_ASync					( D_ID3D11Resource *pResource, UINT Subresource, D_D3D11_MAP MapType, UINT MapFlags, D_D3D11_MAPPED_SUBRESOURCE *pMappedResource, int ASyncThread = FALSE ) ;
extern	void	D3D11DeviceContext_Unmap_ASync					( D_ID3D11Resource *pResource, UINT Subresource, int ASyncThread = FALSE ) ;
extern	void	D3D11DeviceContext_CopyResource_ASync			( D_ID3D11Resource *pDstResource, D_ID3D11Resource *pSrcResource, int ASyncThread = FALSE ) ;
extern	void	D3D11DeviceContext_UpdateSubresource_ASync		( D_ID3D11Resource *pDstResource, UINT DstSubresource, const D_D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch, int ASyncThread = FALSE ) ;
extern	void	D3D11DeviceContext_ClearRenderTargetView_ASync	( D_ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ], int ASyncThread = FALSE ) ;
extern	void	D3D11DeviceContext_ClearDepthStencilView_ASync	( D_ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, D_UINT8 Stencil, int ASyncThread = FALSE ) ;



extern	ULONG	Direct3D11_ObjectRelease					( void *pObject ) ;
extern	ULONG	Direct3D11_Release_Buffer					( D_ID3D11Buffer *pBuffer ) ;
extern	ULONG	Direct3D11_Release_Texture1D				( D_ID3D11Texture1D *pTexture1D ) ;
extern	ULONG	Direct3D11_Release_Texture2D				( D_ID3D11Texture2D *pTexture2D ) ;
extern	ULONG	Direct3D11_Release_Texture3D				( D_ID3D11Texture3D *pTexture3D ) ;
extern	ULONG	Direct3D11_Release_ShaderResourceView		( D_ID3D11ShaderResourceView *pSRView ) ;
extern	ULONG	Direct3D11_Release_RenderTargetView			( D_ID3D11RenderTargetView *pRTView ) ;
extern	ULONG	Direct3D11_Release_DepthStencilView			( D_ID3D11DepthStencilView *pDepthStencilView ) ;
extern	ULONG	Direct3D11_Release_InputLayout				( D_ID3D11InputLayout *pInputLayout ) ;
extern	ULONG	Direct3D11_Release_VertexShader				( D_ID3D11VertexShader *pVertexShader ) ;
extern	ULONG	Direct3D11_Release_PixelShader				( D_ID3D11PixelShader *pPixelShader ) ;
extern	ULONG	Direct3D11_Release_BlendState				( D_ID3D11BlendState *pBlendState ) ;
extern	ULONG	Direct3D11_Release_DepthStencilState		( D_ID3D11DepthStencilState *pDepthStencilState ) ;
extern	ULONG	Direct3D11_Release_RasterizerState			( D_ID3D11RasterizerState *pRasterizerState ) ;
extern	ULONG	Direct3D11_Release_SamplerState				( D_ID3D11SamplerState *pSamplerState ) ;
extern	void	Direct3D11_DumpObject						( void ) ;

extern	int		DXGI_LoadDLL							( void ) ;
extern	int		DXGI_FreeDLL							( void ) ;

extern	int		Direct3D11_LoadDLL						( void ) ;
extern	int		Direct3D11_FreeDLL						( void ) ;

extern	int		DXGIFactory_Create						( void ) ;
extern	int		DXGIFactory_Release						( void ) ;
extern	int		DXGIFactory_IsValid						( void ) ;
extern	HRESULT	DXGIFactory_EnumAdapters				( UINT Adapter, D_IDXGIAdapter **ppAdapter ) ;
extern	HRESULT DXGIFactory_CreateSwapChain				( D_DXGI_SWAP_CHAIN_DESC *pDesc, D_IDXGISwapChain **ppSwapChain ) ;
extern	HRESULT	DXGIFactory_MakeWindowAssociation		( HWND WindowHandle, UINT Flags ) ;

extern	HRESULT DXGIAdapter_GetDesc						( D_IDXGIAdapter * pAdapter, D_DXGI_ADAPTER_DESC *pDesc ) ;
extern	HRESULT	DXGIAdapter_EnumOutputs					( D_IDXGIAdapter * pAdapter, UINT Output, D_IDXGIOutput **ppOutput ) ;

extern	HRESULT	DXGIOutput_GetDesc						( D_IDXGIOutput *pOutput, D_DXGI_OUTPUT_DESC *pDesc ) ;
extern	HRESULT DXGIOutput_WaitForVBlank				( D_IDXGIOutput *pOutput ) ;

extern	int		D3D11_CreateDevice						( void ) ;

extern	HRESULT DXGISwapChain_Present					( D_IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags ) ;
extern	HRESULT DXGISwapChain_GetBuffer					( D_IDXGISwapChain *pSwapChain, UINT Buffer, REFIID riid, void **ppSurface ) ;
extern	HRESULT	DXGISwapChain_SetFullscreenState		( D_IDXGISwapChain *pSwapChain, BOOL Fullscreen, D_IDXGIOutput *pTarget ) ;
extern	HRESULT	DXGISwapChain_GetDesc					( D_IDXGISwapChain *pSwapChain, D_DXGI_SWAP_CHAIN_DESC *pDesc ) ;
extern	HRESULT	DXGISwapChain_ResizeBuffers				( D_IDXGISwapChain *pSwapChain, UINT BufferCount, UINT Width, UINT Height, D_DXGI_FORMAT NewFormat, UINT SwapChainFlags ) ;
extern	HRESULT	DXGISwapChain_GetContainingOutput		( D_IDXGISwapChain *pSwapChain, D_IDXGIOutput **ppOutput ) ;

extern	void *	D3D11Device_Get							( void ) ;
extern	int		D3D11Device_Release						( void ) ;
extern	int		D3D11Device_IsValid						( void ) ;
extern	HRESULT	D3D11Device_CreateBuffer				( const D_D3D11_BUFFER_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Buffer **ppBuffer ) ;
extern	HRESULT	D3D11Device_CreateTexture1D				( const D_D3D11_TEXTURE1D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture1D **ppTexture1D ) ;
extern	HRESULT	D3D11Device_CreateTexture2D				( const D_D3D11_TEXTURE2D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture2D **ppTexture2D ) ;
extern	HRESULT	D3D11Device_CreateTexture3D				( const D_D3D11_TEXTURE3D_DESC *pDesc, const D_D3D11_SUBRESOURCE_DATA *pInitialData, D_ID3D11Texture3D **ppTexture3D ) ;
extern	HRESULT	D3D11Device_CreateShaderResourceView	( D_ID3D11Resource *pResource, const D_D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, D_ID3D11ShaderResourceView **ppSRView ) ;
extern	HRESULT	D3D11Device_CreateRenderTargetView		( D_ID3D11Resource *pResource, const D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc, D_ID3D11RenderTargetView **ppRTView ) ;
extern	HRESULT	D3D11Device_CreateDepthStencilView		( D_ID3D11Resource *pResource, const D_D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, D_ID3D11DepthStencilView **ppDepthStencilView ) ;
extern	HRESULT	D3D11Device_CreateInputLayout			( const D_D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, D_ID3D11InputLayout **ppInputLayout ) ;
extern	HRESULT	D3D11Device_CreateVertexShader			( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11VertexShader **ppVertexShader ) ;
extern	HRESULT	D3D11Device_CreatePixelShader			( const void *pShaderBytecode, SIZE_T BytecodeLength, D_ID3D11ClassLinkage *pClassLinkage, D_ID3D11PixelShader **ppPixelShader ) ;
extern	HRESULT	D3D11Device_CreateBlendState			( const D_D3D11_BLEND_DESC         *pBlendStateDesc,   D_ID3D11BlendState        **ppBlendState        ) ;
extern	HRESULT	D3D11Device_CreateDepthStencilState		( const D_D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, D_ID3D11DepthStencilState **ppDepthStencilState ) ;
extern	HRESULT	D3D11Device_CreateRasterizerState		( const D_D3D11_RASTERIZER_DESC    *pRasterizerDesc,   D_ID3D11RasterizerState   **ppRasterizerState   ) ;
extern	HRESULT	D3D11Device_CreateSamplerState			( const D_D3D11_SAMPLER_DESC *pSamplerDesc, D_ID3D11SamplerState **ppSamplerState ) ;
extern	HRESULT	D3D11Device_CheckFormatSupport			( D_DXGI_FORMAT Format, UINT *pFormatSupport ) ;
extern	long	D3D11Device_CheckMultisampleQualityLevels( D_DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels ) ;
extern	int		D3D11Device_CheckMultiSampleParam		( D_DXGI_FORMAT Format, UINT *Samples, UINT *Quality, int SamplesFailedBreak ) ;	// マルチサンプルレンダリングのサンプル数とクオリティをチェック

extern	void *	D3D11DeviceContext_Get					( void ) ;
extern	void *	D3D11DeviceContext_Deferred_Get			( void ) ;
extern	void	D3D11DeviceContext_VSSetConstantBuffers	( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) ;
extern	void	D3D11DeviceContext_PSSetShaderResources	( UINT StartSlot, UINT NumViews,   D_ID3D11ShaderResourceView *const *ppShaderResourceViews ) ;
extern	void	D3D11DeviceContext_PSSetShader			( D_ID3D11PixelShader *pPixelShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) ;
extern	void	D3D11DeviceContext_PSSetSamplers		( UINT StartSlot, UINT NumSamplers, D_ID3D11SamplerState *const *ppSamplers ) ;
extern	void	D3D11DeviceContext_VSSetShader			( D_ID3D11VertexShader *pVertexShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) ;
extern	void	D3D11DeviceContext_DrawIndexed			( UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation ) ;
extern	void	D3D11DeviceContext_Draw					( UINT VertexCount, UINT StartVertexLocation ) ;
extern	HRESULT	D3D11DeviceContext_Map					( D_ID3D11Resource *pResource, UINT Subresource, D_D3D11_MAP MapType, UINT MapFlags, D_D3D11_MAPPED_SUBRESOURCE *pMappedResource ) ;
extern	void	D3D11DeviceContext_Unmap				( D_ID3D11Resource *pResource, UINT Subresource ) ;
extern	void	D3D11DeviceContext_PSSetConstantBuffers	( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppConstantBuffers ) ;
extern	void	D3D11DeviceContext_IASetInputLayout		( D_ID3D11InputLayout *pInputLayout ) ;
extern	void	D3D11DeviceContext_IASetVertexBuffers	( UINT StartSlot, UINT NumBuffers, D_ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets ) ;
extern	void	D3D11DeviceContext_IASetIndexBuffer		( D_ID3D11Buffer *pIndexBuffer, D_DXGI_FORMAT Format, UINT Offset ) ;
extern	void	D3D11DeviceContext_GSSetShader			( D_ID3D11GeometryShader *pShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) ;
extern	void	D3D11DeviceContext_IASetPrimitiveTopology( D_D3D11_PRIMITIVE_TOPOLOGY Topology ) ;
extern	void	D3D11DeviceContext_Begin				( D_ID3D11Asynchronous *pAsync ) ;
extern	void	D3D11DeviceContext_End					( D_ID3D11Asynchronous *pAsync ) ;
extern	void	D3D11DeviceContext_OMSetRenderTargets	( UINT NumViews, D_ID3D11RenderTargetView *const *ppRenderTargetViews, D_ID3D11DepthStencilView *pDepthStencilView ) ;
extern	void	D3D11DeviceContext_OMSetBlendState		( D_ID3D11BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask ) ;
extern	void	D3D11DeviceContext_OMSetDepthStencilState( D_ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef ) ;
extern	void	D3D11DeviceContext_RSSetState			( D_ID3D11RasterizerState *pRasterizerState ) ;
extern	void	D3D11DeviceContext_RSSetViewports		( UINT NumViewports, const D_D3D11_VIEWPORT *pViewports ) ;
extern	void	D3D11DeviceContext_CopyResource			( D_ID3D11Resource *pDstResource, D_ID3D11Resource *pSrcResource ) ;
extern	void	D3D11DeviceContext_UpdateSubresource	( D_ID3D11Resource *pDstResource, UINT DstSubresource, const D_D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch ) ;
extern	void	D3D11DeviceContext_ClearRenderTargetView( D_ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ] ) ;
extern	void	D3D11DeviceContext_ClearDepthStencilView( D_ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, D_UINT8 Stencil ) ;
extern	void	D3D11DeviceContext_ResolveSubresource	( D_ID3D11Resource *pDstResource, UINT DstSubresource, D_ID3D11Resource *pSrcResource, UINT SrcSubresource, D_DXGI_FORMAT Format ) ;
extern	void	D3D11DeviceContext_HSSetShader			( D_ID3D11HullShader *pHullShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) ;
extern	void	D3D11DeviceContext_DSSetShader			( D_ID3D11DomainShader *pDomainShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) ;
extern	void	D3D11DeviceContext_CSSetShader			( D_ID3D11ComputeShader *pComputeShader, D_ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances ) ;
extern	void	D3D11DeviceContext_ClearState			( void ) ;
extern	void	D3D11DeviceContext_Flush				( void ) ;

extern	void	D3D11View_GetResource					( D_ID3D11View *View, D_ID3D11Resource **ppResource ) ;

extern	void	D3D11Texture2D_GetDesc					( D_ID3D11Texture2D *Texture2D, D_D3D11_TEXTURE2D_DESC *pDesc ) ;
extern	void	D3D11RenderTargetView_GetDesc			( D_ID3D11RenderTargetView *RenderTargetView, D_D3D11_RENDER_TARGET_VIEW_DESC *pDesc ) ;

#endif // DX_NON_DIRECT3D11






#ifndef DX_NON_DIRECT3D9

extern	ULONG	Direct3D9_ObjectRelease_ASync						( void *pObject, int ASyncThread = FALSE ) ;
extern	int		Direct3D9_CheckMultiSampleParam_ASync				( D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE *Samples, DWORD *Quality, int SamplesFailedBreak, int ASyncThread = FALSE ) ;	// マルチサンプルレンダリングのサンプル数とクオリティをチェック

extern	HRESULT	Direct3DDevice9_CreateRenderTarget_ASync			( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_CreateDepthStencilSurface_ASync		( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_CreateTexture_ASync					( UINT Width, UINT Height, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_CreateCubeTexture_ASync				( UINT EdgeLength, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_ColorFill_ASync						( D_IDirect3DSurface9* pSurface, CONST RECT* pRect, D_D3DCOLOR color, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_CreateOffscreenPlainSurface_ASync	( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_UpdateSurface_ASync					( D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, D_IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_UpdateTexture_ASync					( D_IDirect3DBaseTexture9* pSourceTexture, D_IDirect3DBaseTexture9* pDestinationTexture, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_CreateVertexShader_ASync			( CONST DWORD* pFunction, D_IDirect3DVertexShader9** ppShader, int ASyncThread ) ;
extern	HRESULT	Direct3DDevice9_CreatePixelShader_ASync				( CONST DWORD* pFunction, D_IDirect3DPixelShader9** ppShader, int ASyncThread ) ;
extern	int		Direct3DDevice9_CreateVertexBuffer_ASync			( DWORD Length, DWORD Usage, DWORD FVFFlag, D_D3DPOOL Pool, D_IDirect3DVertexBuffer9 **BufferP, int ASyncThread = FALSE ) ;		// 頂点バッファを作成する
extern	int		Direct3DDevice9_CreateIndexBuffer_ASync				( DWORD Length, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DIndexBuffer9 **BufferP, int ASyncThread = FALSE ) ;	// インデックスバッファを作成する

extern	HRESULT	Direct3DTexture9_GetSurfaceLevel_ASync				( D_IDirect3DTexture9 *Texture, UINT Level,  D_IDirect3DSurface9** ppSurfaceLevel, int ASyncThread ) ;
extern	HRESULT	Direct3DTexture9_LockRect_ASync						( D_IDirect3DTexture9 *Texture, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, int ASyncThread ) ;
extern	HRESULT	Direct3DTexture9_UnlockRect_ASync					( D_IDirect3DTexture9 *Texture, UINT Level, int ASyncThread ) ;

extern	HRESULT Direct3DCubeTexture9_GetCubeMapSurface_ASync		( D_IDirect3DCubeTexture9 *CubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_IDirect3DSurface9** ppCubeMapSurface, int ASyncThread) ;
extern	HRESULT Direct3DCubeTexture9_LockRect_ASync					( D_IDirect3DCubeTexture9 *CubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, int ASyncThread) ;
extern	HRESULT Direct3DCubeTexture9_UnlockRect_ASync				( D_IDirect3DCubeTexture9 *CubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, int ASyncThread) ;

extern	HRESULT	Direct3DSurface9_LockRect_ASync						( D_IDirect3DSurface9* pSurface, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags, int ASyncThread ) ;
extern	HRESULT	Direct3DSurface9_UnlockRect_ASync					( D_IDirect3DSurface9* pSurface, int ASyncThread ) ;

extern	HRESULT	Direct3DVertexBuffer9_Lock_ASync					( D_IDirect3DVertexBuffer9 *Buffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags, int ASyncThread = FALSE ) ;
extern	HRESULT Direct3DVertexBuffer9_Unlock_ASync					( D_IDirect3DVertexBuffer9 *Buffer, int ASyncThread = FALSE ) ;

extern	HRESULT	Direct3DIndexBuffer9_Lock_ASync						( D_IDirect3DIndexBuffer9 *Buffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags, int ASyncThread = FALSE ) ;
extern	HRESULT Direct3DIndexBuffer9_Unlock_ASync					( D_IDirect3DIndexBuffer9 *Buffer, int ASyncThread = FALSE ) ;



extern	ULONG	Direct3D9_ObjectRelease					( void *pObject ) ;


extern	int		Direct3D9_Create						( void ) ;
extern	int		Direct3D9_LoadDLL						( void ) ;
extern	int		Direct3D9_FreeDLL						( void ) ;
extern	ULONG	Direct3D9_Release						( void ) ;
extern	UINT	Direct3D9_GetAdapterCount				( void ) ;
extern	long	Direct3D9_GetAdapterIdentifier			( DWORD Adapter, DWORD Flags, D_D3DADAPTER_IDENTIFIER9* pIdentifier) ;
extern	UINT	Direct3D9_GetAdapterModeCount			( DWORD Adapter, D_D3DFORMAT Format) ;
extern	long	Direct3D9_EnumAdapterModes				( DWORD Adapter, D_D3DFORMAT Format, DWORD Mode,D_D3DDISPLAYMODE* pMode) ;
extern	long	Direct3D9_GetAdapterDisplayMode			( DWORD Adapter, D_D3DDISPLAYMODE* pMode) ;
extern	long	Direct3D9_CheckDeviceFormat				( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, DWORD Usage, D_D3DRESOURCETYPE RType, D_D3DFORMAT CheckFormat) ;
extern	long	Direct3D9_CheckDeviceMultiSampleType	( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT SurfaceFormat, BOOL Windowed, D_D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) ;
extern	long	Direct3D9_CheckDepthStencilMatch		( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DFORMAT AdapterFormat, D_D3DFORMAT RenderTargetFormat, D_D3DFORMAT DepthStencilFormat) ;
extern	long	Direct3D9_GetDeviceCaps					( DWORD Adapter, D_D3DDEVTYPE DeviceType, D_D3DCAPS9* pCaps) ;
extern	HANDLE	Direct3D9_GetAdapterMonitor				( DWORD Adapter ) ;
extern	int		Direct3D9_CreateDevice					( void ) ;
extern	int		Direct3D9_CheckMultiSampleParam			( D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE *Samples, DWORD *Quality, int SamplesFailedBreak ) ;	// マルチサンプルレンダリングのサンプル数とクオリティをチェック
extern	int		Direct3D9_IsValid						( void ) ;
extern	int		Direct3D9_IsExObject					( void ) ;


extern	int		Direct3DDevice9_Release					( void ) ;
extern	UINT	Direct3DDevice9_GetAvailableTextureMem	( void ) ;
extern	long	Direct3DDevice9_GetDeviceCaps			( D_D3DCAPS9* pCaps) ;
extern	long	Direct3DDevice9_GetBackBuffer			( UINT iSwapChain, UINT iBackBuffer, D_D3DBACKBUFFER_TYPE Type, D_IDirect3DSurface9** ppBackBuffer ) ;
extern	long	Direct3DDevice9_SetDialogBoxMode		( BOOL bEnableDialogs) ;
extern	long	Direct3DDevice9_CreateTexture			( UINT Width, UINT Height, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_CreateVolumeTexture		( UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_CreateCubeTexture		( UINT EdgeLength, UINT Levels, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_CreateVertexBuffer		( UINT Length, DWORD Usage, DWORD FVF, D_D3DPOOL Pool, D_IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_CreateIndexBuffer		( UINT Length, DWORD Usage, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_CreateRenderTarget		( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_CreateDepthStencilSurface( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_UpdateSurface			( D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, D_IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) ;
extern	long	Direct3DDevice9_UpdateTexture			( D_IDirect3DBaseTexture9* pSourceTexture, D_IDirect3DBaseTexture9* pDestinationTexture) ;
extern	long	Direct3DDevice9_GetRenderTargetData		( D_IDirect3DSurface9* pRenderTarget, D_IDirect3DSurface9* pDestSurface) ;
extern	long	Direct3DDevice9_StretchRect				( D_IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, D_IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D_D3DTEXTUREFILTERTYPE Filter) ;
extern	long	Direct3DDevice9_ColorFill				( D_IDirect3DSurface9* pSurface, CONST RECT* pRect, D_D3DCOLOR color) ;
extern	long	Direct3DDevice9_CreateOffscreenPlainSurface( UINT Width, UINT Height, D_D3DFORMAT Format, D_D3DPOOL Pool, D_IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) ;
extern	long	Direct3DDevice9_SetRenderTarget			( DWORD RenderTargetIndex, D_IDirect3DSurface9* pRenderTarget) ;
extern	long	Direct3DDevice9_SetDepthStencilSurface	( D_IDirect3DSurface9* pNewZStencil) ;
extern	long	Direct3DDevice9_GetDepthStencilSurface	( D_IDirect3DSurface9** ppZStencilSurface) ;
extern	long	Direct3DDevice9_BeginScene				( void ) ;
extern	long	Direct3DDevice9_EndScene				( void ) ;
extern	long	Direct3DDevice9_Clear					( DWORD Count, CONST D_D3DRECT* pRects, DWORD Flags, D_D3DCOLOR Color, float Z, DWORD Stencil) ;
extern	long	Direct3DDevice9_SetTransform			( D_D3DTRANSFORMSTATETYPE State, CONST D_D3DMATRIX* pMatrix) ;
extern	long	Direct3DDevice9_SetViewport				( CONST D_D3DVIEWPORT9* pViewport) ;
extern	long	Direct3DDevice9_SetMaterial				( CONST D_D3DMATERIAL9* pMaterial) ;
extern	long	Direct3DDevice9_GetMaterial				( D_D3DMATERIAL9* pMaterial) ;
extern	long	Direct3DDevice9_SetLight				( DWORD Index, CONST D_D3DLIGHT9* Param ) ;
extern	long	Direct3DDevice9_LightEnable				( DWORD Index, BOOL Enable) ;
extern	long	Direct3DDevice9_SetRenderState			( D_D3DRENDERSTATETYPE State, DWORD Value ) ;
extern	long	Direct3DDevice9_SetTexture				( DWORD Stage, D_IDirect3DBaseTexture9* pTexture) ;
extern	long	Direct3DDevice9_SetTextureStageState	( DWORD Stage, D_D3DTEXTURESTAGESTATETYPE Type, DWORD Value) ;
extern	long	Direct3DDevice9_SetSamplerState			( DWORD Sampler, D_D3DSAMPLERSTATETYPE Type, DWORD Value ) ;
extern	long	Direct3DDevice9_SetScissorRect			( CONST RECT* pRect) ;
extern	long	Direct3DDevice9_DrawPrimitive			( D_D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount ) ;
extern	long	Direct3DDevice9_DrawIndexedPrimitive	( D_D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount ) ;
extern	long	Direct3DDevice9_DrawPrimitiveUP			( D_D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride ) ;
extern	long	Direct3DDevice9_DrawIndexedPrimitiveUP	( D_D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D_D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride ) ;
extern	long	Direct3DDevice9_CreateVertexDeclaration	( CONST D_D3DVERTEXELEMENT9* pVertexElements, D_IDirect3DVertexDeclaration9** ppDecl) ;
extern	long	Direct3DDevice9_SetVertexDeclaration	( D_IDirect3DVertexDeclaration9* pDecl ) ;
extern	long	Direct3DDevice9_SetFVF					( DWORD FVF ) ;
extern	long	Direct3DDevice9_CreateVertexShader		( CONST DWORD* pFunction, D_IDirect3DVertexShader9** ppShader) ;
extern	long	Direct3DDevice9_SetVertexShader			( D_IDirect3DVertexShader9* pShader) ;
extern	long	Direct3DDevice9_SetVertexShaderConstantF( UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) ;
extern	long	Direct3DDevice9_SetVertexShaderConstantI( UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) ;
extern	long	Direct3DDevice9_SetVertexShaderConstantB( UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount) ;
extern	long	Direct3DDevice9_SetStreamSource			( UINT StreamNumber, D_IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride ) ;
extern	long	Direct3DDevice9_SetIndices				( D_IDirect3DIndexBuffer9* pIndexData ) ;
extern	long	Direct3DDevice9_CreatePixelShader		( CONST DWORD* pFunction, D_IDirect3DPixelShader9** ppShader) ;
extern	long	Direct3DDevice9_SetPixelShader			( D_IDirect3DPixelShader9* pShader) ;
extern	long	Direct3DDevice9_SetPixelShaderConstantF	( UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) ;
extern	long	Direct3DDevice9_SetPixelShaderConstantI	( UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) ;
extern	long	Direct3DDevice9_SetPixelShaderConstantB	( UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount) ;
extern	int		Direct3DDevice9_Present					( void ) ;
extern	int		Direct3DDevice9_BltRectBackScreenToWindow( HWND Window, RECT BackScreenRect, RECT WindowClientRect ) ;
extern	int		Direct3DDevice9_IsValid					( void ) ;
extern	void *	Direct3DDevice9_GetObject				( void ) ;
extern	int		Direct3DDevice9_IsLost					( void ) ;
extern	int		Direct3DDevice9_SetupTimerPresent		( int EnableFlag ) ;


extern	long	Direct3DTexture9_GetSurfaceLevel		( D_IDirect3DTexture9 *pTexture, UINT Level, D_IDirect3DSurface9 ** ppSurfaceLevel) ;
extern	long	Direct3DTexture9_LockRect				( D_IDirect3DTexture9 *pTexture, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) ;
extern	long	Direct3DTexture9_UnlockRect				( D_IDirect3DTexture9 *pTexture, UINT Level) ;

extern	long	Direct3DCubeTexture9_GetCubeMapSurface	( D_IDirect3DCubeTexture9 *pCubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_IDirect3DSurface9** ppCubeMapSurface) ;
extern	long	Direct3DCubeTexture9_LockRect			( D_IDirect3DCubeTexture9 *pCubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) ;
extern	long	Direct3DCubeTexture9_UnlockRect			( D_IDirect3DCubeTexture9 *pCubeTexture, D_D3DCUBEMAP_FACES FaceType, UINT Level) ;

extern	long	Direct3DSurface9_GetDesc				( D_IDirect3DSurface9 *pSurface, D_D3DSURFACE_DESC *pDesc) ;
extern	long	Direct3DSurface9_LockRect				( D_IDirect3DSurface9 *pSurface, D_D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) ;
extern	long	Direct3DSurface9_UnlockRect				( D_IDirect3DSurface9 *pSurface ) ;


extern	long	Direct3DVertexBuffer9_Lock				( D_IDirect3DVertexBuffer9 *pVertexBuffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags ) ;
extern	long	Direct3DVertexBuffer9_Unlock			( D_IDirect3DVertexBuffer9 *pVertexBuffer ) ;


extern	long	Direct3DIndexBuffer9_Lock				( D_IDirect3DIndexBuffer9 *pIndexBuffer, UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags) ;
extern	long	Direct3DIndexBuffer9_Unlock				( D_IDirect3DIndexBuffer9 *pIndexBuffer ) ;

#endif // DX_NON_DIRECT3D9


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_GRAPHICS

#endif // __DXGRAPHICSAPIWIN_H__




