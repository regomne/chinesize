//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		マスクデータ管理プログラム
// 
//  	Ver 3.20c
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxCompileConfig.h"

#ifndef DX_NON_MASK

// インクルード----------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxMask.h"
#include "DxFont.h"
#include "DxUseCLib.h"
#include "DxFile.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxLog.h"
#include "DxGraphics.h"
#include "DxASyncLoad.h"
#include "DxMemory.h"
#include "DxMath.h"

#ifdef __WINDOWS__
#include "Windows/DxMaskWin.h"
#include "Windows/DxWinAPI.h"
#endif // __WINDOWS__

#ifdef __ANDROID__
#include "Android/DxMaskAndroid.h"
#endif // __ANDROID__

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
		#include "iOS/DxMaskiOS.h"
    #endif // TARGET_OS_IPHONE
#endif // __APPLE__




#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義------------------------------------------------------------------

// マスクハンドルの有効性チェック
#define MASKHCHK( HAND, MPOINT )		HANDLECHK(       DX_HANDLETYPE_GMASK, HAND, *( ( HANDLEINFO ** )&MPOINT ) )
#define MASKHCHK_ASYNC( HAND, MPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_GMASK, HAND, *( ( HANDLEINFO ** )&MPOINT ) )

// 型定義----------------------------------------------------------------------

// データ宣言------------------------------------------------------------------

MASKMANAGEDATA MaskManageData ;

// 関数プロトタイプ宣言 -------------------------------------------------------

// プログラム------------------------------------------------------------------

// マスク処理の初期化
extern int Mask_Initialize( void )
{
	if( MASKD.InitializeFlag == TRUE )
		return -1 ;

	// マスクハンドル管理データの初期化
	InitializeHandleManage( DX_HANDLETYPE_GMASK, sizeof( MASKDATA ), MAX_MASK_NUM, Mask_InitializeHandle, Mask_TerminateHandle, L"Mask" ) ;

	// 初期化フラグを立てる
	MASKD.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// マスク処理の後始末
extern int Mask_Terminate( void )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// マスクスクリーンを削除する
	NS_DeleteMaskScreen() ;

	// 作成したマスクグラフィックを削除
	NS_InitMask() ;

	// 初期化フラグを倒す
	MASKD.InitializeFlag = FALSE ;

	// マスクハンドルの後始末
	TerminateHandleManage( DX_HANDLETYPE_GMASK ) ;

	// 終了
	return 0 ;
}

// マスクデータを初期化する
extern int NS_InitMask( void )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// ハンドルを初期化
	return AllHandleSub( DX_HANDLETYPE_GMASK ) ;
}

// マスクスクリーンを作成する
extern int NS_CreateMaskScreen( void )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	CheckActiveState() ;

	// マスク保存用サーフェスとマスク描画用サーフェスの作成
	Mask_CreateScreenFunction( TRUE, GSYS.Screen.MainScreenSizeX, GSYS.Screen.MainScreenSizeY ) ; 

	// マスクサーフェスが作成されているかフラグをたてる
	MASKD.CreateMaskFlag = TRUE ;
	MASKD.MaskValidFlag = MASKD.MaskUseFlag && MASKD.CreateMaskFlag ;

//	// もしマスク使用フラグが立っていたらマスクを有効にする作業を行う
//	if( MASKD.MaskUseFlag )
		NS_SetUseMaskScreenFlag( TRUE ) ;

	// 終了
	return 0 ;
}

// マスクサーフェスの作成
extern int Mask_CreateSurface( BYTE **MaskBuffer, int *BufferPitch, int Width, int Height, int *TransModeP )
{
	// ピッチの算出
	*BufferPitch = ( Width + 3 ) / 4 * 4 ;

	// バッファの作成
	*MaskBuffer = ( BYTE * )DXALLOC( ( size_t )( *BufferPitch * Height ) ) ;
	if( *MaskBuffer == NULL )
		return -1 ;

	// バッファの初期化
	_MEMSET( *MaskBuffer, 0, ( size_t )( *BufferPitch * Height ) ) ;

	// 透過色モードのセット
	*TransModeP = DX_MASKTRANS_BLACK ;

	// 終了
	return 0 ;
}

// マスクハンドルの初期化
extern int Mask_InitializeHandle( HANDLEINFO * )
{
	// 特に何もせず終了
	return 0 ;
}

// マスクハンドルの後始末
extern int Mask_TerminateHandle( HANDLEINFO *HandleInfo )
{
	MASKDATA *Mask ;

	Mask = ( MASKDATA * )HandleInfo ;

	// マスク用データ領域の解放
	if( Mask->SrcData )
	{
		DXFREE( Mask->SrcData ) ;
		Mask->SrcData = NULL ;
	}

	// 終了
	return 0 ;
}

// MakeMask の実処理関数
static int Mask_MakeMask_Static(
	int MaskHandle,
	int Width,
	int Height,
	int /*ASyncThread*/
)
{
	MASKDATA *Mask ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK_ASYNC( MaskHandle, Mask ) )
		return -1 ;

	// マスク保存用サーフェスの作成
	if( Mask_CreateSurface( &Mask->SrcData, &Mask->SrcDataPitch, Width, Height, &Mask->TransMode ) < 0 )
		return -1 ;

	// データのセット
	Mask->MaskWidth = Width ;
	Mask->MaskHeight = Height ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// MakeMask の非同期読み込みスレッドから呼ばれる関数
static void Mask_MakeMask_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MaskHandle ;
	int Width ;
	int Height ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MaskHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Width = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Height = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = Mask_MakeMask_Static( MaskHandle, Width, Height, TRUE ) ;

	DecASyncLoadCount( MaskHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MaskHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// マスクデータの追加
extern int Mask_MakeMask_UseGParam( int Width, int Height, int ASyncLoadFlag )
{
	int MaskHandle ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	CheckActiveState() ;

	// ハンドルの作成
	MaskHandle = AddHandle( DX_HANDLETYPE_GMASK, FALSE, -1 ) ;
	if( MaskHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MaskHandle ) ;
		AddASyncLoadParamInt( NULL, &Addr, Width ) ;
		AddASyncLoadParamInt( NULL, &Addr, Height ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = Mask_MakeMask_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MaskHandle ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Width ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Height ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( MaskHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( Mask_MakeMask_Static( MaskHandle, Width, Height, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return MaskHandle ;

ERR :
	SubHandle( MaskHandle ) ;

	return -1 ;
}

// マスクデータの追加
extern int NS_MakeMask( int Width, int Height )
{
	return Mask_MakeMask_UseGParam( Width, Height, GetASyncLoadFlag() ) ;
}

// マスクデータを削除
extern int NS_DeleteMask( int MaskHandle )
{
	return SubHandle( MaskHandle ) ;
}

#ifdef __WINDOWS__

// マスクデータサーフェスにＢＭＰデータをマスクデータと見たてて転送
extern int NS_BmpBltToMask( HBITMAP Bmp, int BmpPointX, int BmpPointY, int MaskHandle )
{
	MASKDATA * MaskData ;
	BITMAP bm ;
	int ColorBitDepth ;

	SETUP_WIN_API
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK_ASYNC( MaskHandle, MaskData ) )
		return -1 ;

	// ビットマップデータの取得
	WinAPIData.Win32Func.GetObjectAFunc( Bmp, sizeof( BITMAP ), ( void * )&bm ) ;
	bm.bmWidthBytes += bm.bmWidthBytes % 4 ? 4 - bm.bmWidthBytes % 4 : 0 ;

	// 転送処理
	ColorBitDepth = NS_GetScreenBitDepth() ;
	{
		BYTE *SrcPoint, *DestPoint ;
		int SrcAddIndex, DestAddIndex ;
		int DestHeight, DestWidth ;

		SrcPoint = ( BYTE *)bm.bmBits + BmpPointX * bm.bmBitsPixel / 8 + BmpPointY * bm.bmWidthBytes ;
		DestPoint = ( BYTE * )MaskData->SrcData ;

		SrcAddIndex = bm.bmWidthBytes - MaskData->MaskWidth * bm.bmBitsPixel / 8 ;
		DestAddIndex = MaskData->SrcDataPitch - MaskData->MaskWidth ;

		DestHeight = MaskData->MaskHeight ;
		DestWidth = MaskData->MaskWidth ;

#ifdef DX_NON_INLINE_ASM
		int i ;
		do
		{
			i = DestWidth ;
			do
			{
				*DestPoint = *SrcPoint ;
				DestPoint ++ ;
				SrcPoint += 3 ;
			}while( -- i != 0 ) ;
			DestPoint += DestAddIndex ;
			SrcPoint += SrcAddIndex ;
		}while( -- DestHeight != 0 ) ;
#else
		__asm{
			PUSH	EDI
			PUSH	ESI
			PUSHF
			CLD
			MOV		EDI, DestPoint
			MOV		ESI, SrcPoint
			MOV		EDX, DestHeight
			MOV		EBX, DestWidth
		LOOP81:
			MOV		ECX, EBX
		LOOP82:
			MOV		AL	, [ESI]
			MOV		[EDI], AL
			INC		EDI
			ADD		ESI, 3
			LOOP	LOOP82

			ADD		ESI, SrcAddIndex
			ADD		EDI, DestAddIndex
			DEC		EDX
			JNZ		LOOP81

			POPF
			POP		ESI
			POP		EDI
		}
#endif
	}

	// 終了
	return 0 ;
}

#endif // __WINDOWS_

// マスクハンドルにBASEIMAGEデータを転送する
extern int NS_GraphImageBltToMask( const BASEIMAGE *BaseImage, int ImageX, int ImageY, int MaskHandle )
{
	MASKDATA * MaskData ;
	int       UseTempBaseImage = FALSE ;
	BASEIMAGE TempBaseImage ;
//	int       ColorBitDepth ;
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK_ASYNC( MaskHandle, MaskData ) )
		return -1 ;

	// BASEIMAGE がフルカラー( 24bit )でも32bitカラーでもなかった場合はではなかった場合はフルカラー画像に変換
	if( BaseImage->ColorData.Format        != DX_BASEIMAGE_FORMAT_NORMAL ||
		BaseImage->ColorData.FloatTypeFlag != FALSE ||
		( BaseImage->ColorData.ColorBitDepth != 24 &&
		  BaseImage->ColorData.ColorBitDepth != 32 ) ||
		BaseImage->ColorData.RedMask       != 0xff0000 ||
		BaseImage->ColorData.GreenMask     != 0x00ff00 ||
		BaseImage->ColorData.BlueMask      != 0x0000ff )
	{
		UseTempBaseImage = TRUE ;
		NS_CreateRGB8ColorBaseImage( BaseImage->Width, BaseImage->Height, &TempBaseImage ) ;
		NS_BltBaseImage( 0, 0, ( BASEIMAGE * )BaseImage, &TempBaseImage ) ;
		BaseImage = &TempBaseImage ;
	}

	// 転送処理
//	ColorBitDepth = NS_GetScreenBitDepth() ;
	{
		BYTE *SrcPoint,   *DestPoint ;
		int   SrcAddIndex, DestAddIndex ;
		int   DestHeight,  DestWidth ;

		SrcPoint  = ( BYTE * )BaseImage->GraphData + ImageX * BaseImage->ColorData.PixelByte + ImageY * BaseImage->Pitch ;
		DestPoint = ( BYTE * )MaskData->SrcData ;

		SrcAddIndex  = BaseImage->Pitch - MaskData->MaskWidth * BaseImage->ColorData.PixelByte ;
		DestAddIndex = MaskData->SrcDataPitch - MaskData->MaskWidth ;

		DestHeight = MaskData->MaskHeight ;
		DestWidth  = MaskData->MaskWidth ;

#ifdef DX_NON_INLINE_ASM
		int i ;
		if( BaseImage->ColorData.ColorBitDepth == 24 )
		{
			do
			{
				i = DestWidth ;
				do
				{
					*DestPoint = *SrcPoint ;
					DestPoint ++ ;
					SrcPoint += 3 ;
				}while( -- i != 0 ) ;
				DestPoint += DestAddIndex ;
				SrcPoint  += SrcAddIndex ;
			}while( -- DestHeight != 0 ) ;
		}
		else
		{
			do
			{
				i = DestWidth ;
				do
				{
					*DestPoint = *SrcPoint ;
					DestPoint ++ ;
					SrcPoint += 4 ;
				}while( -- i != 0 ) ;
				DestPoint += DestAddIndex ;
				SrcPoint  += SrcAddIndex ;
			}while( -- DestHeight != 0 ) ;
		}
#else
		if( BaseImage->ColorData.ColorBitDepth == 24 )
		{
			__asm{
				PUSH	EDI
				PUSH	ESI
				PUSHF
				CLD
				MOV		EDI, DestPoint
				MOV		ESI, SrcPoint
				MOV		EDX, DestHeight
				MOV		EBX, DestWidth
			LOOP81_24:
				MOV		ECX, EBX
			LOOP82_24:
				MOV		AL	, [ESI]
				MOV		[EDI], AL
				INC		EDI
				ADD		ESI, 3
				LOOP	LOOP82_24

				ADD		ESI, SrcAddIndex
				ADD		EDI, DestAddIndex
				DEC		EDX
				JNZ		LOOP81_24

				POPF
				POP		ESI
				POP		EDI
			}
		}
		else
		{
			__asm{
				PUSH	EDI
				PUSH	ESI
				PUSHF
				CLD
				MOV		EDI, DestPoint
				MOV		ESI, SrcPoint
				MOV		EDX, DestHeight
				MOV		EBX, DestWidth
			LOOP81_32:
				MOV		ECX, EBX
			LOOP82_32:
				MOV		AL	, [ESI]
				MOV		[EDI], AL
				INC		EDI
				ADD		ESI, 4
				LOOP	LOOP82_32

				ADD		ESI, SrcAddIndex
				ADD		EDI, DestAddIndex
				DEC		EDX
				JNZ		LOOP81_32

				POPF
				POP		ESI
				POP		EDI
			}
		}
#endif
	}

	// フルカラー画像に変換したものを使用した場合はフルカラー画像を解放する
	if( UseTempBaseImage )
	{
		NS_ReleaseBaseImage( &TempBaseImage ) ;
	}

	// 終了
	return 0 ;
}

// マスクの大きさを得る 
extern int NS_GetMaskSize( int *WidthBuf, int *HeightBuf, int MaskHandle )
{
	MASKDATA * Mask ;
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK( MaskHandle, Mask ) )
		return -1 ;

	*WidthBuf = Mask->MaskWidth ;
	*HeightBuf = Mask->MaskHeight ;

	// 終了
	return 0 ;
}

// LoadMask の実処理関数
static int Mask_LoadMask_Static(
	int MaskHandle,
	const wchar_t *FileName,
	int /*ASyncThread*/
)
{
	BASEIMAGE BaseImage ;
	MASKDATA * Mask ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK_ASYNC( MaskHandle, Mask ) )
		return -1 ;

	// 画像の読み込みを試みる
	if( CreateGraphImageOrDIBGraph_WCHAR_T( FileName, NULL, 0, LOADIMAGE_TYPE_FILE, FALSE, FALSE, FALSE, &BaseImage, NULL, NULL ) < 0 )
	{
		return -1 ;
	}

	// 通常フォーマットではなかったら通常フォーマットに変換
	if( BaseImage.ColorData.Format != DX_BASEIMAGE_FORMAT_NORMAL )
	{
		NS_ConvertNormalFormatBaseImage( &BaseImage ) ;
	}

	// マスク保存用サーフェスの作成
	if( Mask_CreateSurface( &Mask->SrcData, &Mask->SrcDataPitch, BaseImage.Width, BaseImage.Height, &Mask->TransMode ) < 0 )
	{
		NS_ReleaseBaseImage( &BaseImage ) ;
		return -1 ;
	}

	// データのセット
	Mask->MaskWidth  = BaseImage.Width ;
	Mask->MaskHeight = BaseImage.Height ;

	// マスクデータを転送
	NS_GraphImageBltToMask( &BaseImage, 0, 0, MaskHandle ) ;

	// 画像を解放
	NS_ReleaseBaseImage( &BaseImage ) ;

	// 終了
	return MaskHandle ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMask の非同期読み込みスレッドから呼ばれる関数
static void Mask_LoadMask_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MaskHandle ;
	const wchar_t *FileName ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MaskHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = Mask_LoadMask_Static( MaskHandle, FileName, TRUE ) ;

	DecASyncLoadCount( MaskHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MaskHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// マスクデータを画像ファイルから読み込む
extern int Mask_LoadMask_UseGParam( const wchar_t *FileName, int ASyncLoadFlag )
{
	int MaskHandle ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	CheckActiveState() ;

	// ハンドルの作成
	MaskHandle = AddHandle( DX_HANDLETYPE_GMASK, FALSE, -1 ) ;
	if( MaskHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MaskHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = Mask_LoadMask_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MaskHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( MaskHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( Mask_LoadMask_Static( MaskHandle, FileName, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return MaskHandle ;

ERR :
	SubHandle( MaskHandle ) ;

	return -1 ;
}

// マスクデータをロードする
extern int NS_LoadMask( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadMask_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadMask_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルを読み込みマスクハンドルを作成する
extern int NS_LoadMaskWithStrLen( const TCHAR *FileName, size_t FileNameLength )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadMask_WCHAR_T( UseFileNameBuffer ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadMask_WCHAR_T( UseFileNameBuffer ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// マスクデータをロードする
extern int LoadMask_WCHAR_T( const wchar_t *FileName )
{
	return Mask_LoadMask_UseGParam( FileName, GetASyncLoadFlag() ) ;
}

// LoadDivMask の実処理関数
static int Mask_LoadDivMask_Static(
	const wchar_t *FileName,
	int AllNum,
	int XNum,
	int YNum,
	int XSize,
	int YSize,
	int *HandleArray,
	int /*ASyncThread*/
)
{
	MASKDATA * Mask ;
	BASEIMAGE BaseImage ;
	int i, j, k ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// 画像の読み込みを試みる
	if( CreateGraphImageOrDIBGraph_WCHAR_T( FileName, NULL, 0, LOADIMAGE_TYPE_FILE, FALSE, FALSE, FALSE, &BaseImage, NULL, NULL ) < 0 )
	{
		return -1 ;
	}

	// サイズ確認
	if( XNum * XSize > BaseImage.Width || YNum * YSize > BaseImage.Height )
		goto ERR ;

	// 分割転送開始
	k = 0 ;
	for( i = 0 ; k != AllNum && i < YNum ; i ++ )
	{
		for( j = 0 ; k != AllNum && j < XNum ; j ++, k ++ )
		{
			if( MASKHCHK_ASYNC( HandleArray[ k ], Mask ) )
				goto ERR ;

			// マスクの作成
			if( Mask_CreateSurface( &Mask->SrcData, &Mask->SrcDataPitch, XSize, YSize, &Mask->TransMode ) < 0 )
				goto ERR ;

			// データのセット
			Mask->MaskWidth = XSize ;
			Mask->MaskHeight = YSize ;

			// マスクデータの転送
			NS_GraphImageBltToMask( &BaseImage, XSize * j, YSize * i, HandleArray[ k ] ) ;
		}
	}

	// 画像を解放
	NS_ReleaseBaseImage( &BaseImage ) ;

	// 終了
	return 0 ;

	// エラー終了
ERR :
	// 画像を解放
	NS_ReleaseBaseImage( &BaseImage ) ;
	
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadDivMask の非同期読み込みスレッドから呼ばれる関数
static void Mask_LoadDivMask_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	const wchar_t *FileName ;
	int AllNum ;
	int XNum ;
	int YNum ;
	int XSize ;
	int YSize ;
	int *HandleArray ;
	int Addr ;
	int i ;
	int Result ;

	Addr = 0 ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	AllNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	XNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	YNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	XSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	YSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	HandleArray = ( int * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;

	Result = Mask_LoadDivMask_Static( FileName, AllNum, XNum, YNum, XSize, YSize, HandleArray, TRUE ) ;

	for( i = 0 ; i < AllNum ; i ++ )
		DecASyncLoadCount( HandleArray[ i ] ) ;

	if( Result < 0 )
	{
		for( i = 0 ; i < AllNum ; i ++ )
			NS_DeleteMask( HandleArray[ i ] ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// マスクを画像ファイルから分割読みこみ
extern int Mask_LoadDivMask_UseGParam(
	const wchar_t *FileName,
	int AllNum,
	int XNum,
	int YNum,
	int XSize,
	int YSize,
	int *HandleArray,
	int ASyncLoadFlag
)
{
	int i ;

	CheckActiveState() ;

	if( AllNum == 0 )
		return -1 ;

	// グラフィックハンドルの作成
	_MEMSET( HandleArray, 0, sizeof( int ) * AllNum ) ;
	for( i = 0 ; i < AllNum ; i ++ )
	{
		HandleArray[ i ] = AddHandle( DX_HANDLETYPE_GMASK, FALSE, -1 ) ;
		if( HandleArray[ i ] < 0 )
		{
			goto ERR ;
		}
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FileName, FullPath, sizeof( FullPath ) ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ; 
		AddASyncLoadParamInt( NULL, &Addr, AllNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, XNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, YNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, XSize ) ;
		AddASyncLoadParamInt( NULL, &Addr, YSize ) ;
		AddASyncLoadParamStruct( NULL, &Addr, HandleArray, ( int )( sizeof( int ) * AllNum ) ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = Mask_LoadDivMask_ASync ;
		Addr = 0 ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, AllNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, XNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, YNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, XSize ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, YSize ) ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, HandleArray, ( int )( sizeof( int ) * AllNum ) ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		for( i = 0 ; i < AllNum ; i ++ )
			IncASyncLoadCount( HandleArray[ i ], AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( Mask_LoadDivMask_Static( FileName, AllNum, XNum, YNum, XSize, YSize, HandleArray, FALSE ) < 0 )
			goto ERR ;
	}

	// 正常終了
	return 0 ;

ERR :
	for( i = 0 ; i < AllNum ; i ++ )
	{
		NS_DeleteMask( HandleArray[ i ] ) ;
	}

	return -1 ;
}

// マスクを画像から分割読みこみ
extern int NS_LoadDivMask( const TCHAR *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
#ifdef UNICODE
	return LoadDivMask_WCHAR_T(
		FileName, AllNum, XNum, YNum, XSize, YSize, HandleArray
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadDivMask_WCHAR_T(
		UseFileNameBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// 画像ファイルを分割読み込みしてマスクハンドルを作成する
extern int NS_LoadDivMaskWithStrLen( const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadDivMask_WCHAR_T( UseFileNameBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, FileNameLength, return -1 )
	Result = LoadDivMask_WCHAR_T( UseFileNameBuffer, AllNum, XNum, YNum, XSize, YSize, HandleArray ) ;
	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( FileName )
#endif
	return Result ;
}

// マスクを画像から分割読みこみ
extern int LoadDivMask_WCHAR_T( const wchar_t *FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	return Mask_LoadDivMask_UseGParam( FileName, AllNum, XNum, YNum, XSize, YSize, HandleArray, GetASyncLoadFlag() ) ;
}












// CreateMaskFromMem の実処理関数
static int Mask_CreateMaskFromMem_Static(
	int         MaskHandle,
	const void *FileImage,
	int         FileImageSize,
	int       /*ASyncThread*/
)
{
	BASEIMAGE BaseImage ;
	MASKDATA * Mask ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK_ASYNC( MaskHandle, Mask ) )
		return -1 ;

	// ロードを試みる
	if( NS_CreateBaseImage( NULL, FileImage, FileImageSize, LOADIMAGE_TYPE_MEM, &BaseImage, FALSE ) < 0 )
	{
		return -1 ;
	}
	
	// マスク保存用サーフェスの作成
	if( Mask_CreateSurface( &Mask->SrcData, &Mask->SrcDataPitch, BaseImage.Width, BaseImage.Height, &Mask->TransMode ) < 0 )
	{
		NS_ReleaseBaseImage( &BaseImage ) ;
		return -1 ;
	}

	// データのセット
	Mask->MaskWidth  = BaseImage.Width ;
	Mask->MaskHeight = BaseImage.Height ;

	// マスクデータを転送
	NS_GraphImageBltToMask( &BaseImage, 0, 0, MaskHandle ) ;

	// BASEIMAGE を解放
	NS_ReleaseBaseImage( &BaseImage ) ;

	// 終了
	return MaskHandle ;
}

#ifndef DX_NON_ASYNCLOAD
// CreateMaskFromMem の非同期読み込みスレッドから呼ばれる関数
static void Mask_CreateMaskFromMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int         MaskHandle ;
	const void *FileImage ;
	int         FileImageSize ;
	int         Addr ;
	int         Result ;

	Addr = 0 ;
	MaskHandle    = GetASyncLoadParamInt(   AParam->Data, &Addr ) ;
	FileImage     = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt(   AParam->Data, &Addr ) ;

	Result = Mask_CreateMaskFromMem_Static( MaskHandle, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( MaskHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MaskHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// マスクデータをメモリ上の画像ファイルイメージから読み込む
extern int Mask_CreateMaskFromMem_UseGParam( const void *FileImage, int FileImageSize, int ASyncLoadFlag )
{
	int MaskHandle ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	CheckActiveState() ;

	// ハンドルの作成
	MaskHandle = AddHandle( DX_HANDLETYPE_GMASK, FALSE, -1 ) ;
	if( MaskHandle == -1 )
	{
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt(        NULL, &Addr, MaskHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt(        NULL, &Addr, FileImageSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = Mask_CreateMaskFromMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, MaskHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImage ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, FileImageSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( MaskHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( Mask_CreateMaskFromMem_Static( MaskHandle, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return MaskHandle ;

ERR :
	SubHandle( MaskHandle ) ;

	return -1 ;
}

// メモリ上にある画像ファイルイメージを読み込みマスクハンドルを作成する
extern int NS_CreateMaskFromMem( const void *FileImage, int FileImageSize )
{
	return Mask_CreateMaskFromMem_UseGParam( FileImage, FileImageSize, GetASyncLoadFlag() ) ;
}












// CreateDivMaskFromMem の実処理関数
static int Mask_CreateDivMaskFromMem_Static(
	const void *FileImage,
	int FileImageSize,
	int AllNum,
	int XNum,
	int YNum,
	int XSize,
	int YSize,
	int *HandleArray,
	int /*ASyncThread*/
)
{
	BASEIMAGE BaseImage ;
	MASKDATA * Mask ;
	int i ;
	int j ;
	int k ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// ロードを試みる
	if( NS_CreateBaseImage( NULL, FileImage, FileImageSize, LOADIMAGE_TYPE_MEM, &BaseImage, FALSE ) < 0 )
	{
		return -1 ;
	}

	// サイズ確認
	if( XNum * XSize > BaseImage.Width || YNum * YSize > BaseImage.Height )
		goto ERR ;

	// 分割転送開始
	k = 0 ;
	for( i = 0 ; k != AllNum && i < YNum ; i ++ )
	{
		for( j = 0 ; k != AllNum && j < XNum ; j ++, k ++ )
		{
			if( MASKHCHK_ASYNC( HandleArray[ k ], Mask ) )
				goto ERR ;

			// マスクの作成
			if( Mask_CreateSurface( &Mask->SrcData, &Mask->SrcDataPitch, XSize, YSize, &Mask->TransMode ) < 0 )
				goto ERR ;

			// データのセット
			Mask->MaskWidth = XSize ;
			Mask->MaskHeight = YSize ;

			// マスクデータの転送
			NS_GraphImageBltToMask( &BaseImage, XSize * j, YSize * i, HandleArray[ k ] ) ;
		}
	}

	// BASEIMAGE を解放
	NS_ReleaseBaseImage( &BaseImage ) ;

	// 終了
	return 0 ;

	// エラー終了
ERR :
	// BASEIMAGE を解放
	NS_ReleaseBaseImage( &BaseImage ) ;
	
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// CreateDivMaskFromMem の非同期読み込みスレッドから呼ばれる関数
static void Mask_CreateDivMaskFromMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	const void *FileImage ;
	int         FileImageSize ;
	int         AllNum ;
	int         XNum ;
	int         YNum ;
	int         XSize ;
	int         YSize ;
	int        *HandleArray ;
	int         Addr ;
	int         i ;
	int         Result ;

	Addr = 0 ;
	FileImage     =          GetASyncLoadParamVoidP(  AParam->Data, &Addr ) ;
	FileImageSize =          GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	AllNum        =          GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	XNum          =          GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	YNum          =          GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	XSize         =          GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	YSize         =          GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	HandleArray     = ( int * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;

	Result = Mask_CreateDivMaskFromMem_Static( FileImage, FileImageSize, AllNum, XNum, YNum, XSize, YSize, HandleArray, TRUE ) ;

	for( i = 0 ; i < AllNum ; i ++ )
		DecASyncLoadCount( HandleArray[ i ] ) ;

	if( Result < 0 )
	{
		for( i = 0 ; i < AllNum ; i ++ )
			NS_DeleteMask( HandleArray[ i ] ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// マスクをメモリ上の画像ファイルイメージから分割読みこみ
extern int Mask_CreateDivMaskFromMem_UseGParam(
	const void *FileImage,
	int FileImageSize,
	int AllNum,
	int XNum,
	int YNum,
	int XSize,
	int YSize,
	int *HandleArray,
	int ASyncLoadFlag
)
{
	int i ;

	CheckActiveState() ;

	if( AllNum == 0 )
		return -1 ;

	// グラフィックハンドルの作成
	_MEMSET( HandleArray, 0, sizeof( int ) * AllNum ) ;
	for( i = 0 ; i < AllNum ; i ++ )
	{
		HandleArray[ i ] = AddHandle( DX_HANDLETYPE_GMASK, FALSE, -1 ) ;
		if( HandleArray[ i ] < 0 )
		{
			goto ERR ;
		}
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt(        NULL, &Addr, FileImageSize ) ;
		AddASyncLoadParamInt(        NULL, &Addr, AllNum ) ;
		AddASyncLoadParamInt(        NULL, &Addr, XNum ) ;
		AddASyncLoadParamInt(        NULL, &Addr, YNum ) ;
		AddASyncLoadParamInt(        NULL, &Addr, XSize ) ;
		AddASyncLoadParamInt(        NULL, &Addr, YSize ) ;
		AddASyncLoadParamStruct(     NULL, &Addr, HandleArray, ( int )( sizeof( int ) * AllNum ) ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = Mask_CreateDivMaskFromMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImage ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, FileImageSize ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, AllNum ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, XNum ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, YNum ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, XSize ) ;
		AddASyncLoadParamInt(        AParam->Data, &Addr, YSize ) ;
		AddASyncLoadParamStruct(     AParam->Data, &Addr, HandleArray, ( int )( sizeof( int ) * AllNum ) ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		for( i = 0 ; i < AllNum ; i ++ )
			IncASyncLoadCount( HandleArray[ i ], AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( Mask_CreateDivMaskFromMem_Static( FileImage, FileImageSize, AllNum, XNum, YNum, XSize, YSize, HandleArray, FALSE ) < 0 )
			goto ERR ;
	}

	// 正常終了
	return 0 ;

ERR :
	for( i = 0 ; i < AllNum ; i ++ )
	{
		NS_DeleteMask( HandleArray[ i ] ) ;
	}

	return -1 ;
}

// メモリ上にある画像ファイルイメージを分割読み込みしてマスクハンドルを作成する
extern int NS_CreateDivMaskFromMem( const void *FileImage, int FileImageSize, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray )
{
	return Mask_CreateDivMaskFromMem_UseGParam( FileImage, FileImageSize, AllNum, XNum, YNum, XSize, YSize, HandleArray, GetASyncLoadFlag() ) ;
}












#ifndef DX_NON_FONT

// 書式指定ありの文字列をマスクスクリーンに描画する
extern int NS_DrawFormatStringMask( int x, int y, int Flag, const TCHAR *FormatString, ... )
{
	va_list VaList ;
	TCHAR String[ 2048 ] ;
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// 編集後の文字列を取得する
	{
		va_start( VaList, FormatString ) ;
		_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
		va_end( VaList ) ;
	}

	return NS_DrawStringMaskToHandle( x, y, Flag, NS_GetDefaultFontHandle(), String ) ;
}

// 書式指定ありの文字列をマスクスクリーンに描画する
extern int DrawFormatStringMask_WCHAR_T( int x, int y, int Flag, const wchar_t *FormatString, ... )
{
	va_list VaList ;
	wchar_t String[ 1024 ] ;
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// 編集後の文字列を取得する
	{
		va_start( VaList, FormatString ) ;
		_VSWNPRINTF( String, sizeof( String ) / 2, FormatString, VaList ) ;
		va_end( VaList ) ;
	}

	return DrawStringMaskToHandle_WCHAR_T( x, y, Flag, NS_GetDefaultFontHandle(), String, -1 ) ;
}

// 書式指定ありの文字列をマスクスクリーンに描画する
extern int NS_DrawFormatStringMaskToHandle( int x, int y, int Flag, int FontHandle, const TCHAR *FormatString, ... )
{
	va_list VaList ;
	TCHAR String[ 2048 ] ;
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// 編集後の文字列を取得する
	{
		va_start( VaList, FormatString ) ;
		_TVSNPRINTF( String, sizeof( String ) / sizeof( TCHAR ), FormatString, VaList ) ;
		va_end( VaList ) ;
	}

	return NS_DrawStringMaskToHandle( x, y, Flag, FontHandle, String ) ;
}

// 書式指定ありの文字列をマスクスクリーンに描画する
extern int DrawFormatStringMaskToHandle_WCHAR_T( int x, int y, int Flag, int FontHandle, const wchar_t *FormatString, ... )
{
	va_list VaList ;
	wchar_t String[ 2048 ] ;
	
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// 編集後の文字列を取得する
	{
		va_start( VaList, FormatString ) ;
		_VSWNPRINTF( String, sizeof( String ) / 2, FormatString, VaList ) ;
		va_end( VaList ) ;
	}

	return DrawStringMaskToHandle_WCHAR_T( x, y, Flag, FontHandle, String, -1 ) ;
}

// 文字列をマスクスクリーンに描画する(フォントハンドル指定版)
extern int NS_DrawStringMaskToHandle( int x, int y, int Flag, int FontHandle, const TCHAR *String )
{
#ifdef UNICODE
	return DrawStringMaskToHandle_WCHAR_T(
		x, y, Flag, FontHandle, String, -1
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( String, return -1 )

	Result = DrawStringMaskToHandle_WCHAR_T(
		x, y, Flag, FontHandle, UseStringBuffer, -1
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// 文字列をマスクスクリーンに描画する( フォントハンドル指定版 )( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
extern int NS_DrawNStringMaskToHandle( int x, int y, int Flag, int FontHandle, const TCHAR *String, size_t StringLength )
{
#ifdef UNICODE
	return DrawStringMaskToHandle_WCHAR_T(
		x, y, Flag, FontHandle, String, ( int )StringLength
	) ;
#else
	int Result ;

	TCHAR_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( String, StringLength, return -1 )

	Result = DrawStringMaskToHandle_WCHAR_T(
		x, y, Flag, FontHandle, UseStringBuffer, ( int )wchar_StringLength
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( String )

	return Result ;
#endif
}

// 文字列をマスクスクリーンに描画する(フォントハンドル指定版)
extern int DrawStringMaskToHandle_WCHAR_T( int x, int y, int Flag, int FontHandle, const wchar_t *String, int StrLen )
{
	BASEIMAGE BaseImage ;
	unsigned int Color ;
	int Result ;
	SIZE DrawSize ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// マスクスクリーンが作成されていなかったらエラー
	if( MASKD.MaskBuffer == NULL )
		return -1 ;

	RefreshDefaultFont() ;

	// BASEIMAGEデータの作成
	NS_CreatePaletteColorData( &BaseImage.ColorData ) ;
	NS_GetDrawScreenSize( &BaseImage.Width, &BaseImage.Height ) ;
	BaseImage.Pitch = MASKD.MaskBufferPitch ;
	BaseImage.GraphData = MASKD.MaskBuffer ;

	// マスクスクリーン上に文字を描画
	Color = ( DWORD )( Flag == 0 ? 0 : 0xff ) ;
	Result = FontCacheStringDrawToHandle_WCHAR_T(
		x, y, String, StrLen, Color, Color, 
		&BaseImage, &GSYS.DrawSetting.DrawArea, FontHandle,
		FALSE, &DrawSize
	) ;

	RECT Rect ;

	if( GSYS.Setting.ValidHardware )
	{
		// 更新
		Rect.left   = x ;
		Rect.top    = y ;
		Rect.right  = x + DrawSize.cx ;
		Rect.bottom = y + DrawSize.cy ;
		Mask_UpdateMaskImageTexture_PF( &Rect ) ;
	}

	return Result ;
}

// 文字列をマスクスクリーンに描画する
extern int NS_DrawStringMask( int x, int y, int Flag, const TCHAR *String )
{
	return NS_DrawStringMaskToHandle( x, y, Flag, NS_GetDefaultFontHandle(), String ) ;
}

// 文字列をマスクスクリーンに描画する
extern int NS_DrawNStringMask( int x, int y, int Flag, const TCHAR *String, size_t StringLength )
{
	return NS_DrawNStringMaskToHandle( x, y, Flag, NS_GetDefaultFontHandle(), String, StringLength ) ;
}

// 文字列をマスクスクリーンに描画する
extern int DrawStringMask_WCHAR_T( int x, int y, int Flag, const wchar_t *String, int StrLen )
{
	return DrawStringMaskToHandle_WCHAR_T( x, y, Flag, NS_GetDefaultFontHandle(), String, StrLen ) ;
}

#endif // DX_NON_FONT

// マスクスクリーンを使用中かフラグの取得
extern int NS_GetMaskUseFlag( void )
{
	return MASKD.MaskUseFlag ;
}

// マスクの数値に対する効果を逆転させる
extern int NS_SetMaskReverseEffectFlag( int ReverseFlag )
{
	MASKD.MaskReverseEffectFlag = ReverseFlag ;

	// 終了
	return 0 ;
}

// マスク使用モードの取得
extern int NS_GetUseMaskScreenFlag( void )
{
	return MASKD.MaskUseFlag ;
}

// マスクスクリーンを復旧させる
extern int Mask_ReCreateSurface( void )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// メインのマスクサーフェスを作成する
	Mask_CreateScreenFunction( MASKD.CreateMaskFlag, GSYS.DrawSetting.DrawSizeX, GSYS.DrawSetting.DrawSizeY ) ;

	// もしマスク使用フラグが立っていたらマスクを有効にする作業を行う
	if( MASKD.MaskUseFlag )
		NS_SetUseMaskScreenFlag( TRUE ) ;

	// 終了
	return 0 ;
}


// マスクパターンの展開
extern int DrawMaskToDirectData_Base( int DrawPointX, int DrawPointY, void *DestBufP, int DestPitch, 
											int SrcWidth, int SrcHeight, const void *SrcData, int TransMode  )
{
	BYTE *DestP ,*SrcP ;
	BYTE TransColor ;
	int DestAddPoint ;
	int SrcAddPoint ;
	int BltWidth, BltHeight ;
	RECT Rect ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKD.MaskBuffer == NULL ) return 0 ;

	SETRECT( Rect, DrawPointX, DrawPointY, DrawPointX + SrcWidth, DrawPointY + SrcHeight ) ;
	RectClipping_Inline( &Rect, &GSYS.DrawSetting.DrawArea ) ;
	if( Rect.left == Rect.right || Rect.top == Rect.bottom ) return 0 ;
	Rect.left -= DrawPointX ;
	Rect.right -= DrawPointX ;
	Rect.top -= DrawPointY ;
	Rect.bottom -= DrawPointY ;
	if( Rect.left > 0 ) DrawPointX += Rect.left ;
	if( Rect.top > 0 ) DrawPointY += Rect.top ;

	// 転送準備
	BltWidth = Rect.right - Rect.left ;
	BltHeight = Rect.bottom - Rect.top ;
	SrcP = ( BYTE * )SrcData + Rect.left + Rect.top * SrcWidth ;
	SrcAddPoint = SrcWidth - BltWidth ;

	DestP = ( BYTE * )DestBufP + DrawPointX + DrawPointY * DestPitch ;
	DestAddPoint = DestPitch - BltWidth ;

	TransColor = ( BYTE )( TransMode == DX_MASKTRANS_WHITE ? 0xff : 0 ) ;

	// 透過色があるかどうかで処理を分岐
	if( TransMode == DX_MASKTRANS_NONE )
	{
#ifdef DX_NON_INLINE_ASM
		int i ;
		do
		{
			i = BltWidth ;

			do
			{
				*DestP = *SrcP ;
				DestP ++ ;
				SrcP ++ ;
			}while( -- i != 0 ) ;

			DestP += DestAddPoint ;
			SrcP += SrcAddPoint ;

		}while( -- BltHeight != 0 ) ;
#else
		// 透過色ないバージョン
		__asm{
			CLD
			MOV		EDI, DestP
			MOV		ESI, SrcP
			MOV		EDX, BltWidth
			MOV		EBX, BltHeight
			MOV		EAX, SrcAddPoint
			MOV		ECX, DestAddPoint
			PUSH	EBP
			MOV		EBP, ECX
		LOOP811:
			MOV		ECX, EDX
			REP		MOVSB
			ADD		EDI, EBP
			ADD		ESI, EAX
			DEC		EBX
			JNZ		LOOP811
			POP		EBP
		}
#endif
	}
	else
	{
		// 透過色ありバージョン
		
#ifdef DX_NON_INLINE_ASM
		int i ;
		TransColor &= 0xff ;
		do
		{
			i = BltWidth ;

			do
			{
				if( *SrcP != TransColor )
					*DestP = *SrcP ;
				DestP ++ ;
				SrcP ++ ;
			}while( -- i != 0 ) ;

			DestP += DestAddPoint ;
			SrcP += SrcAddPoint ;

		}while( -- BltHeight != 0 ) ;
#else
		__asm{
			MOV		EDI, DestP
			MOV		ESI, SrcP
			MOV		BL, TransColor
	LOOP821:
			MOV		ECX, BltWidth
	LOOP822:
			MOV		AL, [ESI]
			TEST	AL, BL
			JZ		R821
			MOV		[EDI], AL
	R821:
			INC		EDI
			INC		ESI
			LOOP	LOOP822

			ADD		EDI, DestAddPoint
			ADD		ESI, SrcAddPoint
			DEC		BltHeight
			JNZ		LOOP821
		}
#endif
	}

	// 終了
	return 0 ;
}

// マスクのデータを直接マスク画面全体に描画する
extern int NS_DrawFillMaskToDirectData( int x1, int y1, int x2, int y2,  int Width, int Height, const void *MaskData )
{
	int MaskHandle ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	CheckActiveState() ;

	// 空のマスクを作成
	if( ( MaskHandle = NS_MakeMask( Width, Height ) ) == -1 ) return -1 ;

	// マスクにデータをセット
	NS_SetDataToMask( Width, Height, MaskData, MaskHandle ) ;

	// マスクを画面全体に描画
	NS_DrawFillMask( x1, y1, x2, y2, MaskHandle ) ;

	// マスクハンドルを削除
	NS_DeleteMask( MaskHandle ) ;

	// 終了
	return 0 ;
}

// マスクのデータをマスクに転送する
extern int NS_SetDataToMask( int Width, int Height, const void *MaskData, int MaskHandle )
{
	MASKDATA * Mask ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK( MaskHandle, Mask ) )
		return -1 ;
	
	if( Mask->MaskWidth != Width || Mask->MaskHeight != Height ) return -1 ;

	CheckActiveState() ;

	// マスクデータがなかった場合は処理を終了
	if( MaskData == NULL ) return 0 ;

	// マスクグラフィックにマスクデータを展開
	DrawMaskToDirectData_Base( 0, 0, Mask->SrcData, Mask->SrcDataPitch,
							Width, Height, MaskData, DX_MASKTRANS_NONE ) ;

	// 終了
	return 0 ;
}



// マスク画面上の描画状態を取得する
extern int NS_GetMaskScreenData( int x1, int y1, int x2, int y2, int MaskHandle )
{
	MASKDATA * Mask ; 
	RECT Rect, Rect2 ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK( MaskHandle, Mask ) )
		return -1 ;

	if( MASKD.MaskBuffer == NULL )
		return 0 ;

	// 取得矩形のセット
	SETRECT( Rect, x1, y1, x2, y2 ) ;
	NS_GetDrawScreenSize( ( int * )&Rect2.right, ( int * )&Rect2.bottom ) ;
	Rect2.left = 0 ; Rect2.top = 0 ;
	RectClipping_Inline( &Rect, &Rect2 ) ;

	if( Rect.left == Rect.right || Rect.top == Rect.bottom ) return -1 ;
	if( Rect.right - Rect.left != Mask->MaskWidth ||
		Rect.bottom - Rect.top != Mask->MaskHeight ) return -1 ;

	// データの転送
	{
		BYTE *Dest, *Src ;
		int DestAddPitch, SrcAddPitch ;
		int DestWidth, DestHeight ;

		Rect.left -= x1 ;
		Rect.right -= x1 ;
		Rect.top -= y1 ;
		Rect.bottom -= y1 ;
		if( Rect.left > 0 ){ x1 += Rect.left ; }
		if( Rect.top  > 0 ){ y1 += Rect.top  ; }

		DestWidth = Rect.right - Rect.left ;
		DestHeight = Rect.bottom - Rect.top ;
		Dest = Mask->SrcData + Rect.left + ( Rect.top * Mask->SrcDataPitch ) ;
		Src = MASKD.MaskBuffer + x1 + ( y1 * MASKD.MaskBufferPitch ) ;

		SrcAddPitch = Mask->SrcDataPitch - DestWidth ;
		DestAddPitch = MASKD.MaskBufferPitch - DestWidth ;

#ifdef DX_NON_INLINE_ASM
		int i ;
		do
		{
			i = DestWidth ;
			do
			{
				*Dest = *Src ;
				Dest ++ ;
				Src ++ ;
			}while( -- i != 0 ) ;

			Dest += DestAddPitch ;
			Src += SrcAddPitch ;

		}while( -- DestHeight != 0 ) ;
#else
		__asm{
			CLD
			MOV		EDI, Dest
			MOV		ESI, Src
			MOV		EDX, DestWidth
			MOV		EBX, DestHeight
			MOV		EAX, SrcAddPitch
			MOV		ECX, DestAddPitch
			PUSH	EBP
			MOV		EBP, ECX
		LOOP1_b:
			MOV		ECX, EDX
			REP		MOVSB
			ADD		EDI, EBP
			ADD		ESI, EAX
			DEC		EBX
			JNZ		LOOP1_b
			POP		EBP
		}
#endif
	}

	// 終了
	return 0 ;
}

// マスクスクリーンを削除する
extern int NS_DeleteMaskScreen( void )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

//	CheckActiveState() ;

	// もしマスク使用フラグが立っていたらマスクを無効にする作業を行う
	if( MASKD.MaskUseFlag )
		NS_SetUseMaskScreenFlag( FALSE ) ;

	// マスク用サーフェスを削除する
	Mask_ReleaseSurface() ;

	// マスクサーフェスが作成されているかフラグを倒す
	MASKD.CreateMaskFlag = FALSE ;
	MASKD.MaskValidFlag = MASKD.MaskUseFlag && MASKD.CreateMaskFlag ;

	// 終了
	return 0 ;
}














// マスクスクリーンを作成する関数
extern int Mask_CreateScreenFunction( int MaskSurfaceFlag, int Width, int Height )
{
	bool   OldEnable = false ;
	BYTE  *MaskBufferOld = NULL ;
	int    MaskBufferPitchOld = 0 ;
	int    MaskBufferSizeXOld = 0 ;
	int    MaskBufferSizeYOld = 0 ;
	MEMIMG MaskDrawMemImgOld ;

	if( MaskSurfaceFlag == FALSE ) return 0 ;

	// サイズの調整
	if( MASKD.MaskBuffer != NULL )
	{
		// サイズは大きいほうに合わせる
		if( Width  < MASKD.MaskBufferSizeX ) Width  = MASKD.MaskBufferSizeX ;
		if( Height < MASKD.MaskBufferSizeY ) Height = MASKD.MaskBufferSizeY ;
	}

	// 既にマスクバッファが存在して、且つ今までよりも大きなサイズのバッファが指定された場合はバッファを大きく作り直す
	if( MASKD.MaskBuffer != NULL && ( Width > MASKD.MaskBufferSizeX || Height > MASKD.MaskBufferSizeY ) )
	{
		OldEnable = true ;

		MaskBufferOld      = MASKD.MaskBuffer ;
		MASKD.MaskBuffer   = NULL ;
		MaskBufferPitchOld = MASKD.MaskBufferPitch ;

		MaskBufferSizeXOld = MASKD.MaskBufferSizeX ;
		MaskBufferSizeYOld = MASKD.MaskBufferSizeY ;

		MaskDrawMemImgOld  = MASKD.MaskDrawMemImg ;
		_MEMSET( &MASKD.MaskDrawMemImg, 0, sizeof( MEMIMG ) ) ;

		// 環境依存処理
		Mask_CreateScreenFunction_Timing0_PF() ;
	}

	MASKD.MaskBufferSizeX = Width ;
	MASKD.MaskBufferSizeY = Height ;

	// マスク保存用バッファの作成
	if( MASKD.MaskBuffer == NULL )
	{
		// マスクバッファのピッチを算出( 4の倍数にする )
		MASKD.MaskBufferPitch = ( Width + 3 ) / 4 * 4 ;

		// メモリの確保
		MASKD.MaskBuffer = ( BYTE * )DXALLOC( ( size_t )( MASKD.MaskBufferPitch * Height ) ) ;

		// マスクのクリア
		_MEMSET( MASKD.MaskBuffer, 0, ( size_t )( MASKD.MaskBufferPitch * Height ) ) ;
	}

	// ハードウエアの機能を使用する場合はテクスチャも作成する
	if( GSYS.Setting.ValidHardware == TRUE )
	{
		if( Mask_CreateScreenFunction_Timing1_PF( Width, Height ) < 0 )
		{
			return -1 ;
		}
	}
	else
	{
		// 描画先用サーフェスの作成
		if( CheckValidMemImg( &MASKD.MaskDrawMemImg ) == FALSE )
		{
			// MEMIMG を使用する場合は画面と同じ大きさの MEMIMG を作成する
			if( MakeMemImgScreen( &MASKD.MaskDrawMemImg, Width, Height, -1 ) < 0 )
				return DXST_LOGFILE_ADDUTF16LE( "\xde\x30\xb9\x30\xaf\x30\x28\x75\x20\x00\x4d\x00\x45\x00\x4d\x00\x49\x00\x4d\x00\x47\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"マスク用 MEMIMG の作成に失敗しました" @*/ ) ;

			MASKD.ValidMaskDrawMemImg = TRUE ;
		}
	}

	// 作り直した場合は以前の内容をコピーする
	if( OldEnable )
	{
		int i ;

		// マスクバッファの内容をコピー
		for( i = 0 ; i < MaskBufferSizeYOld ; i ++ )
		{
			_MEMCPY( MASKD.MaskBuffer + MASKD.MaskBufferPitch * i, MaskBufferOld + MaskBufferPitchOld * i, ( size_t )MaskBufferSizeXOld ) ;
		}
		DXFREE( MaskBufferOld ) ;
		MaskBufferOld = NULL ;

		// ハードウエアの機能を使用する場合はテクスチャも作成する
		if( GSYS.Setting.ValidHardware == TRUE )
		{
			if( Mask_CreateScreenFunction_Timing2_PF( MaskBufferSizeXOld, MaskBufferSizeYOld ) < 0 )
			{
				return -1 ;
			}
		}
		else
		{
			// 古い描画先用MEMIMG の内容を新しい MEMIMG にコピーする
			DrawMemImg( &MASKD.MaskDrawMemImg, &MaskDrawMemImgOld, 0, 0, FALSE, NULL ) ;
			TerminateMemImg( &MaskDrawMemImgOld ) ;
		}
	}

	// 終了
	return 0 ;
}

// マスクスクリーンを一時削除する
extern int Mask_ReleaseSurface( void )
{
	// マスク保存用バッファの解放
	if( MASKD.MaskBuffer != NULL )
	{
		DXFREE( MASKD.MaskBuffer ) ;
		MASKD.MaskBuffer = NULL ;
	}

	// マスク描画用の MEMIMG を解放
	if( MASKD.ValidMaskDrawMemImg == TRUE )
	{
		TerminateMemImg( &MASKD.MaskDrawMemImg ) ;
		MASKD.ValidMaskDrawMemImg = FALSE ;
	}

	// 環境依存処理
	Mask_ReleaseSurface_PF() ;

	// 終了
	return 0 ;
}

// マスク使用モードを変更
extern int NS_SetUseMaskScreenFlag( int ValidFlag )
{
	IMAGEDATA *Image ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// マスクスクリーンがない場合はここで処理を終了
	if( MASKD.MaskBuffer == NULL ) return -1 ;

	// フルスクリーンマスク処理を行っている場合はマスク描画の結果を反映させる
	MASK_FULLSCREEN_MASKUPDATE

	// 今までとフラグが同じ場合は無視
//	if( ValidFlag == MASKD.MaskUseFlag ) return 0 ;

	// マスクを使用するかのフラグをセット
	MASKD.MaskUseFlag = ValidFlag ;
	MASKD.MaskValidFlag = MASKD.MaskUseFlag && MASKD.CreateMaskFlag ;

	// ハードウエアアクセラレータが有効かどうかで処理を分岐
	if( GSYS.Setting.ValidHardware )
	{
		// ハードウエアアクセラレータが有効な場合

		// 環境依存処理
		Mask_SetUseMaskScreenFlag_PF() ;
	}
	else
	{
		// ソフトウエアレンダリングの場合

		// 描画先の決定
		if( MASKD.MaskValidFlag )
		{
			// マスクが有効な場合はマスクを描画先にする
			GSYS.SoftRender.TargetMemImg = &MASKD.MaskDrawMemImg ;
		}
		else
		{
			// 描画先の画像データアドレスを取得する
			Image = Graphics_Image_GetData( GSYS.DrawSetting.TargetScreen[ 0 ] ) ;

			// それ以外の場合は描画可能画像かメインバッファ
			GSYS.SoftRender.TargetMemImg = Image ? &Image->Soft.MemImg : &GSYS.SoftRender.MainBufferMemImg ;
		}
	}

	// 描画先によってビューポート行列を変化させる環境の場合は描画範囲を再設定する
	if( GSYS.HardInfo.ScreenAxisYReverse_OnlyDrawValidGraph )
	{
		NS_SetDrawArea(
			GSYS.DrawSetting.OriginalDrawRect.left,
			GSYS.DrawSetting.OriginalDrawRect.top,
			GSYS.DrawSetting.OriginalDrawRect.right,
			GSYS.DrawSetting.OriginalDrawRect.bottom
		) ;
	}

	// 終了
	return 0 ;
}

// マスクを使用した描画の前に呼ぶ関数
// ( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern int Mask_DrawBeginFunction( RECT Rect )
{
	// フルスクリーンマスク処理を行っている場合はマスク描画の結果を反映させる
	MASK_FULLSCREEN_MASKUPDATE

	MASKD.MaskBeginFunctionCount ++ ;
	if( MASKD.MaskBeginFunctionCount == 1 )
	{
		// 描画エリアでクリッピング
		RectClipping_Inline( &Rect, &GSYS.DrawSetting.DrawArea ) ;

		// ハードウエアを使用するかどうかで処理を分岐
		if( GSYS.Setting.ValidHardware )
		{
			// ハードウエアを使用する場合
			Mask_DrawBeginFunction_PF( &Rect ) ;
		}
		else
		{
			IMAGEDATA *Image ;

			// ハードウエアを使用しない場合

			// 描画先の画像データアドレスを取得する
			Image = Graphics_Image_GetData( GSYS.DrawSetting.TargetScreen[ 0 ] ) ;

			// 描画対象 MEMIMG からマスク処理用 MEMIMG にイメージを転送
			if( MASKD.MaskValidFlag )
			{
				BltMemImg(
					&MASKD.MaskDrawMemImg, 
					Image ? &Image->Soft.MemImg : &GSYS.SoftRender.MainBufferMemImg,
					&Rect,
					Rect.left,
					Rect.top ) ;
			}
		}
	}

	// 終了
	return 0 ;
}

// マスクを使用した描画の後に呼ぶ関数
// ( ついでにサブバッファを使用した描画エリア機能を使用している場合の処理もいれてしまっているよ )
extern int Mask_DrawAfterFunction( RECT Rect )
{
	MASKD.MaskBeginFunctionCount -- ;
	if( MASKD.MaskBeginFunctionCount == 0 )
	{
		// 描画エリアでクリッピング
		RectClipping_Inline( &Rect, &GSYS.DrawSetting.DrawArea ) ;
		if( Rect.left == Rect.right || Rect.top == Rect.bottom ) return 0 ;

		// Direct3D を使用しているかどうかで処理を分岐
		// ハードウエアを使用するかどうかで処理を分岐
		if( GSYS.Setting.ValidHardware )
		{
			// ハードウエアを使用する場合
			Mask_DrawAfterFunction_PF( &Rect ) ;
		}
		else
		{
			IMAGEDATA *Image ;

			// ハードウエアを使用しない場合

			// マスクが掛かっていない部分を描画先に転送する
			if( MASKD.MaskValidFlag )
			{
				BYTE *Dest, *Src, *Mask ;
				int DestAddPitch, SrcAddPitch, MaskAddPitch ;
				int DestWidth, DestHeight ;
				MEMIMG *TargetMemImg ;
				int PixelByte ;

				// 描画先の画像データアドレスを取得する
				Image = Graphics_Image_GetData( GSYS.DrawSetting.TargetScreen[ 0 ] ) ;
				
				TargetMemImg = Image ? &Image->Soft.MemImg : &GSYS.SoftRender.MainBufferMemImg ;
				PixelByte = TargetMemImg->Base->ColorDataP->PixelByte ;

				DestWidth = Rect.right - Rect.left ;
				DestHeight = Rect.bottom - Rect.top ;

				MaskAddPitch = MASKD.MaskBufferPitch                     - DestWidth ;
				SrcAddPitch  = ( int )( MASKD.MaskDrawMemImg.Base->Pitch - PixelByte * DestWidth ) ;
				DestAddPitch = ( int )( TargetMemImg->Base->Pitch        - PixelByte * DestWidth ) ;

				Mask = ( BYTE * )MASKD.MaskBuffer              + Rect.left             + Rect.top * MASKD.MaskBufferPitch ;
				Src  = ( BYTE * )MASKD.MaskDrawMemImg.UseImage + Rect.left * PixelByte + Rect.top * MASKD.MaskDrawMemImg.Base->Pitch ;
				Dest = ( BYTE * )TargetMemImg->UseImage        + Rect.left * PixelByte + Rect.top * TargetMemImg->Base->Pitch ;

				if( MASKD.MaskReverseEffectFlag == TRUE )
				{
#ifdef DX_NON_INLINE_ASM
					int i ;
					// 効果反転版
					switch( PixelByte )
					{
					case 2 :
						do
						{
							i = DestWidth ;
							do
							{
								if( *Mask != 0 )
									*( ( WORD * )Dest ) = *( ( WORD * )Src ) ;

								Dest += 2 ;
								Src += 2 ;
								Mask ++ ;

							}while( -- i != 0 ) ;

							Dest += DestAddPitch ;
							Src += SrcAddPitch ;
							Mask += MaskAddPitch ;

						}while( -- DestHeight != 0 ) ;
						break ;

					case 4 :
						do
						{
							i = DestWidth ;
							do
							{
								if( *Mask != 0 )
									*( ( DWORD * )Dest ) = *( ( DWORD * )Src ) ;

								Dest += 4 ;
								Src += 4 ;
								Mask ++ ;

							}while( -- i != 0 ) ;

							Dest += DestAddPitch ;
							Src += SrcAddPitch ;
							Mask += MaskAddPitch ;

						}while( -- DestHeight != 0 ) ;
						break ;
					}
#else
					// 効果反転版
					switch( PixelByte )
					{
					case 2 :
						__asm
						{
							MOV		EDI, Dest
							MOV		ESI, Src
							MOV		EDX, Mask
	LOOP_2_1:
							MOV		ECX, DestWidth
	LOOP_2_2:
							MOV		AL, [ EDX ]
							TEST	AL, 0xff
							JZ		LABEL_2_1
							MOV		BX, [ ESI ]
							MOV		[ EDI ], BX
	LABEL_2_1:
							ADD		ESI, 2
							ADD		EDI, 2
							INC		EDX
							DEC		ECX
							JNZ		LOOP_2_2

							ADD		ESI, SrcAddPitch
							ADD		EDI, DestAddPitch
							ADD		EDX, MaskAddPitch

							DEC		DestHeight
							JNZ		LOOP_2_1
						}
						break ;

					case 4 :
						__asm
						{
							MOV		EDI, Dest
							MOV		ESI, Src
							MOV		EDX, Mask
	LOOP_4_1:
							MOV		ECX, DestWidth
	LOOP_4_2:
							MOV		AL, [ EDX ]
							TEST	AL, 0xff
							JZ		LABEL_4_1
							MOV		EBX, [ ESI ]
							MOV		[ EDI ], EBX
	LABEL_4_1:
							ADD		ESI, 4
							ADD		EDI, 4
							INC		EDX
							DEC		ECX
							JNZ		LOOP_4_2

							ADD		ESI, SrcAddPitch
							ADD		EDI, DestAddPitch
							ADD		EDX, MaskAddPitch

							DEC		DestHeight
							JNZ		LOOP_4_1
						}
						break ;
					}
#endif
				}
				else
				{
#ifdef DX_NON_INLINE_ASM
					int i ;
					// 効果反転してない版
					switch( PixelByte )
					{
					case 2 :
						do
						{
							i = DestWidth ;
							do
							{
								if( *Mask == 0 )
									*( ( WORD * )Dest ) = *( ( WORD * )Src ) ;

								Dest += 2 ;
								Src += 2 ;
								Mask ++ ;

							}while( -- i != 0 ) ;

							Dest += DestAddPitch ;
							Src += SrcAddPitch ;
							Mask += MaskAddPitch ;

						}while( -- DestHeight != 0 ) ;
						break ;

					case 4 :
						do
						{
							i = DestWidth ;
							do
							{
								if( *Mask == 0 )
									*( ( DWORD * )Dest ) = *( ( DWORD * )Src ) ;

								Dest += 4 ;
								Src += 4 ;
								Mask ++ ;

							}while( -- i != 0 ) ;

							Dest += DestAddPitch ;
							Src += SrcAddPitch ;
							Mask += MaskAddPitch ;

						}while( -- DestHeight != 0 ) ;
						break ;
					}
#else
					// 効果反転してない版
					switch( PixelByte )
					{
					case 2 :
						__asm
						{
							MOV		EDI, Dest
							MOV		ESI, Src
							MOV		EDX, Mask
	LOOP_2_1R:
							MOV		ECX, DestWidth
	LOOP_2_2R:
							MOV		AL, [ EDX ]
							TEST	AL, 0xff
							JNZ		LABEL_2_1R
							MOV		BX, [ ESI ]
							MOV		[ EDI ], BX
	LABEL_2_1R:
							ADD		ESI, 2
							ADD		EDI, 2
							INC		EDX
							DEC		ECX
							JNZ		LOOP_2_2R

							ADD		ESI, SrcAddPitch
							ADD		EDI, DestAddPitch
							ADD		EDX, MaskAddPitch

							DEC		DestHeight
							JNZ		LOOP_2_1R
						}
						break ;

					case 4 :
						__asm
						{
							MOV		EDI, Dest
							MOV		ESI, Src
							MOV		EDX, Mask
	LOOP_4_1R:
							MOV		ECX, DestWidth
	LOOP_4_2R:
							MOV		AL, [ EDX ]
							TEST	AL, 0xff
							JNZ		LABEL_4_1R
							MOV		EBX, [ ESI ]
							MOV		[ EDI ], EBX
	LABEL_4_1R:
							ADD		ESI, 4
							ADD		EDI, 4
							INC		EDX
							DEC		ECX
							JNZ		LOOP_4_2R

							ADD		ESI, SrcAddPitch
							ADD		EDI, DestAddPitch
							ADD		EDX, MaskAddPitch

							DEC		DestHeight
							JNZ		LOOP_4_1R
						}
						break ;
					}
#endif
				}
			}
		}
	}

	// 終了
	return 0 ;
}

// マスクをセットする
extern int NS_DrawMask( int x, int y, int MaskHandle, int TransMode )
{
	MASKDATA * Mask ; 
	RECT Rect, MovRect ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	if( MASKHCHK( MaskHandle, Mask ) )
		return -1 ;

	if( MASKD.MaskBuffer == NULL )
		return 0 ;

	// クリッピング処理
	SETRECT( Rect, x, y, x + Mask->MaskWidth, y + Mask->MaskHeight ) ;
	RectClipping_Inline( &Rect, &GSYS.DrawSetting.DrawArea ) ;
	MovRect = Rect ;
	if( Rect.left == Rect.right || Rect.top == Rect.bottom ) return 0 ;
	Rect.left   -= x ;
	Rect.right  -= x ;
	Rect.top    -= y ;
	Rect.bottom -= y ;
	if( Rect.left > 0 ){ x += Rect.left ; }
	if( Rect.top  > 0 ){ y += Rect.top  ; }

	// 転送処理
	{
		BYTE *Dest, *Src ;
		int DestAddPitch, SrcAddPitch ;
		int DestWidth, DestHeight, DWordNum, BNum ;
#ifdef DX_NON_INLINE_ASM
		int i ;
#endif

		DestWidth = Rect.right - Rect.left ;
		DestHeight = Rect.bottom - Rect.top ;
		DWordNum = DestWidth / 4 ;
		BNum = DestWidth % 4 ;
		Src = Mask->SrcData + Rect.left + ( Rect.top * Mask->SrcDataPitch ) ;
		Dest = MASKD.MaskBuffer + x + ( y * MASKD.MaskBufferPitch ) ;

		SrcAddPitch = Mask->SrcDataPitch - DestWidth ;
		DestAddPitch = MASKD.MaskBufferPitch - DestWidth ;


		switch( TransMode )
		{
		case DX_MASKTRANS_NONE :
			// 透過色なし転送
#ifdef DX_NON_INLINE_ASM
			do
			{
				i = DestWidth ;
				do
				{
					*Dest = *Src ;
					Dest ++ ;
					Src ++ ;
				}while( -- i != 0 ) ;
				Dest += DestAddPitch ;
				Src += SrcAddPitch ;
			}while( -- DestHeight != 0 ) ;
#else
			__asm{
				CLD
				MOV		EDI, Dest
				MOV		ESI, Src
				MOV		EDX, DestWidth
				MOV		EBX, DestHeight
				MOV		EAX, SrcAddPitch
				MOV		ECX, DestAddPitch
				PUSH	EBP
				MOV		EBP, ECX
			LOOP1_a:
				MOV		ECX, EDX
				REP		MOVSB
				ADD		EDI, EBP
				ADD		ESI, EAX
				DEC		EBX
				JNZ		LOOP1_a
				POP		EBP
			}
#endif
			break ;

		case DX_MASKTRANS_BLACK :
			// 黒透過色
#ifdef DX_NON_INLINE_ASM
			do
			{
				if( DWordNum != 0 )
				{
					i = DWordNum ;
					do
					{
						*( ( DWORD * )Dest ) |= *( ( DWORD * )Src ) ;
						Dest += 4 ;
						Src += 4 ;
					}while( -- i != 0 ) ;
				}

				if( BNum != 0 )
				{
					i = BNum ;
					do
					{
						*Dest |= *Src ;
						Dest ++ ;
						Src ++ ;
					}while( -- i != 0 ) ;
				}

				Dest += DestAddPitch ;
				Src += SrcAddPitch ;
			}while( -- DestHeight != 0 ) ;
#else
			__asm{
				MOV		EDI, Dest
				MOV		ESI, Src
				MOV		EDX, BNum
				SHL		EDX, 30
				OR		EDX, DWordNum
				MOV		EBX, DestHeight

			LOOP1B:
				MOV		ECX, EDX
				AND		ECX, 0x3fffffff // ((1 << 30) - 1)
				CMP		ECX, 0
				JE		R10B

			LOOP2B:
				MOV		EAX, [ESI]
				OR		[EDI], EAX

				ADD		EDI, 4
				ADD		ESI, 4

				DEC		ECX
				JNZ		LOOP2B

			R10B:

				MOV		ECX, EDX
				SHR		ECX, 30
				CMP		ECX, 0
				JE		R11B

			LOOP3B:
				MOV		AL, [ESI]
				OR		[EDI], AL
				INC		ESI
				INC		EDI
				DEC		ECX
				JNZ		LOOP3B

			R11B:

				ADD		EDI, DestAddPitch
				ADD		ESI, SrcAddPitch
				DEC		EBX
				JNZ		LOOP1B
			}
#endif
			break ;

		case DX_MASKTRANS_WHITE :
			// 白透過色
#ifdef DX_NON_INLINE_ASM
			do
			{
				if( DWordNum != 0 )
				{
					i = DWordNum ;
					do
					{
						*( ( DWORD * )Dest ) &= *( ( DWORD * )Src ) ;
						Dest += 4 ;
						Src += 4 ;
					}while( -- i != 0 ) ;
				}

				if( BNum != 0 )
				{
					i = BNum ;
					do
					{
						*Dest &= *Src ;
						Dest ++ ;
						Src ++ ;
					}while( -- i != 0 ) ;
				}

				Dest += DestAddPitch ;
				Src += SrcAddPitch ;
			}while( -- DestHeight != 0 ) ;
#else
			__asm
			{
				MOV		EDI, Dest
				MOV		ESI, Src
				MOV		EDX, BNum
				SHL		EDX, 30
				OR		EDX, DWordNum
				MOV		EBX, DestHeight

			LOOP1W:
				MOV		ECX, EDX
				AND		ECX, 0x3fffffff // ((1 << 30) - 1)
				CMP		ECX, 0
				JE		R10W

			LOOP2W:
				MOV		EAX, [ESI]
				AND		[EDI], EAX
				ADD		EDI, 4
				ADD		ESI, 4

				DEC		ECX
				JNZ		LOOP2W

			R10W:
				MOV		ECX, EDX
				SHR		ECX, 30
				CMP		ECX, 0
				JE		R11W

			LOOP3W:
				MOV		AL, [ESI]
				AND		[EDI], AL
				INC		ESI
				INC		EDI
				DEC		ECX
				JNZ		LOOP3W

			R11W:
				ADD		EDI, DestAddPitch
				ADD		ESI, SrcAddPitch
				DEC		EBX
				JNZ		LOOP1W
			}
#endif
			break ;
		}
	}

	// 更新
	if( GSYS.Setting.ValidHardware )
	{
		Mask_UpdateMaskImageTexture_PF( &MovRect ) ;
	}

	// 終了
	return 0 ;
}

// 指定のマスクを画面いっぱいに展開する
extern int NS_DrawFillMask( int x1, int y1, int x2, int y2, int MaskHandle )
{
	MASKDATA * Mask ;
	RECT Rect, MovRect ;

	if( MASKD.InitializeFlag == FALSE )
		return -1 ;


	if( MASKHCHK( MaskHandle, Mask ) )
		return -1 ;

	if( MASKD.MaskBuffer == NULL )
		return 0 ;

	if( x1 == x2 || y1 == y2 ) return 0 ;

	CheckActiveState() ;

	// クリッピング処理
	SETRECT( Rect, x1, y1, x2, y2 ) ;
	RectClipping_Inline( &Rect, &GSYS.DrawSetting.DrawArea ) ;
	MovRect = Rect ;
	if( Rect.left == Rect.right || Rect.top == Rect.bottom ) return 0 ;
	Rect.left -= x1 ;
	Rect.right -= x1 ;
	Rect.top -= y1 ;
	Rect.bottom -= y1 ;
	if( Rect.left > 0 ){ x1 += Rect.left ; }
	if( Rect.top  > 0 ){ y1 += Rect.top  ; }

	{
		BYTE *Dest, *Src/*, *Src2*/, *Src3 ;
		int DestWidth, DestHeight, SrcWidth, SrcHeight ;
		DWORD SrcAddPitch, DestAddPitch ;
		DWORD FirstX, FirstY ;
//		DWORD CounterY ;

		SrcWidth = Mask->MaskWidth ;
		SrcHeight = Mask->MaskHeight ;

		DestWidth = Rect.right - Rect.left ;
		DestHeight = Rect.bottom - Rect.top ;

		FirstX = ( DWORD )( Mask->MaskWidth  - Rect.left % Mask->MaskWidth  ) ;
		FirstY = ( DWORD )( Mask->MaskHeight - Rect.top  % Mask->MaskHeight ) ;

		Dest = MASKD.MaskBuffer + x1 + y1 * MASKD.MaskBufferPitch ;
		Src = Mask->SrcData + ( Rect.left % Mask->MaskWidth ) + ( Rect.top % Mask->MaskHeight ) * Mask->SrcDataPitch ;
//		Src2 = Mask->SrcData ;
		Src3 = Mask->SrcData + ( Rect.left % Mask->MaskWidth ) ;

		SrcAddPitch  = ( DWORD )( Mask->SrcDataPitch - ( ( DestWidth + Rect.left % Mask->MaskWidth ) % Mask->MaskWidth == 0 ? 0 : ( DestWidth + Rect.left % Mask->MaskWidth ) % Mask->MaskWidth ) + Rect.left % Mask->MaskWidth ) ;
		DestAddPitch = ( DWORD )( MASKD.MaskBufferPitch - DestWidth ) ;

//		CounterY = FirstY ;

#ifdef DX_NON_INLINE_ASM
		int i, TempWidth, TempHeight ;

		TempHeight = ( int )FirstY ;
		do
		{
			TempWidth = ( int )FirstX ;
			i = DestWidth ;
			do
			{
				*Dest = *Src ;
				Dest ++ ;
				Src ++ ;
				if( -- TempWidth == 0 )
				{
					TempWidth = SrcWidth ;
					Src -= SrcWidth ;
				}
			}while( -- i != 0 ) ;

			Dest += DestAddPitch ;
			Src += SrcAddPitch ;

			if( -- TempHeight == 0 )
			{
				Src = Src3 ;
				TempHeight = SrcHeight ;
			}

		}while( -- DestHeight != 0 ) ;
#else
		__asm{
			CLD
			MOV		EDI, Dest
			MOV		ESI, Src
			MOV		EAX, SrcWidth
			MOV		EBX, FirstY
		LOOP1:
			MOV		ECX, FirstX
			MOV		EDX, DestWidth
		LOOP2:
			MOVSB
			DEC		ECX
			JZ		R2
		R1:
			DEC		EDX
			JZ		RE
			JMP		LOOP2

		R2:
			MOV		ECX, EAX
			SUB		ESI, ECX
			JMP		R1

		RE:
			ADD		ESI, SrcAddPitch
			ADD		EDI, DestAddPitch
			DEC		DestHeight
			JZ		RE2
			DEC		EBX
			JZ		R3
			JMP		LOOP1

		R3:
			MOV		ESI, Src3
			MOV		ECX, SrcHeight
			MOV		EBX, ECX
			JMP		LOOP1

		RE2:
		}
#endif
	}

	if( GSYS.Setting.ValidHardware )
	{
		Mask_UpdateMaskImageTexture_PF( &MovRect ) ;
	}

	// 終了
	return 0 ;
}

// マスクのセット
extern int NS_DrawMaskToDirectData( int x, int y, int Width, int Height, const void *MaskData, int TransMode )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// マスクスクリーンがない場合はここで処理を終了
	if( MASKD.MaskBuffer == NULL ) return 0 ;

	// マスクデータがなかった場合は処理を終了
	if( MaskData == NULL ) return 0 ;

	CheckActiveState() ;

	// マスクスクリーンにマスクを展開
	DrawMaskToDirectData_Base( x, y, MASKD.MaskBuffer, MASKD.MaskBufferPitch,
							Width, Height, MaskData, TransMode ) ;

	RECT Rect ;

	if( GSYS.Setting.ValidHardware )
	{
		// 更新
		Rect.left   = x ;
		Rect.top    = y ;
		Rect.right  = x + Width ;
		Rect.bottom = y + Height ;
		Mask_UpdateMaskImageTexture_PF( &Rect ) ;
	}

	// 終了
	return 0 ;
}

// マスクスクリーンを指定の色で塗りつぶす
extern int NS_FillMaskScreen( int Flag )
{
	if( MASKD.InitializeFlag == FALSE )
		return -1 ;

	// マスクスクリーンがない場合はここで処理を終了
	if( MASKD.MaskBuffer == NULL ) return -1 ;

	CheckActiveState() ;

	// マスクスクリーンをクリアする
	_MEMSET( MASKD.MaskBuffer, ( unsigned char )( Flag ? 0xff : 0 ), ( size_t )( MASKD.MaskBufferPitch * GSYS.DrawSetting.DrawSizeY ) ) ;

	if( GSYS.Setting.ValidHardware )
	{
		Mask_FillMaskScreen_PF( Flag ) ;
	}

	// 終了
	return 0 ;
}

// マスクスクリーンとして使用するグラフィックのハンドルを設定する、-1を渡すと解除( 引数で渡すグラフィックハンドルは MakeScreen で作成した「アルファチャンネル付きの描画対象にできるグラフィックハンドル」である必要があります( アルファチャンネルがマスクに使用されます ) )
extern int NS_SetMaskScreenGraph( int GraphHandle )
{
	IMAGEDATA *Image ;

	if( MASKD.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	// -1 の場合は 0 にする
	if( GraphHandle == -1 )
	{
		GraphHandle = 0 ;
	}
	else
	{
		// 無効なグラフィックハンドルか、描画対象にできないグラフィックハンドルである場合はエラー
		Image = Graphics_Image_GetData( GraphHandle ) ;
		if( Image == NULL ||
			Image->Orig->FormatDesc.TextureFlag == FALSE ||
			Image->Orig->FormatDesc.DrawValidFlag == FALSE )
		{
			return -1 ;
		}
	}

	// 今までと値が同じである場合は何もせず終了
	if( GraphHandle == MASKD.MaskScreenGraphHandle )
	{
		return 0 ;
	}

	// フルスクリーンマスク処理を行っている場合はマスク描画の結果を反映させる
	MASK_FULLSCREEN_MASKUPDATE

	// グラフィックハンドルを保存
	MASKD.MaskScreenGraphHandle = GraphHandle ;

	// 正常終了
	return 0 ;
}

// マスクスクリーンとして使用するグラフィックの、どのチャンネルをマスクとして使用するかを設定する( デフォルトは DX_MASKGRAPH_CH_A、 尚、DX_MASKGRAPH_CH_A以外を使用する場合はグラフィックスデバイスがシェーダーモデル2.0以降に対応している必要があります )
extern int NS_SetMaskScreenGraphUseChannel(	int UseChannel )
{
	if( UseChannel < 0 || UseChannel > DX_MASKGRAPH_CH_B )
	{
		return -1 ;
	}

	MASKD.MaskScreenGraphHandleUseChannel = UseChannel ;

	// 正常終了
	return 0 ;
}











#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif


