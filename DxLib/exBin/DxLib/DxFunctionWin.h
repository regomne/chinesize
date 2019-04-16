// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows専用関数プロトタイプ宣言用ヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXFUNCTIONWIN
#define __DXFUNCTIONWIN

// 関数プロトタイプ宣言------------------------------------------------------------------

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

#define DX_FUNCTION_START


// DxWindow.cpp関数プロトタイプ宣言

// 便利関数
extern	int				GetResourceInfo(			const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            void **DataPointerP , int *DataSizeP ) ;		// 指定の名前、タイプのリソースのアドレスとサイズを取得する( 戻り値  -1:失敗  0:成功 )
extern	int				GetResourceInfoWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, void **DataPointerP , int *DataSizeP ) ;		// 指定の名前、タイプのリソースのアドレスとサイズを取得する( 戻り値  -1:失敗  0:成功 )
extern	const TCHAR*	GetResourceIDString(		int ResourceID ) ;																																			// リソースＩＤ値からリソース名を取得する

// ウインドウ関係情報取得関数
extern	int			GetWindowCRect(					RECT *RectBuf ) ;										// GetWindowClientRect の旧名称
extern	int			GetWindowClientRect(			RECT *RectBuf ) ;										// メインウインドウのクライアント領域を取得する
extern	int			GetWindowFrameRect(				RECT *RectBuf ) ;										// メインウインドウの枠の部分も含めた全体の領域を取得する
extern	int			GetWindowActiveFlag(			void ) ;												// メインウインドウがアクティブかどうかを取得する( 戻り値  TRUE:アクティブ  FALSE:非アクティブ )
extern	int			GetWindowMinSizeFlag(			void ) ;												// メインウインドウが最小化されているかどうかを取得する( 戻り値  TRUE:最小化されている  FALSE:最小化されていない )
extern	int			GetWindowMaxSizeFlag(			void ) ;												// メインウインドウが最大化されているかどうかを取得する( 戻り値  TRUE:最大化されている  FALSE:最大化されていない )
extern	int			GetActiveFlag(					void ) ;												// GetWindowActiveFlag の別名関数
extern	HWND		GetMainWindowHandle(			void ) ;												// メインウインドウのウインドウハンドルを取得する
extern	int			GetWindowModeFlag(				void ) ;												// ウインドウモードで起動しているかどうかを取得する( 戻り値  TRUE:ウインドウモード  FALSE:フルスクリーンモード )
extern	int			GetDefaultState(				int *SizeX, int *SizeY, int *ColorBitDepth, int *RefreshRate = NULL , int *LeftTopX = NULL , int *LeftTopY = NULL , int *PixelSizeX = NULL , int *PixelSizeY = NULL ) ;		// 起動時のデスクトップの画面情報を取得する( SizeX:デスクトップの横解像度を格納する変数のポインタ  SizeY:縦解像度を格納する変数のポインタ  ColorBitDepth:画面カラービット数を格納する変数のポインタ  RefreshRate:デスクトップのリフレッシュレートを格納する変数のポインタ  LeftTopX:デスクトップの左上Ｘ座標を格納する変数のポインタ  LeftTopY:デスクトップの左上Ｙ座標を格納する変数のポインタ  PixelSizeX:OSの拡大率設定の影響を受けないデスクトップの横解像度を格納する変数のポインタ  PixelSizeY:OSの拡大率設定の影響を受けないデスクトップの縦解像度を格納する変数のポインタ )
extern	int			GetNoActiveState(				int ResetFlag = TRUE ) ;								// メインウインドウが非アクティブになり、処理が一時停止していたかどうかを取得する(引数 ResetFlag=TRUE:状態をリセット FALSE:状態をリセットしない    戻り値: 0=一時停止はしていない  1=一時停止していた )
extern	int			GetMouseDispFlag(				void ) ;												// マウスポインタを表示するかどうかの設定を取得する( 戻り値  TRUE:表示する  FALSE:表示しない )
extern	int			GetAlwaysRunFlag(				void ) ;												// メインウインドウが非アクティブになっても処理を実行し続けるかどうかの設定を取得する( TRUE:実行する  FALSE:停止する )
extern	int			_GetSystemInfo(					int *DxLibVer , int *DirectXVer , int *WindowsVer ) ;	// ＤＸライブラリと DirectX のバージョンと Windows のバージョン番号を取得する
extern	int			GetPcInfo(						TCHAR *OSString , TCHAR *DirectXString , TCHAR *CPUString , int *CPUSpeed /* 単位MHz */ , double *FreeMemorySize /* 単位MByte */ , double *TotalMemorySize , TCHAR *VideoDriverFileName , TCHAR *VideoDriverString , double *FreeVideoMemorySize /* 単位MByte */ , double *TotalVideoMemorySize ) ;	// ＰＣの情報を取得する
extern	int			GetUseMMXFlag(					void ) ;												// ＭＭＸが使えるかどうかの情報を得る
extern	int			GetUseSSEFlag(					void ) ;												// ＳＳＥが使えるかどうかの情報を得る
extern	int			GetUseSSE2Flag(					void ) ;												// ＳＳＥ２が使えるかどうかの情報を得る
extern	int			GetWindowCloseFlag(				void ) ;												// ウインドウを閉じようとしているかの情報を得る
extern	HINSTANCE	GetTaskInstance(				void ) ;												// ソフトのインスタンスを取得する
extern	int			GetUseWindowRgnFlag(			void ) ;												// リージョンを使っているかどうかを取得する
extern	int			GetWindowSizeChangeEnableFlag(	int *FitScreen = NULL ) ;								// ウインドウのサイズを変更できる設定になっているかどうかを取得する( 戻り値  TRUE:変更できるようになっている  FALSE:変更できないようになっている )
extern	double		GetWindowSizeExtendRate(		double *ExRateX = NULL , double *ExRateY = NULL ) ;		// 描画画面のサイズに対するウインドウサイズの比率を取得する( 戻り値は ExRateX に代入される値と同じです )
extern	int			GetWindowSize(					int *Width, int *Height ) ;								// メインウインドウのクライアント領域のサイズを取得する
extern	int			GetWindowEdgeWidth(				int *LeftWidth, int *RightWidth, int *TopWidth, int *BottomWidth ) ;	// ウインドウの上下左右の縁の幅を取得する
extern	int			GetWindowPosition(				int *x, int *y ) ;										// メインウインドウのデスクトップ上の左上端座標を取得する( 枠も含める )
extern	int			GetWindowUserCloseFlag(			int StateResetFlag = FALSE ) ;							// メインウインドウの閉じるボタンが押されたかどうかを取得する
extern	int			GetNotDrawFlag(					void ) ;												// ＤＸライブラリの描画機能を使うかどうかの設定を取得する
extern	int			GetPaintMessageFlag(			void ) ;												// WM_PAINT メッセージが来たかどうかを取得する(戻り値  TRUE:WM_PAINTメッセージが来た(一度取得すると以後、再び WM_PAINTメッセージが来るまで FALSE が返ってくるようになる)  FALSE:WM_PAINT メッセージは来ていない)
extern	int			GetValidHiPerformanceCounter(	void ) ;												// パフォーマンスカウンタが有効かどうかを取得する(戻り値  TRUE:有効  FALSE:無効)
extern	TCHAR		GetInputSystemChar(				int DeleteFlag ) ;										// 入力されたシステム文字を取得する

