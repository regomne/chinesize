// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ウインドウプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXWINDOW_H__
#define __DXWINDOW_H__

// インクルード ------------------------------------------------------------------
#include "../DxCompileConfig.h"
#include "../DxLib.h"
#include "../DxMemory.h"
#include "../DxFile.h"
#include "../DxThread.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define MAX_MENUITEMSELECT_NUM		(64)				// 保持しておける選択されたメニュー項目の最大数
#define MAX_MENUITEM_NUM			(128)				// メニュー項目の最大数
#define MAX_ACCELERATOR_NUM			(64)				// アクセラレータの最大数
#define MAX_TOOLBARITEM_NUM			(64)				// ツールバーに置けるアイテムの最大数

#define MAX_MESTAKEOVERWIN_NUM		(100)				// メッセージプロセスを肩代わりするウインドウの最大数
#define MAX_DRAGFILE_NUM			(1000)				// ドラッグ&ドロップされたファイル名を保存する最高数
#define MAX_THREADWAIT_NUM			(4096)				// 同時に待つことが出来るスレッドの最大数

typedef HHOOK 						( *MSGFUNC )( HWND MainWindow, HHOOK *pKeyboardHookHandle, int Enable ) ;		// メッセージフック設定関数

#define WSA_WINSOCKMESSAGE 			(WM_USER + 261)

// 構造体定義 --------------------------------------------------------------------

// メニュー項目の情報
struct WINMENUITEMINFO
{
	HMENU					Menu ;								// メニュー
	short					Index ;								// ナンバー
	unsigned short			ID ;								// ＩＤ
	wchar_t					Name[128] ;							// 名前
} ;

// ツールバー項目の情報
struct WINTOOLBARITEMINFO
{
	int						ID ;								// ID
	int						ImageIndex ;						// 画像のインデックス
	int						Type ;								// タイプ
	int						State ;								// 状態
	int						Click ;								// クリックされたか
} ;

// ＰＣの情報
struct PCINFO
{
	wchar_t					OSString[256] ;						// ＯＳの記述
	wchar_t					DirectXString[256] ;				// ＤｉｒｅｃｔＸの記述
	wchar_t					CPUString[256] ;					// ＣＰＵの記述
	int						CPUSpeed ;							// ＣＰＵの速度(単位MHz)
	LONGLONG				FreeMemorySize ;					// 空きメモリサイズ(単位byte)
	LONGLONG				TotalMemorySize ;					// 総メモリサイズ(単位byte)
	wchar_t					VideoDriverFileName[256] ;			// ビデオカードドライバファイル名
	wchar_t					VideoDriverString[256] ;			// ビデオカードドライバの記述
	unsigned int			VideoFreeMemorySize ;				// 空きＶＲＡＭサイズ(単位byte)
	unsigned int			VideoTotalMemorySize ;				// ＶＲＡＭの総サイズ(単位byte)
} ;

#ifndef DX_NON_INPUT
// マウスの入力情報
struct WINMOUSEINPUT
{
	int						LogType ;							// ログタイプ( MOUSE_INPUT_LOG_DOWN 又は MOUSE_INPUT_LOG_UP )
	int						Button ;							// クリックしたボタン
	int						ClickX, ClickY ;					// クリックしたクライアント座標
	int						IsDummy ;							// GetMousePoint と GetMouseInput を使用したダミー情報かどうか
} ;
#endif // DX_NON_INPUT

// ウインドウ系データ構造体
struct WINDATA
{
	PCINFO					PcInfo ;							// ＰＣの情報
	HINSTANCE				Instance ;							// ソフトのインスタンスハンドル
	HWND					MainWindow ;						// メインウインドウハンドル
	HWND					UserChildWindow ;					// 表示用子ウインドウハンドル

	int						ProcessorNum ;						// 論理ＣＰＵコアの数
	int						AeroDisableFlag ;					// Aeroを無効にしているかどうかのフラグ
	int						SystembarHeight ;					// システムバーの高さ

#ifdef DX_THREAD_SAFE
	HANDLE					ProcessMessageThreadHandle ;		// ProcessMessage をひたすら呼びつづけるプロセスのハンドル
	DWORD					ProcessMessageThreadID ;			// ProcessMessage をひたすら呼びつづけるプロセスのＩＤ
	DWORD					ProcessMessageThreadExitFlag ;		// ProcessMessage をひたすら呼びつづけるプロセスが終了時に立てるフラグ
#endif

	HRGN					WindowRgn ;							// メインウインドウのリージョン
	int						WindowStyle ;						// ウインドウのスタイルタイプインデックス値
	int						WindowZType ;						// ウインドウのＺオーダータイプ( DX_WIN_ZTYPE_NORMAL など )
	int						WindowZTypeNoActivateFlag ;			// WindowZType に伴う SetWindowPos 呼び出しの際にウィンドウをアクティブにしないかどうかのフラグ( TRUE:アクティブにしない  FALSE:アクティブにする )
	int						UserWindowFlag ;					// MainWindow はＤＸライブラリが作成したウインドウではないか、フラグ(ＴＲＵＥ：ユーザーから渡されたウインドウ  ＦＡＬＳＥ：ＤＸライブラリが作成したウインドウ)
	int						NotUserWindowMessageProcessDXLibFlag ;	// UserWindowFlag が立っている場合、ウインドウのメッセージ処理をＤＸライブラリが行わないかどうか、フラグ(ＴＲＵＥ：ＤＸライブラリは何もせずユーザーがメッセージ処理を行う　ＦＡＬＳＥ：ＤＸライブラリが行う)
	WNDPROC					DefaultUserWindowProc ;				// ＤＸライブラリのプロシージャを設定する前にユーザーのウインドウに設定されていたプロシージャ
	DWORD					MainThreadID ;						// InitializeWindow を呼び出したスレッドのＩＤ


	RECT					WindowEdgeRect ;					// ウインドウモードで動作している時のウインドウの矩形領域
	RECT					WindowRect ;						// ウインドウモードで動作している時のウインドウのクライアント領域
	int						WindowModeFlag ;					// ウインドウモードで動作しているか、のフラグ
	int						ChangeWindodwFlag ;					// ウインドウモードに変更中か、フラグ
	int						UseChangeWindowModeFlag ;			// ALT+ENTER によるフルスクリーン←→ウインドウの変更機能が有効であるかフラグ
	void					(*ChangeWindowModeCallBackFunction)(void *) ; // フルスクリーン←→ウインドウの変更が起きたときに呼ぶ関数
	void					*ChangeWindowModeCallBackFunctionData ;	// フルスクリーン←→ウインドウの変更が起きたときに呼ぶ関数に渡すデータ
	void					(*ShutdownCallBackFunction)(void *) ; // シャットダウンによるソフトの強制終了が発生した際に呼ばれるコールバック関数
	void					*ShutdownCallBackFunctionData ;		// シャットダウンによるソフトの強制終了が発生した際に呼ばれるコールバック関数に渡すデータ
	wchar_t					ShutdownMessage[ 512 ] ;			// シャットダウンによるソフトの強制終了が発生した際に呼ばれるコールバック関数の処理が実行されている間に表示されるメッセージ

	wchar_t					InputSysChara ;						// 入力されたシステム文字コード

	int						QuitMessageFlag ;					// WM_QUITメッセージが送られてきたかどうかのフラグ変数
	int						CloseMessagePostFlag ;				// WM_CLOSEメッセージを送った時にＴＲＵＥになるフラグ変数
	int						DestroyMessageCatchFlag ;			// WM_DESTROY メッセージが来た時にＴＲＵＥになるフラグ変数
	int						DisplayChangeMessageFlag ;			// WM_DISPLAYCHANGE メッセージが来かたフラグ
	
	int						ScreenCopyRequestFlag ;				// ScreenCopy をして欲しいかどうかのフラグ
	int						ScreenCopyRequestStartTime ;		// ScreenCopy をして欲しいかどうかのフラグが立ったときの時間
	int						ActiveFlag ;						// 実行中のソフトがアクティブ状態か（ＴＲＵＥ：アクティブ ＦＡＬＳＥ：非アクティブ）
	int						WindowMinSizeFlag ;					// 最小化されているかどうかのフラグ
	int						WaitTimeValidFlag ;					// WaitTime が有効かどうかのフラグ
	int						WaitTime ;							// 他のソフトがアクティブになり、ソフトが停止し始めた時のカウント
	int						StopFlag ;							// 一時的にオブジェクトをリリースしているかフラグ
	int						ChangeWindowModeFlag ;				// ウインドウモードの変更を行う指定が起こったらＴＲＵＥになるフラグ
	int						NonUserCloseEnableFlag ;			// ユーザーが×ボタンを押した時にライブラリ側でウインドウを閉じるかどうかのフラグ
	int						NonDxLibEndPostQuitMessageFlag ;	//ＤＸライブラリの終了時に PostQuitMessage を呼ばないかどうかのフラグ
	int						AltF4_EndFlag ;						// 強制終了か、フラグ
	int						WM_PAINTFlag ;						// WM_PAINT 処理中か、フラグ
	int						RecvWM_PAINTFlag;					// WM_PAINT が来たかどうか、フラグ
	int						NotUseDxLibWM_PAINTProcess ;		// ＤＸライブラリの WM_PAINT の処理を実行しないかのフラグ
	int						UseFPUPreserve ;					// FPU精度を落とさない設定を使用するかどうか( TRUE:使用する  FALSE:使用しない )

	int						MouseDispFlag ;						// マウスの表示フラグ
	int						MouseDispState ;					// マウスの表示状態
	int						MouseMoveZ ;						// マウスのホイールの移動量
	int						MouseMoveHZ ;						// マウスの水平ホイールの移動量

	int						WM_ACTIVATE_StockNum ;				// WM_ACTIVATE メッセージストックの数
	int						WM_ACTIVATE_StartIndex ;			// WM_ACTIVATE メッセージリングバッファの開始インデックス
	int						WM_ACTIVATE_EndIndex ;				// WM_ACTIVATE メッセージリングバッファの終了インデックス
	WPARAM					WM_ACTIVATE_wParam[ 512 ] ;			// WM_ACTIVATE にメッセージが来た祭の wParam
	LPARAM					WM_ACTIVATE_lParam[ 512 ] ;			// WM_ACTIVATE にメッセージが来た祭の lParam
	int						WM_ACTIVATE_APPMes[ 512 ] ;			// WM_ACTIVATEAPP の肩代わりか
	int						WM_ACTIVATE_Dummy[ 512 ] ;			// WM_ACTIVATE のダミーメッセージかどうか
	int						WM_ACTIVATE_ProcessFlag ;			// WM_ACTIVATEProcess を実行中かどうか
	int						WM_CHAR_Time ;						// WM_CHAR が発生した時間
	WPARAM					WM_CHAR_wParam ;					// WM_CHAR の wParam

	int						PerformanceTimerFlag ;				// パフォーマンスカウンターが使えるかフラグ
	LONGLONG				PerformanceClock ;					// パフォーマンスカウンターの周波数

//	SIZE					DefaultScreenSize ;					// 起動時のデスクトップの画面サイズ
//	int						DefaultColorBitCount ;				// 起動時のデスクトップのカラービット数
//	int						DefaultRefreshRate ;				// 起動時のデスクトップのリフレッシュレート

	int						ComInitializeFlag ;					// ＣＯＭを初期化したか、のフラグ
	int						WindowCreateFlag ;					// ウインドウ作成中か、フラグ
	int						ProcessMessageFlag ;				// ProcessMessage を実行中か、フラグ
	int						DoubleStartValidFlag ;				// ２重起動を許すかどうかのフラグ(TRUE:許す FALSE:許さない)

	BASEIMAGE				PauseGraph ;						// アクティブが他に移っているときに描画する画像
	int						PauseGraphHandle ;					// アクティブが他に移っているときに描画する画像のグラフィックハンドル
	int						(*ActiveStateChangeCallBackFunction)( int ActiveState, void *UserData ) ;	// ウインドウのアクティブ状態に変化があったときに呼ばれる関数
	void					*ActiveStateChangeCallBackFunctionData ;	// ActiveStateChangeCallBackFunction に渡すデータアドレス

	wchar_t					CurrentDirectory[ FILEPATH_MAX ] ;	// 起動時のカレントディレクトリ
	int						EnableWindowText ;					// WindowText が有効かどうか
	wchar_t					WindowText[ 256 ] ;					// メインウインドウテキスト
	wchar_t					ClassName[ 256 ] ;					// メインウインドウのクラス名

	int						NonActiveRunFlag ;					// ウインドウがアクティブではなくても処理を実行するかどうかのフラグ
	int						DrawBackGraphFlag ;					// DrawBackGraph 関数を実行中かどうか( TRUE:実行中  FALSE:実行中ではない )

	int						IconID ;							// 使用するアイコンのＩＤ
	HICON					IconHandle ;						// 使用するアイコンのハンドル

	int						UseAccelFlag ;						// アクセラレーターを使用するかどうかフラグ
	HACCEL					Accel ;								// アクセラレーター

	HMENU					Menu ;								// 使用しているメニューのハンドル
	int						MenuSetupFlag ;						// メニューをセットしてあるかどうかフラグ( TRUE:セットしてある  FALSE:してない )
	int						MenuUseFlag ;						// メニューを使用しているかフラグ
	int						MenuResourceID ;					// メニューのリソースＩＤ
	int						(*MenuProc)( WORD ID ) ;			// メニューメッセージのコールバック関数
	void					(*MenuCallBackFunction)( const TCHAR *ItemName, int ItemID ) ;	// メニューメッセージのコールバック関数
	int						NotMenuDisplayFlag ;				// メニュー表示フラグ
	int						MenuDisplayState ;					// メニューが表示されているかどうかフラグ
	int						NotMenuAutoDisplayFlag ;			// メニューを自動で表示したりしなかったりする
	int						MousePosInMenuBarFlag ;				// メニューバーの領域にマウスポインタが入っているか、フラグ
	int						SelectMenuItem[MAX_MENUITEMSELECT_NUM] ; // 選択されたメニュー項目のリスト
	int						SelectMenuItemNum ;					// 選択されたメニュー項目の数
	WINMENUITEMINFO			MenuItemInfo[MAX_MENUITEM_NUM] ;	// メニューの選択項目の情報
	int						MenuItemInfoNum ;					// メニューの選択項目の数

	int						ToolBarUseFlag ;					// ツールバーを使用するかどうかの情報
	HWND					ToolBarHandle ;						// ツールバーのウインドウハンドル
	HBITMAP					ToolBarButtonImage ;				// ツールバーのボタンのビットマップ
	int						ToolBarItemNum ;					// ツールバーのアイテムの数
	WINTOOLBARITEMINFO		ToolBarItem[MAX_TOOLBARITEM_NUM] ;	// ツールバーのボタンの情報

	int						WindowSizeChangeEnable ;			// ウインドウのサイズを変更できるかフラグ
	double					WindowSizeExRateX, WindowSizeExRateY ;	// 描画画面のサイズに対するウインドウサイズの比率
	int						ScreenNotFitWindowSize ;			// ウインドウのクライアント領域に画面をフィットさせないかどうかフラグ( TRUE:フィットさせない  FALSE:フィットする )
	int						WindowWidth, WindowHeight ;			// ウインドウのサイズ( ユーザー指定 )
	int						WindowSizeValid ;					// ウインドウのサイズが有効かどうか( TRUE:有効  FALSE:無効 )
	int						WindowSizeValidResetRequest ;		// ウインドウのサイズが有効かどうかのフラグをリセットするリクエストフラグ
	int						WindowX, WindowY ;					// ウインドウの位置( ユーザー指定 )
	int						WindowPosValid ;					// ウインドウの位置が有効かどうか( TRUE:有効  FALSE:無効 )
	int						WindowMaxWidth, WindowMaxHeight ;	// ウインドウの最大サイズ( ユーザー指定 )
	int						WindowMaxSizeValid ;				// ウインドウの最大サイズが有効かどうか( TRUE:有効  FALSE:無効 )
	int						WindowMinWidth, WindowMinHeight ;	// ウインドウの最小サイズ( ユーザー指定 )
	int						WindowMinSizeValid ;				// ウインドウの最小サイズが有効かどうか( TRUE:有効  FALSE:無効 )
	int						WindowLeftEdgeWidth ;				// ウインドウの左側の縁の幅
	int						WindowRightEdgeWidth ;				// ウインドウの右側の縁の幅
	int						WindowTopEdgeWidth ;				// ウインドウの上側の縁の幅
	int						WindowBottomEdgeWidth ;				// ウインドウの下側の縁の幅

	int						VisibleFlag ;
	int						NotWindowVisibleFlag ;				// ウインドウを表示しないフラグ
	int						WindowMinimizeFlag ;				// ウインドウを最小化状態にするかどうかのフラグ
	int						WindowMaximizeFlag ;				// ウインドウを最大化状態にするかどうかのフラグ
	RECT					WindowMaximizedClientRect ;			// ウインドウ最大化状態でのクライアント矩形
	RECT					WindowMaximizedRect ;				// ウインドウ最大化状態でのウインドウ矩形
	RECT					WindowMaximizedAdjustRect ;			// ウインドウ最大化状態でのウインドウ矩形をＤＸライブラリ側で補正したもの
	int						ValidFirstWindowMaximizedRect ;		// 一番最初の最大化状態でのウインドウの矩形が有効化どうか( TRUE:有効  FALSE:無効 )
	RECT					FirstWindowMaximizedRect ;			// 一番最初の最大化状態でのウインドウの矩形
	int						NotMoveMousePointerOutClientAreaFlag ;	// ウインドウのクライアントエリア外にマウスポインタが移動できないようにするかどうかのフラグ
	int						NotActive_WindowMoveOrSystemMenu ;	// WM_ENTERSIZEMOVE や WM_ENTERMENULOOP によって非アクティブになっている
	int						SetClipCursorFlag ;					// ClipCursor の設定が有効になっているかどうかフラグ
	int						SysCommandOffFlag ;					// タスクスイッチを抑制する処理を行うかフラグ
	HHOOK					TaskHookHandle ;					// フックハンドル
	HHOOK					GetMessageHookHandle ;				// WH_GETMESSAGE フックハンドル
	HHOOK					KeyboardHookHandle ;				// WH_KEYBOARD_LL フックハンドル
	int						LockInitializeFlag ;				// ロックをかけろというフラグ
	wchar_t					HookDLLFilePath[FILEPATH_MAX] ;		// フック処理をするＤＬＬファイルへのパス
	int						NotUseUserHookDllFlag ;				// ユーザー指定のＤＬＬを使用していないかどうかフラグ
	HMODULE					LoadResourModule ;					// リソースから読み込む系で使用するモジュール( NULL の場合は GetModuleHandle( NULL ) を使用 )
	int						WM_PAINTMessageFlag ;				// WM_PAINT メッセージが来た際に TRUE になるフラグ

	int						BackBufferTransColorFlag ;			// バックバッファの透過色の部分を透過させるかどうかのフラグ( TRUE:透過させる  FALSE:透過させない )
	int						UseUpdateLayerdWindowFlag ;			// UpdateLayerdWindow を使用するかどうかのフラグ( TRUE;使用する  FALSE:使用しない )
	HBITMAP					BackBufferTransBitmap ;				// バックバッファの透過色の部分を透過させるためのビットマップ
	void					*BackBufferTransBitmapImage ;		// BackBufferTransBitmap のイメージの先頭アドレス
	SIZE					BackBufferTransBitmapSize ;			// BackBufferTransBitmap のサイズ
	COLORDATA				BackBufferTransBitmapColorData ;	// BackBufferTransBitmap のカラーフォーマット

	HINSTANCE				MessageHookDLL ;					// メッセージフックＤＬＬのハンドル
	MSGFUNC					MessageHookCallBadk ;				// メッセージフックのコールバック関数
	DWORD					MessageHookThredID ;				// メッセージフックＤＬＬのスレッドＩＤ

	int						DirectXVersion ;					// DirectXのバージョン
	int						WindowsVersion ;					// Windowsのバージョン
	int						UseMMXFlag ;						// MMXが使えるかフラグ
	int						UseSSEFlag ;						// SSEが使えるかどうかフラグ
	int						UseSSE2Flag ;						// SSE2が使えるかどうかフラグ
	int						UseRDTSCFlag ;						// RDTSC 命令が使えるか、フラグ
	LONGLONG				OneSecCount ;						// 一秒間あたりのクロック数

	WNDPROC					UserWindowProc ;					// ユーザー定義のウインドウプロージャ
	int						UseUserWindowProcReturnValue ;		// ユーザー定義のウインドウプロージャの戻り値を使用するかどうか( TRUE:使用する  FALSE:使用しない )

	HWND					MesTakeOverWindow[MAX_MESTAKEOVERWIN_NUM] ;	// メッセージ処理を肩代わりするウインドウのリスト
	int						MesTakeOverWindowNum ;				// メッセージ処理を肩代わりするウインドウの数
	
	POINT					InitializeWindowPos ;				// 初期ウインドウの位置
	int						InitializeWindowPosValidFlag ;		// 初期ウインドウの位置の座標の有効フラグ
	
	HWND					DialogBoxHandle ;					// ダイアログボックスのハンドル
	
	int						DragFileValidFlag ;					// ファイルのドラッグ＆ドロップを許すかフラグ
	wchar_t					*DragFileName[MAX_DRAGFILE_NUM] ;	// ドラッグ＆ドロップされたファイル名
	int						DragFileNum ;						// ドラッグ＆ドロップされたファイルの数

//	WINFILEACCESS			*ReadOnlyFileAccessFirst ;			// ファイルアクセス構造体リストの先頭へのポインタ
//	int						ReadOnlyFileAccessLockFlag ;		// ファイルアクセス構造体リストへのアクセスを禁止しているか、フラグ

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )
//	int						DxConflictCheckFlag ;				// ＤＸライブラリの関数が複数のタスクから同時に実行されないようにするためのカウンタその１
	int						DxConflictCheckCounter ;			// ＤＸライブラリの関数が複数のタスクから同時に実行されないようにするためのカウンタその２
	int						DxUseThreadFlag ;					// ＤＸライブラリを使用しているスレッドがあるかどうか、フラグ( FALSE:ない  TRUE:ある )
	DWORD					DxUseThreadID ;						// ＤＸライブラリを使用しているスレッドのＩＤ
	DWORD_PTR				DxConflictWaitThreadID[MAX_THREADWAIT_NUM][2] ;	// 0:待ちスレッドのID  1:イベントハンドル
	int						DxConflictWaitThreadIDInitializeFlag ;	// DxConflictWaitThreadID を初期化してあるかどうかのフラグ
	int						DxConflictWaitThreadNum ;			// 待ちスレッドの数
	DX_CRITICAL_SECTION		DxConflictCheckCriticalSection ;	// 衝突防止用クリティカルセクション
#endif
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// ウインドウのデータ
extern WINDATA WinData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// ＣＯＭ初期化、終了関係関数
extern	int			InitializeCom( void ) ;													// ＣＯＭインターフェースを初期化する
extern	int			TerminateCom( void ) ;													// ＣＯＭインターフェースを終了する

// 初期化終了系関数
extern int			InitializeWindow( void ) ;												// ウインドウ関係の初期化関数
extern int			TerminateWindow( void ) ;												// ウインドウ関係の処理終了関数

// 情報出力系
extern	int			OutSystemInfo( void ) ;													// ＯＳやＤｉｒｅｃｔＸのバージョンを取得する

// 衝突防止用関数
//extern	void	PreparationDxFunction( void ) ;											// ＤＸライブラリ内部で公開関数を使用する前に呼ぶ関数
extern	void		CheckConflictAndWaitDxFunction( void ) ;								// 同時にＤＸライブラリの関数が呼ばれていないかチェックして、同時に呼ばれていたら待つ関数
extern	void		PostConflictProcessDxFunction( void ) ;									// CheckConfictAndWaitDxFunction を使用したＤＸライブラリの関数が return する前に呼ぶべき関数

// ソフトのウインドウにフォーカスを移す
extern	void		SetAbsoluteForegroundWindow( HWND hWnd, int Flag = TRUE ) ;


// wchar_t版関数
extern	int			GetResourceInfo_WCHAR_T(			const wchar_t *ResourceName , const wchar_t *ResourceType , void **DataPointerP , int *DataSizeP ) ;
extern	int			GetPcInfo_WCHAR_T(					wchar_t *OSString , wchar_t *DirectXString , wchar_t *CPUString , int *CPUSpeed /* 単位MHz */ , double *FreeMemorySize /* 単位MByte */ , double *TotalMemorySize , wchar_t *VideoDriverFileName , wchar_t *VideoDriverString , double *FreeVideoMemorySize /* 単位MByte */ , double *TotalVideoMemorySize ) ;
extern	wchar_t		GetInputSystemChar_WCHAR_T(			int DeleteFlag ) ;
extern	int			LoadPauseGraph_WCHAR_T(				const wchar_t *FileName ) ;
extern	int			SetMainWindowText_WCHAR_T(			const wchar_t *WindowText ) ;
extern	int			SetMainWindowClassName_WCHAR_T(		const wchar_t *ClassName ) ;
extern	int			SetShutdownCallbackFunction_WCHAR_T(void (* CallbackFunction )( void * ), void *Data, const wchar_t *Message ) ;
extern	int			SetSysCommandOffFlag_WCHAR_T(		int Flag , const wchar_t *HookDllPath = NULL ) ;
extern	int			GetDragFilePath_WCHAR_T(			wchar_t *FilePathBuffer ) ;
extern	int			SetWindowRgnGraph_WCHAR_T(			const wchar_t *FileName ) ;
extern	int			SetupToolBar_WCHAR_T(				const wchar_t *BitmapName , int DivNum , int ResourceID = -1 ) ;
extern	int			AddKeyAccel_WCHAR_T(				const wchar_t *ItemName , int ItemID , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;
extern	int			AddKeyAccel_Name_WCHAR_T(			const wchar_t *ItemName , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;
extern	int			AddMenuItem_WCHAR_T(				int AddType /* MENUITEM_ADD_CHILD等 */ , const wchar_t *ItemName, int ItemID, int SeparatorFlag, const wchar_t *NewItemName = NULL , int NewItemID = -1 ) ;
extern	int			DeleteMenuItem_WCHAR_T(				const wchar_t *ItemName, int ItemID ) ;
extern	int			CheckMenuItemSelect_WCHAR_T(		const wchar_t *ItemName, int ItemID ) ;
extern	int			SetMenuItemEnable_WCHAR_T(			const wchar_t *ItemName, int ItemID, int EnableFlag ) ;
extern	int			SetMenuItemMark_WCHAR_T(			const wchar_t *ItemName, int ItemID, int Mark ) ;
extern	int			AddMenuItem_Name_WCHAR_T(			const wchar_t *ParentItemName, const wchar_t *NewItemName ) ;
extern	int			AddMenuLine_Name_WCHAR_T(			const wchar_t *ParentItemName ) ;
extern	int			InsertMenuItem_Name_WCHAR_T(		const wchar_t *ItemName, const wchar_t *NewItemName ) ;
extern	int			InsertMenuLine_Name_WCHAR_T(		const wchar_t *ItemName ) ;
extern	int			DeleteMenuItem_Name_WCHAR_T(		const wchar_t *ItemName ) ;
extern	int			CheckMenuItemSelect_Name_WCHAR_T(	const wchar_t *ItemName ) ;
extern	int			SetMenuItemEnable_Name_WCHAR_T(		const wchar_t *ItemName, int EnableFlag ) ;
extern	int			SetMenuItemMark_Name_WCHAR_T(		const wchar_t *ItemName, int Mark ) ;
extern	int			AddMenuItem_ID_WCHAR_T(				int ParentItemID, const wchar_t *NewItemName, int NewItemID = -1 ) ;
extern	int			GetMenuItemID_WCHAR_T(				const wchar_t *ItemName ) ;
extern	int			GetMenuItemName_WCHAR_T(			int ItemID, wchar_t *NameBuffer ) ;


/*
extern __inline void PreparationDxFunction( void )
{
#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )
	// メインカウンタをインクリメントする
	WinData.DxConflictCheckCounter ++ ;
#endif
}
*/


// 設定系関数
extern	int			SetWindowModeFlag( int Flag ) ;											// ウインドウモードで起動するかをセット
extern	int			SetWindowStyle( int CenterPosition = TRUE ) ;							// ウインドウのスタイルをセットする
extern	int			RefreshDragFileValidFlag( void ) ;										// ファイルのドラッグ＆ドロップ機能を有効にするかどうかの設定を再設定する


// 情報取得警官数
extern	PCINFO		*GetPcInfoStructP( void ) ;												// ＰＣ情報構造体のアドレスを得る
extern	HWND		GetDisplayWindowHandle( void ) ;										// 表示に使用するウインドウのハンドルを取得する


// 便利関数
//extern	int			CheckConflict( int *Check_Inc_Counter ) ;									// マルチスレッド時の衝突を防ぐための関数
//extern	int			CheckConflict2( int *Check_NonInc_Counter, int *NonCheck_Inc_Counter ) ;	// マルチスレッド時の衝突を防ぐための関数2
//extern	int			CheckConflict3( int *Check_NonInc_Counter, int *Check_Inc_Counter ) ;		// マルチスレッド時の衝突を防ぐための関数3

// メッセージ処理関数
extern	void		DxActiveWait( void ) ;													// アクティブになるまで何もしない
extern	int			CheckActiveWait( void ) ;												// 非アクティブかどうかをチェックする
extern	int			GetQuitMessageFlag( void ) ;											// WM_QUIT が発行されているかどうかを取得する
extern	int			WM_ACTIVATEProcessUseStock( WPARAM wParam, LPARAM lParam, int APPMes = FALSE, int Dummy = FALSE ) ;
extern	int			WM_ACTIVATEProcess(         WPARAM wParam, LPARAM lParam, int APPMes = FALSE, int Dummy = FALSE ) ;

// Aero の有効、無効設定
extern	int			SetEnableAero( int Flag ) ;

// ツールバー関係
extern	int			GetToolBarHeight( void ) ;													// ツールバーの高さを得る

// メニュー関係
extern	int			MenuAutoDisplayProcess( void ) ;																// メニューを自動的に表示したり非表示にしたりする処理を行う

// 補助関数
extern	void		_FileTimeToLocalDateData( FILETIME *FileTime, DATEDATA *DateData ) ;
extern	int			UpdateBackBufferTransColorWindow( const BASEIMAGE *SrcImage, const RECT *SrcImageRect = NULL, HDC Direct3DDC = NULL, int NotColorKey = FALSE, int PreMultipliedAlphaImage = FALSE ) ;	// 指定の基本イメージを使用して UpdateLayeredWindow を行う

// リソース関係
extern	int			GetBmpImageToResource( int ResourceID, BITMAPINFO **BmpInfoP, void **GraphDataP ) ;					// ＢＭＰリソースから BITMAPINFO と画像イメージを構築する

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXWINDOW_H__
