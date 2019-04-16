// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用システムプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxSystemWin.h"

// インクルード ------------------------------------------------------------------
#include "DxWindow.h"
#include "DxGraphicsWin.h"
#include "../DxLib.h"
#include "../DxSystem.h"
#include "../DxArchive_.h"
#include "../DxGraphics.h"
#include "../DxModel.h"
#include "../DxFile.h"
#include "../DxNetwork.h"
#include "../DxInputString.h"
#include "../DxInput.h"
#include "../DxSound.h"
#include "../DxSoundConvert.h"
#include "../DxBaseImage.h"
#include "../DxSoftImage.h"
#include "../DxMovie.h"
#include "../DxFont.h"
#include "../DxLog.h"
#include "../DxASyncLoad.h"
#include "../DxUseCLib.h"


#include "../DxBaseFunc.h"
#include "../DxMemory.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGuid.h"


#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

//#define MAX_EVENTPROCESS_NUM						(5)					// 一度に処理するイベントの最大数

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

// プログラム --------------------------------------------------------------------

// ライブラリ初期化関数
extern int NS_DxLib_Init( void )
{
	// 既に初期化済みの場合は何もせず終了
	if( DxSysData.DxLib_InitializeFlag == TRUE ) return 0 ;

	DXST_LOGFILEFMT_ADDUTF16LE(( "\x24\xff\x38\xff\xe9\x30\xa4\x30\xd6\x30\xe9\x30\xea\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x8b\x95\xcb\x59\x00"/*@ L"ＤＸライブラリの初期化処理開始" @*/ )) ;
	DXST_LOGFILE_TABADD ;

	SETUP_WIN_API

	// 初期化中フラグを立てる
	DxSysData.DxLib_RunInitializeFlag = TRUE ;

	// QuitMessage を飛ばしたフラグを倒す
	WinData.QuitMessageFlag = FALSE ;

	// DxSysData の共通初期化処理
	DxLib_SysInit() ;

	// キャラクターコード関係の初期化を行う
	InitCharCode() ;

	// DxBaseFunc の初期化
	_INIT_BASEFUNC() ;

	// 使用する文字セットをセット
	_SET_DEFAULT_CHARCODEFORMAT() ;

#ifndef DX_NON_OGGTHEORA
	// Theora 用の初期化
	TheoraDecode_GrobalInitialize() ;
#endif

	// アーカイブファイルアクセス用のデータを初期化
#ifndef DX_NON_DXA
	DXA_DIR_Initialize() ;
#endif

	// ストリームデータ読み込み制御用ポインタ構造体のデフォルト値をセット
	NS_ChangeStreamFunction( NULL ) ;

	// カレントディレクトリ保存
	DX_FGETDIR( WinData.CurrentDirectory, sizeof( WinData.CurrentDirectory ) ) ; 

#ifndef DX_NON_LOG
	// ログファイルの初期化
	LogFileInitialize() ;
#endif

#ifndef DX_NON_GRAPHICS

	// フルスクリーンモードの場合は指定の画面解像度が使用可能かどうかを調べる
	if( WinData.WindowModeFlag == FALSE )
	{
		int Num, i, Width, Height, ColorBitDepth ;
		int DisplayIndex ;
		DISPLAYMODEDATA Mode ;

SCREENMODECHECK:
		ColorBitDepth = NS_GetColorBitDepth() ;
		NS_GetDrawScreenSize( &Width, &Height ) ; 

		DXST_LOGFILEFMT_ADDUTF16LE(( "\x2d\x8a\x9a\x5b\x55\x30\x8c\x30\x66\x30\x44\x30\x8b\x30\x3b\x75\x62\x97\x2d\x8a\x9a\x5b\x20\x00\x25\x00\x64\x00\x78\x00\x25\x00\x64\x00\x20\x00\x25\x00\x64\x00\x62\x00\x69\x00\x74\x00\x20\x00\x63\x00\x6f\x00\x6c\x00\x6f\x00\x72\x00\x00"/*@ L"設定されている画面設定 %dx%d %dbit color" @*/, Width, Height, ColorBitDepth ) ) ;

		DisplayIndex = GSYS.Screen.ValidUseDisplayIndex ? GSYS.Screen.UseDisplayIndex : 0 ;

		Num = NS_GetDisplayModeNum( DisplayIndex ) ;
		for( i = 0 ; i < Num ; i ++ )
		{
			Mode = NS_GetDisplayMode( i, DisplayIndex ) ;
//			DXST_LOGFILEFMT_ADDUTF16LE(( L"%dx%d %dbit color", Mode.Width, Mode.Height, Mode.ColorBitDepth ) ) ;
			if( Mode.Width == Width && Mode.Height == Height && Mode.ColorBitDepth == ColorBitDepth )
				break ;
		}
		if( i == Num )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x8b\x30\x3b\x75\x62\x97\xe2\x30\xfc\x30\xc9\x30\x6a\x30\x57\x30\x00"/*@ L"対応している画面モードなし" @*/ )) ;

			// 640x480 16bitがだめだった場合は 32bitを試してみる
			if( Width == 640 && Height == 480 && ColorBitDepth == 16 )
			{
				NS_SetGraphMode( Width, Height, 32, GSYS.Screen.MainScreenRefreshRate ) ;
				goto SCREENMODECHECK ;
			}
		}
	}

	// 24ビットカラーチェック
	if( DxSysData.NotWinFlag == FALSE )
	{
		int CrBitNum ;

		// 現在の画面モードでの色の数を取得
		{
			HDC hdc ;
			hdc = WinAPIData.Win32Func.GetDCFunc( NULL ) ;
			CrBitNum = WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, PLANES ) * WinAPIData.Win32Func.GetDeviceCapsFunc( hdc, BITSPIXEL ) ;
			WinAPIData.Win32Func.ReleaseDCFunc( NULL, hdc ) ;
		}

		if( ( CrBitNum == 24 && WinData.WindowModeFlag == TRUE ) ||
			( NS_GetColorBitDepth() == 24 ) )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x24\xff\x38\xff\xe9\x30\xa4\x30\xd6\x30\xe9\x30\xea\x30\x6f\x30\x12\xff\x14\xff\xd3\x30\xc3\x30\xc8\x30\xab\x30\xe9\x30\xfc\x30\xe2\x30\xfc\x30\xc9\x30\x6b\x30\xfe\x5b\xdc\x5f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"ＤＸライブラリは２４ビットカラーモードに対応していません\n" @*/ ) ;

			DXST_LOGFILE_TABSUB ;
			DXST_LOGFILEFMT_ERRCODE_ADDUTF16LE(( DX_ERRORCODE_WIN_DESKTOP_24BIT_COLOR, "\x24\xff\x38\xff\xe9\x30\xa4\x30\xd6\x30\xe9\x30\xea\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x31\x59\x57\x65\x00"/*@ L"ＤＸライブラリの初期化処理失敗" @*/ ) ) ;
			return -1 ;
		}
	}

#endif // DX_NON_GRAPHICS

	// パフォーマンスカウンター情報の初期化
	{
		// パフォーマンスカウンターの周波数を取得する
		WinAPIData.Win32Func.QueryPerformanceFrequencyFunc( ( LARGE_INTEGER * )&WinData.PerformanceClock ) ;

		// 使えるかどうかを保存しておく
		WinData.PerformanceTimerFlag = WinData.PerformanceClock != 0 ;
	}
	
	// システムログを出力
	OutSystemInfo() ;

#ifndef DX_NON_GRAPHICS
	// デフォルトのグラフィック復元関数を登録
	NS_SetRestoreGraphCallback( NULL ) ;
#endif // DX_NON_GRAPHICS

	// 各処理系の初期化
	InitializeCom() ;											// ＣＯＭの初期化
	if( InitializeWindow() == -1 ) goto ERROR_DX ;				// ウインドウ関係の初期化
#ifndef DX_NON_NETWORK
#if 0
	if( HTTP_Initialize() == -1 ) goto ERROR_DX ;				// HTTP 関係の処理の初期化
#endif
#endif

	if( DxSysData.NotInputFlag == FALSE )
	{
#ifndef DX_NON_INPUT
		if( InitializeInputSystem() == -1 ) goto ERROR_DX ;			// 入力システムの初期化
#endif // DX_NON_INPUT
	}

	if( DxSysData.NotSoundFlag == FALSE )
	{
#ifndef DX_NON_SOUND
		InitializeSoundConvert() ;									// サウンド変換処理の初期化
		InitializeSoundSystem() ;									// サウンドシステムのの初期化
#endif // DX_NON_SOUND
	}
	if( DxSysData.NotDrawFlag == FALSE )
	{
		InitializeBaseImageManage() ;
#ifndef DX_NON_SOFTIMAGE
		InitializeSoftImageManage() ;
#endif // DX_NON_SOFTIMAGE
#ifndef DX_NON_MOVIE
		InitializeMovieManage() ;
#endif

#ifndef DX_NON_GRAPHICS
		if( Graphics_Initialize() < 0 ) goto ERROR_DX ;
#endif // DX_NON_GRAPHICS
	}
#ifndef DX_NON_INPUTSTRING
	InitializeInputCharBuf() ;									// 文字コードバッファの初期化
#endif // DX_NON_INPUTSTRING

	// ＤＸライブラリ初期化完了フラグをたてる
	DxSysData.DxLib_InitializeFlag = TRUE ;

	// ＶＳＹＮＣ待ちをする
//	NS_SetWaitVSyncFlag( TRUE ) ;

	// フルスクリーンモードだった場合ウインドウを最前面に持ってくる
	if( WinData.WindowModeFlag == FALSE && WinData.UserWindowFlag == FALSE )
		SetAbsoluteForegroundWindow( WinData.MainWindow ) ;
	
#if !defined( DX_NON_LOG ) && !defined( DX_NON_PRINTF_DX )
	// ログ出力処理の初期化を行う
	InitializeLog() ;
#endif

#ifndef DX_NON_GRAPHICS
	// 描画先の変更
	NS_SetDrawScreen( DX_SCREEN_BACK ) ;
	NS_SetDrawScreen( DX_SCREEN_FRONT ) ;
#endif // DX_NON_GRAPHICS

	if( DxSysData.NotDrawFlag == FALSE )
	{
#ifndef DX_NON_MODEL
		// モデルバージョン１の初期化
		if( MV1Initialize() < 0 )
		{
			goto ERROR_DX ;
		}
#endif
	}

	// 初期化中フラグを倒す
	DxSysData.DxLib_RunInitializeFlag = FALSE ;

	// ウインドウのサイズが有効かどうかのフラグを倒すリクエストがされていたらフラグを倒す
	if( WinData.WindowSizeValidResetRequest == TRUE )
	{
		WinData.WindowSizeValid = FALSE ;
	}

	DXST_LOGFILE_TABSUB ;
	DXST_LOGFILEFMT_ADDUTF16LE(( "\x24\xff\x38\xff\xe9\x30\xa4\x30\xd6\x30\xe9\x30\xea\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x42\x7d\x86\x4e\x00"/*@ L"ＤＸライブラリの初期化処理終了" @*/ ) ) ;

	// 終了
	return 0 ;

ERROR_DX:
	NS_DxLib_End() ;

	DXST_LOGFILE_TABSUB ;
	DXST_LOGFILEFMT_ADDUTF16LE(( "\x24\xff\x38\xff\xe9\x30\xa4\x30\xd6\x30\xe9\x30\xea\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x31\x59\x57\x65\x00"/*@ L"ＤＸライブラリの初期化処理失敗" @*/ ) ) ;

	// 初期化中フラグを倒す
	DxSysData.DxLib_RunInitializeFlag = FALSE ;

	return -1 ;
} 

// ライブラリ使用の終了関数
extern int NS_DxLib_End( void )
{
	// 既に終了処理が行われているか、そもそも初期化されていない場合は何もしない
	if( DxSysData.DxLib_InitializeFlag == FALSE ) return 0;

	SETUP_WIN_API

	WinData.AltF4_EndFlag = 1 ;

#ifndef DX_NON_SOFTIMAGE
	// 登録した全てのソフトイメージを削除
	InitSoftImage() ;
#endif // DX_NON_SOFTIMAGE

	// 各処理系の終了
#if !defined( DX_NON_LOG ) && !defined( DX_NON_PRINTF_DX )
	TerminateLog() ;			// ログ処理の後始末
#endif

#ifndef DX_NON_NETWORK
#if 0
	HTTP_Terminate() ;			// HTTP 関係の処理の後始末を行う
#endif
	TerminateNetWork() ;		// ＷｉｎＳｏｃｋｅｔｓ関係の終了
#endif

#ifndef DX_NON_SOUND
	NS_StopMusic() ;			// ＭＩＤＩが演奏されている状態の場合それを止める
#endif // DX_NON_SOUND

#ifndef DX_NON_MODEL
	MV1Terminate() ;			// モデルバージョン１の後始末
#if 0
	NS_InitModel() ;			// モデルデータの削除
#endif
#endif

#ifndef DX_NON_GRAPHICS
	Graphics_Terminate() ;
#endif // DX_NON_GRAPHICS

	TerminateBaseImageManage() ;

#ifndef DX_NON_SOFTIMAGE
	TerminateSoftImageManage() ;
#endif // DX_NON_SOFTIMAGE

#ifndef DX_NON_MOVIE
	TerminateMovieManage() ;
#endif

#ifndef DX_NON_INPUT
	TerminateInputSystem() ;	// 入力システムの終了
#endif // DX_NON_INPUT

#ifndef DX_NON_INPUTSTRING
	TerminateInputCharBuf() ;
#endif // DX_NON_INPUTSTRING

#ifndef DX_NON_SOUND
	TerminateSoundSystem() ;	// サウンドシステムの後始末
	TerminateSoundConvert() ;	// サウンド変換処理の終了
#endif // DX_NON_SOUND

	TerminateWindow() ;			// Ｗｉｎｄｏｗ関係の終了処理

	TerminateCom() ;			// ＣＯＭの終了

#ifndef DX_NON_LOG
	// ログファイルの後始末
	LogFileTerminate() ;
#endif

	// ＤＸライブラリ初期化完了フラグを倒す
	DxSysData.DxLib_InitializeFlag = FALSE ;

	// ＱＵＩＴメッセージが来るまでループ
	if( DxSysData.NotWinFlag == FALSE )
		while( NS_ProcessMessage() == 0 && WinData.QuitMessageFlag == FALSE && WinData.DestroyMessageCatchFlag == FALSE ){}

	// ウインドウクラスの登録を抹消
	WinAPIData.Win32Func.UnregisterClassWFunc( WinData.ClassName, WinData.Instance ) ;

	// ウインドウ閉じ待ち
	while( DxSysData.NotWinFlag == FALSE && WinAPIData.Win32Func.FindWindowWFunc( WinData.ClassName, NULL ) == WinData.MainWindow )
	{
		WinAPIData.Win32Func.DestroyWindowFunc( WinData.MainWindow ) ;
		WinAPIData.Win32Func.SleepFunc( 100 ) ;
	}

	// カレントディレクトリを元に戻す
	DX_FSETDIR( WinData.CurrentDirectory ) ; 

	// アーカイブファイルアクセス用のデータの後始末
#ifndef DX_NON_DXA
	DXA_DIR_Terminate() ;
#endif

#ifdef DX_USE_DXLIB_MEM_DUMP
	// メモリダンプを行う
	NS_DxDumpAlloc() ;
#endif

	// メモリの後始末を行う
	MemoryTerminate() ;

	// WinAPI を解放する
	ReleaseWinAPI() ;

	// 終了
	return 0 ;
}

// ライブラリの内部で使用している構造体をゼロ初期化して、DxLib_Init の前に行った設定を無効化する( DxLib_Init の前でのみ有効 )
extern int NS_DxLib_GlobalStructInitialize( void )
{
//	_MEMSET( &GRA2, 0, sizeof( GRA2 ) ) ;
#ifndef DX_NON_GRAPHICS
	_MEMSET( &GraphicsSysData, 0, sizeof( GraphicsSysData ) );
#endif // DX_NON_GRAPHICS

	_MEMSET( &BaseImageManage, 0, sizeof( BaseImageManage ) ) ;
	_MEMSET( &WinData, 0, sizeof( WinData ) );
#ifndef DX_NON_SOUND
	_MEMSET( &SoundSysData, 0, sizeof( SoundSysData ) );
#endif // DX_NON_SOUND

	return 0;
}

// ウインドウズのメッセージループに代わる処理を行う
extern int NS_ProcessMessage( void )
{
	int	EventCon ;
	int LoopCount ;
	MSG msg = { 0 } ;
	int Flag = FALSE, hr, hr2, StopCheckFlag ;
	static int EndFlag = FALSE ;

	// もしフラグがたっていたらなにもせず終了
	if( EndFlag )
	{
		return 0 ;
	}

	SETUP_WIN_API

	// ProcessMessage を実行中フラグを立てる
	WinData.ProcessMessageFlag = TRUE ;

	// クライアント領域から出ないようにする設定で、且つクライアント領域内に居たら出れないようにする
	if( WinData.NotMoveMousePointerOutClientAreaFlag && WinData.ActiveFlag && WinData.WindowMinSizeFlag == FALSE && WinData.WindowModeFlag && WinData.SetClipCursorFlag == FALSE )
	{
		POINT MousePos ;

		WinAPIData.Win32Func.GetCursorPosFunc( &MousePos ) ;
		if( WinData.WindowRect.left < MousePos.x && MousePos.x <= WinData.WindowRect.right &&
			WinData.WindowRect.top  < MousePos.y && MousePos.y <= WinData.WindowRect.bottom )
		{
			WinData.SetClipCursorFlag = TRUE ;
			WinAPIData.Win32Func.ClipCursorFunc( &WinData.WindowRect ) ;
		}
	}

	// DxSysData.NotWinFlag が立っている場合はウインドウ関連の処理は行わない
	if( DxSysData.NotWinFlag == FALSE )
	{
		// ウインドウが作成されていなかったら何もせず終了
		if( WinData.MainWindow == NULL ) goto END ;

		// ＤＸライブラリが作成したウインドウを使用している場合のみメッセージ処理を行う
		if( WinData.UserWindowFlag == FALSE )
		{
			// もしウインドウモード変更フラグが立っていたらウインドウモードを変更する処理を行う
			if( WinData.ChangeWindowModeFlag == TRUE && WinData.UseChangeWindowModeFlag == TRUE && WinData.MainThreadID == WinAPIData.Win32Func.GetCurrentThreadIdFunc() )
			{
				// フラグを倒す
				WinData.ChangeWindowModeFlag = FALSE ;

				// ウインドウモードを変更する
				NS_ChangeWindowMode( WinData.WindowModeFlag == TRUE ? FALSE : TRUE ) ;

				// コールバック関数を呼ぶ
				if( WinData.ChangeWindowModeCallBackFunction != NULL ) 
					WinData.ChangeWindowModeCallBackFunction( WinData.ChangeWindowModeCallBackFunctionData ) ;
			}

			// メッセージが何もないかあった場合はﾒｯｾｰｼﾞの処理が終わるまでループする（制限あり）
			EventCon = 0 ;
			LoopCount = 0 ;
			StopCheckFlag = FALSE ;
			while(
				(
				  WinData.CloseMessagePostFlag == FALSE &&
				  WinData.WM_ACTIVATE_StockNum == 0 &&
				  WinData.NonActiveRunFlag     == FALSE &&
				  (
				    WinData.ActiveFlag == FALSE ||
				    WinData.WindowMinSizeFlag == TRUE
				  ) 
				) ||
				(
				  Flag == FALSE &&
				  WinData.QuitMessageFlag == FALSE
				)
			)
			{
				// ソフトが非アクティブになり停止した場合は時刻を保存しておく
				if( StopCheckFlag == FALSE && ( WinData.ActiveFlag == FALSE || WinData.WindowMinSizeFlag == TRUE ) && WinData.NonActiveRunFlag == FALSE )
				{
					StopCheckFlag = TRUE ;
					WinData.WaitTime = NS_GetNowCount() ;
					WinData.WaitTimeValidFlag = TRUE ;
				}

				Flag = TRUE ;

				if( ( WinData.ActiveFlag == FALSE || WinData.WindowMinSizeFlag == TRUE ) && WinData.NonActiveRunFlag == FALSE )
				{
					WinAPIData.Win32Func.SleepFunc( 1 ) ;

					// ScreenCopy のリクエストフラグが立っていたら一定時間が経過していたら ScreenCopy を行う
					if( WinData.ScreenCopyRequestFlag == TRUE && NS_GetNowCount() - WinData.ScreenCopyRequestStartTime > 1000 / 60 * 4 )
					{
#ifndef DX_NON_GRAPHICS
						// 非アクティブの場合は非アクティブ時に表示する専用の画像がある場合は裏画面のコピーは行わない
						if( WinData.PauseGraph.GraphData == NULL )
						{
							NS_ScreenCopy() ;
						}
#endif // DX_NON_GRAPHICS
						WinData.ScreenCopyRequestStartTime = NS_GetNowCount() ;
						WinData.ScreenCopyRequestFlag = FALSE ;
					}
				}

				hr  = 1 ;
				hr2 = 0 ;
				while( hr )
				{
					MSG msg2 ;
					if(
//						( hr2 = ( WinAPIData.Win32Func.PeekMessageWFunc( &msg2, NULL, 0, 0, PM_NOREMOVE ) != 0 && msg2.message == WM_SYSCOMMAND && msg2.wParam == SC_SCREENSAVE ) ) ||
						( hr2 = ( WinAPIData.Win32Func.PeekMessageWFunc( &msg2,       NULL, 0, 0, PM_NOREMOVE ) != 0 /*&& msg2.message == WM_SYSCOMMAND && msg2.wParam == SC_SCREENSAVE*/ ) ) != 0 || // ←ATOKの場合スクリーンセーバーメッセージに限定すると半角／全角キーで日本語入力モードに移行できなかったので、無条件に。何か問題があったらそのとき考えよう・・・
						( hr  = ( WinAPIData.Win32Func.PeekMessageWFunc( &msg,  ( HWND )-1, 0, 0, PM_REMOVE   ) || WinAPIData.Win32Func.PeekMessageWFunc( &msg, WinData.MainWindow, 0, 0, PM_REMOVE ) ) ) != 0
					  )
					{
						if( hr2 != 0 )
						{
							WinAPIData.Win32Func.PeekMessageWFunc( &msg2, NULL, 0, 0, PM_REMOVE ) ;
							msg = msg2 ;
						}

						// ダイアログボックス用のメッセージか調べる
						if( WinData.DialogBoxHandle == NULL || WinAPIData.Win32Func.IsDialogMessageWFunc( WinData.DialogBoxHandle, &msg ) == 0 )
						{
							// ダイアログボックスのメッセージではなかったら普通に処理
							// (ウインドウモードの時のみ)
							if( msg.message == WM_ACTIVATEAPP && WinData.WindowModeFlag == TRUE )
							{
								// アクティブかどうかを保存
								WinData.ActiveFlag = LOWORD( msg.wParam ) ;
							}

							if( WinData.QuitMessageFlag ) goto R1 ;
							if( msg.message == WM_QUIT ) 
							{
								WinData.QuitMessageFlag = TRUE ;
								DxLib_EndRequest() ;
							}

							// アクセラレータが有効な場合はアクセラレータキーを処理する
							if( WinAPIData.Win32Func.TranslateAcceleratorWFunc( WinData.MainWindow, WinData.Accel, &msg ) == 0 )
							{
//								DXST_LOGFILEFMT_ADDUTF16LE(( L"DispatchMessage\n" )) ;

								// アクセラレータキーメッセージではなかったら普通に処理する
								WinAPIData.Win32Func.TranslateMessageFunc( &msg );
								WinAPIData.Win32Func.DispatchMessageWFunc( &msg );
							}
							EventCon ++ ;
//							if ( EventCon >= MAX_EVENTPROCESS_NUM )
//							{
//								EventCon = 0 ;
//								Flag = TRUE ;
//
//								goto R2;
//							}

							Flag = FALSE ;
						}
						else
						{
							Flag = FALSE ;
						}
					}

					// ダイアログボックスのメッセージ処理
					if( WinData.DialogBoxHandle != NULL )
					{
						if( WinAPIData.Win32Func.PeekMessageWFunc( &msg, WinData.DialogBoxHandle, 0, 0, PM_REMOVE ) )
						{
							WinAPIData.Win32Func.IsDialogMessageWFunc( WinData.DialogBoxHandle, &msg );
						}
					}

					// 子ウインドウのメッセージ処理
					{
						int i ;

						for( i = 0 ; i < WinData.MesTakeOverWindowNum ; i ++ )
						{
							if( WinAPIData.Win32Func.PeekMessageWFunc( &msg, WinData.MesTakeOverWindow[i], 0, 0, PM_REMOVE ) )
							{
								WinAPIData.Win32Func.TranslateMessageFunc( &msg );
								WinAPIData.Win32Func.DispatchMessageWFunc( &msg );
							}
						}
					}
				}

//		R2 :
				if( ( WinData.ActiveFlag == FALSE || WinData.WindowMinSizeFlag == TRUE ) && WinData.NonActiveRunFlag == FALSE )
				{
					// メモリ関係の周期的処理を行う
					MemoryProcess() ;

#ifndef DX_NON_NETWORK
					// 通信関係のメッセージ処理を行う
					NS_ProcessNetMessage( TRUE ) ;

#if 0
					// HTTP 処理を行う
					HTTP_ProcessAll() ;
#endif
#endif

#ifndef DX_NON_SOUND

#ifndef DX_NON_MULTITHREAD
					if( WinData.ProcessorNum <= 1 )
#endif // DX_NON_MULTITHREAD
					{
						// サウンドの周期的処理を行う
						NS_ProcessStreamSoundMemAll() ;
						ST_SoftSoundPlayerProcessAll() ;
						ProcessPlay3DSoundMemAll() ;
					}
					ProcessPlayFinishDeleteSoundMemAll() ;
#endif // DX_NON_SOUND

#ifndef DX_NON_GRAPHICS
					// 描画処理のメッセージループ時に行うべき処理を実行する
					Graphics_Win_MessageLoop_Process() ;
#endif // DX_NON_GRAPHICS

#ifndef DX_NON_ASYNCLOAD
					// メインスレッドが処理する非同期読み込みの処理を行う
					ProcessASyncLoadRequestMainThread() ;
#endif // DX_NON_ASYNCLOAD

					WinAPIData.Win32Func.SleepFunc( 24 ) ;
				}

				if( WinData.QuitMessageFlag ) goto R1 ;

				// メインスレッドではなかったら直ぐ抜ける
				if( WinData.MainThreadID != WinAPIData.Win32Func.GetCurrentThreadIdFunc() ) break ;

				// DxLib_End が呼ばれていたらループ１０回で抜ける
				LoopCount ++ ;
				if( WinData.AltF4_EndFlag && LoopCount >= 10 )
					break ;
			}

		R1 :
			// メニューが有効になっているときの処理
			if( WinData.MainThreadID == WinAPIData.Win32Func.GetCurrentThreadIdFunc() )
			{
				// メニューを自動的に表示したり非表示にしたりする
				// フラグが立っている場合はその処理を行う
				if( WinData.NotMenuAutoDisplayFlag == FALSE )
					MenuAutoDisplayProcess() ;

				// キーボードの入力状態を更新
	//			UpdateKeyboardInputState() ;
			}

			// メニューが有効になっているときの処理
		/*	{
				// 既にメニュー処理が行われているか、メニューが無効だった場合は何もしない
				if( DxSysData.DxLib_InitializeFlag == TRUE && WinData.MenuUseFlag == TRUE && WinData.MenuShredRunFlag == FALSE )
				{
					int Key ;

					// エスケープキーか指定のキーが押されたらメニュー処理を開始
					Key = ( WinData.MenuStartKey == 0 ) ? KEY_INPUT_ESCAPE : WinData.MenuStartKey ;
					if( CheckHitKey( Key ) == 1 )
					{
						MenuProcess() ;
					}
				}
			}
		*/
			// もしロック指定があったらロックする
		/*	if( WinData.SysCommandOffFlag == TRUE )
			{
				if( WinData.LockInitializeFlag == TRUE )
				{
					LockSetForegroundWindow( LSFW_LOCK ) ;
					WinData.LockInitializeFlag = FALSE ;
				}
			}
		*/
		}

		// WM_ACTIVATE メッセージ処理のリクエストがある場合はここで処理する
		if( WinData.CloseMessagePostFlag == FALSE )
		{
			while( WinData.WM_ACTIVATE_StockNum != 0 )
			{
				WPARAM wParam ;
				LPARAM lParam ;
				int APPMes ;

				wParam = WinData.WM_ACTIVATE_wParam[ WinData.WM_ACTIVATE_StartIndex ] ;
				lParam = WinData.WM_ACTIVATE_lParam[ WinData.WM_ACTIVATE_StartIndex ] ;
				APPMes = WinData.WM_ACTIVATE_APPMes[ WinData.WM_ACTIVATE_StartIndex ] ;
				WinData.WM_ACTIVATE_StartIndex = ( WinData.WM_ACTIVATE_StartIndex + 1 ) % 512 ;
				WinData.WM_ACTIVATE_StockNum -- ;
				WM_ACTIVATEProcess( wParam, lParam, APPMes ) ;
			}
		}
	}

	// ファイル処理の周期的処理を行う
//	ReadOnlyFileAccessProcessAll() ;

#ifndef DX_NON_SOUND

#ifndef DX_NON_MULTITHREAD
	if( WinData.ProcessorNum <= 1 )
#endif // DX_NON_MULTITHREAD
	{
		// サウンドの周期的処理を行う
		NS_ProcessStreamSoundMemAll() ;
		ST_SoftSoundPlayerProcessAll() ;
		ProcessPlay3DSoundMemAll() ;
	}
	ProcessPlayFinishDeleteSoundMemAll() ;
#endif // DX_NON_SOUND

#ifndef DX_NON_GRAPHICS
	// 描画処理のメッセージループ時に行うべき処理を実行する
	Graphics_Win_MessageLoop_Process() ;
#endif // DX_NON_GRAPHICS

#ifndef DX_NON_ASYNCLOAD
	// メインスレッドが処理する非同期読み込みの処理を行う
	if( WinData.QuitMessageFlag == FALSE )
	{
		ProcessASyncLoadRequestMainThread() ;
	}
#endif // DX_NON_ASYNCLOAD

	// 演奏の周期的処理を行う
#ifndef DX_NON_SOUND
	NS_ProcessMusicMem() ;
#endif // DX_NON_SOUND

#ifndef DX_NON_INPUT
	// キーボードの周期的処理を行う
	//KeyboradBufferProcess() ;

	// キーボード入力の更新処理を行う
	UpdateKeyboardInputState( FALSE ) ;

	// パッドの周期的処理を行う
	JoypadEffectProcess() ;
#endif // DX_NON_INPUT

#ifndef DX_NON_NETWORK
	// 通信関係のメッセージ処理を行う
	NS_ProcessNetMessage( TRUE ) ;

#if 0
	// HTTP 処理を行う
	HTTP_ProcessAll() ;
#endif
#endif

	// メモリ関係の周期的処理を行う
	MemoryProcess() ;

#ifndef DX_NON_KEYEX
	// キー入力処理を行う
	{
		// フラグをたてる
		EndFlag = TRUE ;

		NS_ProcessActKeyInput() ;

		// フラグを倒す
		EndFlag = FALSE ;
	}
#endif
END :

	// ProcessMessage を実行中フラグを倒す
	WinData.ProcessMessageFlag = FALSE ;

	return WinData.QuitMessageFlag == TRUE ? -1 : 0 ;
}










// エラー処理関数

// エラー処理
extern int DxLib_Error( const wchar_t *ErrorStr )
{
	// エラーログの排出
	DXST_LOGFILE_ADDW( ErrorStr ) ;
	DXST_LOGFILE_ADDW( L"\n" ) ;

	// 各処理系の終了
	NS_DxLib_End() ;

	SETUP_WIN_API

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )
	// クリティカルセクションとイベントハンドルを解放する
	if( WinData.DxConflictWaitThreadIDInitializeFlag == TRUE )
	{
		int i ;

		// 初期化フラグを倒す
		WinData.DxConflictWaitThreadIDInitializeFlag = FALSE ;

		// 衝突時に使用するイベントの解放
		for( i = 0 ; i < MAX_THREADWAIT_NUM ; i ++ )
		{
			if( WinData.DxConflictWaitThreadID[i][1] != 0 )
				WinAPIData.Win32Func.CloseHandleFunc( (HANDLE)WinData.DxConflictWaitThreadID[i][1] ) ;
			WinData.DxConflictWaitThreadID[i][1] = 0 ;
		}

		// クリティカルセクションも削除する
		CriticalSection_Delete( &WinData.DxConflictCheckCriticalSection ) ;
	}
#endif // defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

//	while( WinData.QuitMessageFlag == FALSE && NS_ProcessMessage() == 0 ){}

//	DXST_LOGFILE_ADDW( L"ソフトをエラー終了します" ) ;
	WinAPIData.Win32Func.ExitProcessFunc( (DWORD)-1 ) ;

	return -1 ;
}

// ライブラリのエラー処理を行う( UTF16LE版 )
extern int DxLib_ErrorUTF16LE( const char *ErrorStr )
{
	int Result ;

	CHAR_TO_WCHAR_T_STRING_BEGIN( ErrorStr )
	CHAR_TO_WCHAR_T_STRING_SETUP( ErrorStr, return -1, DX_CHARCODEFORMAT_UTF16LE )

	Result = DxLib_Error( UseErrorStrBuffer ) ;

	CHAR_TO_WCHAR_T_STRING_END( ErrorStr )

	return Result ;
}

































// カウンタ及び時刻取得系関数

// ミリ秒単位の精度を持つカウンタの現在値を得る
extern int NS_GetNowCount( int /*UseRDTSCFlag*/ )
{
#if 0
#ifndef DX_NON_INLINE_ASM
	// 当分 RDTSC は使用しない(一秒間辺りのカウント数を正確に取得するには時間が掛かり過ぎて、起動時間
	// の増大に繋がる為)
//	if( WinData.UseRDTSCFlag == TRUE && UseRDTSCFlag == TRUE && DxSysData.DxLib_InitializeFlag == TRUE )
	if( FALSE )
	{
		LARGE_INTEGER Clock ;

#ifdef __BCC
		unsigned int low, high ;
		unsigned int lowP, highP ;
		
		lowP = (unsigned int)&low ;
		highP = (unsigned int)&high ;

		__asm
		{
			db		0x0f
			db		0x31
			MOV		EDI, lowP
			MOV		[EDI], EAX
			MOV		EDI, highP
			MOV		[EDI], EDX
//			MOV		low , EAX
//			MOV		high , EDX
		}
		
		Clock.LowPart = low ;
		Clock.HighPart = high ;
#else
		__asm
		{
//			db		0fh
//			db		031h
		RDTSC
			MOV		Clock.LowPart , EAX
			MOV		Clock.HighPart , EDX
		}
#endif
		return ( int )( _DTOL( ( double )Clock.QuadPart * 1000.0 / WinData.OneSecCount ) & 0x7fffffff ) ;
	}
	else
#endif // DX_NON_INLINE_ASM
#endif // 0
	{
		SETUP_WIN_API

		if( WinAPIData.Win32Func.WinMMDLL == NULL )
		{
			LoadWinAPI() ;
			if( WinAPIData.Win32Func.WinMMDLL == NULL )
			{
				return -1 ;
			}
		}

		return ( int )( WinAPIData.Win32Func.timeGetTimeFunc() & 0x7fffffff ) ;
	}
}

// GetNowTimeの高精度バージョン
extern LONGLONG NS_GetNowHiPerformanceCount( int /*UseRDTSCFlag*/ )
{
	LONGLONG Result, MulNum ;
	unsigned int /*FPUStatus,*/ Temp[4] ;

#if 0
#ifndef DX_NON_INLINE_ASM
//	FPUStatus = 0x00020000 ;
	// 当分 RDTSC は使用しない(一秒間辺りのカウント数を正確に取得するには時間が掛かり過ぎて、起動時間
	// の増大に繋がる為)
//	if( WinData.UseRDTSCFlag == TRUE && UseRDTSCFlag == TRUE && DxSysData.DxLib_InitializeFlag == TRUE )
	if( FALSE )
	{
		LARGE_INTEGER Clock ;

#ifdef __BCC
		unsigned int low, high ;
		unsigned int lowP, highP ;
		
		lowP = (unsigned int)&low ;
		highP = (unsigned int)&high ;

		__asm
		{
			db		0x0f
			db		0x31
			MOV		EDI, lowP
			MOV		[EDI], EAX
			MOV		EDI, highP
			MOV		[EDI], EDX
//			MOV		low , EAX
//			MOV		high , EDX
		}

		Clock.LowPart = low ;
		Clock.HighPart = high ;
#else
		__asm
		{
		RDTSC
			MOV		Clock.LowPart, EAX
			MOV		Clock.HighPart, EDX
		}
#endif
		// 精度設定を上げる
//		FPUStatus = _control87( 0x00000000, 0x00030000 ) ;
		MulNum = 1000000;
		_MUL128_1( (DWORD *)&MulNum, (DWORD *)&Clock.QuadPart, (DWORD *)Temp );
		_DIV128_1( (DWORD *)Temp, (DWORD *)&WinData.OneSecCount, (DWORD *)&Result );
//		Result = _DTOL64( ( double )Clock.QuadPart * 1000000.0 / WinData.OneSecCount ) ;
	}
	else
#endif  // DX_NON_INLINE_ASM
#endif // 0
	{
		SETUP_WIN_API

		if( WinData.PerformanceTimerFlag )
		{
			LONGLONG NowTime ;

			// パフォーマンスカウンタからカウントを得る
			WinAPIData.Win32Func.QueryPerformanceCounterFunc( ( LARGE_INTEGER * )&NowTime ) ;

//			// 精度設定を上げる
//			FPUStatus = _control87( 0x00000000, 0x00030000 ) ;
			MulNum = 1000000;
			_MUL128_1( (DWORD *)&MulNum, (DWORD *)&NowTime, (DWORD *)Temp );
			_DIV128_1( (DWORD *)Temp, (DWORD *)&WinData.PerformanceClock, (DWORD *)&Result );
//			Result = ( LONGLONG )_DTOL64( ( double )NowTime / ( double )WinData.PerformanceClock * 1000000.0 ) ;
		}
		else
		{
			if( WinAPIData.Win32Func.WinMMDLL == NULL )
				return -1 ;

			// パフォーマンスカウンタがない場合は通常のカウンタに１０００を掛けた値を返す
//			Result = ( LONGLONG )_DTOL64( (double)WinAPIData.Win32Func.timeGetTimeFunc() * 1000 ) ;
			MulNum = 1000;
			((DWORD *)&Result)[0] = (DWORD)WinAPIData.Win32Func.timeGetTimeFunc();
			((DWORD *)&Result)[1] = 0;
			_MUL128_1( (DWORD *)&MulNum, (DWORD *)&Result, (DWORD *)Temp );
			((DWORD *)&Result)[0] = Temp[0];
			((DWORD *)&Result)[1] = Temp[1];
		}
	}

	// 精度設定を元に戻す
//	_control87( FPUStatus, 0x00030000 ) ;

	return Result ;
}

// 現在時刻を取得する
extern int NS_GetDateTime( DATEDATA *DateBuf )
{
	SYSTEMTIME LocalTime ;

	SETUP_WIN_API

	// ローカル時刻を得る	
	WinAPIData.Win32Func.GetLocalTimeFunc( &LocalTime ) ;
	
	// ローカル時刻データを元に専用のデータ型データに時刻を繁栄させる
	DateBuf->Year	= LocalTime.wYear ;
	DateBuf->Mon	= LocalTime.wMonth ;
	DateBuf->Day	= LocalTime.wDay ;
	DateBuf->Hour	= LocalTime.wHour ;
	DateBuf->Min	= LocalTime.wMinute ;
	DateBuf->Sec	= LocalTime.wSecond ;

	// 終了
	return 0 ;
}







// 乱数取得

#ifndef DX_NON_MERSENNE_TWISTER

// 乱数の初期値を設定する
extern int NS_SRand( int Seed )
{
	// 初期値セット
	srandMT( ( unsigned long )Seed ) ;

	// 終了
	return 0 ;
}

// 乱数を取得する( RandMax : 返って来る値の最大値 )
extern int NS_GetRand( int RandMax )
{
	int Result ;
	LONGLONG RandMaxLL ;

	RandMaxLL = RandMax ;
	RandMaxLL ++ ;
	Result = ( int )( ( ( LONGLONG )randMT() * RandMaxLL ) >> 32 ) ;

	return Result ;
}

#else // DX_NON_MERSENNE_TWISTER

// 乱数の初期値を設定する
extern int NS_SRand( int Seed )
{
	// 初期値セット
	srand( Seed ) ;

	// 終了
	return 0 ;
}

// 乱数を取得する( RandMax : 返って来る値の最大値 )
extern int NS_GetRand( int RandMax )
{
	int Result ;
	LONGLONG RandMaxLL ;

	RandMaxLL = RandMax ;
	RandMaxLL ++ ;
	Result = ( int )( ( ( LONGLONG )rand() * RandMaxLL ) / ( ( LONGLONG )RAND_MAX + 1 ) ) ;

	return Result ;
}

#endif // DX_NON_MERSENNE_TWISTER



// バッテリー関連

// 電池の残量を % で取得する( 戻り値： 100=フル充電状態  0=充電残量無し )
extern int NS_GetBatteryLifePercent( void )
{
	SYSTEM_POWER_STATUS SystemPowerStatus ;

	SETUP_WIN_API

	if( WinAPIData.Win32Func.GetSystemPowerStatusFunc == NULL )
	{
		return -1 ;
	}

	if( WinAPIData.Win32Func.GetSystemPowerStatusFunc( &SystemPowerStatus ) == 0 )
	{
		return -1 ;
	}

	if( SystemPowerStatus.ACLineStatus == AC_LINE_ONLINE )
	{
		return 100 ;
	}

	if( SystemPowerStatus.BatteryLifePercent == BATTERY_PERCENTAGE_UNKNOWN )
	{
		return -1 ;
	}

	return SystemPowerStatus.BatteryLifePercent ;
}




// クリップボード関係

#ifdef UNICODE
	#define CLIPBOARD_TEXT CF_UNICODETEXT
#else
	#define CLIPBOARD_TEXT CF_TEXT
#endif

// クリップボードに格納されているテキストデータを読み出す、-1 の場合はクリップボードにテキストデータは無いということ( DestBuffer に NULL を渡すと格納に必要なデータサイズが返ってくる )
extern int GetClipboardText_PF( TCHAR *DestBuffer )
{
	HGLOBAL Mem ;
	void *SrcBuffer ;

	SETUP_WIN_API

	// クリップボードをオープン
	if( WinAPIData.Win32Func.OpenClipboardFunc( WinData.MainWindow ) == 0 )
		return -1 ;

	// クリップボードに格納されているデータがテキストデータかどうかを取得する
	if( WinAPIData.Win32Func.IsClipboardFormatAvailableFunc( CLIPBOARD_TEXT ) == 0 )
	{
		WinAPIData.Win32Func.CloseClipboardFunc() ;
		return -1 ;
	}

	// クリップボードに格納されているテキストデータのメモリハンドルを取得する
	Mem = WinAPIData.Win32Func.GetClipboardDataFunc( CLIPBOARD_TEXT ) ;

	// 出力バッファが NULL の場合はテキストデータのサイズを返す
	if( DestBuffer == NULL )
	{
		SIZE_T Size ;

		Size = WinAPIData.Win32Func.GlobalSizeFunc( Mem ) + 1 ;
		WinAPIData.Win32Func.CloseClipboardFunc() ;
		return ( int )Size ;
	}

	// 出力バッファにテキストデータをコピーする
	SrcBuffer = WinAPIData.Win32Func.GlobalLockFunc( Mem ) ;
	_TSTRCPY( DestBuffer, ( TCHAR * )SrcBuffer ) ;
	WinAPIData.Win32Func.GlobalUnlockFunc( Mem ) ;
	WinAPIData.Win32Func.CloseClipboardFunc() ;

	// 終了
	return 0 ;
}

// クリップボードに格納されているテキストデータを読み出す、-1 の場合はクリップボードにテキストデータは無いということ( DestBuffer に NULL を渡すと格納に必要なデータサイズが返ってくる )
extern int GetClipboardText_WCHAR_T_PF( wchar_t *DestBuffer )
{
	HGLOBAL Mem ;
	void *SrcBuffer ;

	SETUP_WIN_API

	// クリップボードをオープン
	if( WinAPIData.Win32Func.OpenClipboardFunc( WinData.MainWindow ) == 0 )
		return -1 ;

	// クリップボードに格納されているデータがテキストデータかどうかを取得する
	if( WinAPIData.Win32Func.IsClipboardFormatAvailableFunc( CF_UNICODETEXT ) == 0 )
	{
		WinAPIData.Win32Func.CloseClipboardFunc() ;
		return -1 ;
	}

	// クリップボードに格納されているテキストデータのメモリハンドルを取得する
	Mem = WinAPIData.Win32Func.GetClipboardDataFunc( CF_UNICODETEXT ) ;

	// 出力バッファが NULL の場合はテキストデータのサイズを返す
	if( DestBuffer == NULL )
	{
		SIZE_T Size ;

		Size = WinAPIData.Win32Func.GlobalSizeFunc( Mem ) + 1 ;
		WinAPIData.Win32Func.CloseClipboardFunc() ;
		return ( int )Size ;
	}

	// 出力バッファにテキストデータをコピーする
	SrcBuffer = WinAPIData.Win32Func.GlobalLockFunc( Mem ) ;
	_WCSCPY( DestBuffer, ( wchar_t * )SrcBuffer ) ;
	WinAPIData.Win32Func.GlobalUnlockFunc( Mem ) ;
	WinAPIData.Win32Func.CloseClipboardFunc() ;

	// 終了
	return 0 ;
}

// クリップボードにテキストデータを格納する
extern int SetClipboardText_WCHAR_T_PF( const wchar_t *Text )
{
	HGLOBAL Mem ;
	void *Buffer ;
	int Len ;

	SETUP_WIN_API

	// 文字列の長さを取得
	Len = ( int )_WCSLEN( Text ) ;

	// 文字列を格納するメモリ領域の確保
	Mem = WinAPIData.Win32Func.GlobalAllocFunc( GMEM_FIXED, ( Len + 1 ) * sizeof( wchar_t ) ) ;

	// 文字列を確保したメモリ領域に格納
	Buffer = WinAPIData.Win32Func.GlobalLockFunc( Mem ) ;
	_WCSCPY( ( wchar_t * )Buffer, Text ) ;
	WinAPIData.Win32Func.GlobalUnlockFunc( Mem ) ;

	// クリップボードをオープン
	if( WinAPIData.Win32Func.OpenClipboardFunc( WinData.MainWindow ) )
	{
		// クリップボードに文字列を格納する
		WinAPIData.Win32Func.EmptyClipboardFunc() ;
		WinAPIData.Win32Func.SetClipboardDataFunc( CF_UNICODETEXT, Mem ) ;
		WinAPIData.Win32Func.CloseClipboardFunc() ;
	}
	else
	{
		// メモリの解放
		WinAPIData.Win32Func.GlobalFreeFunc( Mem ) ;
	}

	// 終了
	return 0 ;
}













#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE





