//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WindowsOS用BaseImageプログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "../DxCompileConfig.h"

// インクルード ---------------------------------------------------------------
#include "DxBaseImageWin.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

int ( *DefaultImageLoadFunc_PF[] )( STREAMDATA *Src, BASEIMAGE *BaseImage, int GetFormatOnly ) =
{
	NULL
} ;

// 関数宣言 -------------------------------------------------------------------

// プログラム -----------------------------------------------------------------

// 環境依存初期化・終了関数

// 基本イメージ管理情報の環境依存処理の初期化
extern int InitializeBaseImageManage_PF( void )
{
	return 0 ;
}

// 基本イメージ管理情報の環境依存処理の後始末
extern int TerminateBaseImageManage_PF( void )
{
	return 0 ;
}

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

