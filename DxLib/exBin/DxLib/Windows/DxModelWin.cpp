//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用モデルデータ制御プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxModelWin.h"

#ifndef DX_NON_MODEL

// インクルード ---------------------------------------------------------------
#include "DxGraphicsWin.h"
#include "DxGraphicsD3D9.h"
#include "DxGraphicsD3D11.h"
#include "DxModelD3D9.h"
#include "DxModelD3D11.h"
#include "DxWinAPI.h"
#include "DxWindow.h"
#include "../DxMask.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------


// プログラム -----------------------------------------------------------------

// モデル機能の後始末
extern int MV1_Terminate_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_Terminate_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_Terminate_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// モデルデータハンドルの後始末
extern int 	MV1_TerminateModelBaseHandle_PF( MV1_MODEL_BASE *ModelBase )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_TerminateModelBaseHandle_PF( ModelBase ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_TerminateModelBaseHandle_PF( ModelBase ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// トライアングルリストの一時処理用のバッファを開放する
extern int MV1_TerminateTriangleListBaseTempBuffer_PF( MV1_TRIANGLE_LIST_BASE *MBTList )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_TerminateTriangleListBaseTempBuffer_PF( MBTList ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_TerminateTriangleListBaseTempBuffer_PF( MBTList ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 同時複数描画関係の情報をセットアップする
extern void MV1_SetupPackDrawInfo_PF( MV1_MODEL_BASE *ModelBase )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		MV1_D3D11_SetupPackDrawInfo_PF( ModelBase ) ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		MV1_D3D9_SetupPackDrawInfo_PF( ModelBase ) ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}

// 頂点バッファのセットアップをする( -1:エラー )
extern int MV1_SetupVertexBufferBase_PF( int MV1ModelBaseHandle, int DuplicateNum, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_SetupVertexBufferBase_PF( MV1ModelBaseHandle, DuplicateNum, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_SetupVertexBufferBase_PF( MV1ModelBaseHandle, DuplicateNum, ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// モデルデータの頂点バッファのセットアップをする( -1:エラー )
extern int MV1_SetupVertexBuffer_PF( int MHandle, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_SetupVertexBuffer_PF( MHandle, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_SetupVertexBuffer_PF( MHandle, ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 頂点バッファの後始末をする( -1:エラー )
extern int MV1_TerminateVertexBufferBase_PF( int MV1ModelBaseHandle )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_TerminateVertexBufferBase_PF( MV1ModelBaseHandle ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_TerminateVertexBufferBase_PF( MV1ModelBaseHandle ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// 頂点バッファの後始末をする( -1:エラー )
extern int MV1_TerminateVertexBuffer_PF( int MV1ModelHandle )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_TerminateVertexBuffer_PF( MV1ModelHandle ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_TerminateVertexBuffer_PF( MV1ModelHandle ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// シェイプデータのセットアップをする
extern int MV1_SetupShapeVertex_PF( int MHandle )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_SetupShapeVertex_PF( MHandle ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_SetupShapeVertex_PF( MHandle ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ３Ｄモデルのレンダリングの準備を行う
extern int MV1_BeginRender_PF( MV1_MODEL *Model )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_BeginRender_PF( Model ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_BeginRender_PF( Model ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// ３Ｄモデルのレンダリングの後始末を行う
extern int MV1_EndRender_PF( void )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return MV1_D3D11_EndRender_PF() ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return MV1_D3D9_EndRender_PF() ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// メッシュ描画部分を抜き出したもの
extern void MV1_DrawMesh_PF( MV1_MESH *Mesh, int TriangleListIndex )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		MV1_D3D11_DrawMesh_PF( Mesh, TriangleListIndex ) ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		MV1_D3D9_DrawMesh_PF( Mesh, TriangleListIndex ) ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}



#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif  // DX_NON_MODEL