// 設定系関数
extern	int			ChangeWindowMode(						int Flag ) ;																		// ウインドウモードを変更する
extern	int			SetUseCharSet(							int CharSet /* = DX_CHARSET_SHFTJIS 等 */ ) ;										// ＤＸライブラリの文字列処理で前提とする文字列セットを設定する
extern	int			LoadPauseGraph(							const TCHAR *FileName                        ) ;									// アクティブウインドウが他のソフトに移っている際に表示する画像をファイルから読み込む( FileName に NULL を渡すことで解除)
extern	int			LoadPauseGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;									// アクティブウインドウが他のソフトに移っている際に表示する画像をファイルから読み込む( FileName に NULL を渡すことで解除)
extern	int			LoadPauseGraphFromMem(					const void *MemImage , int MemImageSize ) ;											// アクティブウインドウが他のソフトに移っている際に表示する画像をメモリから読み込む( MemImage:ファイルイメージの先頭アドレス,NULL にすることで設定解除  MemImageSize:ファイルイメージのサイズ( 単位:Byte ) )
extern	int			SetActiveStateChangeCallBackFunction(	int (* CallBackFunction )( int ActiveState , void *UserData ) , void *UserData ) ;	// メインウインドウのアクティブ状態に変化があったときに呼ばれるコールバック関数を設定する( CallBackFunction:呼ばれるコールバック関数、NULL を渡すと設定解除  UserData:CallBackFunction の第２引数に渡される値 )
extern	int			SetWindowText(							const TCHAR *WindowText                          ) ;								// メインウインドウのウインドウテキストを変更する
extern	int			SetWindowTextWithStrLen(				const TCHAR *WindowText, size_t WindowTextLength ) ;								// メインウインドウのウインドウテキストを変更する
extern	int			SetMainWindowText(						const TCHAR *WindowText                          ) ;								// SetWindowText の別名関数
extern	int			SetMainWindowTextWithStrLen(			const TCHAR *WindowText, size_t WindowTextLength ) ;								// SetWindowText の別名関数
extern	int			SetMainWindowClassName(					const TCHAR *ClassName                         ) ;									// メインウインドウのクラス名を設定する( DxLib_Init の前でのみ使用可能 )
extern	int			SetMainWindowClassNameWithStrLen(		const TCHAR *ClassName, size_t ClassNameLength ) ;									// メインウインドウのクラス名を設定する( DxLib_Init の前でのみ使用可能 )
extern	int			SetWindowIconID(						int ID ) ;																			// メインウインドウで使用するアイコンのＩＤをセットする
extern	int			SetWindowIconHandle(					HICON Icon ) ;																		// メインウインドウで使用するアイコンのハンドルをセットする
extern	int			SetUseASyncChangeWindowModeFunction(	int Flag , void (* CallBackFunction )( void * ) , void *Data ) ;					// 最大化ボタンやALT+ENTERキーによる非同期なウインドウモードの変更の機能の設定を行う( Flag:非同期ウインドウモード変更を有効にするかどうかのフラグ( TRUE:有効にする  FALSE:無効にする( デフォルト ) )   CallBackFunction:非同期ウインドウモード変更が行われた場合に呼ばれるコールバック関数のアドレス   Data:CallBackFunction の引数に渡される void 型のポインタ )
extern	int			SetShutdownCallbackFunction(			void (* CallbackFunction )( void * ), void *Data, const TCHAR *Message ) ;			// シャットダウンによるソフトの強制終了の際に呼ばれるコールバック関数を登録する( Message はコールバック関数の終了を待つ間表示されるメッセージ )
extern	int			SetWindowStyleMode(						int Mode ) ;																		// メインウインドウのスタイルを変更する
extern	int			SetWindowZOrder(						int ZType /* = DX_WIN_ZTYPE_TOP 等 */ , int WindowActivateFlag = TRUE ) ;			// メインウインドウの奥行き位置を変更する
extern	int			SetWindowSizeChangeEnableFlag(			int Flag, int FitScreen = TRUE ) ;													// メインウインドウの端を掴んでウインドウのサイズを変更できるようにするかどうかの設定を行う( Flag:変更を可能にするかどうか( TRUE:可能にする  FALSE:不可能にする( デフォルト ) )  FitScreen:ウインドウのクライアント領域に画面をフィットさせる(拡大させる)かどうか  TRUE:フィットさせる  FALSE:フィットさせない )
extern	int			SetWindowSizeExtendRate(				double ExRateX, double ExRateY = -1.0 ) ;											// 描画画面のサイズに対するメインウインドウサイズの比率を設定する( ExRateY がマイナスの場合は ExRateX の値が ExRateY にも使用されます )
extern	int			SetWindowSize(							int Width, int Height ) ;															// メインウインドウのクライアント領域のサイズを設定する
extern	int			SetWindowMaxSize(						int MaxWidth, int MaxHeight ) ;														// メインウインドウのクライアント領域の最大サイズを設定する( SetWindowSizeChangeEnableFlag の第一引数を TRUE で呼び出して、ウインドウのサイズが変更できる状態でのみ使用されるパラメータです )
extern	int			SetWindowMinSize(						int MinWidth, int MinHeight ) ;														// メインウインドウのクライアント領域の最小サイズを設定する( SetWindowSizeChangeEnableFlag の第一引数を TRUE で呼び出して、ウインドウのサイズが変更できる状態でのみ使用されるパラメータです )
extern	int			SetWindowPosition(						int x, int y ) ;																	// メインウインドウの位置を設定する( 枠も含めた左上座標 )
extern	int			SetSysCommandOffFlag(					int Flag , const TCHAR *HookDllPath = NULL                                ) ;		// タスクスイッチを有効にするかどうかを設定する
extern	int			SetSysCommandOffFlagWithStrLen(			int Flag , const TCHAR *HookDllPath = NULL , size_t HookDllPathLength = 0 ) ;		// タスクスイッチを有効にするかどうかを設定する
extern	int			SetHookWinProc(							WNDPROC WinProc ) ;																	// メインウインドウへのメッセージをフックするウインドウプロージャを登録する
extern	int			SetUseHookWinProcReturnValue(			int UseFlag ) ;																		// SetHookWinProc で設定したウインドウプロージャの戻り値を使用するかどうかを設定する、SetHookWinProc で設定したウインドウプロージャの中でのみ使用可能( UseFlag TRUE:戻り値を使用して、ＤＸライブラリのウインドウプロージャの処理は行わない  FALSE:戻り値は使用せず、ウインドウプロージャから出た後、ＤＸライブラリのウインドウプロージャの処理を行う )
extern	int			SetDoubleStartValidFlag(				int Flag ) ;																		// ＤＸライブラリを使用したソフトの二重起動を可能にするかどうかの設定を行う( TRUE:可能にする  FALSE:不可能にする( デフォルト ) )
extern	int			CheckDoubleStart(						void ) ;																			// ＤＸライブラリを使用したソフトが既に起動しているかどうかを取得する( TRUE:既に起動している  FALSE:起動していない )
extern	int			AddMessageTakeOverWindow(				HWND Window ) ;																		// メッセージ処理をＤＸライブラリに肩代わりしてもらうウインドウを追加する
extern	int			SubMessageTakeOverWindow(				HWND Window ) ;																		// メッセージ処理をＤＸライブラリに肩代わりしてもらうウインドウを減らす

extern	int			SetWindowInitPosition(					int x , int y ) ;																	// メインウインドウの初期位置を設定する
extern	int			SetNotWinFlag(							int Flag ) ;																		// ＤＸライブラリのウインドウ関連の機能を使用しないかどうかを設定する( TRUE:使用しない  FALSE:使用する( デフォルト ) )
extern	int			SetNotDrawFlag(							int Flag ) ;																		// ＤＸライブラリの描画機能を使うかどうかを設定する( TRUE:使用しない  FALSE:使用する( デフォルト ) )
extern	int			SetNotSoundFlag(						int Flag ) ;																		// ＤＸライブラリのサウンド機能を使うかどうかを設定する( TRUE:使用しない  FALSE:使用する( デフォルト ) )
extern	int			SetNotInputFlag(						int Flag ) ;																		// ＤＸライブラリの入力状態の取得機能を使うかどうかを設定する( TRUE:使用しない  FALSE:使用する( デフォルト ) )
extern	int			SetDialogBoxHandle(						HWND WindowHandle ) ;																// ＤＸライブラリでメッセージ処理を行うダイアログボックスを登録する
extern	int			SetWindowVisibleFlag(					int Flag ) ;																		// メインウインドウを表示するかどうかを設定する( TRUE:表示する  FALSE:表示しない )
extern	int			SetWindowMinimizeFlag(					int Flag ) ;																		// メインウインドウを最小化するかどうかを設定する( TRUE:最小化する  FALSE:最小化を解除 )
extern	int			SetWindowUserCloseEnableFlag(			int Flag ) ;																		// メインウインドウの×ボタンを押した時にＤＸライブラリが自動的にウインドウを閉じるかどうかを設定する( TRUE:自動的に閉じる( デフォルト )  FALSE:閉じない )
extern	int			SetDxLibEndPostQuitMessageFlag(			int Flag ) ;																		// ＤＸライブラリ終了時に PostQuitMessage を呼ぶかどうかを設定する( TRUE:PostQuitMessage を呼ぶ( デフォルト )  FALSE:呼ばない )
extern	int			SetUserWindow(							HWND WindowHandle ) ;																// ＤＸライブラリで使用するウインドウのハンドルをセットする( DxLib_Init を実行する以前でのみ有効 )
extern	int			SetUserChildWindow(						HWND WindowHandle ) ;																// ＤＸライブラリで使用する表示用の子ウインドウのハンドルをセットする( DxLib_Init を実行する以前でのみ有効 )
extern	int			SetUserWindowMessageProcessDXLibFlag(	int Flag ) ;																		// SetUseWindow で設定したウインドウのメッセージループ処理をＤＸライブラリで行うかどうかを設定する( TRUE:ＤＸライブラリで行う( デフォルト )  FALSE:ＤＸライブラリでは行わない )
extern	int			SetUseFPUPreserveFlag(					int Flag ) ;																		// FPUの精度を落とさない設定を使用するかどうかを設定する、DxLib_Init を呼び出す前のみ有効( TRUE:使用する(精度が落ちない)  FALSE:使用しない(精度を落とす(デフォルト) )
extern	int			SetValidMousePointerWindowOutClientAreaMoveFlag( int Flag ) ;																// マウスポインタがウインドウのクライアントエリアの外にいけるかどうかを設定する( TRUE:いける( デフォルト設定 )  FALSE:いけない )
extern	int			SetUseBackBufferTransColorFlag(			int Flag ) ;																		// バックバッファの透過色の部分を透過させるかどうかを設定する( TRUE:透過させる  FALSE:透過させない( デフォルト ) )
extern	int			SetUseUpdateLayerdWindowFlag(			int Flag ) ;																		// UpdateLayerdWindowForBaseImage や UpdateLayerdWindowForSoftImage を使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない )
extern	int			SetResourceModule(						HMODULE ResourceModule ) ;															// リソースを読み込む際に使用するモジュールを設定する( NULL を指定すると初期状態に戻ります、デフォルトでは NULL )
extern	int			SetUseDxLibWM_PAINTProcess(				int Flag ) ;																		// WM_PAINT メッセージが来た際に『ＤＸライブラリの WM_PAINTメッセージが来た際の処理』を行うかどうかを設定する( 別スレッドで描画処理を行う場合などで使用 )

// ドラッグ＆ドロップされたファイル関係
extern	int			SetDragFileValidFlag(		int Flag ) ;																	// ファイルのメインウインドウへのドラッグ＆ドロップ機能を有効にするかどうかのフラグをセットする
extern	int			DragFileInfoClear(			void ) ;																		// メインウインドウへドラッグ＆ドロップされたファイルの情報をリセットする
extern	int			GetDragFilePath(			TCHAR *FilePathBuffer ) ;														// メインウインドウへドラッグ＆ドロップされたファイル名を取得する( FilePathBuffer:ファイル名を格納するバッファの先頭アドレス　　戻り値　-1:取得できなかった  0:取得できた )
extern	int			GetDragFileNum(				void ) ;																		// メインウインドウへドラッグ＆ドロップされたファイルの数を取得する

// ウインドウ描画領域設定系関数
extern	HRGN		CreateRgnFromGraph(			int Width , int Height , const void *MaskData , int Pitch , int Byte ) ;		// 任意の画像イメージからRGNハンドルを作成する( Width:横ドット数  Height:縦ドット数  MaskData:ドット情報配列の先頭アドレス  Pitch:１ラインあたりのbyteサイズ  Byte:１ドット辺りのbyteサイズ( 対応しているのは 1〜4 )、ドットの数値が0かそれ以外かで判別 )
extern	HRGN		CreateRgnFromBaseImage(		BASEIMAGE *BaseImage, int TransColorR, int TransColorG, int TransColorB ) ;		// 任意の基本イメージデータと透過色からRGNハンドルを作成する( BaseImage:基本イメージデータのアドレス  TransColorR,TransColorG,TransColorB:透過色( それぞれ０〜２５５ )
extern	int			SetWindowRgnGraph(			 const TCHAR *FileName                        ) ;								// 任意の画像ファイルからＲＧＮをセットする
extern	int			SetWindowRgnGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength ) ;								// 任意の画像ファイルからＲＧＮをセットする
extern	int			UpdateTransColorWindowRgn(	void ) ;																		// 描画先の画面の透過色の部分を透過させるＲＧＮをセットする( 使用される透過色は関数 SetTransColor で設定した色 )

// ツールバー関係
extern	int			SetupToolBar(				const TCHAR *BitmapName,                          int DivNum, int ResourceID = -1 ) ;									// ツールバーの準備、BitmapName に NULL, ResourceID に -1 を渡すとツールバーを解除( BitmapName:ツールバーのボタンに使用する画像ファイルパス、ResourceID に -1 以外を渡す場合は NULL にする  DivNum:ボタン画像中のボタンの数  ResourceID:ツールバーのボタンに使用するビットマップリソースのＩＤ、BitmapName に NULL を渡すとこの引数が使用される )
extern	int			SetupToolBarWithStrLen(		const TCHAR *BitmapName, size_t BitmapNameLength, int DivNum, int ResourceID = -1 ) ;									// ツールバーの準備、BitmapName に NULL, ResourceID に -1 を渡すとツールバーを解除( BitmapName:ツールバーのボタンに使用する画像ファイルパス、ResourceID に -1 以外を渡す場合は NULL にする  DivNum:ボタン画像中のボタンの数  ResourceID:ツールバーのボタンに使用するビットマップリソースのＩＤ、BitmapName に NULL を渡すとこの引数が使用される )
extern	int			AddToolBarButton(			int Type /* TOOLBUTTON_TYPE_NORMAL 等 */ , int State /* TOOLBUTTON_STATE_ENABLE 等 */ , int ImageIndex, int ID ) ;		// ツールバーにボタンを追加する( Type:ボタンタイプ( TOOLBUTTON_TYPE_NORMAL 等( 解説は #define の定義を参照してください ) )  State:初期状態( TOOLBUTTON_STATE_ENABLE 等( 解説は #define の定義を参照してください ) )  ImageIndex:使用するボタンの画像番号  ID:ボタンに割り当てる識別番号  )
extern	int			AddToolBarSep(				void ) ;																												// ツールバーに隙間を追加する
extern	int			GetToolBarButtonState(		int ID ) ;																												// ツールバーのボタンの状態を取得する( ID:AddToolBarButtonで設定したボタンの識別番号　　戻り値　TRUE:押されている or 押された  FALSE:押されていない )
extern	int			SetToolBarButtonState(		int ID , int State /* TOOLBUTTON_STATE_ENABLE 等 */ ) ;																	// ツールバーのボタンの状態を設定する( ID:AddToolBarButtonで設定したボタンの識別番号　State:設定する状態( TOOLBUTTON_STATE_ENABLE 等( 解説は #define の定義を参照してください ) )
extern	int			DeleteAllToolBarButton(		void ) ;																												// ツールバーのボタンを全て削除する

// メニュー関係
extern	int			SetUseMenuFlag(						int Flag ) ;																										// メニューを有効にするかどうかを設定する( TRUE:使用する  FALSE:使用しない )
extern	int			SetUseKeyAccelFlag(					int Flag ) ;																										// キーボードアクセラレーターを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない )

extern	int			AddKeyAccel(						const TCHAR *ItemName,                        int ItemID , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;	// ショートカットキーを追加する( ItemName:ショートカットキーを割り当てるメニューのアイテム名( AddMenuItem で NewItemName に渡した名前 )、ItemID を使用する場合は NULL を渡す  ItemID:メニュー項目の識別番号( AddMenuItem の引数 NewItemID で指定したもの )、ItemName を使用する場合は -1 を渡す  KeyCode:ショートカットキーのキー( KEY_INPUT_L 等 )  CtrlFlag:同時にCTRLキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )  AltFlag:同時にALTキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )  ShiftFlag:同時にSHIFTキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )
extern	int			AddKeyAccelWithStrLen(				const TCHAR *ItemName, size_t ItemNameLength, int ItemID , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;	// ショートカットキーを追加する( ItemName:ショートカットキーを割り当てるメニューのアイテム名( AddMenuItem で NewItemName に渡した名前 )、ItemID を使用する場合は NULL を渡す  ItemID:メニュー項目の識別番号( AddMenuItem の引数 NewItemID で指定したもの )、ItemName を使用する場合は -1 を渡す  KeyCode:ショートカットキーのキー( KEY_INPUT_L 等 )  CtrlFlag:同時にCTRLキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )  AltFlag:同時にALTキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )  ShiftFlag:同時にSHIFTキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )
extern	int			AddKeyAccel_Name(					const TCHAR *ItemName,                        int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;				// ショートカットキーを追加する( 各引数の解説は AddKeyAccel と同じ、ItemID が無くなっただけ )
extern	int			AddKeyAccel_NameWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength, int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;				// ショートカットキーを追加する( 各引数の解説は AddKeyAccel と同じ、ItemID が無くなっただけ )
extern	int			AddKeyAccel_ID(						int ItemID, int KeyCode, int CtrlFlag, int AltFlag, int ShiftFlag ) ;												// ショートカットキーを追加する( 各引数の解説は AddKeyAccel と同じ、ItemName が無くなっただけ )
extern	int			ClearKeyAccel(						void ) ;																											// ショートカットキーの情報をリセットする

extern	int			AddMenuItem(						int AddType /* MENUITEM_ADD_CHILD等 */ , const TCHAR *ItemName,                        int ItemID, int SeparatorFlag, const TCHAR *NewItemName = NULL ,                                int NewItemID = -1 ) ;	// メニューに項目を追加する( AddType:項目タイプ( MENUITEM_ADD_CHILD 等( 解説は #define の定義を参照してください ) )    ItemName:AddType が MENUITEM_ADD_CHILDの場合は親となる項目の名前、MENUITEM_ADD_INSERTの場合は挿入位置となる項目の名前、NULL を指定すると ItemID が使用される   ItemID:ItemName の代わりに識別番号で指定するもの、AddType毎の違いは ItemName の解説の通り、-1を指定すると ItemName が使用される　　SeparatorFlag:区切り線を追加するかどうか( TRUE:区切り線を追加、この場合 NewItemName と NewItemID は無視される  FALSE:追加するのは区切り線ではない )　　NewItemName:新しい項目の名前  NewItemID:新しい項目の識別番号、-1を指定すると内部で適当な番号が割り当てられる )
extern	int			AddMenuItemWithStrLen(				int AddType /* MENUITEM_ADD_CHILD等 */ , const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int SeparatorFlag, const TCHAR *NewItemName = NULL , size_t NewItemNameLength = 0 , int NewItemID = -1 ) ;	// メニューに項目を追加する( AddType:項目タイプ( MENUITEM_ADD_CHILD 等( 解説は #define の定義を参照してください ) )    ItemName:AddType が MENUITEM_ADD_CHILDの場合は親となる項目の名前、MENUITEM_ADD_INSERTの場合は挿入位置となる項目の名前、NULL を指定すると ItemID が使用される   ItemID:ItemName の代わりに識別番号で指定するもの、AddType毎の違いは ItemName の解説の通り、-1を指定すると ItemName が使用される　　SeparatorFlag:区切り線を追加するかどうか( TRUE:区切り線を追加、この場合 NewItemName と NewItemID は無視される  FALSE:追加するのは区切り線ではない )　　NewItemName:新しい項目の名前  NewItemID:新しい項目の識別番号、-1を指定すると内部で適当な番号が割り当てられる )
extern	int			DeleteMenuItem(						const TCHAR *ItemName,                        int ItemID ) ;														// メニューから選択項目を削除する( ItemName:削除する項目の名前( AddMenuItem で NewItemName に渡した名前 )、NULL を指定すると ItemID が使用される  ItemID:削除する項目の識別番号( AddMenuItem で NewItemID に渡した番号 )、-1 を指定すると ItemName が使用される )
extern	int			DeleteMenuItemWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength, int ItemID ) ;														// メニューから選択項目を削除する( ItemName:削除する項目の名前( AddMenuItem で NewItemName に渡した名前 )、NULL を指定すると ItemID が使用される  ItemID:削除する項目の識別番号( AddMenuItem で NewItemID に渡した番号 )、-1 を指定すると ItemName が使用される )
extern	int			CheckMenuItemSelect(				const TCHAR *ItemName,                        int ItemID ) ;														// メニューが選択されたかどうかを取得する( 戻り値　 0:選択されていない  1:選択された   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			CheckMenuItemSelectWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, int ItemID ) ;														// メニューが選択されたかどうかを取得する( 戻り値　 0:選択されていない  1:選択された   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			SetMenuItemEnable(					const TCHAR *ItemName,                        int ItemID, int EnableFlag ) ;										// メニューの項目を選択出来るかどうかを設定する( EnableFlag:項目が選択できるかどうか( TRUE:選択できる   FALSE:選択できない )   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			SetMenuItemEnableWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int EnableFlag ) ;										// メニューの項目を選択出来るかどうかを設定する( EnableFlag:項目が選択できるかどうか( TRUE:選択できる   FALSE:選択できない )   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			SetMenuItemMark(					const TCHAR *ItemName,                        int ItemID, int Mark ) ;												// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等( 解説は #define の定義を参照してください )    ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください ) )
extern	int			SetMenuItemMarkWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int Mark ) ;												// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等( 解説は #define の定義を参照してください )    ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください ) )
extern	int			CheckMenuItemSelectAll(				void ) ;																											// メニューの項目がどれか選択されたかどうかを取得する( 戻り値  TRUE:どれか選択された  FALSE:選択されていない )

extern	int			AddMenuItem_Name(					const TCHAR *ParentItemName,                              const TCHAR *NewItemName                           ) ;	// メニューに選択項目を追加する( ParentItemName:親となる項目の名前、親が持つリストの末端に新しい項目を追加します  NewItemName:新しい項目の名前 )
extern	int			AddMenuItem_NameWithStrLen(			const TCHAR *ParentItemName, size_t ParentItemNameLength, const TCHAR *NewItemName, size_t NewItemNameLength ) ;	// メニューに選択項目を追加する( ParentItemName:親となる項目の名前、親が持つリストの末端に新しい項目を追加します  NewItemName:新しい項目の名前 )
extern	int			AddMenuLine_Name(					const TCHAR *ParentItemName ) ;																						// メニューのリストに区切り線を追加する( ParentItemName:区切り線を付ける項目リストの親の名前、リストの末端に区切り線を追加します )
extern	int			AddMenuLine_NameWithStrLen(			const TCHAR *ParentItemName, size_t ParentItemNameLength ) ;														// メニューのリストに区切り線を追加する( ParentItemName:区切り線を付ける項目リストの親の名前、リストの末端に区切り線を追加します )
extern	int			InsertMenuItem_Name(				const TCHAR *ItemName,                        const TCHAR *NewItemName                           ) ;				// 指定の項目と、指定の項目の一つ上の項目との間に新しい項目を追加する
extern	int			InsertMenuItem_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, const TCHAR *NewItemName, size_t NewItemNameLength ) ;				// 指定の項目と、指定の項目の一つ上の項目との間に新しい項目を追加する
extern	int			InsertMenuLine_Name(				const TCHAR *ItemName                        ) ;																	// 指定の項目と、指定の項目の一つ上の項目との間に区切り線を追加する
extern	int			InsertMenuLine_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength ) ;																	// 指定の項目と、指定の項目の一つ上の項目との間に区切り線を追加する
extern	int			DeleteMenuItem_Name(				const TCHAR *ItemName                        ) ;																	// メニューから選択項目を削除する
extern	int			DeleteMenuItem_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength ) ;																	// メニューから選択項目を削除する
extern	int			CheckMenuItemSelect_Name(			const TCHAR *ItemName                        ) ;																	// メニューが選択されたかどうかを取得する( 戻り値　0:選択されていない  1:選択された )
extern	int			CheckMenuItemSelect_NameWithStrLen(	const TCHAR *ItemName, size_t ItemNameLength ) ;																	// メニューが選択されたかどうかを取得する( 戻り値　0:選択されていない  1:選択された )
extern	int			SetMenuItemEnable_Name(				const TCHAR *ItemName,                        int EnableFlag ) ;													// メニューの項目を選択出来るかどうかを設定する( EnableFlag　1:選択できる  0:選択できない )
extern	int			SetMenuItemEnable_NameWithStrLen(	const TCHAR *ItemName, size_t ItemNameLength, int EnableFlag ) ;													// メニューの項目を選択出来るかどうかを設定する( EnableFlag　1:選択できる  0:選択できない )
extern	int			SetMenuItemMark_Name(				const TCHAR *ItemName,                        int Mark ) ;															// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等 ) )
extern	int			SetMenuItemMark_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, int Mark ) ;															// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等 ) )

extern	int			AddMenuItem_ID(						int ParentItemID, const TCHAR *NewItemName,                           int NewItemID = -1 ) ;						// メニューに選択項目を追加する
extern	int			AddMenuItem_IDWithStrLen(			int ParentItemID, const TCHAR *NewItemName, size_t NewItemNameLength, int NewItemID = -1 ) ;						// メニューに選択項目を追加する
extern	int			AddMenuLine_ID(						int ParentItemID ) ;																								// メニューのリストに区切り線を追加する
extern	int			InsertMenuItem_ID(					int ItemID, int NewItemID ) ;																						// 指定の項目と、指定の項目の一つ上の項目との間に新しい項目を追加する
extern	int			InsertMenuLine_ID(					int ItemID, int NewItemID ) ;																						// 指定の項目と、指定の項目の一つ上の項目との間に区切り線を追加する
extern	int			DeleteMenuItem_ID(					int ItemID ) ;																										// メニューから選択項目を削除する
extern	int			CheckMenuItemSelect_ID(				int ItemID ) ;																										// メニューが選択されたかどうかを取得する( 戻り値　　0:選択されていない  1:選択された )
extern	int			SetMenuItemEnable_ID(				int ItemID, int EnableFlag ) ;																						// メニューの項目を選択出来るかどうかを設定する( EnableFlag　1:選択できる  0:選択できない )
extern	int			SetMenuItemMark_ID(					int ItemID, int Mark ) ;																							// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等 ) )

extern	int			DeleteMenuItemAll(					void ) ;																											// メニューの全ての選択項目を削除する
extern	int			ClearMenuItemSelect(				void ) ;																											// メニューが選択されたかどうかの情報をリセット
extern	int			GetMenuItemID(						const TCHAR *ItemName                        ) ;																	// メニューの項目名から項目識別番号を取得する
extern	int			GetMenuItemIDWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength ) ;																	// メニューの項目名から項目識別番号を取得する
extern	int			GetMenuItemName(					int ItemID, TCHAR *NameBuffer ) ;																					// メニューの項目識別番号から項目名を取得する
extern	int 		LoadMenuResource(					int MenuResourceID ) ;																								// メニューをリソースから読み込む
extern	int			SetMenuItemSelectCallBackFunction(	void (* CallBackFunction )( const TCHAR *ItemName, int ItemID ) ) ;													// メニューの選択項目が選択されたときに呼ばれるコールバック関数を設定する( CallBackFunction:項目が選択されたときに呼ばれるコールバック関数、引数に項目名と項目の識別番号を渡されて呼ばれる )

extern	int			SetWindowMenu(						int MenuID, int (* MenuProc )( WORD ID ) ) ;																		// (古い関数)ウインドウにメニューを設定する
extern	int			SetDisplayMenuFlag(					int Flag ) ;																										// (古い関数)メニューを表示するかどうかをセットする
extern	int			GetDisplayMenuFlag(					void ) ;																											// (古い関数)メニューを表示しているかどうかを取得する
extern	int			GetUseMenuFlag(						void ) ;																											// メニューを使用しているかどうかを得る
extern	int			SetAutoMenuDisplayFlag(				int Flag ) ;																										// フルスクリーン時にメニューを自動で表示したり非表示にしたりするかどうかのフラグをセットする












// DxNetwork.cpp関数プロトタイプ宣言

#ifndef DX_NON_NETWORK
extern	int			GetWinSockLastError( void ) ;				// WinSock で最後に発生したエラーのコードを取得する
#endif // DX_NON_NETWORK













// DxInputString.cpp関数プロトタイプ宣言

#ifndef DX_NON_KEYEX

extern	int			SetUseTSFFlag( int UseFlag ) ;											// ＩＭＥの漢字変換候補表示の処理に TSF を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )

#endif // DX_NON_KEYEX














// DxInput.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUT

extern	int			SetKeyExclusiveCooperativeLevelFlag(	int Flag ) ;															// DirectInput のキーボードの協調レベルを排他レベルにするかどうかを設定する( TRUE:排他レベルにする  FALSE:標準レベルにする( デフォルト ) )、DxLib_Init の呼び出し前でのみ実行可能　
extern	int			SetKeyboardNotDirectInputFlag(			int Flag ) ;															// キーボードの入力処理に DirectInput を使わないかどうかを設定する( TRUE:DirectInput を使わず、Windows標準機能を使用する　　FALSE:DirectInput を使用する )
extern	int			SetUseDirectInputFlag(					int Flag ) ;															// 入力処理に DirectInput を使用するかどうかを設定する( TRUE:DirectInput を使用する　　FALSE:DirectInput を使わず、Windows標準機能を使用する )
extern	int			SetUseXInputFlag(						int Flag ) ;															// Xbox360コントローラの入力処理に XInput を使用するかどうかを設定する( TRUE:XInput を使用する( デフォルト )　　FALSE:XInput を使用しない )
extern	int			GetJoypadGUID(							int PadIndex, GUID *GuidInstanceBuffer, GUID *GuidProductBuffer = NULL ) ;	// ジョイパッドのＧＵIＤを得る
extern	int			GetJoypadName(							int InputType, TCHAR *InstanceNameBuffer, TCHAR *ProductNameBuffer ) ;	// ジョイパッドのデバイス登録名と製品登録名を取得する( InstanceNameBuffer, ProductNameBuffer 共に 260 以上のバッファサイズが必要 )
extern	int			ConvertKeyCodeToVirtualKey(				int KeyCode ) ;															// ＤＸライブラリのキーコード( KEY_INPUT_A など )に対応する Windows の仮想キーコード( VK_LEFT など ) を取得する( KeyCode:変換したいＤＸライブラリのキーコード　戻り値：Windowsの仮想キーコード )
extern	int			ConvertVirtualKeyToKeyCode(				int VirtualKey ) ;														//  Windows の仮想キーコード( VK_LEFT など ) に対応するＤＸライブラリのキーコード( KEY_INPUT_A など )を取得する( VirtualKey:変換したいWindowsの仮想キーコード　戻り値：ＤＸライブラリのキーコード )

#endif // DX_NON_INPUT











#ifndef DX_NOTUSE_DRAWFUNCTION

// 画像からグラフィックハンドルを作成する関数
extern	int			LoadGraphToResource(				int ResourceID ) ;																														// 画像リソースからグラフィックハンドルを作成する
extern	int			LoadDivGraphToResource(				int ResourceID, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray ) ;											// 画像リソースを分割してグラフィックハンドルを作成する
extern	int			LoadDivGraphFToResource(			int ResourceID, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;											// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern	int			LoadGraphToResource(				const TCHAR *ResourceName,                            const TCHAR *ResourceType                            ) ;																				// 画像リソースからグラフィックハンドルを作成する
extern	int			LoadGraphToResourceWithStrLen(		const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength ) ;																				// 画像リソースからグラフィックハンドルを作成する
extern	int			LoadDivGraphToResource(				const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する
extern	int			LoadDivGraphToResourceWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する
extern	int			LoadDivGraphFToResource(			const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern	int			LoadDivGraphFToResourceWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern	int			CreateGraphFromID3D11Texture2D(     const void *pID3D11Texture2D ) ;							// ID3D11Texture2D からグラフィックハンドルを作成する

// 画像情報関係関数
extern	const void*	GetGraphID3D11Texture2D(		int GrHandle ) ;												// グラフィックハンドルが持つ ID3D11Texture2D を取得する( Direct3D11 を使用している場合のみ有効 )( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	GetGraphID3D11RenderTargetView(	int GrHandle ) ;												// グラフィックハンドルが持つ ID3D11RenderTargetView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	GetGraphID3D11DepthStencilView(	int GrHandle ) ;												// グラフィックハンドルが持つ ID3D11DepthStencilView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11DepthStencilView * にキャストしてください )

// 画面関係関数
extern	int			BltBackScreenToWindow(			HWND Window, int ClientX, int ClientY ) ;										// 裏画面の内容を指定のウインドウに転送する
extern	int			BltRectBackScreenToWindow(		HWND Window, RECT BackScreenRect, RECT WindowClientRect ) ;						// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int			SetScreenFlipTargetWindow(		HWND TargetWindow, double ScaleX = 1.0 , double ScaleY = 1.0 ) ;				// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int			GetDesktopScreenGraph(			int x1, int y1, int x2, int y2, int GrHandle, int DestX = 0 , int DestY = 0 ) ;	// デスクトップ画面から指定領域の画像情報をグラフィックハンドルに転送する

// その他設定関係関数
extern	int			SetMultiThreadFlag(								int Flag ) ;									// DirectDraw や Direct3D の協調レベルをマルチスレッド対応にするかどうかをセットする( TRUE:マルチスレッド対応にする  FALSE:マルチスレッド対応にしない( デフォルト ) )
extern	int			SetUseDirectDrawDeviceIndex(					int Index ) ;									// 使用する DirectDraw デバイスのインデックスを設定する
extern	int			SetAeroDisableFlag(								int Flag ) ;									// Vista,7 の Windows Aero を無効にするかどうかを設定する( TRUE:無効にする( デフォルト )  FALSE:有効にする )( DxLib_Init の前に呼ぶ必要があります )
extern	int			SetUseDirect3D9Ex(								int Flag ) ;									// Vista以降の環境で Direct3D9Ex を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )( DxLib_Init の前に呼ぶ必要があります )
extern	int			SetUseDirect3D11(								int Flag ) ;									// Direct3D11 を使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない )
extern	int			SetUseDirect3D11MinFeatureLevel(				int Level /* DX_DIRECT3D_11_FEATURE_LEVEL_10_0 など */ ) ;		// Direct3D11 で使用する最低機能レベルを指定する関数です、尚、DX_DIRECT3D_11_FEATURE_LEVEL_11_0 より低い機能レベルでの正常な動作は保証しません( デフォルトは DX_DIRECT3D_11_FEATURE_LEVEL_11_0 )
extern	int			SetUseDirect3D11WARPDriver(						int Flag ) ;									// D3D_DRIVER_TYPE_WARP タイプの Direct3D 11 ドライバを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern	int			SetUseDirect3DVersion(							int Version /* DX_DIRECT3D_9 など */ ) ;		// 使用する Direct3D のバージョンを設定する、DxLib_Init 呼び出しの前でのみ使用可能　
extern	int			GetUseDirect3DVersion(							void ) ;										// 使用している Direct3D のバージョンを取得する( DX_DIRECT3D_9 など )
extern	int			GetUseDirect3D11FeatureLevel(					void ) ;										// 使用している Direct3D11 の FeatureLevel ( DX_DIRECT3D_11_FEATURE_LEVEL_9_1 等 )を取得する( 戻り値　-1：エラー　-1以外：Feature Level )
extern	int			SetUseDirectDrawFlag(							int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )DirectDrawを使用するかどうかを設定する
extern	int			SetUseGDIFlag(									int Flag ) ;									// GDI描画を使用するかどうかを設定する
extern	int			GetUseGDIFlag(									void ) ;										// GDI描画を使用するかどうかを取得する
extern	int			SetDDrawUseGuid(								const GUID *Guid ) ;							// DirectDrawが使用するGUIDを設定する
extern	const void*	GetUseDDrawObj(									void ) ;										// 現在使用しているDirectDrawオブジェクトのアドレスを取得する( 戻り値を IDirectDraw7 * にキャストして下さい )
extern	const GUID*	GetDirectDrawDeviceGUID(						int Number ) ;									// 有効な DirectDraw デバイスの GUID を取得する
extern	int			GetDirectDrawDeviceDescription(					int Number, char *StringBuffer ) ;				// 有効な DirectDraw デバイスの名前を取得する
extern	int			GetDirectDrawDeviceNum(							void ) ;										// 有効な DirectDraw デバイスの数を取得する
extern	const void*	GetUseDirect3DDevice9(							void ) ;										// 使用中のDirect3DDevice9オブジェクトを取得する( 戻り値を IDirect3DDevice9 * にキャストして下さい )
extern	const void*	GetUseDirect3D9BackBufferSurface(				void ) ;										// 使用中のバックバッファのDirect3DSurface9オブジェクトを取得する( 戻り値を D_IDirect3DSurface9 * にキャストしてください )
extern	const void*	GetUseDirect3D11Device(							void ) ;										// 使用中のID3D11Deviceオブジェクトを取得する( 戻り値を ID3D11Device * にキャストして下さい )
extern	const void*	GetUseDirect3D11DeviceContext(					void ) ;										// 使用中のID3D11DeviceContextオブジェクトを取得する( 戻り値を ID3D11DeviceContext * にキャストして下さい )
extern	const void*	GetUseDirect3D11BackBufferTexture2D(			void ) ;										// 使用中のバックバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	GetUseDirect3D11BackBufferRenderTargetView(		void ) ;										// 使用中のバックバッファのID3D11RenderTargetViewオブジェクトを取得する( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	GetUseDirect3D11DepthStencilTexture2D(			void ) ;										// 使用中の深度ステンシルバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	int			SetDrawScreen_ID3D11RenderTargetView(			const void *pID3D11RenderTargetView, const void *pID3D11DepthStencilView = NULL ) ;		// 指定の ID3D11RenderTargetView を描画対象にする( pID3D11DepthStencilView が NULL の場合はデフォルトの深度ステンシルバッファを使用する )
extern	int			RefreshDxLibDirect3DSetting(					void ) ;										// ＤＸライブラリが行ったDirect3Dの設定を再度行う( 特殊用途 )

#ifndef DX_NON_MEDIA_FOUNDATION
extern	int			SetUseMediaFoundationFlag(						int Flag ) ;									// Media Foundation を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
#endif // DX_NON_MEDIA_FOUNDATION

// 色情報取得用関数
extern	int			ColorKaiseki(				const void *PixelData, COLORDATA* ColorData ) ;																				// 色ビット情報解析










// DxMask.cpp 関数プロトタイプ宣言
#ifndef DX_NON_MASK

extern	int			BmpBltToMask(					HBITMAP Bmp, int BmpPointX, int BmpPointY, int MaskHandle ) ;									// マスクハンドルにＢＭＰデータを転送する

#endif // DX_NON_MASK

#endif // DX_NOTUSE_DRAWFUNCTION









// DxFont.cpp 関数プロトタイプ宣言

#ifndef DX_NON_FONT

extern	HANDLE		AddFontFile(					const TCHAR *FontFilePath                            ) ;										// 指定のフォントファイルをシステムに追加する( 戻り値  NULL:失敗  NULL以外:フォントハンドル( WindowsOS のものなので、ＤＸライブラリのフォントハンドルとは別物です ) )
extern	HANDLE		AddFontFileWithStrLen(			const TCHAR *FontFilePath, size_t FontFilePathLength ) ;										// 指定のフォントファイルをシステムに追加する( 戻り値  NULL:失敗  NULL以外:フォントハンドル( WindowsOS のものなので、ＤＸライブラリのフォントハンドルとは別物です ) )
extern	HANDLE		AddFontFileFromMem(				const void *FontFileImage, int FontFileImageSize ) ;											// 指定のメモリアドレスに展開したフォントファイルイメージをシステムに追加する( 戻り値  NULL:失敗  NULL以外:フォントハンドル( WindowsOS のものなので、ＤＸライブラリのフォントハンドルとは別物です ) )
extern	int			RemoveFontFile(					HANDLE FontHandle ) ;																			// 指定のフォントハンドルをシステムから削除する( 引数は AddFontFile や AddFontFileFromMem の戻り値 )

#ifndef DX_NON_SAVEFUNCTION

extern	int			CreateFontDataFile(				const TCHAR *SaveFilePath,                            const TCHAR *FontName,                        int Size, int BitDepth /* DX_FONTIMAGE_BIT_1等 */ , int Thick, int Italic = FALSE , int CharSet = -1 , const TCHAR *SaveCharaList = NULL                                  ) ;				// フォントデータファイルを作成する
extern	int			CreateFontDataFileWithStrLen(	const TCHAR *SaveFilePath, size_t SaveFilePathLength, const TCHAR *FontName, size_t FontNameLength, int Size, int BitDepth /* DX_FONTIMAGE_BIT_1等 */ , int Thick, int Italic = FALSE , int CharSet = -1 , const TCHAR *SaveCharaList = NULL , size_t SaveCharaListLength = 0 ) ;				// フォントデータファイルを作成する

#endif // DX_NON_SAVEFUNCTION
#endif // DX_NON_FONT









// 基本イメージデータのロード＋ＤＩＢ関係
extern	HBITMAP		CreateDIBGraph(								const TCHAR *FileName,                                                                                                                                                               int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイルからＤＩＢデータを作成する
extern	HBITMAP		CreateDIBGraphWithStrLen(					const TCHAR *FileName, size_t FileNameLength,                                                                                                                                        int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイルからＤＩＢデータを作成する
extern	HBITMAP		CreateDIBGraphToMem(						const BITMAPINFO *BmpInfo, const void *GraphData,                                                                                                                                    int ReverseFlag,          COLORDATA *SrcColor ) ;					// ＢＭＰデータからＤＩＢデータクを作成する
extern	int			CreateDIBGraph_plus_Alpha(					const TCHAR *FileName,                        HBITMAP *RGBBmp, HBITMAP *AlphaBmp,                                                                                                    int ReverseFlag = FALSE , COLORDATA *SrcColor = NULL ) ;			// 画像ファイルからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	int			CreateDIBGraph_plus_AlphaWithStrLen(		const TCHAR *FileName, size_t FileNameLength, HBITMAP *RGBBmp, HBITMAP *AlphaBmp,                                                                                                    int ReverseFlag = FALSE , COLORDATA *SrcColor = NULL ) ;			// 画像ファイルからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	HBITMAP		CreateDIBGraphVer2(							const TCHAR *FileName,                        const void *MemImage, int MemImageSize,                                             int ImageType,                                     int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータを作成する
extern	HBITMAP		CreateDIBGraphVer2WithStrLen(				const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize,                                             int ImageType,                                     int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータを作成する
extern	int			CreateDIBGraphVer2_plus_Alpha(				const TCHAR *FileName,                        const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	int			CreateDIBGraphVer2_plus_AlphaWithStrLen(	const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	int			ConvBitmapToGraphImage(						const BITMAPINFO *BmpInfo, void *GraphData, BASEIMAGE *GraphImage, int CopyFlag ) ;																																			// ＢＭＰデータから基本イメージデータを構築する( Ret 0:正常終了  1:コピーを行った  -1:エラー )
extern	int			ConvGraphImageToBitmap(						const BASEIMAGE *GraphImage, BITMAPINFO *BmpInfo, void **GraphData, int CopyFlag, int FullColorConv = TRUE ) ;																												// 基本イメージデータをＢＭＰデータに変換するGraphImage を ＢＭＰ に変換する(アルファデータはあっても無視される)( 戻り値　0:正常終了  1:コピーを行った  -1:エラー )

// 基本イメージデータ構造体関係
extern	int			UpdateLayerdWindowForBaseImage(                       const BASEIMAGE *BaseImage ) ;																										// 基本イメージデータを使用して UpdateLayerdWindow を行う
extern	int			UpdateLayerdWindowForBaseImageRect(                   const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 ) ;																		// 基本イメージデータの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int			UpdateLayerdWindowForPremultipliedAlphaBaseImage(     const BASEIMAGE *BaseImage ) ;																										// 乗算済みアルファの基本イメージデータを使用して UpdateLayerdWindow を行う
extern	int			UpdateLayerdWindowForPremultipliedAlphaBaseImageRect( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 ) ;																		// 乗算済みアルファの基本イメージデータの指定の範囲を使用して UpdateLayerdWindow を行う

// デスクトップキャプチャ
extern	int			GetDesktopScreenBaseImage( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY ) ;						// デスクトップの指定の領域を基本イメージデータに転送する










// DxSoftImage.cpp関数プロトタイプ宣言
#ifndef DX_NON_SOFTIMAGE

extern	int			UpdateLayerdWindowForSoftImage(                       int SIHandle ) ;												// ソフトウエアイメージハンドルを使用して UpdateLayerdWindow を行う
extern	int			UpdateLayerdWindowForSoftImageRect(                   int SIHandle, int x1, int y1, int x2, int y2 ) ;				// ソフトウエアイメージハンドルの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int			UpdateLayerdWindowForPremultipliedAlphaSoftImage(     int SIHandle ) ;												// 乗算済みアルファのソフトウエアイメージハンドルを使用して UpdateLayerdWindow を行う
extern	int			UpdateLayerdWindowForPremultipliedAlphaSoftImageRect( int SIHandle, int x1, int y1, int x2, int y2 ) ;				// 乗算済みアルファのソフトウエアイメージハンドルの指定の範囲を使用して UpdateLayerdWindow を行う

// デスクトップキャプチャ
extern	int			GetDesktopScreenSoftImage( int x1, int y1, int x2, int y2, int SIHandle,         int DestX, int DestY ) ;						// デスクトップの指定の領域をソフトウエアイメージハンドルに転送する

#endif // DX_NON_SOFTIMAGE










// DxSound.cpp関数プロトタイプ宣言
#ifndef DX_NON_SOUND

// サウンドデータ管理系関数
extern	int			LoadSoundMemByResource(              const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            int BufferNum = 1 ) ;				// サウンドリソースからサウンドハンドルを作成する
extern	int			LoadSoundMemByResourceWithStrLen(    const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int BufferNum = 1 ) ;				// サウンドリソースからサウンドハンドルを作成する

// 設定関係関数
extern	int			SetUseSoftwareMixingSoundFlag(       int Flag ) ;																																	// サウンドの処理をソフトウエアで行うかどうかを設定する( TRUE:ソフトウエア  FALSE:ハードウエア( デフォルト ) )
extern	int			SetEnableXAudioFlag(                 int Flag ) ;																																	// サウンドの再生にXAudioを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )

// 情報取得系関数
extern	const void*	GetDSoundObj(                        void ) ;	/* 戻り値を IDirectSound * にキャストして下さい */																					// ＤＸライブラリが使用している DirectSound オブジェクトを取得する

// ＭＩＤＩ制御関数
extern	int			LoadMusicMemByResource(				const TCHAR *ResourceName,                            const TCHAR *ResourceType                            ) ;									// リソース上のＭＩＤＩファイルからＭＩＤＩハンドルを作成する
extern	int			LoadMusicMemByResourceWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength ) ;									// リソース上のＭＩＤＩファイルからＭＩＤＩハンドルを作成する
extern	int			PlayMusicByResource(				const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            int PlayType ) ;	// リソースからＭＩＤＩファイルを読み込んで演奏する
extern	int			PlayMusicByResourceWithStrLen(		const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int PlayType ) ;	// リソースからＭＩＤＩファイルを読み込んで演奏する

#endif // DX_NON_SOUND









#undef DX_FUNCTION_START
#define DX_FUNCTION_END
#undef DX_FUNCTION_END

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXFUNCTIONWIN

