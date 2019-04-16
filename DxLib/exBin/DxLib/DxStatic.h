// ----------------------------------------------------------------------------
//
//		ＤＸライブラリ　内部使用関連用ヘッダファイル
//
//				Ver 3.20c
//
// ----------------------------------------------------------------------------

// 多重インクルード防止用マクロ
#ifndef __DXSTATIC_H__
#define __DXSTATIC_H__

// インクルード --------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 ----------------------------------

#ifdef VISUALCPP_2005
#define RST  __restrict
#else
#define RST
#endif

// 構造体定義 --------------------------------------------------------------------

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

// データ定義 --------------------------------------------

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

// 関数宣言 -----------------------------------------------

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// ライブラリ関数内部呼び出し簡略化マクロ -----------------


// DxSystem.cpp関数プロトタイプ宣言

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

// 初期化終了系関数
extern	int			NS_DxLib_Init( void ) ;													// ライブラリ初期化関数
extern	int			NS_DxLib_End( void ) ;														// ライブラリ使用の終了関数

// メッセージ処理関数
extern	int			NS_ProcessMessage( void ) ;												// ウインドウズのメッセージループに代わる処理を行う

#else

// 初期化終了系関数
#define NS_DxLib_Init	DxLib_Init
#define NS_DxLib_End	DxLib_End

// メッセージ処理関数
#define NS_ProcessMessage		ProcessMessage

#endif


#ifdef DX_THREAD_SAFE

// 設定系関数
extern	int			NS_SetAlwaysRunFlag(						int Flag ) ;																		// ウインドウがアクティブではない状態でも処理を続行するか、フラグをセットする

extern	int			NS_DxLib_GlobalStructInitialize( void ) ;									// ライブラリの内部で使用している構造体をゼロ初期化して、DxLib_Init の前に行った設定を無効化する( DxLib_Init の前でのみ有効 )
extern	int			NS_DxLib_IsInit( void ) ;													// ライブラリが初期化されているかどうかを取得する( 戻り値: TRUE=初期化されている  FALSE=されていない )

// エラー関係関数
extern	int			NS_GetLastErrorCode( void ) ;												// 最後に発生したエラーのエラーコードを取得する( 戻り値　0:エラーが発生していない、又はエラーコード出力に対応したエラーが発生していない　　0以外：エラーコード、DX_ERRORCODE_WIN_DESKTOP_24BIT_COLOR など )
extern	int			NS_GetLastErrorMessage( TCHAR *StringBuffer, int StringBufferBytes ) ;		// 最後に発生したエラーのエラーメッセージを指定の文字列バッファに取得する

#else

#define NS_SetAlwaysRunFlag					SetAlwaysRunFlag

#define NS_DxLib_GlobalStructInitialize		DxLib_GlobalStructInitialize
#define NS_DxLib_IsInit						DxLib_IsInit

#define NS_GetLastErrorCode					GetLastErrorCode
#define NS_GetLastErrorMessage				GetLastErrorMessage

#endif






// DxNetwork.cpp関数プロトタイプ宣言

#ifdef DX_THREAD_SAFE

#ifndef DX_NON_NETWORK

// 通信関係
extern	int			NS_ProcessNetMessage(				int RunReleaseProcess = FALSE ) ;														// 通信メッセージの処理をする関数

extern	int			NS_GetHostIPbyName(					const TCHAR *HostName,							IPDATA      *IPDataBuf, int IPDataBufLength = 1, int *IPDataGetNum = NULL ) ;	// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv4版 )
extern	int			NS_GetHostIPbyNameWithStrLen(		const TCHAR *HostName, size_t HostNameLength,	IPDATA      *IPDataBuf, int IPDataBufLength = 1, int *IPDataGetNum = NULL ) ;	// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv4版 )
extern	int			NS_GetHostIPbyName_IPv6(			const TCHAR *HostName,							IPDATA_IPv6 *IPDataBuf, int IPDataBufLength = 1, int *IPDataGetNum = NULL ) ;	// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv6版 )
extern	int			NS_GetHostIPbyName_IPv6WithStrLen(	const TCHAR *HostName, size_t HostNameLength,	IPDATA_IPv6 *IPDataBuf, int IPDataBufLength = 1, int *IPDataGetNum = NULL ) ;	// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv6版 )
extern	int			NS_ConnectNetWork(					IPDATA      IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv4版 )
extern	int			NS_ConnectNetWork_IPv6(			IPDATA_IPv6 IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv6版 )
extern	int			NS_ConnectNetWork_ASync(			IPDATA      IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv4版 )、非同期版
extern	int			NS_ConnectNetWork_IPv6_ASync(		IPDATA_IPv6 IPData, int Port = -1 ) ;													// 他マシンに接続する( IPv6版 )、非同期版
extern	int			NS_PreparationListenNetWork(		int Port = -1 ) ;																		// 接続を受けられる状態にする( IPv4版 )
extern	int			NS_PreparationListenNetWork_IPv6(	int Port = -1 ) ;																		// 接続を受けられる状態にする( IPv6版 )
extern	int			NS_StopListenNetWork(				void ) ;																				// 接続を受けつけ状態の解除
extern	int			NS_CloseNetWork(					int NetHandle ) ;																		// 接続を終了する

extern	int			NS_GetNetWorkAcceptState(			int NetHandle ) ;																		// 接続状態を取得する
extern	int			NS_GetNetWorkDataLength(			int NetHandle ) ;																		// 受信データの量を得る
extern	int			NS_GetNetWorkSendDataLength(		int NetHandle ) ;																		// 未送信のデータの量を得る 
extern	int			NS_GetNewAcceptNetWork(			void ) ;																				// 新たに接続した通信回線を得る
extern	int			NS_GetLostNetWork(					void ) ;																				// 接続を切断された通信回線を得る
extern	int			NS_GetNetWorkIP(					int NetHandle, IPDATA      *IpBuf ) ;													// 接続先のＩＰを得る( IPv4版 )
extern	int			NS_GetNetWorkIP_IPv6(				int NetHandle, IPDATA_IPv6 *IpBuf ) ;													// 接続先のＩＰを得る( IPv6版 )
extern	int			NS_GetMyIPAddress(					IPDATA *IpBuf, int IpBufLength = 1, int *IpNum = NULL ) ;							// 自分のＩＰを得る
extern	int			NS_GetMyIPAddress_IPv6(				IPDATA_IPv6 *IpBuf, int IpBufLength = 1 , int *IpNum = NULL ) ;							// 自分のIPv6を得る
extern	int			NS_SetConnectTimeOutWait(			int Time ) ;																			// 接続のタイムアウトまでの時間を設定する
extern	int			NS_SetUseDXNetWorkProtocol(		int Flag ) ;																			// ＤＸライブラリの通信形態を使うかどうかをセットする
extern	int			NS_GetUseDXNetWorkProtocol(		void ) ; 																				// ＤＸライブラリの通信形態を使うかどうかを取得する
extern	int			NS_SetUseDXProtocol(				int Flag ) ;																			// SetUseDXNetWorkProtocol の別名
extern	int			NS_GetUseDXProtocol(				void ) ; 																				// GetUseDXNetWorkProtocol の別名
extern	int			NS_SetNetWorkCloseAfterLostFlag(	int Flag ) ;																			// 接続が切断された直後に接続ハンドルを解放するかどうかのフラグをセットする
extern	int			NS_GetNetWorkCloseAfterLostFlag(	void ) ;																				// 接続が切断された直後に接続ハンドルを解放するかどうかのフラグを取得する
//extern	int			NS_SetProxySetting( int UseFlag, const char *Address, int Port ) ;														// ＨＴＴＰ通信で使用するプロキシ設定を行う
//extern	int			NS_GetProxySetting( int *UseFlagBuffer, char *AddressBuffer, int *PortBuffer ) ;										// ＨＴＴＰ通信で使用するプロキシ設定を取得する
//extern	int			NS_SetIEProxySetting( void ) ;																							// ＩＥのプロキシ設定を適応する

extern	int			NS_NetWorkRecv(			int NetHandle, void *Buffer, int Length ) ;														// 受信したデータを読み込む
extern	int			NS_NetWorkRecvToPeek(		int NetHandle, void *Buffer, int Length ) ;														// 受信したデータを読み込む、読み込んだデータはバッファから削除されない
extern	int			NS_NetWorkRecvBufferClear(	int NetHandle ) ;																				// 受信したデータをクリアする
extern	int			NS_NetWorkSend(			int NetHandle, const void *Buffer, int Length ) ;														// データを送信する

extern	int			NS_MakeUDPSocket(			int RecvPort = -1 ) ;																			// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )
extern	int			NS_MakeUDPSocket_IPv6(		int RecvPort = -1 ) ;																			// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )( IPv6版 )
extern	int			NS_DeleteUDPSocket(		int NetUDPHandle ) ;																			// UDPを使用した通信を行うソケットハンドルを削除する
extern	int			NS_NetWorkSendUDP(		int NetUDPHandle, IPDATA       SendIP, int SendPort,  const void *Buffer, int Length ) ;				// UDPを使用した通信で指定のＩＰにデータを送信する、Length は最大65507、SendPort を -1 にすると MakeUDPSocket に RecvPort で渡したポートが使用されます( 戻り値  0以上;送信できたデータサイズ  -1:エラー  -2:送信データが大きすぎる  -3:送信準備ができていない  )
extern	int			NS_NetWorkSendUDP_IPv6(	int NetUDPHandle, IPDATA_IPv6  SendIP, int SendPort,  const void *Buffer, int Length ) ;				// UDPを使用した通信で指定のＩＰにデータを送信する、Length は最大65507、SendPort を -1 にすると MakeUDPSocket に RecvPort で渡したポートが使用されます( 戻り値  0以上;送信できたデータサイズ  -1:エラー  -2:送信データが大きすぎる  -3:送信準備ができていない  )( IPv6版 )
extern	int			NS_NetWorkRecvUDP(		int NetUDPHandle, IPDATA      *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek ) ;	// UDPを使用した通信でデータを受信する、Peek に TRUE を渡すと受信に成功してもデータを受信キューから削除しません( 戻り値  0以上:受信したデータのサイズ  -1:エラー  -2:バッファのサイズが足りない  -3:受信データがない )
extern	int			NS_NetWorkRecvUDP_IPv6(	int NetUDPHandle, IPDATA_IPv6 *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek ) ;	// UDPを使用した通信でデータを受信する、Peek に TRUE を渡すと受信に成功してもデータを受信キューから削除しません( 戻り値  0以上:受信したデータのサイズ  -1:エラー  -2:バッファのサイズが足りない  -3:受信データがない )( IPv6版 )
//extern int		NS_CheckNetWorkSendUDP(	int NetUDPHandle ) ;																			// UDPを使用した通信でデータが送信できる状態かどうかを調べる( 戻り値  -1:エラー  TRUE:送信可能  FALSE:送信不可能 )
extern	int			NS_CheckNetWorkRecvUDP(	int NetUDPHandle ) ;																			// UDPを使用した通信で新たな受信データが存在するかどうかを調べる( 戻り値  -1:エラー  TRUE:受信データあり  FALSE:受信データなし )

/*	使用不可
extern	int			NS_HTTP_FileDownload(			const char *FileURL, const char *SavePath = NULL , void **SaveBufferP = NULL , int *FileSize = NULL , char **ParamList = NULL ) ;						// HTTP を使用してネットワーク上のファイルをダウンロードする
extern	int			NS_HTTP_GetFileSize(			const char *FileURL ) ;																		// HTTP を使用してネットワーク上のファイルのサイズを得る

extern	int			NS_HTTP_StartFileDownload(		const char *FileURL, const char *SavePath, void **SaveBufferP = NULL , char **ParamList = NULL ) ;	// HTTP を使用したネットワーク上のファイルをダウンロードする処理を開始する
extern	int			NS_HTTP_StartGetFileSize(		const char *FileURL ) ;																		// HTTP を使用したネットワーク上のファイルのサイズを得る処理を開始する
extern	int			NS_HTTP_Close(					int HttpHandle ) ;																			// HTTP の処理を終了し、ハンドルを解放する
extern	int			NS_HTTP_CloseAll(				void ) ;																					// 全てのハンドルに対して HTTP_Close を行う
extern	int			NS_HTTP_GetState(				int HttpHandle ) ;																			// HTTP 処理の現在の状態を得る( NET_RES_COMPLETE 等 )
extern	int			NS_HTTP_GetError(				int HttpHandle ) ;																			// HTTP 処理でエラーが発生した場合、エラーの内容を得る( HTTP_ERR_NONE 等 )
extern	int			NS_HTTP_GetDownloadFileSize(	int HttpHandle ) ;																			// HTTP 処理で対象となっているファイルのサイズを得る( 戻り値: -1 = エラー・若しくはまだファイルのサイズを取得していない  0以上 = ファイルのサイズ )
extern	int			NS_HTTP_GetDownloadedFileSize( int HttpHandle ) ;																			// HTTP 処理で既にダウンロードしたファイルのサイズを取得する

extern	int			NS_fgetsForNetHandle(			int NetHandle, char *strbuffer ) ;															// fgets のネットワークハンドル版( -1:取得できず 0:取得できた )
extern	int			NS_URLAnalys(					const char *URL, char *HostBuf = NULL , char *PathBuf = NULL , char *FileNameBuf = NULL , int *PortBuf = NULL ) ;	// ＵＲＬを解析する
extern	int			NS_URLConvert(					char *URL, int ParamConvert = TRUE , int NonConvert = FALSE ) ;								// HTTP に渡せない記号が使われた文字列を渡せるような文字列に変換する( 戻り値: -1 = エラー  0以上 = 変換後の文字列のサイズ )
extern	int			NS_URLParamAnalysis(			char **ParamList, char **ParamStringP ) ;													// HTTP 用パラメータリストから一つのパラメータ文字列を作成する( 戻り値:  -1 = エラー  0以上 = パラメータの文字列の長さ )
*/

#endif	// DX_NON_NETWORK

#else	// DX_THREAD_SAFE

#ifndef DX_NON_NETWORK

// 通信関係
#define NS_ProcessNetMessage					ProcessNetMessage

#define NS_GetHostIPbyName						GetHostIPbyName
#define NS_GetHostIPbyNameWithStrLen			GetHostIPbyNameWithStrLen
#define NS_GetHostIPbyName_IPv6					GetHostIPbyName_IPv6
#define NS_GetHostIPbyName_IPv6WithStrLen		GetHostIPbyName_IPv6WithStrLen
#define NS_ConnectNetWork						ConnectNetWork
#define NS_ConnectNetWork_IPv6					ConnectNetWork_IPv6
#define NS_ConnectNetWork_ASync					ConnectNetWork_ASync
#define NS_ConnectNetWork_IPv6_ASync			ConnectNetWork_IPv6_ASync
#define NS_PreparationListenNetWork				PreparationListenNetWork
#define NS_PreparationListenNetWork_IPv6		PreparationListenNetWork_IPv6
#define NS_StopListenNetWork					StopListenNetWork
#define NS_CloseNetWork							CloseNetWork

#define NS_GetNetWorkAcceptState				GetNetWorkAcceptState
#define NS_GetNetWorkDataLength					GetNetWorkDataLength
#define NS_GetNetWorkSendDataLength				GetNetWorkSendDataLength
#define NS_GetNewAcceptNetWork					GetNewAcceptNetWork
#define NS_GetLostNetWork						GetLostNetWork
#define NS_GetNetWorkIP							GetNetWorkIP
#define NS_GetNetWorkIP_IPv6					GetNetWorkIP_IPv6
#define NS_GetMyIPAddress						GetMyIPAddress
#define NS_GetMyIPAddress_IPv6					GetMyIPAddress_IPv6
#define NS_SetConnectTimeOutWait				SetConnectTimeOutWait
#define NS_SetUseDXNetWorkProtocol				SetUseDXNetWorkProtocol
#define NS_GetUseDXNetWorkProtocol				GetUseDXNetWorkProtocol
#define NS_SetUseDXProtocol						SetUseDXProtocol
#define NS_GetUseDXProtocol						GetUseDXProtocol
#define NS_SetNetWorkCloseAfterLostFlag			SetNetWorkCloseAfterLostFlag
#define NS_GetNetWorkCloseAfterLostFlag			GetNetWorkCloseAfterLostFlag
//#define NS_SetProxySetting					SetProxySetting
//#define NS_GetProxySetting					GetProxySetting
//#define NS_SetIEProxySetting					SetIEProxySetting

#define NS_NetWorkRecv							NetWorkRecv
#define NS_NetWorkRecvToPeek					NetWorkRecvToPeek
#define NS_NetWorkRecvBufferClear				NetWorkRecvBufferClear
#define NS_NetWorkSend							NetWorkSend

#define NS_MakeUDPSocket						MakeUDPSocket
#define NS_MakeUDPSocket_IPv6					MakeUDPSocket_IPv6
#define NS_DeleteUDPSocket						DeleteUDPSocket
#define NS_NetWorkSendUDP						NetWorkSendUDP
#define NS_NetWorkSendUDP_IPv6					NetWorkSendUDP_IPv6
#define NS_NetWorkRecvUDP						NetWorkRecvUDP
#define NS_NetWorkRecvUDP_IPv6					NetWorkRecvUDP_IPv6
#define NS_CheckNetWorkSendUDP					CheckNetWorkSendUDP
#define NS_CheckNetWorkRecvUDP					CheckNetWorkRecvUDP

/* 使用不可
#define NS_HTTP_FileDownload					HTTP_FileDownload
#define NS_HTTP_GetFileSize						HTTP_GetFileSize

#define NS_HTTP_StartFileDownload				HTTP_StartFileDownload
#define NS_HTTP_StartGetFileSize				HTTP_StartGetFileSize
#define NS_HTTP_Close							HTTP_Close
#define NS_HTTP_CloseAll						HTTP_CloseAll
#define NS_HTTP_GetState						HTTP_GetState
#define NS_HTTP_GetError						HTTP_GetError
#define NS_HTTP_GetDownloadFileSize				HTTP_GetDownloadFileSize
#define NS_HTTP_GetDownloadedFileSize			HTTP_GetDownloadedFileSize

#define NS_fgetsForNetHandle					fgetsForNetHandle
#define NS_URLAnalys							URLAnalys
#define NS_URLConvert							URLConvert
#define NS_URLParamAnalysis						URLParamAnalysis
*/

#endif	// DX_NON_NETWORK

#endif	// DX_THREAD_SAFE









#ifdef DX_THREAD_SAFE


// DxSystem.cpp関数プロトタイプ宣言

// ウエイト系関数
extern	int			NS_WaitTimer( int WaitTime ) ;												// 指定の時間だけ処理をとめる
#ifndef DX_NON_INPUT
extern	int			NS_WaitKey( void ) ;														// キーの入力待ち
#endif // DX_NON_INPUT

// カウンタ及び時刻取得系関数
extern	int			NS_GetNowCount(				int UseRDTSCFlag = FALSE ) ;				// ミリ秒単位の精度を持つカウンタの現在値を得る
extern	LONGLONG	NS_GetNowHiPerformanceCount(	int UseRDTSCFlag = FALSE ) ;				// GetNowCountの高精度バージョン
extern	int			NS_GetDateTime(				DATEDATA *DateBuf ) ;						// 現在時刻を取得する 

// 乱数取得
extern	int			NS_GetRand( int RandMax ) ;												// 乱数を取得する( RandMax : 返って来る値の最大値 )
extern	int			NS_SRand(	 int Seed ) ;													// 乱数の初期値を設定する

// バッテリー関連
extern	int			NS_GetBatteryLifePercent( void ) ;											// 電池の残量を % で取得する( 戻り値： 100=フル充電状態  0=充電残量無し )

// クリップボード関係
extern	int			NS_GetClipboardText(			TCHAR *DestBuffer ) ;									// クリップボードに格納されているテキストデータを読み出す、-1 の場合はクリップボードにテキストデータは無いということ( DestBuffer に NULL を渡すと格納に必要なデータサイズが返ってくる )
extern	int			NS_SetClipboardText(			const TCHAR *Text ) ;									// クリップボードにテキストデータを格納する
extern	int			NS_SetClipboardTextWithStrLen(	const TCHAR *Text, size_t TextLength ) ;										// クリップボードにテキストデータを格納する
















// DxLog.cpp関数プロトタイプ宣言

#ifndef DX_NON_LOG

// ログ出力設定関数
extern	int			NS_SetOutApplicationLogValidFlag(			int Flag ) ;																		// ログ出力を行うか否かのセット
extern	int			NS_SetApplicationLogFileName(      const TCHAR *FileName ) ;				// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern	int			NS_SetApplicationLogFileNameWithStrLen(      const TCHAR *FileName, size_t FileNameLength ) ;				// ログファイルの名前を設定する( Log.txt 以外にしたい場合に使用 )
extern	int			NS_SetApplicationLogSaveDirectory(	          const TCHAR *DirectoryPath ) ;								// ログファイル( Log.txt ) を保存するディレクトリパスを設定する
extern	int			NS_SetApplicationLogSaveDirectoryWithStrLen( const TCHAR *DirectoryPath, size_t DirectoryPathLength ) ;	// ログファイル( Log.txt ) を保存するディレクトリパスを設定する
extern	int			NS_SetUseDateNameLogFile(					int Flag ) ;																		// ログファイル名に日付をつけるかどうかをセットする

// ログファイル関数
extern	int			NS_LogFileAdd(			 const TCHAR *String ) ;							// ログファイル( Log.txt ) に文字列を出力する
extern	int			NS_LogFileAddWithStrLen(	const TCHAR *String, size_t StringLength ) ;	// ログファイル( Log.txt ) に文字列を出力する
extern 	int			NS_LogFileFmtAdd(		 const TCHAR *FormatString , ... ) ;				// 書式付きで ログファイル( Log.txt ) に文字列を出力する( 書式は printf と同じ )
extern	int			NS_LogFileTabAdd(		 void ) ;											// ログファイル( Log.txt ) に出力する文字列の前に付けるタブの数を一つ増やす
extern	int			NS_LogFileTabSub(		 void ) ;											// ログファイル( Log.txt ) に出力する文字列の前に付けるタブの数を一つ減らす
extern	int			NS_ErrorLogAdd(			const TCHAR *String ) ;							// LogFileAdd の旧名称関数
extern 	int			NS_ErrorLogFmtAdd(		 const TCHAR *FormatString , ... ) ;				// LogFileFmtAdd の旧名称関数
extern	int			NS_ErrorLogTabAdd(		 void ) ;											// LogFileTabAdd の旧名称関数
extern	int			NS_ErrorLogTabSub(		 void ) ;											// LogFileTabSub の旧名称関数
extern	int			NS_SetUseTimeStampFlag( int UseFlag ) ;									// タイムスタンプの有無を設定する
extern 	int			NS_AppLogAdd(			 const TCHAR *String , ... ) ;						// 書式付きログ文字列を書き出す

#ifndef DX_NON_PRINTF_DX

// ログ出力機能関数
extern	int			NS_SetLogDrawOutFlag(	 int DrawFlag ) ;									// ログ出力フラグをセットする
extern 	int			NS_GetLogDrawFlag(		 void ) ;											// ログ出力をするかフラグの取得
extern	int			NS_SetLogFontSize(		 int Size ) ;										// printfDx で画面に出力するログフォントのサイズを変更する
extern	int			NS_SetLogFontHandle(	 int FontHandle ) ;									// printfDx の結果を画面に出力する際に使用するフォントのハンドルを変更する
extern	int			NS_SetLogDrawArea(		 int x1, int y1, int x2, int y2 ) ;					// printfDx の結果を画面に出力する際の描画する領域を設定する

// 簡易画面出力関数
extern 	int			NS_printfDx(			 const TCHAR *FormatString , ... ) ;				// 簡易画面出力
extern	int			NS_putsDx(				 const TCHAR *String, int NewLine = TRUE ) ;		// puts と同じ引数で画面に文字列を表示するための関数
extern	int			NS_putsDxWithStrLen(	 const TCHAR *String, size_t StringLength, int NewLine = TRUE ) ;	// puts と同じ引数で画面に文字列を表示するための関数
extern	int			NS_clsDx(				 void ) ;											// 簡易画面出力をクリアする
#endif

#endif // DX_NON_LOG















#ifndef DX_NON_ASYNCLOAD

// DxASyncLoad.cpp関数プロトタイプ宣言

// 非同期読み込み関係
extern	int			NS_SetUseASyncLoadFlag(		int Flag ) ;								// 読み込み処理系の関数で非同期読み込みを行うかどうかを設定する( 非同期読み込みに対応している関数のみ有効 )( TRUE:非同期読み込みを行う  FALSE:非同期読み込みを行わない( デフォルト ) )
extern	int			NS_GetUseASyncLoadFlag(			void ) ;									// 読み込み処理系の関数で非同期読み込みを行うかどうかを取得する( 非同期読み込みに対応している関数のみ有効 )( TRUE:非同期読み込みを行う  FALSE:非同期読み込みを行わない( デフォルト ) )
extern	int			NS_CheckHandleASyncLoad(		int Handle ) ;								// ハンドルの非同期読み込みが完了しているかどうかを取得する( TRUE:まだ完了していない  FALSE:完了している  -1:エラー )
extern	int			NS_GetHandleASyncLoadResult(	int Handle ) ;								// ハンドルの非同期読み込み処理の戻り値を取得する( 非同期読み込み中の場合は一つ前の非同期読み込み処理の戻り値が返ってきます )
extern	int			NS_SetASyncLoadFinishDeleteFlag(	int Handle ) ;								// ハンドルの非同期読み込み処理が完了したらハンドルを削除するフラグを立てる
extern	int			NS_GetASyncLoadNum(			void ) ;									// 非同期読み込み中の処理の数を取得する
extern	int			NS_SetASyncLoadThreadNum(		int ThreadNum ) ;							// 非同期読み込み処理を行うスレッドの数を設定する

#endif // DX_NON_ASYNCLOAD













// DxHandle.cpp関数プロトタイプ宣言

extern	int			NS_SetDeleteHandleFlag(		int Handle, int *DeleteFlag ) ;					// ハンドルが削除されたときに−１が設定される変数を登録する














// DxWindow.cpp関数プロトタイプ宣言

#ifdef __WINDOWS__

// 便利関数
extern	int			NS_GetResourceInfo(		const TCHAR *ResourceName , const TCHAR *ResourceType , void **DataPointerP , int *DataSizeP ) ;		// 指定のリソースを取得する( -1:失敗  0:成功 )
extern	int			NS_GetResourceInfoWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, void **DataPointerP , int *DataSizeP ) ;		// 指定の名前、タイプのリソースのアドレスとサイズを取得する( 戻り値  -1:失敗  0:成功 )
extern	const TCHAR* NS_GetResourceIDString(	int ResourceID ) ;																						// リソースＩＤからリソースＩＤ文字列を得る 

// ウインドウ関係情報取得関数
extern	int			NS_GetWindowCRect(					RECT *RectBuf ) ;										// ウインドウのクライアント領域を取得する
extern	int			NS_GetWindowClientRect(			RECT *RectBuf ) ;										// メインウインドウのクライアント領域を取得する
extern	int			NS_GetWindowFrameRect(				RECT *RectBuf ) ;										// メインウインドウの枠の部分も含めた全体の領域を取得する
extern	int			NS_GetWindowActiveFlag(			void ) ;												// ウインドウのアクティブフラグを取得
extern	int			NS_GetWindowMinSizeFlag(			void ) ;												// メインウインドウが最小化されているかどうかを取得する( 戻り値  TRUE:最小化されている  FALSE:最小化されていない )
extern	int			NS_GetWindowMaxSizeFlag(			void ) ;												// メインウインドウが最大化されているかどうかを取得する( 戻り値  TRUE:最大化されている  FALSE:最大化されていない )
extern	HWND		NS_GetMainWindowHandle(			void ) ;												// メインウインドウのハンドルを取得する
extern	int			NS_GetWindowModeFlag(				void ) ;												// ウインドウモードで起動しているか、のフラグを取得する
extern	int			NS_GetDefaultState(				int *SizeX , int *SizeY , int *ColorBitDepth, int *RefreshRate = NULL , int *LeftTopX = NULL , int *LeftTopY = NULL, int *PixelSizeX = NULL , int *PixelSizeY = NULL ) ;		// 起動時のデスクトップの画面モードを取得する
extern	int			NS_GetActiveFlag(					void ) ;												// ソフトがアクティブかどうかを取得する
extern	int			NS_GetNoActiveState(				int ResetFlag = TRUE ) ;								// 非アクティブになり、処理が一時停止していたかどうかを取得する(引数 ResetFlag=TRUE:状態をリセット FALSE:状態をリセットしない    戻り値: 0=一時停止はしていない  1=一時停止していた )
extern	int			NS_GetMouseDispFlag(				void ) ;												// マウスを表示するかどうかのフラグを取得する
extern	int			NS_GetAlwaysRunFlag(				void ) ;												// ウインドウがアクティブではない状態でも処理を続行するか、フラグを取得する
extern	int			NS__GetSystemInfo(					int *DxLibVer , int *DirectXVer , int *WindowsVer ) ;	// ＤＸライブラリと DirectX のバージョンと Windows のバージョンを得る
extern	int			NS_GetPcInfo(						TCHAR *OSString , TCHAR *DirectXString , TCHAR *CPUString , int *CPUSpeed /* 単位MHz */ , double *FreeMemorySize /* 単位MByte */ , double *TotalMemorySize , TCHAR *VideoDriverFileName , TCHAR *VideoDriverString , double *FreeVideoMemorySize /* 単位MByte */ , double *TotalVideoMemorySize ) ;	// ＰＣの情報を得る
extern	int			NS_GetUseMMXFlag(					void ) ;												// ＭＭＸが使えるかどうかの情報を得る
extern	int			NS_GetUseSSEFlag(					void ) ;												// ＳＳＥが使えるかどうかの情報を得る
extern	int			NS_GetUseSSE2Flag(					void ) ;												// ＳＳＥ２が使えるかどうかの情報を得る
extern	int			NS_GetWindowCloseFlag(				void ) ;												// ウインドウを閉じようとしているかの情報を得る
extern	HINSTANCE	NS_GetTaskInstance(				void ) ;												// ソフトのインスタンスを取得する
extern	int			NS_GetUseWindowRgnFlag(			void ) ;												// リージョンを使っているかどうかを取得する
extern	int			NS_GetWindowSizeChangeEnableFlag(	int *FitScreen = NULL ) ;								// ウインドウのサイズを変更できるかどうかのフラグを取得する
extern	double		NS_GetWindowSizeExtendRate(		double *ExRateX = NULL , double *ExRateY = NULL ) ;		// 描画画面のサイズに対するウインドウサイズの比率を取得する( 戻り値は ExRateX に代入される値と同じです )
extern	int			NS_GetWindowSize(					int *Width, int *Height ) ;								// ウインドウモードのウインドウのクライアント領域のサイズを取得する
extern	int			NS_GetWindowEdgeWidth(				int *LeftWidth, int *RightWidth, int *TopWidth, int *BottomWidth ) ;	// ウインドウの上下左右の縁の幅を取得する
extern	int			NS_GetWindowPosition(				int *x, int *y ) ;										// ウインドウモードのウインドウの位置を取得する( 枠も含めた左上座標 )
extern	int			NS_GetWindowUserCloseFlag(			int StateResetFlag = FALSE ) ;							// ウインドウの閉じるボタンが押されたかどうかを取得する
extern	int			NS_GetNotDrawFlag(					void ) ;												// 描画機能を使うかどうかのフラグを取得する
extern	int			NS_GetPaintMessageFlag(			void ) ;												// WM_PAINT メッセージが来たかどうかを取得する(戻り値  TRUE:WM_PAINTメッセージが来た(一度取得すると以後、再び WM_PAINTメッセージが来るまで FALSE が返ってくるようになる)  FALSE:WM_PAINT メッセージは来ていない)
extern	int			NS_GetValidHiPerformanceCounter(	void ) ;												// パフォーマンスカウンタが有効かどうかを取得する(戻り値  TRUE:有効  FALSE:無効)
extern	TCHAR		NS_GetInputSystemChar(				int DeleteFlag ) ;										// 入力されたシステム文字を取得する


// 設定系関数
extern	int			NS_ChangeWindowMode(						int Flag ) ;																		// ウインドウモードを変更する
extern	int			NS_SetUseCharSet(							int CharSet /* = DX_CHARSET_SHFTJIS 等 */ ) ;										// ＤＸライブラリの文字列処理で前提とする文字列セットを設定する
extern	int			NS_LoadPauseGraph(							const TCHAR *FileName ) ;															// アクティブウインドウが他のソフトに移っている際に表示する画像のロード(NULL で解除)
extern	int			NS_LoadPauseGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;									// アクティブウインドウが他のソフトに移っている際に表示する画像をファイルから読み込む( FileName に NULL を渡すことで解除)
extern	int			NS_LoadPauseGraphFromMem(					const void *MemImage , int MemImageSize ) ;												// アクティブウインドウが他のソフトに移っている際に表示する画像のロード(NULL で解除)
extern	int			NS_SetActiveStateChangeCallBackFunction(	int (* CallBackFunction )( int ActiveState , void *UserData ) , void *UserData ) ;	// ウインドウのアクティブ状態に変化があったときに呼ばれるコールバック関数をセットする( NULL をセットすると呼ばれなくなる )
extern	int			NS_SetWindowText(							const TCHAR *WindowText ) ;															// メインウインドウのウインドウテキストを変更する
extern	int			NS_SetWindowTextWithStrLen(				const TCHAR *WindowText, size_t WindowTextLength ) ;								// メインウインドウのウインドウテキストを変更する
extern	int			NS_SetMainWindowText(						const TCHAR *WindowText                          ) ;								// SetWindowText の別名関数
extern	int			NS_SetMainWindowTextWithStrLen(			const TCHAR *WindowText, size_t WindowTextLength ) ;								// SetWindowText の別名関数
extern	int			NS_SetMainWindowClassName(					const TCHAR *ClassName                         ) ;									// メインウインドウのクラス名を設定する( DxLib_Init の前でのみ使用可能 )
extern	int			NS_SetMainWindowClassNameWithStrLen(		const TCHAR *ClassName, size_t ClassNameLength ) ;									// メインウインドウのクラス名を設定する( DxLib_Init の前でのみ使用可能 )
extern	int			NS_SetWindowIconID(						int ID ) ;																			// 使用するアイコンのＩＤをセットする
extern	int			NS_SetWindowIconHandle(					HICON Icon ) ;																		// 使用するアイコンのハンドルをセットする
extern	int			NS_SetUseASyncChangeWindowModeFunction(	int Flag , void (* CallBackFunction )( void * ) , void *Data ) ;					// 最大化ボタンやALT+ENTERキーによる非同期なウインドウモードの変更の機能の設定を行う
extern	int			NS_SetShutdownCallbackFunction(			void (* CallbackFunction )( void * ), void *Data, const TCHAR *Message ) ;			// シャットダウンによるソフトの強制終了の際に呼ばれるコールバック関数を登録する
extern	int			NS_SetWindowStyleMode(						int Mode ) ;																		// ウインドウのスタイルを変更する
extern	int			NS_SetWindowZOrder(						int ZType /* = DX_WIN_ZTYPE_TOP 等 */ , int WindowActivateFlag ) ;					// メインウインドウの奥行き位置を変更する
extern	int			NS_SetWindowSizeChangeEnableFlag(			int Flag, int FitScreen = TRUE ) ;													// ウインドウのサイズを変更できるかどうかのフラグをセットする( NotFitScreen:ウインドウのクライアント領域に画面をフィットさせる(拡大させる)かどうか  TRUE:フィットさせる  FALSE:フィットさせない )
extern	int			NS_SetWindowSizeExtendRate(				double ExRateX, double ExRateY = -1.0 ) ;											// 描画画面のサイズに対するウインドウサイズの比率を設定する( ExRateY がマイナスの場合は ExRateX の値が ExRateY にも使用されます )
extern	int			NS_SetWindowSize(							int Width, int Height ) ;															// ウインドウモード時のウインドウのクライアント領域のサイズを設定する
extern	int			NS_SetWindowMaxSize(						int MaxWidth, int MaxHeight ) ;														// メインウインドウのクライアント領域の最大サイズを設定する( SetWindowSizeChangeEnableFlag の第一引数を TRUE で呼び出して、ウインドウのサイズが変更できる状態でのみ使用されるパラメータです )
extern	int			NS_SetWindowMinSize(						int MinWidth, int MinHeight ) ;														// メインウインドウのクライアント領域の最小サイズを設定する( SetWindowSizeChangeEnableFlag の第一引数を TRUE で呼び出して、ウインドウのサイズが変更できる状態でのみ使用されるパラメータです )
extern	int			NS_SetWindowPosition(						int x, int y ) ;																	// ウインドウモードのウインドウの位置を設定する( 枠も含めた左上座標 )
extern	int			NS_SetSysCommandOffFlag(					int Flag , const TCHAR *HookDllPath = NULL ) ;										// タスクスイッチを有効にするかどうかを設定する
extern	int			NS_SetSysCommandOffFlagWithStrLen(			int Flag , const TCHAR *HookDllPath = NULL , size_t HookDllPathLength = 0 ) ;		// タスクスイッチを有効にするかどうかを設定する
extern	int			NS_SetHookWinProc(							WNDPROC WinProc ) ;																	// メッセージをフックするウインドウプロージャを登録する
extern	int			NS_SetUseHookWinProcReturnValue(			int UseFlag ) ;																		// SetHookWinProc で設定したウインドウプロージャの戻り値を使用するかどうかを設定する、SetHookWinProc で設定したウインドウプロージャの中でのみ使用可能( UseFlag TRUE:戻り値を使用して、ＤＸライブラリのウインドウプロージャの処理は行わない  FALSE:戻り値は使用せず、ウインドウプロージャから出た後、ＤＸライブラリのウインドウプロージャの処理を行う )
extern	int			NS_SetDoubleStartValidFlag(				int Flag ) ;																		// ２重起動を許すかどうかのフラグをセットする
extern	int			NS_CheckDoubleStart(						void ) ;																			// ＤＸライブラリを使用したソフトが既に起動しているかどうかを取得する( TRUE:既に起動している  FALSE:起動していない )
extern	int			NS_AddMessageTakeOverWindow(				HWND Window ) ;																		// メッセージ処理をＤＸライブラリに肩代わりしてもらうウインドウを追加する
extern	int			NS_SubMessageTakeOverWindow(				HWND Window ) ;																		// メッセージ処理をＤＸライブラリに肩代わりしてもらうウインドウを減らす

extern	int			NS_SetWindowInitPosition(					int x , int y ) ;																	// ウインドウの初期位置を設定する
extern	int			NS_SetNotWinFlag(							int Flag ) ;																		// ＤＸライブラリのウインドウ関連の機能を使用しないフラグ
extern	int			NS_SetNotDrawFlag(							int Flag ) ;																		// 描画機能を使うかどうかのフラグをセットする
extern	int			NS_SetNotSoundFlag(						int Flag ) ;																		// サウンド機能を使うかどうかのフラグをセットする
extern	int			NS_SetNotInputFlag(						int Flag ) ;																		// 入力状態の取得機能を使うかどうかのフラグをセットする
extern	int			NS_SetDialogBoxHandle(						HWND WindowHandle ) ;																// ＤＸライブラリでメッセージ処理を行うダイアログボックスを登録する
extern	int			NS_SetWindowVisibleFlag(					int Flag ) ;																		// メインウインドウを表示するかどうかのフラグをセットする
extern	int			NS_SetWindowMinimizeFlag(					int Flag ) ;																		// メインウインドウを最小化するかどうかのフラグをセットする
extern	int			NS_SetWindowUserCloseEnableFlag(			int Flag ) ;																		// メインウインドウの×ボタンを押した時にライブラリが自動的にウインドウを閉じるかどうかのフラグをセットする
extern	int			NS_SetDxLibEndPostQuitMessageFlag(			int Flag ) ;																		// ＤＸライブラリ終了時に PostQuitMessage を呼ぶかどうかのフラグをセットする
extern	int			NS_SetUserWindow(							HWND WindowHandle ) ;																// ＤＸライブラリで使用するウインドウのハンドルをセットする(DxLib_Init を実行する以前でのみ有効)
extern	int			NS_SetUserChildWindow(						HWND WindowHandle ) ;																// ＤＸライブラリで使用する表示用の子ウインドウのハンドルをセットする(DxLib_Init を実行する以前でのみ有効)
extern	int			NS_SetUserWindowMessageProcessDXLibFlag(	int Flag ) ;																		// SetUseWindow で設定したウインドウのメッセージループ処理をＤＸライブラリで行うかどうか、フラグをセットする
extern	int			NS_SetUseFPUPreserveFlag(					int Flag ) ;																		// FPUの精度を落とさない設定を使用するかどうかを設定する、DxLib_Init を呼び出す前のみ有効( TRUE:使用する(精度が落ちない)  FALSE:使用しない(精度を落とす(デフォルト) )
extern	int			NS_SetValidMousePointerWindowOutClientAreaMoveFlag( int Flag ) ;																// マウスポインタがウインドウのクライアントエリアの外にいけるかどうかを設定する( TRUE:いける( デフォルト設定 )  FALSE:いけない )
extern	int			NS_SetUseBackBufferTransColorFlag(			int Flag ) ;																		// バックバッファの透過色の部分を透過させるかどうかを設定する( TRUE:透過させる  FALSE:透過させない )
extern	int			NS_SetUseUpdateLayerdWindowFlag(			int Flag ) ;																		// UpdateLayerdWindowForBaseImage や UpdateLayerdWindowForSoftImage を使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない )
extern	int			NS_SetResourceModule(						HMODULE ResourceModule ) ;															// リソースを読み込む際に使用するモジュールを設定する( NULL を指定すると初期状態に戻ります、デフォルトでは NULL )
extern	int			NS_SetUseDxLibWM_PAINTProcess(				int Flag ) ;																		// WM_PAINT メッセージが来た際に『ＤＸライブラリの WM_PAINTメッセージが来た際の処理』を行うかどうかを設定する( 別スレッドで描画処理を行う場合などで使用 )

// ドラッグ＆ドロップされたファイル関係
extern	int			NS_SetDragFileValidFlag(		int Flag ) ;											// ファイルのドラッグ＆ドロップ機能を有効にするかどうかのフラグをセットする
extern	int			NS_DragFileInfoClear(			void ) ;												// ドラッグ＆ドロップされたファイルの情報を初期化する
extern	int			NS_GetDragFilePath(			TCHAR *FilePathBuffer ) ;								// ドラッグ＆ドロップされたファイル名を取得する( -1:取得できなかった  0:取得できた )
extern	int			NS_GetDragFileNum(				void ) ;												// ドラッグ＆ドロップされたファイルの数を取得する

// ウインドウ描画領域設定系関数
extern	HRGN		NS_CreateRgnFromGraph(			int Width , int Height , const void *MaskData , int Pitch , int Byte ) ;				// 任意のグラフィックからRGNハンドルを作成する
extern	HRGN		NS_CreateRgnFromBaseImage(		BASEIMAGE *BaseImage, int TransColorR, int TransColorG, int TransColorB ) ;		// 任意の基本イメージデータと透過色からRGNハンドルを作成する
extern	int			NS_SetWindowRgnGraph(			const TCHAR *FileName ) ;														// 任意のグラフィックからＲＧＮをセットする
extern	int			NS_SetWindowRgnGraphWithStrLen( const TCHAR *FileName, size_t FileNameLength ) ;								// 任意の画像ファイルからＲＧＮをセットする
extern	int			NS_UpdateTransColorWindowRgn(	void ) ;																		// 描画先の画面の透過色の部分を透過させるＲＧＮをセットする

// ツールバー関係
extern	int			NS_SetupToolBar(				const TCHAR *BitmapName , int DivNum , int ResourceID = -1 ) ;															// ツールバーの準備( BitmapName に NULL, ResourceID に -1 を指定するとツールバーを解除、ResourceID が -1 以外で BitmapName が NULL の時にのみ ResourceID が使用される )
extern	int			NS_SetupToolBarWithStrLen(		const TCHAR *BitmapName, size_t BitmapNameLength, int DivNum, int ResourceID = -1 ) ;									// ツールバーの準備、BitmapName に NULL, ResourceID に -1 を渡すとツールバーを解除( BitmapName:ツールバーのボタンに使用する画像ファイルパス、ResourceID に -1 以外を渡す場合は NULL にする  DivNum:ボタン画像中のボタンの数  ResourceID:ツールバーのボタンに使用するビットマップリソースのＩＤ、BitmapName に NULL を渡すとこの引数が使用される )
extern	int			NS_AddToolBarButton(			int Type /* TOOLBUTTON_TYPE_NORMAL 等 */ , int State /* TOOLBUTTON_STATE_ENABLE 等 */ , int ImageIndex, int ID ) ;		// ツールバーにボタンを追加
extern	int			NS_AddToolBarSep(				void ) ;																												// ツールバーに隙間を追加
extern	int			NS_GetToolBarButtonState(		int ID ) ;																												// ツールバーのボタンの状態を取得( TRUE:押されている or 押された  FALSE:押されていない )
extern	int			NS_SetToolBarButtonState(		int ID , int State ) ;																									// ツールバーのボタンの状態を設定
extern	int			NS_DeleteAllToolBarButton(		void ) ;																												// ツールバーのボタンを全て削除

// メニュー関係
extern	int			NS_SetUseMenuFlag(				int Flag ) ;																						// メニューを有効にするかどうかを設定する
extern	int			NS_SetUseKeyAccelFlag(			int Flag ) ;																						// キーボードアクセラレーターを使用するかどうかを設定する

extern	int			NS_AddKeyAccel(				const TCHAR *ItemName , int ItemID , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;	// ショートカットキーを追加する
extern	int			NS_AddKeyAccelWithStrLen(				const TCHAR *ItemName, size_t ItemNameLength, int ItemID , int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;	// ショートカットキーを追加する( ItemName:ショートカットキーを割り当てるメニューのアイテム名( AddMenuItem で NewItemName に渡した名前 )、ItemID を使用する場合は NULL を渡す  ItemID:メニュー項目の識別番号( AddMenuItem の引数 NewItemID で指定したもの )、ItemName を使用する場合は -1 を渡す  KeyCode:ショートカットキーのキー( KEY_INPUT_L 等 )  CtrlFlag:同時にCTRLキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )  AltFlag:同時にALTキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )  ShiftFlag:同時にSHIFTキーを押す必要があるようにするかどうか( TRUE:押す必要がある  FALSE:押さなくても良い )
extern	int			NS_AddKeyAccel_Name(					const TCHAR *ItemName,                        int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;				// ショートカットキーを追加する( 各引数の解説は AddKeyAccel と同じ、ItemID が無くなっただけ )
extern	int			NS_AddKeyAccel_NameWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength, int KeyCode , int CtrlFlag , int AltFlag , int ShiftFlag ) ;				// ショートカットキーを追加する( 各引数の解説は AddKeyAccel と同じ、ItemID が無くなっただけ )
extern	int			NS_AddKeyAccel_ID(				int ItemID, int KeyCode, int CtrlFlag, int AltFlag, int ShiftFlag ) ;								// ショートカットキーを追加する
extern	int			NS_ClearKeyAccel(				void ) ;																							// ショートカットキーの情報を初期化する

extern	int			NS_AddMenuItem(				int AddType /* MENUITEM_ADD_CHILD等 */ , const TCHAR *ItemName, int ItemID, int SeparatorFlag, const TCHAR *NewItemName = NULL , int NewItemID = -1 ) ;	// メニューに項目を追加する
extern	int			NS_AddMenuItemWithStrLen(				int AddType /* MENUITEM_ADD_CHILD等 */ , const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int SeparatorFlag, const TCHAR *NewItemName = NULL , size_t NewItemNameLength = 0 , int NewItemID = -1 ) ;	// メニューに項目を追加する( AddType:項目タイプ( MENUITEM_ADD_CHILD 等( 解説は #define の定義を参照してください ) )    ItemName:AddType が MENUITEM_ADD_CHILDの場合は親となる項目の名前、MENUITEM_ADD_INSERTの場合は挿入位置となる項目の名前、NULL を指定すると ItemID が使用される   ItemID:ItemName の代わりに識別番号で指定するもの、AddType毎の違いは ItemName の解説の通り、-1を指定すると ItemName が使用される　　SeparatorFlag:区切り線を追加するかどうか( TRUE:区切り線を追加、この場合 NewItemName と NewItemID は無視される  FALSE:追加するのは区切り線ではない )　　NewItemName:新しい項目の名前  NewItemID:新しい項目の識別番号、-1を指定すると内部で適当な番号が割り当てられる )
extern	int			NS_DeleteMenuItem(						const TCHAR *ItemName,                        int ItemID ) ;														// メニューから選択項目を削除する( ItemName:削除する項目の名前( AddMenuItem で NewItemName に渡した名前 )、NULL を指定すると ItemID が使用される  ItemID:削除する項目の識別番号( AddMenuItem で NewItemID に渡した番号 )、-1 を指定すると ItemName が使用される )
extern	int			NS_DeleteMenuItemWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength, int ItemID ) ;														// メニューから選択項目を削除する( ItemName:削除する項目の名前( AddMenuItem で NewItemName に渡した名前 )、NULL を指定すると ItemID が使用される  ItemID:削除する項目の識別番号( AddMenuItem で NewItemID に渡した番号 )、-1 を指定すると ItemName が使用される )
extern	int			NS_CheckMenuItemSelect(				const TCHAR *ItemName,                        int ItemID ) ;														// メニューが選択されたかどうかを取得する( 戻り値　 0:選択されていない  1:選択された   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			NS_CheckMenuItemSelectWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, int ItemID ) ;														// メニューが選択されたかどうかを取得する( 戻り値　 0:選択されていない  1:選択された   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			NS_SetMenuItemEnable(					const TCHAR *ItemName,                        int ItemID, int EnableFlag ) ;										// メニューの項目を選択出来るかどうかを設定する( EnableFlag:項目が選択できるかどうか( TRUE:選択できる   FALSE:選択できない )   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			NS_SetMenuItemEnableWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int EnableFlag ) ;										// メニューの項目を選択出来るかどうかを設定する( EnableFlag:項目が選択できるかどうか( TRUE:選択できる   FALSE:選択できない )   ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください )
extern	int			NS_SetMenuItemMark(					const TCHAR *ItemName,                        int ItemID, int Mark ) ;												// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等( 解説は #define の定義を参照してください )    ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください ) )
extern	int			NS_SetMenuItemMarkWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength, int ItemID, int Mark ) ;												// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等( 解説は #define の定義を参照してください )    ItemName と ItemID については関数 DeleteMenuItem の注釈を参照してください ) )
extern	int			NS_CheckMenuItemSelectAll(		void ) ;																							// メニューの項目がどれか選択されたかどうかを取得する( 戻り値  TRUE:どれか選択された  FALSE:選択されていない )

extern	int			NS_AddMenuItem_Name(			const TCHAR *ParentItemName, const TCHAR *NewItemName ) ;											// メニューに選択項目を追加する
extern	int			NS_AddMenuItem_NameWithStrLen(			const TCHAR *ParentItemName, size_t ParentItemNameLength, const TCHAR *NewItemName, size_t NewItemNameLength ) ;	// メニューに選択項目を追加する( ParentItemName:親となる項目の名前、親が持つリストの末端に新しい項目を追加します  NewItemName:新しい項目の名前 )
extern	int			NS_AddMenuLine_Name(					const TCHAR *ParentItemName ) ;																						// メニューのリストに区切り線を追加する( ParentItemName:区切り線を付ける項目リストの親の名前、リストの末端に区切り線を追加します )
extern	int			NS_AddMenuLine_NameWithStrLen(			const TCHAR *ParentItemName, size_t ParentItemNameLength ) ;														// メニューのリストに区切り線を追加する( ParentItemName:区切り線を付ける項目リストの親の名前、リストの末端に区切り線を追加します )
extern	int			NS_InsertMenuItem_Name(				const TCHAR *ItemName,                        const TCHAR *NewItemName                           ) ;				// 指定の項目と、指定の項目の一つ上の項目との間に新しい項目を追加する
extern	int			NS_InsertMenuItem_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, const TCHAR *NewItemName, size_t NewItemNameLength ) ;				// 指定の項目と、指定の項目の一つ上の項目との間に新しい項目を追加する
extern	int			NS_InsertMenuLine_Name(				const TCHAR *ItemName                        ) ;																	// 指定の項目と、指定の項目の一つ上の項目との間に区切り線を追加する
extern	int			NS_InsertMenuLine_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength ) ;																	// 指定の項目と、指定の項目の一つ上の項目との間に区切り線を追加する
extern	int			NS_DeleteMenuItem_Name(				const TCHAR *ItemName                        ) ;																	// メニューから選択項目を削除する
extern	int			NS_DeleteMenuItem_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength ) ;																	// メニューから選択項目を削除する
extern	int			NS_CheckMenuItemSelect_Name(			const TCHAR *ItemName                        ) ;																	// メニューが選択されたかどうかを取得する( 戻り値　0:選択されていない  1:選択された )
extern	int			NS_CheckMenuItemSelect_NameWithStrLen(	const TCHAR *ItemName, size_t ItemNameLength ) ;																	// メニューが選択されたかどうかを取得する( 戻り値　0:選択されていない  1:選択された )
extern	int			NS_SetMenuItemEnable_Name(				const TCHAR *ItemName,                        int EnableFlag ) ;													// メニューの項目を選択出来るかどうかを設定する( EnableFlag　1:選択できる  0:選択できない )
extern	int			NS_SetMenuItemEnable_NameWithStrLen(	const TCHAR *ItemName, size_t ItemNameLength, int EnableFlag ) ;													// メニューの項目を選択出来るかどうかを設定する( EnableFlag　1:選択できる  0:選択できない )
extern	int			NS_SetMenuItemMark_Name(				const TCHAR *ItemName,                        int Mark ) ;															// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等 ) )
extern	int			NS_SetMenuItemMark_NameWithStrLen(		const TCHAR *ItemName, size_t ItemNameLength, int Mark ) ;															// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( Mark:設定するマーク( MENUITEM_MARK_NONE 等 ) )

extern	int			NS_AddMenuItem_ID(				int ParentItemID, const TCHAR *NewItemName, int NewItemID = -1 ) ;									// メニューに選択項目を追加する
extern	int			NS_AddMenuItem_IDWithStrLen(			int ParentItemID, const TCHAR *NewItemName, size_t NewItemNameLength, int NewItemID = -1 ) ;						// メニューに選択項目を追加する
extern	int			NS_AddMenuLine_ID(				int ParentItemID ) ;																				// メニューのリストに区切り線を追加する
extern	int			NS_InsertMenuItem_ID(			int ItemID, int NewItemID ) ;																		// 指定の項目と、指定の項目の一つ上の項目との間に新しい項目を追加する
extern	int			NS_InsertMenuLine_ID(			int ItemID, int NewItemID ) ;																		// 指定の項目と、指定の項目の一つ上の項目との間に区切り線を追加する
extern	int			NS_DeleteMenuItem_ID(			int ItemID ) ;																						// メニューから選択項目を削除する
extern	int			NS_CheckMenuItemSelect_ID(		int ItemID ) ;																						// メニューが選択されたかどうかを取得する( 0:選択されていない  1:選択された )
extern	int			NS_SetMenuItemEnable_ID(		int ItemID, int EnableFlag ) ;																		// メニューの項目を選択出来るかどうかを設定する
extern	int			NS_SetMenuItemMark_ID(			int ItemID, int Mark ) ;																			// メニューの項目にチェックマークやラジオボタンを表示するかどうかを設定する( MENUITEM_MARK_NONE 等 )

extern	int			NS_DeleteMenuItemAll(			void ) ;																							// メニューの全ての選択項目を削除する
extern	int			NS_ClearMenuItemSelect(		void ) ;																							// メニューが選択されたかどうかの情報を初期化
extern	int			NS_GetMenuItemID(				const TCHAR *ItemName ) ;																			// メニューの項目名からＩＤを取得する
extern	int			NS_GetMenuItemIDWithStrLen(			const TCHAR *ItemName, size_t ItemNameLength ) ;																	// メニューの項目名から項目識別番号を取得する
extern	int			NS_GetMenuItemName(			int ItemID, TCHAR *NameBuffer ) ;																	// メニューの項目名からＩＤを取得する
extern	int			NS_LoadMenuResource(			int MenuResourceID ) ;																				// メニューをリソースから読み込む
extern	int			NS_SetMenuItemSelectCallBackFunction(		void (* CallBackFunction )( const TCHAR *ItemName, int ItemID ) ) ;						// メニューの選択項目が選択されたときに呼ばれるコールバック関数を設定する

extern	int			NS_SetWindowMenu(				int MenuID, int (* MenuProc )( WORD ID ) ) ;														// (古い関数)ウインドウにメニューを設定する
extern	int			NS_SetDisplayMenuFlag(			int Flag ) ;																						// メニューを表示するかどうかをセットする
extern	int			NS_GetDisplayMenuFlag(			void ) ;																							// メニューを表示しているかどうかを取得する
extern	int			NS_GetUseMenuFlag(				void ) ;																							// メニューを使用しているかどうかを得る
extern	int			NS_SetAutoMenuDisplayFlag(		int Flag ) ;																						// フルスクリーン時にメニューを自動で表示したり非表示にしたりするかどうかのフラグをセットする

#endif // __WINDOWS__

// マウス関係関数
extern	int			NS_SetMouseDispFlag(		int DispFlag ) ;												// マウスの表示フラグのセット
extern	int			NS_GetMousePoint(			int *XBuf, int *YBuf ) ;										// マウスの位置を取得する
extern	int			NS_SetMousePoint(			int PointX, int PointY ) ;										// マウスの位置をセットする
#ifndef DX_NON_INPUT
extern	int			NS_GetMouseInput(			void ) ;														// マウスのボタンの状態を得る 
extern	int			NS_GetMouseWheelRotVol(	int CounterReset = TRUE ) ;										// 垂直マウスホイールの回転量を得る
extern	int			NS_GetMouseHWheelRotVol(	int CounterReset = TRUE ) ;										// 水平マウスホイールの回転量を得る
extern	float		NS_GetMouseWheelRotVolF(	int CounterReset = TRUE ) ;										// 垂直マウスホイールの回転量を取得する( 戻り値が float 型 )
extern	float		NS_GetMouseHWheelRotVolF(	int CounterReset = TRUE ) ;										// 水平マウスホイールの回転量を取得する( 戻り値が float 型 )
extern	int			NS_GetMouseInputLog(		int *Button, int *ClickX, int *ClickY, int LogDelete = TRUE ) ;					// マウスのボタンを押した情報を一つ取得する( Button:押されたボタン( MOUSE_INPUT_LEFT 等 )を格納する変数のアドレス  ClickX:押された時のＸ座標を格納する変数のアドレス  ClickY:押された時のＹ座標を格納する変数のアドレス   LogDelete:取得した押下情報一つ分をログから削除するかどうか( TRUE:削除する  FALSE:削除しない、つまり次にこの関数が呼ばれたときに同じ値を取得することになる )　　戻り値  0:押された情報取得できた　-1:押された情報が無かった、つまり前回の呼び出し( または起動時から最初の呼び出し )の間に一度もマウスのボタンが押されなかった )
extern	int			NS_GetMouseInputLog2(		int *Button, int *ClickX, int *ClickY, int *LogType, int LogDelete = TRUE ) ;	// マウスのボタンを押したり離したりした情報を一つ取得する( Button:押されたり離されたりしたボタン( MOUSE_INPUT_LEFT 等 )を格納する変数のアドレス  ClickX:押されたり離されたりした時のＸ座標を格納する変数のアドレス  ClickY:押されたり離されたりした時のＹ座標を格納する変数のアドレス   LogDelete:取得した押されたり離されたりした情報一つ分をログから削除するかどうか( TRUE:削除する  FALSE:削除しない、つまり次にこの関数が呼ばれたときに同じ値を取得することになる )　　戻り値  0:押されたり離されたりした情報取得できた　-1:押されたり離されたりした情報が無かった、つまり前回の呼び出し( または起動時から最初の呼び出し )の間に一度もマウスのボタンが押されたり離されたりしなかった )
#endif // DX_NON_INPUT













// タッチパネル入力関係関数
extern	int				NS_GetTouchInputNum( void ) ;															// タッチされている数を取得する
extern	int				NS_GetTouchInput( int InputNo, int *PositionX, int *PositionY, int *ID = NULL , int *Device = NULL ) ;		// タッチの情報を取得する

extern	int				NS_GetTouchInputLogNum( void ) ;														// ストックされているタッチ情報の数を取得する
extern	int				NS_ClearTouchInputLog( void ) ;															// ストックされているタッチ情報をクリアする
extern	TOUCHINPUTDATA	NS_GetTouchInputLogOne( int PeekFlag = FALSE ) ;										// ストックされているタッチ情報から一番古い情報をひとつ取得する
extern	int				NS_GetTouchInputLog( TOUCHINPUTDATA *TouchData, int GetNum, int PeekFlag = FALSE ) ;	// ストックされているタッチ情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )

extern	int				NS_GetTouchInputDownLogNum( void ) ;														// ストックされているタッチされ始めた情報の数を取得する
extern	int				NS_ClearTouchInputDownLog( void ) ;														// ストックされているタッチされ始めた情報をクリアする
extern	TOUCHINPUTPOINT	NS_GetTouchInputDownLogOne( int PeekFlag = FALSE ) ;										// ストックされているタッチされ始めた情報から一番古い情報をひとつ取得する
extern	int				NS_GetTouchInputDownLog( TOUCHINPUTPOINT *PointData, int GetNum, int PeekFlag = FALSE ) ;	// ストックされているタッチされ始めた情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )

extern	int				NS_GetTouchInputUpLogNum( void ) ;															// ストックされているタッチが離された情報の数を取得する
extern	int				NS_ClearTouchInputUpLog( void ) ;															// ストックされているタッチが離された情報をクリアする
extern	TOUCHINPUTPOINT	NS_GetTouchInputUpLogOne( int PeekFlag = FALSE ) ;											// ストックされているタッチが離された情報から一番古い情報をひとつ取得する
extern	int				NS_GetTouchInputUpLog( TOUCHINPUTPOINT *PointData, int GetNum, int PeekFlag = FALSE ) ;	// ストックされているタッチが離された情報から古い順に指定数バッファに取得する( 戻り値  -1:エラー  0以上:取得した情報の数 )


















// DxMemory.cpp関数プロトタイプ宣言

// メモリ確保系関数
extern	void*		NS_DxAlloc(						size_t AllocSize , const char *File = NULL , int Line = -1 ) ;					// メモリを確保する
extern	void*		NS_DxCalloc(						size_t AllocSize , const char *File = NULL , int Line = -1 ) ;					// メモリを確保して０で初期化する
extern	void*		NS_DxRealloc(						void *Memory , size_t AllocSize , const char *File = NULL , int Line = -1 ) ;	// メモリの再確保を行う
extern	void		NS_DxFree(							void *Memory ) ;																// メモリを解放する
extern	size_t		NS_DxSetAllocSizeTrap(				size_t Size ) ;																	// 列挙対象にするメモリの確保容量をセットする
extern	int			NS_DxSetAllocPrintFlag(			int Flag ) ;																	// ＤＸライブラリ内でメモリ確保が行われる時に情報を出力するかどうかをセットする
extern	size_t		NS_DxGetAllocSize(					void ) ;																		// DxAlloc や DxCalloc で確保しているメモリサイズを取得する
extern	int			NS_DxGetAllocNum(					void ) ;																		// DxAlloc や DxCalloc で確保しているメモリの数を取得する
extern	void		NS_DxDumpAlloc(					void ) ;																		// DxAlloc や DxCalloc で確保しているメモリを列挙する
extern	void		NS_DxDrawAlloc(					int x, int y, int Width, int Height ) ;											// DxAlloc や DxCalloc で確保しているメモリの状況を描画する
extern	int			NS_DxErrorCheckAlloc(				void ) ;																		// 確保したメモリ情報が破壊されていないか調べる( -1:破壊あり  0:なし )
extern	int			NS_DxSetAllocSizeOutFlag(			int Flag ) ;																	// メモリが確保、解放が行われる度に確保しているメモリの容量を出力するかどうかのフラグをセットする
extern	int			NS_DxSetAllocMemoryErrorCheckFlag(	int Flag ) ;																	// メモリの確保、解放が行われる度に確保しているメモリ確保情報が破損していないか調べるかどうかのフラグをセットする














// DxBaseFunc.cpp 関数プロトタイプ宣言

// 文字コード関係
extern	int			NS_GetCharBytes( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , const void *String ) ;																												// 文字列の先頭の文字のバイト数を取得する
extern	int			NS_ConvertStringCharCodeFormat( int SrcCharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */, const void *SrcString, int DestCharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */, void *DestStringBuffer ) ;		// 文字列の文字コード形式を別の文字コード形式に変換する
extern	int			NS_SetUseCharCodeFormat( int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;																														// 文字列の引数の文字コード形式を設定する( 文字列描画系関数とその他一部関数を除く )( UNICODE版では無効 )
extern	int			NS_Get_wchar_t_CharCodeFormat( void ) ;														// wchar_t型の文字コード形式を取得する( 戻り値： DX_CHARCODEFORMAT_UTF16LE など )

// 文字列関係
extern	void			NS_strcpyDx(      TCHAR *Dest,                   const TCHAR *Src ) ;						// strcpy と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strcpy_sDx(    TCHAR *Dest, size_t DestBytes, const TCHAR *Src ) ;						// strcpy_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpcpyDx(     TCHAR *Dest,                   const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy、Pos はコピー開始位置　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpcpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy_s、Pos はコピー開始位置　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpcpy2Dx(    TCHAR *Dest,                   const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy、Pos はコピー開始位置( 全角文字も 1 扱い )　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpcpy2_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos ) ;				// 位置指定付き strcpy_s、Pos はコピー開始位置( 全角文字も 1 扱い )　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strncpyDx(     TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strncpy_sDx(   TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strncpy2Dx(    TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy の Num が文字数( 全角文字も 1 扱い )になったもの、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strncpy2_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s の Num が文字数( 全角文字も 1 扱い )になったもの、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strrncpyDx(    TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy の文字列の終端からの文字数指定版( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strrncpy_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s の文字列の終端からの文字数指定版( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strrncpy2Dx(   TCHAR *Dest,                   const TCHAR *Src, int Num ) ;				// strncpy の文字列の終端からの文字数( 全角文字も 1 扱い )指定版、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strrncpy2_sDx( TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Num ) ;				// strncpy_s の文字列の終端からの文字数( 全角文字も 1 扱い )指定版、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpncpyDx(    TCHAR *Dest,                   const TCHAR *Src, int Pos, int Num ) ;	// strncpy のコピー開始位置指定版、Pos はコピー開始位置、Num は文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpncpy_sDx(  TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos, int Num ) ;	// strncpy_s のコピー開始位置指定版、Pos はコピー開始位置、Num は文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpncpy2Dx(   TCHAR *Dest,                   const TCHAR *Src, int Pos, int Num ) ;	// strncpy のコピー開始位置指定版、Pos はコピー開始位置( 全角文字も 1 扱い )、Num は文字数( 全角文字も 1 扱い )、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strpncpy2_sDx( TCHAR *Dest, size_t DestBytes, const TCHAR *Src, int Pos, int Num ) ;	// strncpy_s のコピー開始位置指定版、Pos はコピー開始位置( 全角文字も 1 扱い )、Num は文字数( 全角文字も 1 扱い )、終端に必ずヌル文字が代入される( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strcatDx(      TCHAR *Dest,                   const TCHAR *Src ) ;						// strcat と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	void			NS_strcat_sDx(    TCHAR *Dest, size_t DestBytes, const TCHAR *Src ) ;						// strcat_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	size_t			NS_strlenDx(      const TCHAR *Str ) ;												// strlen と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	size_t			NS_strlen2Dx(     const TCHAR *Str ) ;												// strlen の戻り値が文字数( 全角文字も 1 扱い )になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strcmpDx(      const TCHAR *Str1, const TCHAR *Str2 ) ;							// strcmp と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_stricmpDx(     const TCHAR *Str1, const TCHAR *Str2 ) ;							// stricmp と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strncmpDx(     const TCHAR *Str1, const TCHAR *Str2, int Num ) ;				// strncmp と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strncmp2Dx(    const TCHAR *Str1, const TCHAR *Str2, int Num ) ;				// strncmp の Num が文字数( 全角文字も 1 扱い )になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strpncmpDx(    const TCHAR *Str1, const TCHAR *Str2, int Pos, int Num ) ;		// strncmp の比較開始位置指定版、Pos が Str1 の比較開始位置、Num が文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strpncmp2Dx(   const TCHAR *Str1, const TCHAR *Str2, int Pos, int Num ) ;		// strncmp の比較開始位置指定版、Pos が Str1 の比較開始位置( 全角文字も 1 扱い )、Num が文字数( 全角文字も 1 扱い )( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	DWORD			NS_strgetchrDx(   const TCHAR *Str, int Pos, int *CharNums = NULL ) ;				// 文字列の指定の位置の文字コードを取得する、Pos は取得する位置、CharNums は文字数を代入する変数のアドレス、戻り値は文字コード( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	DWORD			NS_strgetchr2Dx(  const TCHAR *Str, int Pos, int *CharNums = NULL ) ;				// 文字列の指定の位置の文字コードを取得する、Pos は取得する位置( 全角文字も 1 扱い )、CharNums は文字数を代入する変数のアドレス、戻り値は文字コード( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strputchrDx(   TCHAR *Str, int Pos, DWORD CharCode ) ;							// 文字列の指定の位置に文字コードを書き込む、Pos は書き込む位置、CharCode は文字コード、戻り値は書き込んだ文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strputchr2Dx(  TCHAR *Str, int Pos, DWORD CharCode ) ;							// 文字列の指定の位置に文字コードを書き込む、Pos は書き込む位置( 全角文字も 1 扱い )、CharCode は文字コード、戻り値は書き込んだ文字数( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	NS_strposDx(      const TCHAR *Str, int Pos ) ;									// 文字列の指定の位置のアドレスを取得する、Pos は取得する位置　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	NS_strpos2Dx(     const TCHAR *Str, int Pos ) ;									// 文字列の指定の位置のアドレスを取得する、Pos は取得する位置( 全角文字も 1 扱い )　( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	NS_strstrDx(      const TCHAR *Str1, const TCHAR *Str2 ) ;							// strstr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strstr2Dx(     const TCHAR *Str1, const TCHAR *Str2 ) ;							// strstr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	NS_strrstrDx(     const TCHAR *Str1, const TCHAR *Str2 ) ;							// strrstr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strrstr2Dx(    const TCHAR *Str1, const TCHAR *Str2 ) ;							// strrstr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	NS_strchrDx(      const TCHAR *Str, DWORD CharCode ) ;								// strchr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strchr2Dx(     const TCHAR *Str, DWORD CharCode ) ;								// strchr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	const TCHAR *	NS_strrchrDx(     const TCHAR *Str, DWORD CharCode ) ;								// strrchr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_strrchr2Dx(    const TCHAR *Str, DWORD CharCode ) ;								// strrchr の戻り値が文字列先頭からの文字数( 全角文字も 1 扱い ) になったもの( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	TCHAR *			NS_struprDx(      TCHAR *Str ) ;													// strupr と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_vsprintfDx(    TCHAR *Buffer,                    const TCHAR *FormatString, va_list Arg ) ;	// vsprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_vsnprintfDx(   TCHAR *Buffer, size_t BufferSize, const TCHAR *FormatString, va_list Arg ) ;	// vsnprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_sprintfDx(     TCHAR *Buffer,                    const TCHAR *FormatString, ... ) ;			// sprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_snprintfDx(    TCHAR *Buffer, size_t BufferSize, const TCHAR *FormatString, ... ) ;			// snprintf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	TCHAR *			NS_itoaDx(        int Value, TCHAR *Buffer,                     int Radix ) ;		// itoa と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	TCHAR *			NS_itoa_sDx(      int Value, TCHAR *Buffer, size_t BufferBytes, int Radix ) ;		// itoa_s と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_atoiDx(       const TCHAR *Str ) ;												// atoi と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	double			NS_atofDx(       const TCHAR *Str ) ;												// atof と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_vsscanfDx(    const TCHAR *String, const TCHAR *FormatString, va_list Arg ) ;	// vsscanf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )
extern	int				NS_sscanfDx(     const TCHAR *String, const TCHAR *FormatString, ... ) ;			// sscanf と同等の機能( マルチバイト文字列版では文字コード形式として SetUseCharCodeFormat で設定した形式が使用されます )













// DxInputString.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUTSTRING

// 文字コードバッファ操作関係
extern	int			NS_StockInputChar(		TCHAR CharCode ) ;								// バッファにコードをストックする
extern	int			NS_ClearInputCharBuf(	void ) ;										// 文字コードバッファをクリアする
extern	TCHAR		NS_GetInputChar(		int DeleteFlag ) ;								// 文字コードバッファに溜まったデータから文字コードを取得する
extern	TCHAR		NS_GetInputCharWait(	int DeleteFlag ) ;								// 文字コードバッファに溜まったデータから１バイト分取得する、バッファになにも文字コードがない場合はキーが押されるまで待つ

extern	int			NS_GetOneChar(			TCHAR *CharBuffer, int DeleteFlag ) ;			// 文字コードバッファに溜まったデータから１文字分取得する
extern	int			NS_GetOneCharWait(		TCHAR *CharBuffer, int DeleteFlag ) ;			// 文字コードバッファに溜まったデータから１文字分取得する、バッファに何も文字コードがない場合はキーが押されるまで待つ
extern	int			NS_GetCtrlCodeCmp(		TCHAR Char ) ;									// アスキーコントロールコードか調べる

#endif // DX_NON_INPUTSTRING

#ifndef DX_NON_KEYEX

extern	int			NS_DrawIMEInputString(				int x, int y, int SelectStringNum , int DrawCandidateList = TRUE ) ;	// 画面上に入力中の文字列を描画する
extern	int			NS_SetUseIMEFlag(					int UseFlag ) ;							// ＩＭＥを使用するかどうかをセットする
extern	int			NS_GetUseIMEFlag(					void ) ;								// ＩＭＥを使用するかどうかの設定を取得する
extern	int			NS_SetInputStringMaxLengthIMESync(	int Flag ) ;							// ＩＭＥで入力できる最大文字数を MakeKeyInput の設定に合わせるかどうかをセットする( TRUE:あわせる  FALSE:あわせない(デフォルト) )
extern	int			NS_SetIMEInputStringMaxLength(		int Length ) ;							// ＩＭＥで一度に入力できる最大文字数を設定する( 0:制限なし  1以上:指定の文字数で制限 )
extern	int			NS_SetUseTSFFlag( int UseFlag ) ;											// ＩＭＥの漢字変換候補表示の処理に TSF を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )

#endif

extern	int			NS_GetStringPoint(				const TCHAR *String, int Point ) ;			// 全角文字、半角文字入り乱れる中から指定の文字数での半角文字数を得る
extern	int			NS_GetStringPointWithStrLen(	const TCHAR *String, size_t StringLength, int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の文字数での半角文字数を得る
extern	int			NS_GetStringPoint2(			const TCHAR *String,                      int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の半角文字数での文字数を得る
extern	int			NS_GetStringPoint2WithStrLen(	const TCHAR *String, size_t StringLength, int Point ) ;		// 全角文字、半角文字入り乱れる中から指定の半角文字数での文字数を得る
extern	int			NS_GetStringLength(			const TCHAR *String ) ;						// 全角文字、半角文字入り乱れる中から文字数を取得する

#ifndef DX_NON_FONT
extern	int			NS_DrawObtainsString(			int x, int y, int AddY, const TCHAR *String, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画
extern	int			NS_DrawObtainsNString(			int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画
extern	int			NS_DrawObtainsString_CharClip(				int x, int y, int AddY, const TCHAR *String,                      unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern	int			NS_DrawObtainsNString_CharClip(				int x, int y, int AddY, const TCHAR *String, size_t StringLength, unsigned int StrColor, unsigned int StrEdgeColor = 0 , int FontHandle = -1 , unsigned int SelectBackColor = 0xffffffff , unsigned int SelectStrColor = 0 , unsigned int SelectStrEdgeColor = 0xffffffff , int SelectStart = -1 , int SelectEnd = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画( クリップが文字単位 )
extern	int			NS_GetObtainsStringCharPosition(			int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画した場合の文字列の末端の座標を取得する
extern	int			NS_GetObtainsStringCharPosition_CharClip(	int x, int y, int AddY, const TCHAR *String, int StrLen, int *PosX, int *PosY, int FontHandle = -1 ) ;		// 描画可能領域に収まるように改行しながら文字列を描画した場合の文字列の末端の座標を取得する( クリップが文字単位 )
#endif // DX_NON_FONT
extern	int			NS_DrawObtainsBox(				int x1, int y1, int x2, int y2, int AddY, unsigned int Color, int FillFlag ) ;																																										// 規定領域に収めたかたちで矩形を描画 

#ifndef DX_NON_KEYEX

extern	int			NS_InputStringToCustom(		int x, int y, size_t BufLength, TCHAR *StrBuffer, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag = FALSE , int EnableNewLineFlag = FALSE , int DisplayCandidateList = TRUE ) ;		// 文字列の入力取得

extern	int			NS_KeyInputString(				int x, int y, size_t CharMaxLength, TCHAR *StrBuffer, int CancelValidFlag ) ;														// 文字列の入力取得
extern	int			NS_KeyInputSingleCharString(	int x, int y, size_t CharMaxLength, TCHAR *StrBuffer, int CancelValidFlag ) ;														// 半角文字列のみの入力取得
extern	int			NS_KeyInputNumber(				int x, int y, int MaxNum, int MinNum, int CancelValidFlag ) ;																	// 数値の入力

extern	int			NS_GetIMEInputModeStr(			TCHAR *GetBuffer ) ;																											// IMEの入力モード文字列の取得
extern	const IMEINPUTDATA* NS_GetIMEInputData(			void ) ;																														// IMEで入力中の文字列の情報を取得する
extern	int			NS_SetKeyInputStringColor(		ULONGLONG NmlStr, ULONGLONG NmlCur, ULONGLONG IMEStrBack, ULONGLONG IMECur, ULONGLONG IMELine, ULONGLONG IMESelectStr, ULONGLONG IMEModeStr , ULONGLONG NmlStrE = 0 , ULONGLONG IMESelectStrE = 0 , ULONGLONG IMEModeStrE = 0 , ULONGLONG IMESelectWinE = ULL_PARAM( 0xffffffffffffffff ) ,	ULONGLONG IMESelectWinF = ULL_PARAM( 0xffffffffffffffff ) , ULONGLONG SelectStrBackColor = ULL_PARAM( 0xffffffffffffffff ) , ULONGLONG SelectStrColor = ULL_PARAM( 0xffffffffffffffff ) , ULONGLONG SelectStrEdgeColor = ULL_PARAM( 0xffffffffffffffff ), ULONGLONG IMEStr = ULL_PARAM( 0xffffffffffffffff ), ULONGLONG IMEStrE = ULL_PARAM( 0xffffffffffffffff ) ) ;	// InputString関数使用時の文字の各色を変更する
extern	int			NS_SetKeyInputStringColor2(	int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */, unsigned int Color ) ;															// InputString関数使用時の文字の各色を変更する
extern	int			NS_ResetKeyInputStringColor2(	int TargetColor /* DX_KEYINPSTRCOLOR_NORMAL_STR 等 */ ) ;																		// SetKeyInputStringColor2 で設定した色をデフォルトに戻す
extern	int			NS_SetKeyInputStringFont(		int FontHandle ) ;																												// キー入力文字列描画関連で使用するフォントのハンドルを変更する(-1でデフォルトのフォントハンドル)
extern	int			NS_SetKeyInputStringEndCharaMode( int EndCharaMode /* DX_KEYINPSTR_ENDCHARAMODE_OVERWRITE 等 */ ) ;															// キー入力文字列処理の入力文字数が限界に達している状態で、文字列の末端部分で入力が行われた場合の処理モードを変更する
extern	int			NS_DrawKeyInputModeString(		int x, int y ) ;																												// 入力モード文字列を描画する

extern	int			NS_InitKeyInput(				void ) ;																														// キー入力データ初期化
extern	int			NS_MakeKeyInput(				size_t MaxStrLength, int CancelValidFlag, int SingleCharOnlyFlag, int NumCharOnlyFlag, int DoubleCharOnlyFlag = FALSE , int EnableNewLineFlag = FALSE ) ;			// 新しいキー入力データの作成
extern	int			NS_DeleteKeyInput(				int InputHandle ) ;																												// キー入力データの削除
extern	int			NS_SetActiveKeyInput(			int InputHandle ) ;																												// 指定のキー入力をアクティブにする( -1 を指定するとアクティブなキー入力ハンドルが無い状態になります )
extern	int			NS_GetActiveKeyInput(			void ) ;																														// 現在アクティブになっているキー入力ハンドルを取得する
extern	int			NS_CheckKeyInput(				int InputHandle ) ;																												// 入力が終了しているか取得する
extern	int			NS_ReStartKeyInput(			int InputHandle ) ;																												// 入力が完了したキー入力を再度編集状態に戻す
extern	int			NS_ProcessActKeyInput(			void ) ;																														// キー入力処理関数
extern	int			NS_DrawKeyInputString(			int x, int y, int InputHandle , int DrawCandidateList = TRUE ) ;																								// キー入力中データの描画
extern	int			NS_SetKeyInputDrawArea(		int x1, int y1, int x2, int y2, int InputHandle ) ;																				// キー入力ハンドルの入力中文字列を描画する際の描画範囲を設定する

extern	int			NS_SetKeyInputSelectArea(		int  SelectStart, int  SelectEnd, int InputHandle ) ;																			// キー入力データの指定の領域を選択状態にする( SelectStart と SelectEnd に -1 を指定すると選択状態が解除されます )
extern	int			NS_GetKeyInputSelectArea(		int *SelectStart, int *SelectEnd, int InputHandle ) ;																			// キー入力データの選択領域を取得する
extern	int			NS_SetKeyInputDrawStartPos(	int DrawStartPos, int InputHandle ) ;																							// キー入力の描画開始文字位置を設定する
extern	int			NS_GetKeyInputDrawStartPos(	int InputHandle ) ;																												// キー入力の描画開始文字位置を取得する
extern	int			NS_SetKeyInputCursorBrinkTime(	int Time ) ;																													// キー入力時のカーソルの点滅する早さをセットする
extern	int			NS_SetKeyInputCursorBrinkFlag(	int Flag ) ;																													// キー入力時のカーソルを点滅させるかどうかをセットする
extern	int			NS_SetKeyInputString(			const TCHAR *String, int InputHandle ) ;																						// キー入力データに指定の文字列をセットする
extern	int			NS_SetKeyInputStringWithStrLen(	const TCHAR *String, size_t StringLength, int InputHandle ) ;																	// キー入力ハンドルに指定の文字列をセットする
extern	int			NS_SetKeyInputNumber(			int   Number,        int InputHandle ) ;																						// キー入力データに指定の数値を文字に置き換えてセットする
extern	int			NS_SetKeyInputNumberToFloat(	float Number,        int InputHandle ) ;																						// キー入力データに指定の浮動小数点値を文字に置き換えてセットする
extern	int			NS_GetKeyInputString(			TCHAR *StrBuffer,    int InputHandle ) ;																						// 入力データの文字列を取得する
extern	int			NS_GetKeyInputNumber(			int InputHandle ) ;																												// 入力データの文字列を整数値として取得する
extern	float		NS_GetKeyInputNumberToFloat(	int InputHandle ) ;																												// 入力データの文字列を浮動小数点値として取得する
extern	int			NS_SetKeyInputCursorPosition(	int Position,        int InputHandle ) ;																						// キー入力の現在のカーソル位置を設定する
extern	int			NS_GetKeyInputCursorPosition(	int InputHandle ) ;																												// キー入力の現在のカーソル位置を取得する

#endif // DX_NON_KEYEX


















// DxFile.cpp関数プロトタイプ宣言

// ファイルアクセス関数
extern	int			NS_FileRead_open(				const TCHAR *FilePath , int ASync = FALSE ) ;					// ファイルを開く
extern	int			NS_FileRead_open_WithStrLen(			const TCHAR *FilePath, size_t FilePathLength, int ASync = FALSE ) ;	// ファイルを開く
extern	int			NS_FileRead_open_mem(			const void *FileImage, size_t FileImageSize ) ;						// メモリに展開されたファイルを開く
extern	LONGLONG	NS_FileRead_size(				const TCHAR *FilePath ) ;										// ファイルのサイズを得る
extern	LONGLONG	NS_FileRead_size_WithStrLen(			const TCHAR *FilePath, size_t FilePathLength ) ;				// ファイルのサイズを取得する
extern	int			NS_FileRead_close(				int FileHandle ) ;												// ファイルを閉じる
extern	LONGLONG	NS_FileRead_tell(				int FileHandle ) ;												// ファイルポインタの現在位置を得る
extern	int			NS_FileRead_seek(				int FileHandle , LONGLONG Offset , int Origin ) ;				// ファイルポインタの位置を変更する
extern	int			NS_FileRead_read(				void *Buffer , int ReadSize , int FileHandle ) ;				// ファイルからデータを読み込む
extern	int			NS_FileRead_idle_chk(			int FileHandle ) ;												// ファイル読み込みが完了しているかどうかを取得する
extern	int			NS_FileRead_eof(				int FileHandle ) ;												// ファイルの終端かどうかを得る
extern	int			NS_FileRead_set_format(		int FileHandle, int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;	// ファイルの文字コード形式を設定する( テキストファイル用 )
extern	int			NS_FileRead_gets(				TCHAR *Buffer , int BufferSize , int FileHandle ) ;				// ファイルから文字列を読み出す
extern	TCHAR		NS_FileRead_getc(				int FileHandle ) ;												// ファイルから一文字読み出す
extern	int			NS_FileRead_scanf(				int FileHandle , const TCHAR *Format , ... ) ;					// ファイルから書式化されたデータを読み出す

extern	DWORD_PTR	NS_FileRead_createInfo(		const TCHAR *ObjectPath ) ;										// ファイル情報ハンドルを作成する( 戻り値  -1:エラー  -1以外:ファイル情報ハンドル )
extern	DWORD_PTR	NS_FileRead_createInfo_WithStrLen(		const TCHAR *ObjectPath, size_t ObjectPathLength ) ;			// ファイル情報ハンドルを作成する( 戻り値  -1:エラー  -1以外:ファイル情報ハンドル )
extern	int			NS_FileRead_getInfoNum(		DWORD_PTR FileInfoHandle ) ;									// ファイル情報ハンドル中のファイルの数を取得する
extern	int			NS_FileRead_getInfo(			int Index , FILEINFO *Buffer , DWORD_PTR FileInfoHandle ) ;		// ファイル情報ハンドル中のファイルの情報を取得する
extern	int			NS_FileRead_deleteInfo(		DWORD_PTR FileInfoHandle ) ;									// ファイル情報ハンドルを削除する

extern	DWORD_PTR	NS_FileRead_findFirst(			const TCHAR *FilePath, FILEINFO *Buffer ) ;						// 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern	DWORD_PTR	NS_FileRead_findFirst_WithStrLen(		const TCHAR *FilePath, size_t FilePathLength, FILEINFO *Buffer ) ; // 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern	int			NS_FileRead_findNext(			DWORD_PTR FindHandle, FILEINFO *Buffer ) ;						// 条件の合致する次のファイルの情報を取得する( 戻り値: -1=エラー  0=成功 )
extern	int			NS_FileRead_findClose(			DWORD_PTR FindHandle ) ;										// ファイル検索ハンドルを閉じる( 戻り値: -1=エラー  0=成功 )

extern	int			NS_FileRead_fullyLoad(			const TCHAR *FilePath ) ;										// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern	int			NS_FileRead_fullyLoad_WithStrLen(		const TCHAR *FilePath, size_t FilePathLength ) ;				// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern	int			NS_FileRead_fullyLoad_delete(	int FLoadHandle ) ;										// FileRead_fullyLoad で読み込んだファイルをメモリから削除する
extern	const void*	NS_FileRead_fullyLoad_getImage( int FLoadHandle ) ;										// FileRead_fullyLoad で読み込んだファイルの内容を格納したメモリアドレスを取得する
extern	LONGLONG	NS_FileRead_fullyLoad_getSize(	int FLoadHandle ) ;										// FileRead_fullyLoad で読み込んだファイルのサイズを取得する

// 設定関係関数
extern	int			NS_GetStreamFunctionDefault(	void ) ;														// ＤＸライブラリでストリームデータアクセスに使用する関数がデフォルトのものか調べる( TRUE:デフォルト  FALSE:デフォルトではない )
extern	int			NS_ChangeStreamFunction(		const STREAMDATASHREDTYPE2 *StreamThread ) ;						// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する
extern	int			NS_ChangeStreamFunctionW(		const STREAMDATASHREDTYPE2W *StreamThreadW ) ;					// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する( wchar_t 使用版 )

// 補助関係関数
extern int			NS_ConvertFullPath( const TCHAR *Src, TCHAR *Dest, const TCHAR *CurrentDir = NULL ) ;			// フルパスではないパス文字列をフルパスに変換する( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern int			NS_ConvertFullPathWithStrLen(			const TCHAR *Src, size_t SrcLength, TCHAR *Dest, const TCHAR *CurrentDir = NULL, size_t CurrentDirLength = 0 ) ;	// フルパスではないパス文字列をフルパスに変換する( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )












// DxNetwork.cpp関数プロトタイプ宣言

#ifndef DX_NON_NETWORK
extern	int			NS_GetWinSockLastError( void ) ;				// WinSock で最後に発生したエラーのコードを取得する
#endif // DX_NON_NETWORK















// DxInput.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUT

// 入力状態取得関数
extern	int			NS_CheckHitKey(							int KeyCode ) ;															// キーボードの押下状態を取得する
extern	int			NS_CheckHitKeyAll(							int CheckType = DX_CHECKINPUT_ALL ) ;									// どれか一つでもキーが押されているかどうかを取得( 押されていたら戻り値が 0 以外になる )
extern	int			NS_GetHitKeyStateAll(						DX_CHAR *KeyStateBuf ) ;												// すべてのキーの押下状態を取得する( KeyStateBuf:char型256個分の配列の先頭アドレス )
extern	int			NS_GetJoypadNum(							void ) ;																// ジョイパッドが接続されている数を取得する
extern	int			NS_GetJoypadInputState(					int InputType ) ;														// ジョイパッドの入力状態を取得する
extern	int			NS_GetJoypadAnalogInput(					int *XBuf, int *YBuf, int InputType ) ;									// ジョイパッドのアナログ的なスティック入力情報を得る
extern	int			NS_GetJoypadAnalogInputRight(				int *XBuf, int *YBuf, int InputType ) ;									// ( 使用非推奨 )ジョイパッドのアナログ的なスティック入力情報を得る(右スティック用)
extern	int			NS_GetJoypadDirectInputState(				int InputType, DINPUT_JOYSTATE *DInputState ) ;							// DirectInput から得られるジョイパッドの生のデータを取得する( DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern	int			NS_CheckJoypadXInput(						int InputType ) ;														// 指定の入力デバイスが XInput に対応しているかどうかを取得する( 戻り値  TRUE:XInput対応の入力デバイス  FALSE:XInput非対応の入力デバイス   -1:エラー )( DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern	int			NS_GetJoypadXInputState(					int InputType, XINPUT_STATE *XInputState ) ;							// XInput から得られる入力デバイス( Xbox360コントローラ等 )の生のデータを取得する( XInput非対応のパッドの場合はエラーとなり -1 を返す、DX_INPUT_KEY や DX_INPUT_KEY_PAD1 など、キーボードが絡むタイプを InputType に渡すとエラーとなり -1 を返す )
extern	int			NS_SetJoypadInputToKeyInput(				int InputType, int PadInput, int KeyInput1, int KeyInput2 = -1 , int KeyInput3 = -1 , int KeyInput4 = -1  ) ; // ジョイパッドの入力に対応したキーボードの入力を設定する( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )　　PadInput:設定を変更するパッドボタンの識別子( PAD_INPUT_1 等 )　　KeyInput1:PadInput を押下したことにするキーコード( KEY_INPUT_A など )その１　　KeyInput2:その２、-1で設定なし　　KeyInput3:その３、-1で設定なし　　KeyInput4:その４、-1で設定なし )
extern	int			NS_SetJoypadDeadZone(						int InputType, double Zone ) ;											// ジョイパッドの無効ゾーンの設定を行う( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )   Zone:新しい無効ゾーン( 0.0 〜 1.0 )、デフォルト値は 0.35 )
extern	double		NS_GetJoypadDeadZone(						int InputType ) ;														// ジョイパッドの無効ゾーンの設定を取得する( InputType:設定を変更するパッドの識別子( DX_INPUT_PAD1等 )  戻り値:無効ゾーン( 0.0 〜 1.0 )
extern	int			NS_StartJoypadVibration(					int InputType, int Power, int Time, int EffectIndex = -1 ) ;			// ジョイパッドの振動を開始する
extern	int			NS_StopJoypadVibration(					int InputType, int EffectIndex = -1 ) ;									// ジョイパッドの振動を停止する
extern	int			NS_GetJoypadPOVState(						int InputType, int POVNumber ) ;										// ジョイパッドのＰＯＶ入力の状態を得る( 戻り値　指定のPOVデータの角度、単位は角度の１００倍( 90度なら 9000 ) 中心位置にある場合は -1 が返る )
extern	int			NS_ReSetupJoypad(							void ) ;																// ジョイパッドの再セットアップを行う( 新たに接続されたジョイパッドがあったら検出される )

extern	int			NS_SetUseJoypadVibrationFlag(				int Flag ) ;															// ジョイパッドの振動機能を使用するかどうかを設定する( TRUE:使用する　　FALSE:使用しない )

#ifdef __WINDOWS__
extern	int			NS_SetKeyExclusiveCooperativeLevelFlag(	int Flag ) ;															// DirectInput のキーボードの協調レベルを排他レベルにするかどうかを設定する( TRUE:排他レベルにする  FALSE:標準レベルにする( デフォルト ) )、DxLib_Init の呼び出し前でのみ実行可能　
extern	int			NS_SetKeyboardNotDirectInputFlag(			int Flag ) ;															// キーボードの入力処理に DirectInput を使わないかどうかを設定する( TRUE:DirectInput を使わず、Windows標準機能を使用する　　FALSE:DirectInput を使用する )
extern	int			NS_SetUseDirectInputFlag(					int Flag ) ;															// 入力処理に DirectInput を使用するかどうかを設定する( TRUE:DirectInput を使用する　　FALSE:DirectInput を使わず、Windows標準機能を使用する )
extern	int			NS_SetUseXInputFlag(						int Flag ) ;															// Xbox360コントローラの入力処理に XInput を使用するかどうかを設定する( TRUE:XInput を使用する( デフォルト )　　FALSE:XInput を使用しない )
extern	int			NS_GetJoypadGUID(							int PadIndex, GUID *GuidInstanceBuffer, GUID *GuidProductBuffer ) ;		// ジョイパッドのＧＵIＤを得る
extern	int			NS_GetJoypadName(							int InputType, TCHAR *InstanceNameBuffer, TCHAR *ProductNameBuffer ) ;	// ジョイパッドのデバイス登録名と製品登録名を取得する
extern	int			NS_ConvertKeyCodeToVirtualKey(				int KeyCode ) ;															// ＤＸライブラリのキーコード( KEY_INPUT_A など )に対応する Windows の仮想キーコード( VK_LEFT など ) を取得する( KeyCode:変換したいＤＸライブラリのキーコード　戻り値：Windowsの仮想キーコード )
extern	int			NS_ConvertVirtualKeyToKeyCode(				int VirtualKey ) ;														//  Windows の仮想キーコード( VK_LEFT など ) に対応するＤＸライブラリのキーコード( KEY_INPUT_A など )を取得する( VirtualKey:変換したいWindowsの仮想キーコード　戻り値：ＤＸライブラリのキーコード )
#endif // __WINDOWS__

#endif // DX_NON_INPUT












#ifndef DX_NOTUSE_DRAWFUNCTION

// 画像処理系関数プロトタイプ宣言

// グラフィックハンドル作成関係関数
extern	int			NS_MakeGraph(						int SizeX, int SizeY, int NotUse3DFlag = FALSE ) ;							// グラフィックハンドルを作成する
extern	int			NS_MakeScreen(						int SizeX, int SizeY, int UseAlphaChannel = FALSE ) ;						// SetDrawScreen で描画対象にできるグラフィックハンドルを作成する
extern	int			NS_DerivationGraph(				int SrcX, int SrcY, int Width, int Height, int SrcGraphHandle ) ;			// 指定のグラフィックの指定部分だけを抜き出して新たなグラフィックハンドルを作成する
extern	int			NS_DerivationGraphF(				float SrcX, float SrcY, float Width, float Height, int SrcGraphHandle ) ;	// 指定のグラフィックハンドルの指定部分だけを抜き出して新たなグラフィックハンドルを作成する( float版 )
extern	int			NS_DeleteGraph(					int GrHandle, int LogOutFlag = FALSE ) ;									// 指定のグラフィックハンドルを削除する
extern	int			NS_DeleteSharingGraph(				int GrHandle ) ;															// 指定のグラフィックハンドルと、同じグラフィックハンドルから派生しているグラフィックハンドル( DerivationGraph で派生したハンドル、LoadDivGraph 読み込んで作成された複数のハンドル )を一度に削除する
extern	int			NS_GetGraphNum(					void ) ;																	// 有効なグラフィックハンドルの数を取得する
extern	int			NS_FillGraph(						int GrHandle, int Red, int Green, int Blue, int Alpha = 255 ) ;				// グラフィックハンドルを特定の色で塗りつぶす
extern	int			NS_FillRectGraph(						int GrHandle, int x, int y, int Width, int Height, int Red, int Green, int Blue, int Alpha = 255 ) ;	// グラフィックハンドルの指定の範囲を指定の色で塗りつぶす
extern	int			NS_SetGraphLostFlag(				int GrHandle, int *LostFlag ) ;												// グラフィックハンドル削除時に立てるフラグのアドレスをセットする
extern	int			NS_InitGraph(						int LogOutFlag = FALSE ) ;													// すべてのグラフィックハンドルを削除する
extern	int			NS_ReloadFileGraphAll(				void ) ;																	// ファイルから読み込んだ画像をグラフィックハンドルに再度読み込む

// シャドウマップハンドル関係関数
extern	int			NS_MakeShadowMap(					int SizeX, int SizeY ) ;													// シャドウマップを作成する
extern	int			NS_DeleteShadowMap(				int SmHandle ) ;															// シャドウマップハンドルを削除する
extern	int			NS_SetShadowMapLightDirection(		int SmHandle, VECTOR Direction ) ;											// シャドウマップが想定するライトの方向を設定する
extern	int			NS_ShadowMap_DrawSetup(			int SmHandle ) ;										// シャドウマップへの描画の準備を行う
extern	int			NS_ShadowMap_DrawEnd(				void ) ;															// シャドウマップへの描画を終了する
extern	int			NS_SetUseShadowMap(				int SlotIndex, int SmHandle ) ;												// 描画で使用するシャドウマップを指定する、有効なスロットは０〜２、SmHandle に -1 を渡すと指定のスロットのシャドウマップを解除
extern	int			NS_SetShadowMapDrawArea(			int SmHandle, VECTOR MinPosition, VECTOR MaxPosition ) ;					// シャドウマップに描画する際の範囲を設定する( この関数で描画範囲を設定しない場合は視錐台を拡大した範囲が描画範囲となる )
extern	int			NS_ResetShadowMapDrawArea(			int SmHandle ) ;															// SetShadowMapDrawArea の設定を解除する
extern	int			NS_SetShadowMapAdjustDepth(		int SmHandle, float Depth ) ;												// シャドウマップを使用した描画時の補正深度を設定する
extern	int			NS_GetShadowMapViewProjectionMatrix( int SmHandle, MATRIX *MatrixBuffer ) ;									// シャドウマップ作成時や適用時に使用するビュー行列と射影行列を乗算した行列を取得する
extern	int			NS_SetShadowMapBlurParam(			int SmHandle, int Param ) ;													// シャドウマップに適用するぼかし度合いを設定する
extern	int			NS_SetShadowMapGradationParam(		int SmHandle, float Param ) ;												// シャドウマップに適用する影をグラデーションさせる範囲を設定する
extern	int			NS_TestDrawShadowMap(				int SmHandle, int x1, int y1, int x2, int y2 ) ;							// シャドウマップを画面にテスト描画する

// グラフィックハンドルへの画像転送関数
extern	int			NS_BltBmpToGraph(					const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int CopyPointX, int CopyPointY,                                                                                                   int GrHandle ) ;						// ＢＭＰの内容をグラフィックハンドルに転送
extern	int			NS_BltBmpToDivGraph(				const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int AllNum, int XNum, int YNum, int Width, int Height,                                                                      const int *GrHandle, int ReverseFlag ) ;	// ＢＭＰの内容を分割作成したグラフィックハンドルたちに転送
extern	int			NS_BltBmpOrGraphImageToGraph(		const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int CopyPointX, int CopyPointY,                              int GrHandle ) ;						// ＢＭＰ か BASEIMAGE をグラフィックハンドルに転送
extern	int			NS_BltBmpOrGraphImageToGraph2(		const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, const RECT *SrcRect, int DestX, int DestY,                   int GrHandle ) ;						// ＢＭＰ か BASEIMAGE の指定の領域をグラフィックハンドルに転送
extern	int			NS_BltBmpOrGraphImageToDivGraph(	const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int Width, int Height, const int *GrHandle, int ReverseFlag ) ;	// ＢＭＰ か BASEIMAGE を分割作成したグラフィックハンドルたちに転送
extern	int			NS_BltBmpOrGraphImageToDivGraphF(	const COLORDATA *BmpColorData, HBITMAP RgbBmp, HBITMAP AlphaBmp, int BmpFlag, const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float Width, float Height, const int *GrHandle, int ReverseFlag ) ;	// ＢＭＰ か BASEIMAGE を分割作成したグラフィックハンドルたちに転送( float型 )

// 画像からグラフィックハンドルを作成する関数
extern	int			NS_LoadBmpToGraph(					const TCHAR *FileName, int TextureFlag, int ReverseFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL ) ;								// 画像ファイルからグラフィックハンドルを作成する
extern	int			NS_LoadBmpToGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int TextureFlag, int ReverseFlag, int SurfaceMode = DX_MOVIESURFACE_NORMAL ) ;										// 画像ファイルからグラフィックハンドルを作成する
extern	int			NS_LoadGraph(							const TCHAR *FileName,                        int NotUse3DFlag = FALSE ) ;																							// 画像ファイルからグラフィックハンドルを作成する
extern	int			NS_LoadGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int NotUse3DFlag = FALSE ) ;																							// 画像ファイルからグラフィックハンドルを作成する
extern	int			NS_LoadReverseGraph(					const TCHAR *FileName,                        int NotUse3DFlag = FALSE ) ;																							// 画像ファイルを反転したものでグラフィックハンドルを作成する
extern	int			NS_LoadReverseGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int NotUse3DFlag = FALSE ) ;																							// 画像ファイルを反転したものでグラフィックハンドルを作成する
extern	int			NS_LoadDivGraph(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphF(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphFWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivBmpToGraph(					const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivBmpToGraphWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivBmpToGraphF(					const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivBmpToGraphFWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, int TextureFlag, int ReverseFlag ) ;		// 画像ファイルを分割してグラフィックハンドルを作成する
extern	int			NS_LoadReverseDivGraph(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			NS_LoadReverseDivGraphWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			NS_LoadReverseDivGraphF(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			NS_LoadReverseDivGraphFWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray, int NotUse3DFlag = FALSE ) ;				// 画像ファイルを反転したものを分割してグラフィックハンドルを作成する
extern	int			NS_LoadBlendGraph(						const TCHAR *FileName ) ;																																			// 画像ファイルからブレンド用グラフィックハンドルを作成する
extern	int			NS_LoadBlendGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength ) ;																													// 画像ファイルからブレンド用グラフィックハンドルを作成する
#ifdef __WINDOWS__
extern	int			NS_LoadGraphToResource(			int ResourceID ) ;																													// 画像リソースからグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphToResource(			int ResourceID, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray ) ;										// 画像リソースを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphFToResource(		int ResourceID, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;										// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern	int			NS_LoadGraphToResource(			const TCHAR *ResourceName, const TCHAR *ResourceType ) ;																			// 画像リソースからグラフィックハンドルを作成する
extern	int			NS_LoadGraphToResourceWithStrLen(		const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength ) ;																				// 画像リソースからグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphToResource(				const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphToResourceWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する
extern	int			NS_LoadDivGraphFToResource(			const TCHAR *ResourceName,                            const TCHAR *ResourceType,                            int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern	int			NS_LoadDivGraphFToResourceWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int AllNum, int XNum, int YNum, float XSize, float YSize, int *HandleArray ) ;	// 画像リソースを分割してグラフィックハンドルを作成する( float型 )
extern	int			NS_CreateGraphFromID3D11Texture2D( const void *pID3D11Texture2D ) ;									// ID3D11Texture2D からグラフィックハンドルを作成する
#endif // __WINDOWS__

extern	int			NS_CreateGraphFromMem(             const void *RGBFileImage, int RGBFileImageSize,               const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ,                  int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// メモリ上のグラフィックイメージからグラフィックハンドルを作成する
extern	int			NS_ReCreateGraphFromMem(           const void *RGBFileImage, int RGBFileImageSize, int GrHandle, const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ,                  int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// メモリ上のグラフィックイメージから既存のグラフィックハンドルにデータを転送する
extern	int			NS_CreateDivGraphFromMem(          const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray,                 int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphFFromMem(         const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray,                 int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから分割グラフィックハンドルを作成する( float版 )
extern	int			NS_ReCreateDivGraphFromMem(        const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray,                 int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから既存の分割グラフィックハンドルにデータを転送する
extern	int			NS_ReCreateDivGraphFFromMem(       const void *RGBFileImage, int RGBFileImageSize, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,                 int TextureFlag = TRUE , int ReverseFlag = FALSE , const void *AlphaFileImage = NULL , int AlphaFileImageSize = 0 ) ;			// メモリ上の画像イメージから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			NS_CreateGraphFromBmp(             const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage,               const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL , int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																			// ビットマップデータからグラフィックハンドルを作成する
extern	int			NS_ReCreateGraphFromBmp(           const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int GrHandle, const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL , int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																			// ビットマップデータから既存のグラフィックハンドルにデータを転送する
extern	int			NS_CreateDivGraphFromBmp(          const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray,          int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphFFromBmp(         const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray,          int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから分割グラフィックハンドルを作成する( float版 )
extern	int			NS_ReCreateDivGraphFromBmp(        const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray,          int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから既存の分割グラフィックハンドルにデータを転送する
extern	int			NS_ReCreateDivGraphFFromBmp(       const BITMAPINFO *RGBBmpInfo, const void *RGBBmpImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray,          int TextureFlag = TRUE , int ReverseFlag = FALSE , const BITMAPINFO *AlphaBmpInfo = NULL , const void *AlphaBmpImage = NULL ) ;	// ビットマップデータから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			NS_CreateDXGraph(					const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,                                                                                 int TextureFlag ) ;																									// 基本イメージデータからサイズを割り出し、それに合ったグラフィックハンドルを作成する
extern	int			NS_CreateGraphFromGraphImage(      const BASEIMAGE *RgbBaseImage,                                                                                                                  int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータからグラフィックハンドルを作成する
extern	int			NS_CreateGraphFromGraphImage(      const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage,                                                                                 int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータからグラフィックハンドルを作成する
extern	int			NS_ReCreateGraphFromGraphImage(    const BASEIMAGE *RgbBaseImage,                                  int GrHandle,                                                                   int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存のグラフィックハンドルにデータを転送する
extern	int			NS_ReCreateGraphFromGraphImage(    const BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int GrHandle,                                                                   int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存のグラフィックハンドルにデータを転送する
extern	int			NS_CreateDivGraphFromGraphImage(         BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphFFromGraphImage(        BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する( float版 )
extern	int			NS_CreateDivGraphFromGraphImage(         BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphFFromGraphImage(        BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY,       int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから分割グラフィックハンドルを作成する( float版 )
extern	int			NS_ReCreateDivGraphFromGraphImage(       BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する
extern	int			NS_ReCreateDivGraphFFromGraphImage(      BASEIMAGE *RgbBaseImage,                                  int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			NS_ReCreateDivGraphFromGraphImage(       BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する
extern	int			NS_ReCreateDivGraphFFromGraphImage(      BASEIMAGE *RgbBaseImage, const BASEIMAGE *AlphaBaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, const int *HandleArray, int TextureFlag = TRUE , int ReverseFlag = FALSE ) ;																// 基本イメージデータから既存の分割グラフィックハンドルにデータを転送する( float版 )
extern	int			NS_CreateGraph(                    int Width, int Height, int Pitch, const void *RGBImage, const void *AlphaImage = NULL , int GrHandle = -1 ) ;																																			// メモリ上のビットマップイメージからグラフィックハンドルを作成する
extern	int			NS_CreateDivGraph(                 int Width, int Height, int Pitch, const void *RGBImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray, const void *AlphaImage = NULL ) ;																						// メモリ上のビットマップイメージから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphF(                int Width, int Height, int Pitch, const void *RGBImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray, const void *AlphaImage = NULL ) ;																						// メモリ上のビットマップイメージから分割グラフィックハンドルを作成する( float版 )
extern	int			NS_ReCreateGraph(                  int Width, int Height, int Pitch, const void *RGBImage, int GrHandle, const void *AlphaImage = NULL ) ;																																					// メモリ上のビットマップイメージからグラフィックハンドルを再作成する
#ifndef DX_NON_SOFTIMAGE
extern	int			NS_CreateBlendGraphFromSoftImage(  int SIHandle ) ;																													// ソフトウエアで扱うイメージからブレンド用画像グラフィックハンドルを作成する( -1:エラー  -1以外:ブレンド用グラフィックハンドル )
extern	int			NS_CreateGraphFromSoftImage(       int SIHandle ) ;																													// ソフトウエアで扱うイメージからグラフィックハンドルを作成する( -1:エラー  -1以外:グラフィックハンドル )
extern	int			NS_CreateGraphFromRectSoftImage(   int SIHandle, int x, int y, int SizeX, int SizeY ) ;																				// ソフトウエアで扱うイメージの指定の領域を使ってグラフィックハンドルを作成する( -1:エラー  -1以外:グラフィックハンドル )
extern	int			NS_ReCreateGraphFromSoftImage(     int SIHandle, int GrHandle ) ;																										// ソフトウエアで扱うイメージから既存のグラフィックハンドルに画像データを転送する
extern	int			NS_ReCreateGraphFromRectSoftImage( int SIHandle, int x, int y, int SizeX, int SizeY, int GrHandle ) ;																	// ソフトウエアで扱うイメージから既存のグラフィックハンドルに画像データを転送する
extern	int			NS_CreateDivGraphFromSoftImage(    int SIHandle, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray ) ;											// ソフトウエアで扱うイメージから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphFFromSoftImage(   int SIHandle, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray ) ;											// ソフトウエアで扱うイメージから分割グラフィックハンドルを作成する( float版 )
#endif // DX_NON_SOFTIMAGE
extern	int			NS_CreateGraphFromBaseImage(       const BASEIMAGE *BaseImage ) ;																										// 基本イメージデータからグラフィックハンドルを作成する
extern	int			NS_CreateGraphFromRectBaseImage(   const BASEIMAGE *BaseImage, int x, int y, int SizeX, int SizeY ) ;																	// 基本イメージデータの指定の領域を使ってグラフィックハンドルを作成する
extern	int			NS_ReCreateGraphFromBaseImage(     const BASEIMAGE *BaseImage,                                     int GrHandle ) ;													// 基本イメージデータから既存のグラフィックハンドルに画像データを転送する
extern	int			NS_ReCreateGraphFromRectBaseImage( const BASEIMAGE *BaseImage, int x, int y, int SizeX, int SizeY, int GrHandle ) ;													// 基本イメージデータの指定の領域を使って既存のグラフィックハンドルに画像データを転送する
extern	int			NS_CreateDivGraphFromBaseImage(          BASEIMAGE *BaseImage, int AllNum, int XNum, int YNum, int   SizeX, int   SizeY, int *HandleArray ) ;							// 基本イメージデータから分割グラフィックハンドルを作成する
extern	int			NS_CreateDivGraphFFromBaseImage(         BASEIMAGE *BaseImage, int AllNum, int XNum, int YNum, float SizeX, float SizeY, int *HandleArray ) ;							// 基本イメージデータから分割グラフィックハンドルを作成する( float版 )
extern	int			NS_ReloadGraph(					const TCHAR *FileName, int GrHandle, int ReverseFlag = FALSE ) ;																	// 画像ファイルからグラフィックハンドルへ画像データを転送する
extern	int			NS_ReloadGraphWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int GrHandle, int ReverseFlag = FALSE ) ;																		// 画像ファイルからグラフィックハンドルへ画像データを転送する
extern	int			NS_ReloadDivGraph(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する
extern	int			NS_ReloadDivGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する
extern	int			NS_ReloadDivGraphF(					const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する( float版 )
extern	int			NS_ReloadDivGraphFWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray, int ReverseFlag = FALSE ) ;	// 画像ファイルからグラフィックハンドルたちへ画像データを分割転送する( float版 )
extern	int			NS_ReloadReverseGraph(					const TCHAR *FileName,                        int GrHandle ) ;																									// ReloadGraph の画像反転処理追加版
extern	int			NS_ReloadReverseGraphWithStrLen(		const TCHAR *FileName, size_t FileNameLength, int GrHandle ) ;																									// ReloadGraph の画像反転処理追加版
extern	int			NS_ReloadReverseDivGraph(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray ) ;								// ReloadDivGraph の画像反転処理追加版
extern	int			NS_ReloadReverseDivGraphWithStrLen(	const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int   XSize, int   YSize, const int *HandleArray ) ;								// ReloadDivGraph の画像反転処理追加版
extern	int			NS_ReloadReverseDivGraphF(				const TCHAR *FileName,                        int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray ) ;								// ReloadDivGraph の画像反転処理追加版( float版 )
extern	int			NS_ReloadReverseDivGraphFWithStrLen(	const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, float XSize, float YSize, const int *HandleArray ) ;								// ReloadDivGraph の画像反転処理追加版( float版 )

// グラフィックハンドル作成時設定係関数
extern	int			NS_SetGraphColorBitDepth(						int ColorBitDepth ) ;							// SetCreateGraphColorBitDepth の旧名称
extern	int			NS_GetGraphColorBitDepth(						void ) ;										// GetCreateGraphColorBitDepth の旧名称
extern	int			NS_SetCreateGraphColorBitDepth(				int BitDepth ) ;								// 作成するグラフィックの色深度を設定する
extern	int			NS_GetCreateGraphColorBitDepth(				void ) ;										// 作成するグラフィックの色深度を取得する
extern	int			NS_SetCreateGraphChannelBitDepth(				int BitDepth ) ;								// 作成するグラフィックの１チャンネル辺りのビット深度を設定する
extern	int			NS_GetCreateGraphChannelBitDepth(				void ) ;										// 作成するグラフィックの１チャンネル辺りのビット深度を取得する
extern	int			NS_SetDrawValidGraphCreateFlag(				int Flag ) ;									// 描画可能なグラフィックを作成するかどうかのフラグをセットする( TRUE:描画可能  FALSE:描画不可能 )
extern	int			NS_GetDrawValidGraphCreateFlag(				void ) ;										// 描画可能なグラフィックを作成するかどうかのフラグを取得する
extern	int			NS_SetDrawValidFlagOf3DGraph(					int Flag ) ;									// SetDrawValidGraphCreateFlag の旧名称
extern	int			NS_SetLeftUpColorIsTransColorFlag(				int Flag ) ;									// 画像左上の色を透過色にするかどうかのフラグをセットする
extern	int			NS_SetUsePaletteGraphFlag(						int Flag ) ;									// 読み込む画像がパレット画像の場合、パレット画像として使用できる場合はパレット画像として使用するかどうかを設定する( TRUE:パレット画像として使用できる場合はパレット画像として使用する( デフォルト )  FALSE:パレット画像として使用できる場合もパレット画像としては使用しない( 通常タイプの画像に変換して使用する ) )
extern	int			NS_SetUseSystemMemGraphCreateFlag(				int Flag ) ;									// システムメモリ上にグラフィックを作成するかどうかのフラグをセットする( TRUE:システムメモリ上に作成  FALSE:ＶＲＡＭ上に作成 )
extern	int			NS_GetUseSystemMemGraphCreateFlag(				void ) ;										// システムメモリ上にグラフィックを作成するかどうかのフラグを取得する( TRUE:システムメモリ上に作成  FALSE:ＶＲＡＭ上に作成 )
extern	int			NS_SetUseBlendGraphCreateFlag(					int Flag ) ;									// ブレンド処理用画像を作成するかどうかのフラグをセットする
extern	int			NS_GetUseBlendGraphCreateFlag(					void ) ;										// ブレンド処理用画像を作成するかどうかのフラグを取得する
extern	int			NS_SetUseAlphaTestGraphCreateFlag(				int Flag ) ;									// アルファテストを使用するグラフィックを作成するかどうかのフラグをセットする
extern	int			NS_GetUseAlphaTestGraphCreateFlag(				void ) ;										// アルファテストを使用するグラフィックを作成するかどうかのフラグを取得する
extern	int			NS_SetUseAlphaTestFlag(						int Flag ) ;									// SetUseAlphaTestGraphCreateFlag の旧名称
extern	int			NS_GetUseAlphaTestFlag(						void ) ;										// GetUseAlphaTestGraphCreateFlag の旧名称
extern	int			NS_SetCubeMapTextureCreateFlag(				int Flag ) ;									// キューブマップテクスチャを作成するかどうかのフラグを設定する
extern	int			NS_GetCubeMapTextureCreateFlag(				void ) ;										// キューブマップテクスチャを作成するかどうかのフラグを取得する
extern	int			NS_SetUseNoBlendModeParam(						int Flag ) ;									// SetDrawBlendMode 関数の第一引数に DX_BLENDMODE_NOBLEND を代入した際に、デフォルトでは第二引数は内部で２５５を指定したことになるが、その自動２５５化をしないかどうかを設定する( TRUE:しない(第二引数の値が使用される)   FALSE:する(第二引数の値は無視されて 255 が常に使用される)(デフォルト) )αチャンネル付き画像に対して描画を行う場合のみ意味がある関数
extern	int			NS_SetDrawValidAlphaChannelGraphCreateFlag(	int Flag ) ;									// 描画可能なαチャンネル付き画像を作成するかどうかのフラグをセットする,SetDrawValidGraphCreateFlag 関数で描画可能画像を作成するように設定されていないと効果がない( TRUE:αチャンネル付き FALSE:αチャンネルなし )
extern	int			NS_GetDrawValidAlphaChannelGraphCreateFlag(	void ) ;										// 描画可能なαチャンネル付き画像を作成するかどうかのフラグを取得する
extern	int			NS_SetDrawValidFloatTypeGraphCreateFlag(		int Flag ) ;									// 描画可能な浮動小数点型の画像を作成するかどうかのフラグをセットする,SetDrawValidGraphCreateFlag 関数で描画可能画像を作成するように設定されていないと効果がない、ＧＰＵが Float型の画像に対応していない場合は画像作成に失敗する( TRUE:Float型 FALSE:Int型 )
extern	int			NS_GetDrawValidFloatTypeGraphCreateFlag(		void ) ;										// 描画可能な浮動小数点型の画像を作成するかどうかのフラグを取得する
extern	int			NS_SetDrawValidGraphCreateZBufferFlag(			int Flag ) ;									// 描画可能画像を作成する際に専用のＺバッファも作成するかどうかを設定する( デフォルトでは TRUE( 作成する ) )
extern	int			NS_GetDrawValidGraphCreateZBufferFlag(			void ) ;										// 描画可能画像を作成する際に専用のＺバッファも作成するかどうかを取得する
extern	int			NS_SetCreateDrawValidGraphZBufferBitDepth(		int BitDepth ) ;								// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するＺバッファのビット深度を設定する( BitDepth:ビット深度( 指定可能な値は 16, 24, 32 の何れか( SetDrawValidGraphCreateFlag 関数で描画対象として使用できるグラフィックハンドルを作成するように設定されていないと効果ありません )
extern	int			NS_GetCreateDrawValidGraphZBufferBitDepth(		void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するＺバッファのビット深度を取得する
extern	int			NS_SetCreateDrawValidGraphMipLevels(			int MipLevels ) ;								// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するMipMapのレベルを設定する
extern	int			NS_GetCreateDrawValidGraphMipLevels(			void ) ;										// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するMipMapのレベルを取得する
extern	int			NS_SetCreateDrawValidGraphChannelNum(			int ChannelNum ) ;								// 作成する描画可能な画像のチャンネル数を設定する,SetDrawValidGraphCreateFlag 関数で描画可能画像を作成するように設定されていないと効果がない
extern	int			NS_GetCreateDrawValidGraphChannelNum(			void ) ;										// 作成する描画可能な画像のチャンネル数を取得する
extern	int			NS_SetCreateDrawValidGraphMultiSample(			int Samples, int Quality ) ;					// SetDrawScreen の引数として渡せる( 描画対象として使用できる )グラフィックハンドルに適用するマルチサンプリング( アンチエイリアシング )設定を行う( Samples:マルチサンプル処理に使用するドット数( 多いほど重くなります )  Quality:マルチサンプル処理の品質 )
extern	int			NS_SetDrawValidMultiSample(					int Samples, int Quality ) ;					// 描画可能な画像のマルチサンプリング設定を行う
extern	int			NS_GetMultiSampleQuality(						int Samples ) ;									// 指定のマルチサンプル数で使用できる最大クオリティ値を取得する( 戻り値がマイナスの場合は引数のサンプル数が使用できないことを示します )
extern	int			NS_SetUseTransColor(							int Flag ) ;									// 透過色機能を使用するかどうかを設定する
extern	int			NS_SetUseTransColorGraphCreateFlag(			int Flag ) ;									// 透過色機能を使用することを前提とした画像の読み込み処理を行うかどうかを設定する( TRUE にすると SetDrawMode( DX_DRAWMODE_BILINEAR ); をした状態で DrawGraphF 等の浮動小数点型座標を受け取る関数で小数点以下の値を指定した場合に発生する描画結果の不自然を緩和する効果がある ( デフォルトは FALSE ) )
extern	int			NS_SetUseGraphAlphaChannel(					int Flag ) ;									// SetUseAlphaChannelGraphCreateFlag の旧名称
extern	int			NS_GetUseGraphAlphaChannel(					void ) ;										// GetUseAlphaChannelGraphCreateFlag の旧名称
extern	int			NS_SetUseAlphaChannelGraphCreateFlag(			int Flag ) ;									// αチャンネル付きグラフィックを作成するかどうかのフラグをセットする( TRUE:αチャンネル付き   FALSE:αチャンネル無し )
extern	int			NS_GetUseAlphaChannelGraphCreateFlag(			void ) ;										// αチャンネル付きグラフィックを作成するかどうかのフラグを取得する( TRUE:αチャンネル付き   FALSE:αチャンネル無し )
extern	int			NS_SetUseNotManageTextureFlag(					int Flag ) ;									// 非管理テクスチャを使用するか、のフラグをセット( TRUE:使用する  FALSE:使用しない )
extern	int			NS_GetUseNotManageTextureFlag(					void ) ;										// 非管理テクスチャを使用するか、のフラグを取得する( TRUE:使用する  FALSE:使用しない )
extern	int			NS_SetUsePlatformTextureFormat(				int PlatformTextureFormat ) ;					// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを指定する( Direct3D9環境なら DX_TEXTUREFORMAT_DIRECT3D9_R8G8B8 など、0 を渡すと解除 )
extern	int			NS_GetUsePlatformTextureFormat(				void ) ;										// 作成するグラフィックハンドルで使用する環境依存のテクスチャフォーマットを取得する
extern	int			NS_SetTransColor(								int Red, int Green, int Blue ) ;				// グラフィックに設定する透過色をセットする
extern	int			NS_GetTransColor(								int *Red, int *Green, int *Blue ) ;				// 透過色を得る
extern	int			NS_SetUseDivGraphFlag(							int Flag ) ;									// 必要ならグラフィックの分割を行うか、フラグのセット
extern	int			NS_SetUseAlphaImageLoadFlag(					int Flag ) ;									// LoadGraph などの際にファイル名の末尾に _a が付いたアルファチャンネル用の画像ファイルを追加で読み込む処理を行うかどうかを設定する( TRUE:行う( デフォルト )  FALSE:行わない )
extern	int			NS_SetUseMaxTextureSize(						int Size ) ;									// 使用するテクスチャーの最大サイズをセット(0でデフォルト)
extern	int			NS_SetUseGraphBaseDataBackup(					int Flag ) ;									// 画像を作成する際に使用した画像データのバックアップをしてデバイスロスト時に使用するかどうかのフラグをセットする( TRUE:バックアップをする( デフォルト )  FALSE:バックアップをしない )
extern	int			NS_GetUseGraphBaseDataBackup(					void ) ;										// 画像を作成する際に使用した画像データのバックアップをしてデバイスロスト時に使用するかどうかのフラグを取得する

// 画像情報関係関数
extern	const unsigned int* NS_GetFullColorImage(				int GrHandle ) ;																		// 指定の画像のＡＲＧＢ８のフルカラーイメージを取得する

extern	int			NS_GraphLock(						int GrHandle, int *PitchBuf, void **DataPointBuf, COLORDATA **ColorDataPP = NULL, int WriteOnly = FALSE ) ;	// グラフィックメモリ領域のロック
extern	int			NS_GraphUnLock(					int GrHandle ) ;																		// グラフィックメモリ領域のロック解除

extern	int			NS_SetUseGraphZBuffer(				int GrHandle, int UseFlag, int BitDepth = -1 ) ;										// グラフィック専用のＺバッファを持つかどうかを設定する( 描画可能画像のみ可能 )
extern	int			NS_CopyGraphZBufferImage(			int DestGrHandle, int SrcGrHandle ) ;													// グラフィックのＺバッファの状態を別のグラフィックのＺバッファにコピーする( DestGrHandle も SrcGrHandle もＺバッファを持っている描画可能画像で、サイズが同じであり、且つアンチエイリアス画像ではないことが条件 )

extern	int			NS_SetDeviceLostDeleteGraphFlag(	int GrHandle, int DeleteFlag ) ;														// グラフィックスデバイスのデバイスロスト発生時に指定のグラフィックハンドルを削除するかどうかを設定する( TRUE:デバイスロスト時に削除する  FALSE:デバイスロストが発生しても削除しない )

extern	int			NS_GetGraphSize(					int GrHandle, int   *SizeXBuf, int   *SizeYBuf ) ;										// グラフィックハンドルが持つ画像のサイズを得る
extern	int			NS_GetGraphSizeF(					int GrHandle, float *SizeXBuf, float *SizeYBuf ) ;										// グラフィックハンドルが持つ画像のサイズを得る( float型 )
extern	int			NS_GetGraphTextureSize(			int GrHandle, int   *SizeXBuf, int   *SizeYBuf ) ;										// グラフィックハンドルが持つ一つ目のテクスチャのサイズを得る
extern	int			NS_GetGraphUseBaseGraphArea(		int GrHandle, int   *UseX,     int   *UseY,    int *UseSizeX, int *UseSizeY ) ;			// LoadDivGraph や DerivationGraph で元画像の一部分を使用している場合に、指定のグラフィックハンドルが使用している元画像の範囲を取得する
extern	int			NS_GetGraphMipmapCount(			int GrHandle ) ;																		// グラフィックが持つミップマップレベルの数を取得する
extern	int			NS_GetGraphFilePath(				int GrHandle, TCHAR *FilePathBuffer ) ;													// グラフィックハンドルが画像ファイルから読み込まれていた場合、その画像のファイルパスを取得する
extern	int			NS_CheckDrawValidGraph(			int GrHandle ) ;																		// 指定のグラフィックハンドルが描画対象にできる( SetDrawScreen の引数に渡せる )グラフィックハンドルかどうかを取得する( 戻り値　TRUE:描画対象にできるグラフィックハンドル　FALSE:描画対象にできないグラフィックハンドル )

#ifdef __WINDOWS__
extern	const void*	NS_GetGraphID3D11Texture2D(		int GrHandle ) ;												// グラフィックハンドルが持つ ID3D11Texture2D を取得する( Direct3D11 を使用している場合のみ有効 )( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	NS_GetGraphID3D11RenderTargetView(	int GrHandle ) ;												// グラフィックハンドルが持つ ID3D11RenderTargetView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	NS_GetGraphID3D11DepthStencilView(	int GrHandle ) ;												// グラフィックハンドルが持つ ID3D11DepthStencilView を取得する( Direct3D11 を使用していて、且つ MakeScreen で作成したグラフィックハンドルでのみ有効 )( 戻り値を ID3D11DepthStencilView * にキャストしてください )
#endif

extern	const COLORDATA*	NS_GetTexColorData(				int AlphaCh, int AlphaTest, int ColorBitDepth, int DrawValid = FALSE ) ;				// カラーデータを得る
extern	const COLORDATA*	NS_GetTexColorData(				const IMAGEFORMATDESC *Format ) ;														// フォーマットに基づいたカラーデータを得る
extern	const COLORDATA*	NS_GetTexColorData(				int FormatIndex ) ;																		// 指定のフォーマットインデックスのカラーデータを得る
extern	int			NS_GetMaxGraphTextureSize(			int *SizeX, int *SizeY ) ;																// グラフィックに使用される最大テクスチャサイズを取得する
extern	int			NS_GetValidRestoreShredPoint(		void ) ;																				// グラフィック復元関数の有無を取得																
extern	int			NS_GetCreateGraphColorData(		COLORDATA *ColorData, IMAGEFORMATDESC *Format ) ;										// これから新たにグラフィックを作成する場合に使用するカラー情報を取得する

// 画像パレット操作関係関数( ソフトウエア画像のみ使用可能 )
extern	int			NS_GetGraphPalette(				int GrHandle, int ColorIndex, int *Red, int *Green, int *Blue ) ;						// メモリ上に読み込んだ画像のパレットを取得する(フルカラー画像の場合は無効)
extern	int			NS_GetGraphOriginalPalette(		int GrHandle, int ColorIndex, int *Red, int *Green, int *Blue ) ;						// メモリ上に読み込んだ画像の SetGraphPalette で変更する前のパレットを取得する
extern	int			NS_SetGraphPalette(				int GrHandle, int ColorIndex, unsigned int Color ) ;												// メモリ上に読み込んだ画像のパレットを変更する(フルカラー画像の場合は無効)
extern	int			NS_ResetGraphPalette(				int GrHandle ) ;																		// SetGraphPalette で変更したパレットを全て元に戻す

// 図形描画関数
extern	int			NS_DrawLine(         int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color, int   Thickness = 1    ) ;						// 線を描画する
extern	int			NS_DrawLineAA(       float x1, float y1, float x2, float y2,                                         unsigned int Color, float Thickness = 1.0f ) ;						// 線を描画する( アンチエイリアス付き )
extern	int			NS_DrawBox(          int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color, int FillFlag ) ;								// 四角形を描画する
extern	int			NS_DrawBoxAA(        float x1, float y1, float x2, float y2,                                         unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;	// 四角形を描画する( アンチエイリアス付き )
extern	int			NS_DrawFillBox(      int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color ) ;												// 中身を塗りつぶす四角形を描画する
extern	int			NS_DrawLineBox(      int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color ) ;												// 枠だけの四角形の描画 する
extern	int			NS_DrawCircle(       int   x,  int   y,  int   r,                                                    unsigned int Color, int FillFlag = TRUE, int   LineThickness = 1 ) ;	// 円を描画する
extern	int			NS_DrawCircleAA(     float x,  float y,  float r,            int posnum,                             unsigned int Color, int FillFlag = TRUE, float LineThickness = 1 ) ;	// 円を描画する( アンチエイリアス付き )
extern	int			NS_DrawOval(         int   x,  int   y,  int   rx, int   ry,                                         unsigned int Color, int FillFlag,        int   LineThickness = 1 ) ;	// 楕円を描画する
extern	int			NS_DrawOvalAA(       float x,  float y,  float rx, float ry, int posnum,                             unsigned int Color, int FillFlag,        float LineThickness = 1 ) ;	// 楕円を描画する( アンチエイリアス付き )
extern	int			NS_DrawOval_Rect(    int   x1, int   y1, int   x2, int   y2,                                         unsigned int Color, int FillFlag ) ;										// 指定の矩形に収まる円( 楕円 )を描画する
extern	int			NS_DrawTriangle(     int   x1, int   y1, int   x2, int   y2, int   x3, int   y3,                     unsigned int Color, int FillFlag ) ;									// 三角形を描画する
extern	int			NS_DrawTriangleAA(   float x1, float y1, float x2, float y2, float x3, float y3,                     unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;		// 三角形を描画する( アンチエイリアス付き )
extern	int			NS_DrawQuadrangle(   int   x1, int   y1, int   x2, int   y2, int   x3, int   y3, int   x4, int   y4, unsigned int Color, int FillFlag ) ;									// 四角形を描画する
extern	int			NS_DrawQuadrangleAA( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;		// 四角形を描画する( アンチエイリアス付き )
extern	int			NS_DrawRoundRect(    int   x1, int   y1, int   x2, int   y2, int   rx, int   ry,                     unsigned int Color, int FillFlag ) ;									// 角の丸い四角形を描画する
extern	int			NS_DrawRoundRectAA(  float x1, float y1, float x2, float y2, float rx, float ry, int posnum,         unsigned int Color, int FillFlag, float LineThickness = 1.0f ) ;		// 角の丸い四角形を描画する( アンチエイリアス付き )
extern	int			NS_BeginAADraw(      void ) ;																																					// DrawTriangleAA などのアンチエイリアス付き図形描画の準備を行う
extern	int			NS_EndAADraw(        void ) ;																																					// DrawTriangleAA などのアンチエイリアス付き図形描画の後始末を行う
extern 	int			NS_DrawPixel(        int   x,  int   y,                                                              unsigned int Color ) ;												// 点を描画する

extern	int			NS_Paint(			int x, int y, unsigned int FillColor, ULONGLONG BoundaryColor = ULL_PARAM( 0xffffffffffffffff ) ) ;		// 指定点から境界色があるところまで塗りつぶす(境界色を -1 にすると指定点の色の領域を塗りつぶす)

extern 	int			NS_DrawPixelSet(   const POINTDATA *PointData, int Num ) ;																	// 点の集合を描画する
extern	int			NS_DrawLineSet(    const LINEDATA *LineData,   int Num ) ;																	// 線の集合を描画する

extern	int			NS_DrawPixel3D(     VECTOR   Pos,                                                                 unsigned int Color ) ;						// ３Ｄの点を描画する
extern	int			NS_DrawPixel3DD(    VECTOR_D Pos,                                                                 unsigned int Color ) ;						// ３Ｄの点を描画する
extern	int			NS_DrawLine3D(      VECTOR   Pos1,   VECTOR   Pos2,                                               unsigned int Color ) ;						// ３Ｄの線分を描画する
extern	int			NS_DrawLine3DD(     VECTOR_D Pos1,   VECTOR_D Pos2,                                               unsigned int Color ) ;						// ３Ｄの線分を描画する
extern	int			NS_DrawTriangle3D(  VECTOR   Pos1,   VECTOR   Pos2, VECTOR   Pos3,                                unsigned int Color, int FillFlag ) ;		// ３Ｄの三角形を描画する
extern	int			NS_DrawTriangle3DD( VECTOR_D Pos1,   VECTOR_D Pos2, VECTOR_D Pos3,                                unsigned int Color, int FillFlag ) ;		// ３Ｄの三角形を描画する
extern	int			NS_DrawCube3D(      VECTOR   Pos1,   VECTOR   Pos2,                            unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの立方体を描画する
extern	int			NS_DrawCube3DD(     VECTOR_D Pos1,   VECTOR_D Pos2,                            unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの立方体を描画する
extern	int			NS_DrawSphere3D(    VECTOR   CenterPos,                  float  r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの球体を描画する
extern	int			NS_DrawSphere3DD(   VECTOR_D CenterPos,                  double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの球体を描画する
extern	int			NS_DrawCapsule3D(   VECTOR   Pos1,   VECTOR   Pos2,      float  r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄのカプセルを描画する
extern	int			NS_DrawCapsule3DD(  VECTOR_D Pos1,   VECTOR_D Pos2,      double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄのカプセルを描画する
extern	int			NS_DrawCone3D(      VECTOR   TopPos, VECTOR   BottomPos, float  r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの円錐を描画する
extern	int			NS_DrawCone3DD(     VECTOR_D TopPos, VECTOR_D BottomPos, double r, int DivNum, unsigned int DifColor, unsigned int SpcColor, int FillFlag ) ;		// ３Ｄの円錐を描画する

// 画像描画関数
extern	int			NS_LoadGraphScreen(          int x, int y, const TCHAR *GraphName, int TransFlag ) ;								// ＢＭＰファイルを読みこんで画面に描画する
extern	int			NS_LoadGraphScreenWithStrLen( int x, int y, const TCHAR *GraphName, size_t GraphNameLength, int TransFlag ) ;										// 画像ファイルを読みこんで画面に描画する

extern	int			NS_DrawGraph(                int x, int y,                                                                 int GrHandle, int TransFlag ) ;										// グラフィックの描画
extern	int			NS_DrawExtendGraph(          int x1, int y1, int x2, int y2,                                               int GrHandle, int TransFlag ) ;										// グラフィックの拡大縮小描画
extern	int			NS_DrawRotaGraph(            int x, int y,                 double ExRate,                    double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// グラフィックの回転描画
extern	int			NS_DrawRotaGraph2(           int x, int y, int cx, int cy, double ExtRate,                   double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE  ) ;				// グラフィックの回転描画２
extern	int			NS_DrawRotaGraph3(           int x, int y, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 				// グラフィックの回転描画３
extern	int			NS_DrawRotaGraphFast(        int x, int y,                 float  ExRate,                    float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// 画像の回転描画( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRotaGraphFast2(       int x, int y, int cx, int cy, float  ExtRate,                   float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// 画像の回転描画２( 回転中心指定付き )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRotaGraphFast3(       int x, int y, int cx, int cy, float  ExtRateX, float  ExtRateY, float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 				// 画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawModiGraph(            int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,               int GrHandle, int TransFlag ) ;										// 画像の自由変形描画
extern	int			NS_DrawTurnGraph(            int x, int y,                                                                 int GrHandle, int TransFlag ) ;										// 画像の左右反転描画
extern	int			NS_DrawReverseGraph(         int x, int y,                                                                 int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;		// 画像の反転描画

extern	int			NS_DrawGraphF(               float xf, float yf,                                                                       int GrHandle, int TransFlag ) ;							// グラフィックの描画
extern	int			NS_DrawExtendGraphF(         float x1f, float y1f, float x2f, float y2,                                                int GrHandle, int TransFlag ) ;							// グラフィックの拡大縮小描画
extern	int			NS_DrawRotaGraphF(           float xf, float yf,                       double ExRate,                    double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE  ) ;	// グラフィックの回転描画
extern	int			NS_DrawRotaGraph2F(          float xf, float yf, float cxf, float cyf, double ExtRate,                   double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE  ) ;	// グラフィックの回転描画２
extern	int			NS_DrawRotaGraph3F(          float xf, float yf, float cxf, float cyf, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 	// グラフィックの回転描画３
extern	int			NS_DrawRotaGraphFastF(       float xf, float yf,                       float  ExRate,                    float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// 画像の回転描画( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRotaGraphFast2F(      float xf, float yf, float cxf, float cyf, float  ExtRate,                   float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// 画像の回転描画２( 回転中心指定付き )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRotaGraphFast3F(      float xf, float yf, float cxf, float cyf, float  ExtRateX, float  ExtRateY, float  Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 	// 画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawModiGraphF(           float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,           int GrHandle, int TransFlag ) ;							// 画像の自由変形描画( float 版 )
extern	int			NS_DrawTurnGraphF(           float xf, float yf,                                                                       int GrHandle, int TransFlag ) ;							// 画像の左右反転描画
extern	int			NS_DrawReverseGraphF(        float xf, float yf,                                                                       int GrHandle, int TransFlag, int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;		// 画像の反転描画( 座標指定が float 版 )

extern	int			NS_DrawChipMap(              int Sx, int Sy, int XNum, int YNum, const int *MapData, int ChipTypeNum, int MapDataPitch, const int *ChipGrHandle, int TransFlag ) ;																											// チップグラフィックを使ったマップ描画
extern	int			NS_DrawChipMap(              int MapWidth, int MapHeight,        const int *MapData, int ChipTypeNum,                   const int *ChipGrHandle, int TransFlag, int MapDrawPointX, int MapDrawPointY, int MapDrawWidth, int MapDrawHeight, int ScreenX, int ScreenY ) ;	// チップグラフィックを使ったマップ描画
extern	int			NS_DrawTile(                 int x1, int y1, int x2, int y2, int Tx, int Ty, double ExtRate, double Angle, int GrHandle, int TransFlag ) ;																														// グラフィックを指定領域にタイル状に描画する

extern	int			NS_DrawRectGraph(            int DestX,  int DestY,                          int SrcX, int SrcY, int    Width, int    Height,                         int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// グラフィックの指定矩形部分のみを描画
extern	int			NS_DrawRectExtendGraph(      int DestX1, int DestY1, int DestX2, int DestY2, int SrcX, int SrcY, int SrcWidth, int SrcHeight,                         int GraphHandle, int TransFlag ) ;							// グラフィックの指定矩形部分のみを拡大描画
extern	int			NS_DrawRectRotaGraph(        int x, int y, int SrcX, int SrcY, int Width, int Height, double ExtRate, double Angle,                                   int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 
extern	int			NS_DrawRectRotaGraph2(       int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, double ExtRate,  double Angle,                  int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 
extern	int			NS_DrawRectRotaGraph3(       int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 
extern	int			NS_DrawRectRotaGraphFast(    int x, int y, int SrcX, int SrcY, int Width, int Height, float ExtRate, float Angle,                                   int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRectRotaGraphFast2(   int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, float ExtRate,  float Angle,                  int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画２( 回転中心指定付き )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRectRotaGraphFast3(   int x, int y, int SrcX, int SrcY, int Width, int Height, int cx, int cy, float ExtRateX, float ExtRateY, float Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;				// 画像の指定矩形部分のみを回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRectModiGraph(        int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int SrcX, int SrcY, int Width, int Height,               int GraphHandle, int TransFlag ) ;							// 画像の指定矩形部分のみを自由変形描画

extern	int			NS_DrawRectGraphF(           float DestX,  float DestY,                              int SrcX, int SrcY, int    Width, int    Height,                           int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// グラフィックの指定矩形部分のみを描画
extern	int			NS_DrawRectExtendGraphF(     float DestX1, float DestY1, float DestX2, float DestY2, int SrcX, int SrcY, int SrcWidth, int SrcHeight,                           int GraphHandle, int TransFlag ) ;					// グラフィックの指定矩形部分のみを拡大描画
extern	int			NS_DrawRectRotaGraphF(       float x, float y, int SrcX, int SrcY, int Width, int Height,                       double ExtRate,                   double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 
extern	int			NS_DrawRectRotaGraph2F(      float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, double ExtRate,                   double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 
extern	int			NS_DrawRectRotaGraph3F(      float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, double ExtRateX, double ExtRateY, double Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 
extern	int			NS_DrawRectRotaGraphFastF(   float x, float y, int SrcX, int SrcY, int Width, int Height,                       float ExtRate,                    float  Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRectRotaGraphFast2F(  float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, float ExtRate,                    float  Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画２( 回転中心指定付き )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRectRotaGraphFast3F(  float x, float y, int SrcX, int SrcY, int Width, int Height, float cxf, float cyf, float ExtRateX,  float  ExtRateY, float  Angle, int GraphHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// 画像の指定矩形部分のみを回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 座標指定が float 版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRectModiGraphF(       float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int SrcX, int SrcY, int Width, int Height,         int GraphHandle, int TransFlag ) ;					// 画像の指定矩形部分のみを自由変形描画( 座標指定が float 版 )

extern	int			NS_DrawBlendGraph(           int x, int y, int GrHandle, int TransFlag,                 int BlendGraph, int BorderParam, int BorderRange ) ;									// ブレンド画像と合成して画像を描画する
extern	int			NS_DrawBlendGraphPos(        int x, int y, int GrHandle, int TransFlag, int bx, int by, int BlendGraph, int BorderParam, int BorderRange ) ;									// ブレンド画像と合成して画像を描画する( ブレンド画像の起点座標を指定する版 )

extern	int			NS_DrawCircleGauge(          int CenterX, int CenterY, double Percent, int GrHandle, double StartPercent = 0.0 ) ;																			// 円グラフ的な描画を行う( GrHandle の画像の上下左右の端は透過色にしておく必要があります )

extern	int			NS_DrawGraphToZBuffer(       int X, int Y,                                                                 int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して画像の描画を行う
extern	int			NS_DrawTurnGraphToZBuffer(   int x, int y,                                                                 int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して画像の左右反転描画
extern	int			NS_DrawReverseGraphToZBuffer( int x, int y,                                                                int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE , int ReverseYFlag = FALSE ) ;							// Ｚバッファに対して画像の反転描画
extern	int			NS_DrawExtendGraphToZBuffer( int x1, int y1, int x2, int y2,                                               int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して画像の拡大縮小描画
extern	int			NS_DrawRotaGraphToZBuffer(   int x, int y, double ExRate, double Angle,                                    int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;		// Ｚバッファに対して画像の回転描画
extern	int			NS_DrawRotaGraph2ToZBuffer(  int x, int y, int cx, int cy, double ExtRate,                   double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;		// Ｚバッファに対して画像の回転描画２
extern	int			NS_DrawRotaGraph3ToZBuffer(  int x, int y, int cx, int cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 	// Ｚバッファに対して画像の回転描画３
extern	int			NS_DrawRotaGraphFastToZBuffer(   int x, int y, float ExRate, float Angle,                                   int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の回転描画( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRotaGraphFast2ToZBuffer(  int x, int y, int cx, int cy, float ExtRate,                  float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;	// Ｚバッファに対して画像の回転描画２( 回転中心指定付き )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawRotaGraphFast3ToZBuffer(  int x, int y, int cx, int cy, float ExtRateX, float ExtRateY, float Angle, int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ; 	// Ｚバッファに対して画像の回転描画３( 回転中心指定付き＋縦横拡大率別指定版 )( 高速版、座標計算のアルゴリズムが簡略化されています、描画結果に不都合が無ければこちらの方が高速です )
extern	int			NS_DrawModiGraphToZBuffer(   int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,               int GrHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して画像の自由変形描画
extern	int			NS_DrawBoxToZBuffer(         int x1, int y1, int x2, int y2,                                               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して矩形の描画を行う
extern	int			NS_DrawCircleToZBuffer(      int x, int y, int r,                                                          int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して円の描画を行う
extern	int			NS_DrawTriangleToZBuffer(    int x1, int y1, int x2, int y2, int x3, int y3,                               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して三角形を描画する
extern	int			NS_DrawQuadrangleToZBuffer(  int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して四角形を描画する
extern	int			NS_DrawRoundRectToZBuffer(   int x1, int y1, int x2, int y2, int rx, int ry,                               int FillFlag, int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;							// Ｚバッファに対して角の丸い四角形を描画する

extern	int			NS_DrawPolygon(                             const VERTEX    *Vertex, int PolygonNum,                                                                                                    int GrHandle, int TransFlag, int UVScaling = FALSE ) ;		// ２Ｄポリゴンを描画する( Vertex:三角形を形成する頂点配列の先頭アドレス( 頂点の数はポリゴンの数×３ )  PolygonNum:描画するポリゴンの数  GrHandle:使用するグラフィックハンドル  TransFlag:透過色処理を行うかどうか( TRUE:行う  FALSE:行わない )  UVScaling:基本FALSEでOK )
extern	int			NS_DrawPolygon2D(                           const VERTEX2D  *Vertex, int PolygonNum,                                                                                                    int GrHandle, int TransFlag ) ;							// ２Ｄポリゴンを描画する
extern	int			NS_DrawPolygon3D(                           const VERTEX3D  *Vertex, int PolygonNum,                                                                                                    int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する
extern	int			NS_DrawPolygonIndexed2D(                    const VERTEX2D  *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum,                                                      int GrHandle, int TransFlag ) ;							// ２Ｄポリゴンを描画する( 頂点インデックスを使用 )
extern	int			NS_DrawPolygonIndexed3D(                    const VERTEX3D  *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum,                                                      int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 頂点インデックスを使用 )
extern	int			NS_DrawPolygonIndexed3DBase(                const VERTEX_3D *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum,   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 頂点インデックスを使用 )( 旧バージョン用 )
extern	int			NS_DrawPolygon3DBase(                       const VERTEX_3D *Vertex, int VertexNum,                                                int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 旧バージョン用 )
extern	int			NS_DrawPolygon3D(                           const VERTEX_3D *Vertex, int PolygonNum,                                                                                                    int GrHandle, int TransFlag ) ;							// ３Ｄポリゴンを描画する( 旧バージョン用 )

extern	int			NS_DrawPolygonBase(                         const VERTEX    *Vertex, int VertexNum,                                                int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag, int UVScaling = FALSE ) ;		// ２Ｄプリミティブを描画する
extern	int			NS_DrawPrimitive2D(                         const VERTEX2D  *Vertex, int VertexNum,                                                int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ２Ｄプリミティブを描画する
extern	int			NS_DrawPrimitive3D(                         const VERTEX3D  *Vertex, int VertexNum,                                                int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄプリミティブを描画する
extern	int			NS_DrawPrimitiveIndexed2D(                  const VERTEX2D  *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum,   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ２Ｄプリミティブを描画する(頂点インデックス使用)
extern	int			NS_DrawPrimitiveIndexed3D(                  const VERTEX3D  *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum,   int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int GrHandle, int TransFlag ) ;							// ３Ｄプリミティブを描画する(頂点インデックス使用)

extern	int			NS_DrawPolygon3D_UseVertexBuffer(           int VertexBufHandle,                                                                                                                                                               int GrHandle, int TransFlag ) ;		// 頂点バッファを使用して３Ｄポリゴンを描画する
extern	int			NS_DrawPrimitive3D_UseVertexBuffer(         int VertexBufHandle,                     int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */,                                                                                     int GrHandle, int TransFlag ) ;		// 頂点バッファを使用して３Ｄプリミティブを描画する
extern	int			NS_DrawPrimitive3D_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */,                 int StartVertex, int UseVertexNum,                                  int GrHandle, int TransFlag ) ;		// 頂点バッファを使用して３Ｄプリミティブを描画する
extern	int			NS_DrawPolygonIndexed3D_UseVertexBuffer(    int VertexBufHandle, int IndexBufHandle,                                                                                                                                           int GrHandle, int TransFlag ) ;		// 頂点バッファとインデックスバッファを使用して３Ｄポリゴンを描画する
extern	int			NS_DrawPrimitiveIndexed3D_UseVertexBuffer(  int VertexBufHandle, int IndexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */,                                                                                     int GrHandle, int TransFlag ) ;		// 頂点バッファとインデックスバッファを使用して３Ｄプリミティブを描画する
extern	int			NS_DrawPrimitiveIndexed3D_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType  /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum, int GrHandle, int TransFlag ) ;		// 頂点バッファとインデックスバッファを使用して３Ｄプリミティブを描画する

extern	int			NS_DrawGraph3D(                             float x, float y, float z,                                                                     int GrHandle, int TransFlag ) ;								// グラフィックの３Ｄ描画
extern	int			NS_DrawExtendGraph3D(                       float x, float y, float z, double ExRateX, double ExRateY,                                     int GrHandle, int TransFlag ) ;								// グラフィックの拡大３Ｄ描画
extern	int			NS_DrawRotaGraph3D(                         float x, float y, float z, double ExRate, double Angle,                                        int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;		// グラフィックの回転３Ｄ描画
extern	int			NS_DrawRota2Graph3D(                        float x, float y, float z, float cx, float cy, double ExtRateX, double ExtRateY, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;		// グラフィックの回転３Ｄ描画(回転中心指定型)
extern	int			NS_DrawModiBillboard3D(                     VECTOR Pos, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,    int GrHandle, int TransFlag ) ;								// グラフィックの自由変形３Ｄ描画
extern	int			NS_DrawBillboard3D(                         VECTOR Pos, float cx, float cy, float Size, float Angle,                                       int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;		// ３Ｄ空間上にグラフィックを描画


// 描画設定関係関数
extern	int			NS_SetDrawMode(						int DrawMode ) ;												// 描画モードをセットする
extern	int			NS_GetDrawMode(						void ) ;														// 描画モードを取得する
extern	int			NS_SetDrawBlendMode(					int BlendMode, int BlendParam ) ;								// 描画ブレンドモードをセットする
extern	int			NS_GetDrawBlendMode(					int *BlendMode, int *BlendParam ) ;								// 描画ブレンドモードを取得する
extern	int			NS_SetDrawAlphaTest(					int TestMode, int TestParam ) ;									// 描画時のアルファテストの設定を行う( TestMode:DX_CMP_GREATER等( -1:デフォルト動作に戻す )  TestParam:描画アルファ値との比較に使用する値 )
extern	int			NS_SetBlendGraph(						int BlendGraph, int BorderParam, int BorderRange ) ;			// ( SetBlendGraphParam の BlendType = DX_BLENDGRAPHTYPE_WIPE の処理を行う旧関数 )描画処理時に描画する画像とブレンドするαチャンネル付き画像をセットする( BlendGraph を -1 でブレンド機能を無効 )
extern	int			NS_SetBlendGraphParam(					int BlendGraph, int BlendType, ... ) ;							// 描画処理時に描画する画像とブレンドする画像のブレンド設定を行う、BlendGraph を -1 にすれば設定を解除、その場合 BlendType とその後ろのパラメータは無視される
//		int			NS_SetBlendGraphParam(					int BlendGraph, int BlendType = DX_BLENDGRAPHTYPE_NORMAL, int Ratio = ( 0( ブレンド率０％ )〜255( ブレンド率１００％ ) ) ) ;
//		int			NS_SetBlendGraphParam(					int BlendGraph, int BlendType = DX_BLENDGRAPHTYPE_WIPE, int BorderParam, int BorderRange ) ;
extern	int			NS_SetBlendGraphPosition(				int x, int y ) ;												// ブレンド画像の起点座標をセットする
extern	int			NS_SetBlendGraphPositionMode(			int BlendGraphPositionMode /* DX_BLENDGRAPH_POSMODE_DRAWGRAPH など */ ) ;	// ブレンド画像の適応座標モードを設定する
extern	int			NS_SetDrawBright(						int RedBright, int GreenBright, int BlueBright ) ;				// 描画輝度をセット
extern	int			NS_GetDrawBright(						int *Red, int *Green, int *Blue ) ;								// 描画輝度を取得する
extern	int			NS_SetIgnoreDrawGraphColor(			int EnableFlag ) ;												// 描画する画像の色成分を無視するかどうかを指定する( EnableFlag:この機能を使うかどうか( TRUE:使う  FALSE:使わない ) )
extern	int			NS_SetMaxAnisotropy(					int MaxAnisotropy ) ;											// 最大異方性の値をセットする
extern	int			NS_SetUseLarge3DPositionSupport(		int UseFlag ) ;													// ３Ｄ処理で使用する座標値が 10000000.0f などの大きな値になっても描画の崩れを小さく抑える処理を使用するかどうかを設定する、DxLib_Init の呼び出し前でのみ使用可能( TRUE:描画の崩れを抑える処理を使用する( CPU負荷が上がります )　　FALSE:描画の崩れを抑える処理は使用しない( デフォルト ) )

extern	int			NS_SetUseZBufferFlag(					int Flag ) ;													// Ｚバッファを有効にするか、フラグをセットする( ２Ｄと３Ｄ描画に影響 )
extern	int			NS_SetWriteZBufferFlag(				int Flag ) ;													// Ｚバッファに書き込みを行うか、フラグをセットする( ２Ｄと３Ｄ描画に影響 )
extern	int			NS_SetZBufferCmpType(					int CmpType /* DX_CMP_NEVER 等 */ ) ;							// Ｚ値の比較モードをセットする( ２Ｄと３Ｄ描画に影響 )
extern	int			NS_SetZBias(							int Bias ) ;													// Ｚバイアスをセットする( ２Ｄと３Ｄ描画に影響 )
extern	int			NS_SetUseZBuffer3D(					int Flag ) ;													// Ｚバッファを有効にするか、フラグをセットする( ３Ｄ図形描画のみに影響 )
extern	int			NS_SetWriteZBuffer3D(					int Flag ) ;													// Ｚバッファに書き込みを行うか、フラグをセットする( ３Ｄ図形描画のみに影響 )
extern	int			NS_SetZBufferCmpType3D(				int CmpType /* DX_CMP_NEVER 等 */ ) ;							// Ｚ値の比較モードをセットする( ３Ｄ図形描画のみに影響 )
extern	int			NS_SetZBias3D(							int Bias ) ;													// Ｚバイアスをセットする( ３Ｄ図形描画のみに影響 )
extern	int			NS_SetDrawZ(							float Z ) ;														// ２Ｄ描画時にＺバッファに書き込むＺ値を変更する

extern	int			NS_SetDrawArea(						int x1, int y1, int x2, int y2 ) ;								// 描画可能領域のセット
extern	int			NS_GetDrawArea(						RECT *Rect ) ;													// 描画可能領域を得る
extern	int			NS_SetDraw3DScale(						float Scale ) ;													// ３Ｄ描画のスケールをセットする
extern	int			NS_SetDrawAreaFull(					void ) ;														// 描画可能領域を描画対象画面全体にする

extern	int			NS_SetRestoreShredPoint(				void (* ShredPoint )( void ) ) ;								// SetRestoreGraphCallback の旧名
extern	int			NS_SetRestoreGraphCallback(			void (* Callback )( void ) ) ;									// グラフィックハンドル復元関数の登録
extern	int			NS_RunRestoreShred(					void ) ;														// グラフィック復元関数の実行
extern	int			NS_SetGraphicsDeviceRestoreCallbackFunction( void (* Callback )( void *Data ), void *CallbackData ) ;	// グラフィックスデバイスがロストから復帰した際に呼ばれるコールバック関数を設定する
extern	int			NS_SetGraphicsDeviceLostCallbackFunction(    void (* Callback )( void *Data ), void *CallbackData ) ;	// グラフィックスデバイスがロストから復帰する前に呼ばれるコールバック関数を設定する

extern	int			NS_SetTransformTo2D(					const MATRIX   *Matrix ) ;										// ２Ｄ描画に使用される変換行列を設定する( 使用されるのは3行2列のみ )
extern	int			NS_SetTransformTo2DD(					const MATRIX_D *Matrix ) ;										// ２Ｄ描画に使用される変換行列を設定する( 使用されるのは3行2列のみ )
extern	int			NS_ResetTransformTo2D(					void ) ;														// ２Ｄ描画用に使用する変換行列の設定を初期状態に戻す
extern	int			NS_SetTransformToWorld(					const MATRIX   *Matrix ) ;										// ローカル座標からワールド座標に変換するための行列を設定する
extern	int			NS_SetTransformToWorldD(				const MATRIX_D *Matrix ) ;										// ローカル座標からワールド座標に変換するための行列を設定する
extern	int			NS_GetTransformToWorldMatrix(			      MATRIX   *MatBuf ) ;										// ローカル座標からワールド座標に変換するための行列を取得する
extern	int			NS_GetTransformToWorldMatrixD(			      MATRIX_D *MatBuf ) ;										// ローカル座標からワールド座標に変換するための行列を取得する
extern	int			NS_SetTransformToView(					const MATRIX   *Matrix ) ;										// ワールド座標からビュー座標に変換するための行列を設定する
extern	int			NS_SetTransformToViewD(					const MATRIX_D *Matrix ) ;										// ワールド座標からビュー座標に変換するための行列を設定する
extern	int			NS_GetTransformToViewMatrix(			      MATRIX   *MatBuf ) ;										// ワールド座標からビュー座標に変換するための行列を取得する
extern	int			NS_GetTransformToViewMatrixD(			      MATRIX_D *MatBuf ) ;										// ワールド座標からビュー座標に変換するための行列を取得する
extern	int			NS_SetTransformToProjection(			const MATRIX   *Matrix ) ;										// ビュー座標からプロジェクション座標に変換するための行列を設定する
extern	int			NS_SetTransformToProjectionD(			const MATRIX_D *Matrix ) ;										// ビュー座標からプロジェクション座標に変換するための行列を設定する
extern	int			NS_GetTransformToProjectionMatrix(		      MATRIX   *MatBuf ) ;										// ビュー座標からプロジェクション座標に変換するための行列を取得する
extern	int			NS_GetTransformToProjectionMatrixD(			  MATRIX_D *MatBuf ) ;										// ビュー座標からプロジェクション座標に変換するための行列を取得する
extern	int			NS_SetTransformToViewport(				const MATRIX   *Matrix ) ;										// ビューポート行列を設定する
extern	int			NS_SetTransformToViewportD(				const MATRIX_D *Matrix ) ;										// ビューポート行列を設定する
extern	int			NS_GetTransformToViewportMatrix(	    MATRIX   *MatBuf ) ;											// ビューポート行列を取得する
extern	int			NS_GetTransformToViewportMatrixD(	    MATRIX_D *MatBuf ) ;											// ビューポート行列を取得する
extern	int			NS_GetTransformToAPIViewportMatrix(		MATRIX   *MatBuf ) ;											// Direct3Dで自動適用されるビューポート行列を取得する
extern	int			NS_GetTransformToAPIViewportMatrixD(	MATRIX_D *MatBuf ) ;											// Direct3Dで自動適用されるビューポート行列を取得する
extern	int			NS_SetDefTransformMatrix(				void ) ;														// デフォルトの変換行列を設定する
extern	int			NS_GetTransformPosition(				VECTOR   *LocalPos, float  *x, float  *y ) ;					// ローカル座標からスクリーン座標を取得する
extern	int			NS_GetTransformPositionD(				VECTOR_D *LocalPos, double *x, double *y ) ;					// ローカル座標からスクリーン座標を取得する
extern	float		NS_GetBillboardPixelSize(				VECTOR   WorldPos, float  WorldSize ) ;							// ワールド空間上のビルボードのサイズからスクリーンに投影した場合のピクセル単位のサイズを取得する
extern	double		NS_GetBillboardPixelSizeD(				VECTOR_D WorldPos, double WorldSize ) ;							// ワールド空間上のビルボードのサイズからスクリーンに投影した場合のピクセル単位のサイズを取得する
extern	VECTOR		NS_ConvWorldPosToViewPos(				VECTOR   WorldPos ) ;											// ワールド座標をビュー座標に変換する
extern	VECTOR_D	NS_ConvWorldPosToViewPosD(				VECTOR_D WorldPos ) ;											// ワールド座標をビュー座標に変換する
extern	VECTOR		NS_ConvWorldPosToScreenPos(				VECTOR   WorldPos ) ;											// ワールド座標をスクリーン座標に変換する
extern	VECTOR_D	NS_ConvWorldPosToScreenPosD(			VECTOR_D WorldPos ) ;											// ワールド座標をスクリーン座標に変換する
extern	FLOAT4		NS_ConvWorldPosToScreenPosPlusW(		VECTOR   WorldPos ) ;											// ワールド座標をスクリーン座標に変換する、最後のＸＹＺ座標をＷで割る前の値を得る
extern	DOUBLE4		NS_ConvWorldPosToScreenPosPlusWD(		VECTOR_D WorldPos ) ;											// ワールド座標をスクリーン座標に変換する、最後のＸＹＺ座標をＷで割る前の値を得る
extern	VECTOR		NS_ConvScreenPosToWorldPos(				VECTOR   ScreenPos ) ;											// スクリーン座標をワールド座標に変換する
extern	VECTOR_D	NS_ConvScreenPosToWorldPosD(			VECTOR_D ScreenPos ) ;											// スクリーン座標をワールド座標に変換する
extern	VECTOR		NS_ConvScreenPosToWorldPos_ZLinear(		VECTOR   ScreenPos ) ;											// スクリーン座標をワールド座標に変換する( Z座標が線形 )
extern	VECTOR_D	NS_ConvScreenPosToWorldPos_ZLinearD(	VECTOR_D ScreenPos ) ;											// スクリーン座標をワールド座標に変換する( Z座標が線形 )

extern	int			NS_SetUseCullingFlag(					int Flag ) ;													// SetUseBackCulling の旧名称
extern	int			NS_SetUseBackCulling(					int Flag /* DX_CULLING_LEFT 等 */ ) ;							// ポリゴンカリングの有効、無効をセットする
extern	int			NS_GetUseBackCulling(					void ) ;														// ポリゴンカリングモードを取得する

extern	int			NS_SetTextureAddressMode(				int Mode /* DX_TEXADDRESS_WRAP 等 */ , int Stage = -1 ) ;		// テクスチャアドレスモードを設定する
extern	int			NS_SetTextureAddressModeUV(			int ModeU, int ModeV, int Stage = -1 ) ;						// テクスチャアドレスモードを設定する
extern	int			NS_SetTextureAddressTransform(			float TransU, float TransV, float ScaleU, float ScaleV, float RotCenterU, float RotCenterV, float Rotate ) ;	// テクスチャ座標変換パラメータをセットする
extern	int			NS_SetTextureAddressTransformMatrix(	MATRIX Matrix ) ;												// テクスチャ座標変換行列をセットする
extern	int			NS_ResetTextureAddressTransform(		void ) ;														// テクスチャ座標変換パラメータをリセットする

extern	int			NS_SetFogEnable(						int Flag ) ;													// フォグを有効にするかどうかを設定する( TRUE:有効  FALSE:無効 )
extern	int			NS_GetFogEnable(						void ) ;														// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
extern	int			NS_SetFogMode(							int Mode /* DX_FOGMODE_NONE 等 */ ) ;							// フォグモードを設定する
extern	int			NS_GetFogMode(							void ) ;														// フォグモードを取得する
extern	int			NS_SetFogColor(						int  r, int  g, int  b ) ;										// フォグカラーを設定する
extern	int			NS_GetFogColor(						int *r, int *g, int *b ) ;										// フォグカラーを取得する
extern	int			NS_SetFogStartEnd(						float  start, float  end ) ;									// フォグが始まる距離と終了する距離を設定する( 0.0f 〜 1.0f )
extern	int			NS_GetFogStartEnd(						float *start, float *end ) ;									// フォグが始まる距離と終了する距離を取得する( 0.0f 〜 1.0f )
extern	int			NS_SetFogDensity(						float density ) ;												// フォグの密度を設定する( 0.0f 〜 1.0f )
extern	float		NS_GetFogDensity(						void ) ;														// フォグの密度を取得する( 0.0f 〜 1.0f )


// 画面関係関数
extern	unsigned int	NS_GetPixel(									int x, int y ) ;																// 指定座標の色を取得する
extern	COLOR_F			NS_GetPixelF(									int x, int y ) ;																// 指定座標の色を取得する( float型 )
extern	int				NS_SetBackgroundColor(						int Red, int Green, int Blue ) ;													// メインウインドウのバックグラウンドカラーを設定する
extern	int				NS_GetBackgroundColor(							int *Red, int *Green, int *Blue ) ;												// メインウインドウの背景色を取得する( Red,Green,Blue:それぞれ ０〜２５５ )
extern	int				NS_GetDrawScreenGraph(							                             int x1, int y1, int x2, int y2,                       int GrHandle, int UseClientFlag = TRUE ) ;	// 描画先の画面から指定領域の画像情報をグラフィックハンドルに転送する
extern	int				NS_BltDrawValidGraph(							int TargetDrawValidGrHandle, int x1, int y1, int x2, int y2, int DestX, int DestY, int DestGrHandle ) ;							// 描画可能グラフィックハンドルから指定領域の画像情報をグラフィックハンドルに転送する
extern	int				NS_ScreenFlip(									void ) ;																		// 裏画面と表画面の内容を交換する
extern 	int				NS_ScreenCopy(									void ) ;																		// 裏画面の内容を表画面に転送する
extern	int				NS_WaitVSync(									int SyncNum ) ;																	// 垂直同期信号を待つ
extern	int				NS_ClearDrawScreen(					        const RECT *ClearRect = NULL ) ;														// 画面の状態を初期化する
extern	int				NS_ClearDrawScreenZBuffer(						const RECT *ClearRect = NULL ) ;														// 画面のＺバッファの状態を初期化する
extern	int				NS_ClsDrawScreen(								void ) ;																		// 画面の状態を初期化する(ClearDrawScreenの旧名称)
extern	int				NS_SetDrawScreen(								int DrawScreen ) ;																// 描画先画面のセット
extern	int				NS_GetDrawScreen(								void ) ;																		// 描画先画面の取得
extern	int				NS_GetActiveGraph(								void ) ;																		// GetDrawScreen の旧名称
extern	int				NS_SetUseSetDrawScreenSettingReset(			int UseFlag ) ;																	// SetDrawScreen を実行した際にカメラや描画範囲の設定をリセットするかを設定する( UseFlag  TRUE:リセットする( デフォルト )  FALSE:リセットしない )
extern	int				NS_GetUseSetDrawScreenSettingReset(			void ) ;																		// SetDrawScreen を実行した際にカメラや描画範囲の設定をリセットするかを取得する
extern	int				NS_SetDrawZBuffer(								int DrawScreen ) ;																// 描画先Ｚバッファのセット( DrawScreen 付属のＺバッファを描画先Ｚバッファにする、DrawScreen を -1 にするとデフォルトの描画先Ｚバッファに戻る )
#ifdef __WINDOWS__
extern	int				NS_BltBackScreenToWindow(						HWND Window, int ClientX, int ClientY ) ;										// 裏画面の内容を指定のウインドウに転送する
extern	int				NS_BltRectBackScreenToWindow(					HWND Window, RECT BackScreenRect, RECT WindowClientRect ) ;						// 裏画面の指定の領域をウインドウのクライアント領域の指定の領域に転送する
extern	int				NS_SetScreenFlipTargetWindow(					HWND TargetWindow, double ScaleX, double ScaleY ) ;								// ScreenFlip で画像を転送する先のウインドウを設定する( NULL を指定すると設定解除 )
extern	int				NS_GetDesktopScreenGraph(			int x1, int y1, int x2, int y2, int GrHandle, int DestX = 0 , int DestY = 0 ) ;				// デスクトップ画面から指定領域の画像情報をグラフィックハンドルに転送する
#endif // __WINDOWS__
extern	int				NS_SetGraphMode(								int ScreenSizeX, int ScreenSizeY, int ColorBitDepth, int RefreshRate = 60 ) ;	// 画面モードを設定する
extern	int				NS_SetFullScreenResolutionMode(					int ResolutionMode /* DX_FSRESOLUTIONMODE_NATIVE 等 */ ) ;						// フルスクリーン解像度モードを設定する
extern	int				NS_GetFullScreenResolutionMode(				int *ResolutionMode, int *UseResolutionMode ) ;									// フルスクリーン解像度モードを取得する( UseResolutionMode は実際に使用されている解像度モード( 例えば DX_FSRESOLUTIONMODE_NATIVE を指定していてもモニタが指定の解像度に対応していない場合は UseResolutionMode が DX_FSRESOLUTIONMODE_DESKTOP や DX_FSRESOLUTIONMODE_MAXIMUM になります ) )
extern	int				NS_SetFullScreenScalingMode(					int ScalingMode /* DX_FSSCALINGMODE_NEAREST 等 */ , int FitScaling = FALSE ) ;	// フルスクリーンモード時の画面拡大モードを設定する
extern	int				NS_SetEmulation320x240(						int Flag ) ;																	// ６４０ｘ４８０の画面で３２０ｘ２４０の画面解像度にするかどうかのフラグをセットする、６４０ｘ４８０以外の解像度では無効( TRUE:有効  FALSE:無効 )
extern	int				NS_SetZBufferSize(								int ZBufferSizeX, int ZBufferSizeY ) ;											// 画面用のＺバッファのサイズを設定する
extern	int				NS_SetZBufferBitDepth(							int BitDepth ) ;																// 画面用のＺバッファのビット深度を設定する( 16 or 24 or 32 )
extern	int				NS_SetWaitVSyncFlag(							int Flag ) ;																	// ＶＳＹＮＣ待ちをするかのフラグセット
extern	int				NS_GetWaitVSyncFlag(							void ) ;																		// ＶＳＹＮＣ待ちをする設定になっているかどうかを取得する
extern	int				NS_SetFullSceneAntiAliasingMode(				int Samples, int Quality ) ;													// 画面のフルスクリーンアンチエイリアスモードの設定をする( DxLib_Init の前でのみ使用可能 )
extern	int				NS_SetGraphDisplayArea(						int x1, int y1, int x2, int y2 ) ;												// ScreenFlip 時に表画面全体に転送する裏画面の領域を設定する( DxLib_Init の前でのみ使用可能 )
extern	int				NS_SetChangeScreenModeGraphicsSystemResetFlag(	int Flag ) ;																	// 画面モード変更時( とウインドウモード変更時 )にグラフィックスシステムの設定やグラフィックハンドルをリセットするかどうかを設定する( TRUE:リセットする  FALSE:リセットしない )
extern	int				NS_GetScreenState(								int *SizeX, int *SizeY, int *ColorBitDepth ) ;									// 現在の画面の大きさとカラービット数を得る 
extern	int				NS_GetDrawScreenSize(							int *XBuf, int *YBuf ) ;														// 描画サイズを取得する
extern	int				NS_GetScreenBitDepth(							void ) ;																		// 使用色ビット数を返す
extern	int				NS_GetColorBitDepth(							void ) ;																		// 画面の色ビット深度を得る
extern	int				NS_GetChangeDisplayFlag(						void ) ;																		// 画面モードが変更されているかどうかのフラグを取得する
extern	int				NS_GetVideoMemorySize(							int *AllSize, int *FreeSize ) ;													// ビデオメモリの容量を得る
extern	int				NS_GetRefreshRate(								void ) ;																		// 現在の画面のリフレッシュレートを取得する
extern	int				NS_GetDisplayNum(								void ) ;																		// ディスプレイの数を取得
extern	int				NS_GetDisplayInfo(								int DisplayIndex, int *DesktopRectX, int *DesktopRectY, int *DesktopSizeX, int *DesktopSizeY, int *IsPrimary ) ;	// ディスプレイのデスクトップ上での矩形位置を取得する
extern	int				NS_GetDisplayModeNum(							int DisplayIndex = 0 ) ;														// 変更可能なディスプレイモードの数を取得する
extern	DISPLAYMODEDATA	NS_GetDisplayMode(								int ModeIndex, int DisplayIndex = 0 ) ;											// 変更可能なディスプレイモードの情報を取得する( ModeIndex は 0 〜 GetDisplayModeNum の戻り値-1 )
extern	int				NS_GetDisplayMaxResolution(					int *SizeX, int *SizeY, int DisplayIndex = 0 ) ;								// ディスプレイの最大解像度を取得する
extern	const COLORDATA* NS_GetDispColorData(							void ) ;																		// ディスプレーのカラーデータポインタを得る
extern	int				NS_GetMultiDrawScreenNum(						void ) ;																		// 同時に描画を行うことができる画面の数を取得する
extern	int				NS_GetDrawFloatCoordType(						void ) ;																		// DrawGraphF 等の浮動小数点値で座標を指定する関数における座標タイプを取得する( 戻り値 : DX_DRAWFLOATCOORDTYPE_DIRECT3D9 など )

// その他設定関係関数
extern	int			NS_SetUseNormalDrawShader(						int Flag ) ;									// 通常描画にプログラマブルシェーダーを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			NS_SetUseSoftwareRenderModeFlag(				int Flag ) ;									// ソフトウエアレンダリングモードを使用するかどうかをセットする
extern	int			NS_SetNotUse3DFlag(							int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )３Ｄ機能を使わないフラグのセット、効果は SetUse3DFlag と同じ
extern	int			NS_SetUse3DFlag(								int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )３Ｄ機能を使うか、のフラグをセット
extern	int			NS_GetUse3DFlag(								void ) ;										// 描画に３Ｄ機能を使うかフラグを取得
extern	int			NS_SetScreenMemToVramFlag(						int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )画面データをＶＲＡＭに置くか、フラグ
extern	int			NS_GetScreenMemToSystemMemFlag(				void ) ;										// 画面グラフィックデータがシステムメモリに存在するかフラグの取得

extern	int			NS_SetUseBasicGraphDraw3DDeviceMethodFlag(		int Flag ) ;									// 単純図形の描画に３Ｄデバイスの機能を使用するかどうかのフラグをセットする
extern	int			NS_SetUseDisplayIndex(							int Index ) ;									// ＤＸライブラリのウインドウを表示するディスプレイデバイスを設定する( -1 を指定するとマウスカーソルがあるディスプレイデバイスにＤＸライブラリのウインドウを表示する )
extern	int			NS_SetWindowDrawRect(							const RECT *DrawRect ) ;								// 通常使用しない
extern	int			NS_RestoreGraphSystem(							void ) ;										// ＤＸライブラリのグラフィック関連の復帰処理を行う
extern	int			NS_SetUseHardwareVertexProcessing(				int Flag ) ;									// ハードウエアの頂点演算処理機能を使用するかどうかを設定する
extern	int			NS_SetUsePixelLighting(						int Flag ) ;									// ピクセル単位でライティングを行うかどうかを設定する、要 ShaderModel 3.0( TRUE:ピクセル単位のライティングを行う  FALSE:頂点単位のライティングを行う( デフォルト ) )
extern	int			NS_SetUseOldDrawModiGraphCodeFlag(				int Flag ) ;									// 以前の DrawModiGraph 関数のコードを使用するかどうかのフラグをセットする
extern	int			NS_SetUseVramFlag(								int Flag ) ;									// ( 現在効果なし )ＶＲＡＭを使用するかのフラグをセットする
extern	int			NS_GetUseVramFlag(								void ) ;										// ( 現在効果なし )２Ｄグラフィックサーフェス作成時にシステムメモリーを使用するかのフラグ取得
extern	int			NS_SetBasicBlendFlag(							int Flag ) ;									// ( 現在効果なし )簡略化ブレンド処理を行うか否かのフラグをセットする
#ifdef __WINDOWS__
extern	int			NS_SetMultiThreadFlag(							int Flag ) ;									// DirectDraw や Direct3D の協調レベルをマルチスレッド対応にするかどうかをセットする
extern	int			NS_SetUseDirectDrawDeviceIndex(				int Index ) ;									// 使用する DirectDraw デバイスのインデックスを設定する
extern	int			NS_SetAeroDisableFlag(							int Flag ) ;									// Vista以降の Windows Aero を無効にするかどうかをセットする、TRUE:無効にする  FALSE:有効にする( DxLib_Init の前に呼ぶ必要があります )
extern	int			NS_SetUseDirect3D9Ex(							int Flag ) ;									// Vista以降の環境で Direct3D9Ex を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )( DxLib_Init の前に呼ぶ必要があります )
extern	int			NS_SetUseDirect3D11(							int Flag ) ;									// Direct3D11 を使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない )
extern	int			NS_SetUseDirect3D11MinFeatureLevel(			int Level /* DX_DIRECT3D_11_FEATURE_LEVEL_10_0 など */ ) ;		// Direct3D11 で使用する最低機能レベルを指定する関数です、尚、DX_DIRECT3D_11_FEATURE_LEVEL_11_0 より低い機能レベルでの正常な動作は保証しません( デフォルトは DX_DIRECT3D_11_FEATURE_LEVEL_11_0 )
extern	int			NS_SetUseDirect3D11WARPDriver(						int Flag ) ;									// D3D_DRIVER_TYPE_WARP タイプの Direct3D 11 ドライバを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern	int			NS_SetUseDirect3DVersion(						int Version /* DX_DIRECT3D_9 など */ ) ;		// 使用する Direct3D のバージョンを設定する、DxLib_Init 呼び出しの前でのみ使用可能　
extern	int			NS_GetUseDirect3DVersion(						void ) ;										// 使用している Direct3D のバージョンを取得する( DX_DIRECT3D_9 など )
extern	int			NS_GetUseDirect3D11FeatureLevel(				void ) ;										// 使用している Direct3D11 の FeatureLevel ( DX_DIRECT3D_11_FEATURE_LEVEL_9_1 等 )を取得する( 戻り値　-1：エラー　-1以外：Feature Level )
extern	int			NS_SetUseDirectDrawFlag(						int Flag ) ;									// ( 同効果のSetUseSoftwareRenderModeFlag を使用して下さい )ＤｉｒｅｃｔＤｒａｗを使用するかどうかをセットする
extern	int			NS_SetUseGDIFlag(								int Flag ) ;									// ＧＤＩ描画を必要とするか、を変更する
extern	int			NS_GetUseGDIFlag(								void ) ;										// ＧＤＩ描画を必要とするかの状態を取得する
extern	int			NS_SetDDrawUseGuid(							const GUID *Guid ) ;									// ＤｉｒｅｃｔＤｒａｗが使用するＧＵＩＤを設定する
extern	const void*		NS_GetUseDDrawObj(								void ) ;										// 現在使用しているＤｉｒｅｃｔＤｒａｗオブジェクトのアドレスを取得する( 戻り値を IDirectDraw7 * にキャストして下さい )
extern	const GUID*		NS_GetDirectDrawDeviceGUID(					int Number ) ;									// 有効な DirectDraw デバイスの GUID を取得する
extern	int			NS_GetDirectDrawDeviceDescription(				int Number, char *StringBuffer ) ;				// 有効な DirectDraw デバイスの名前を得る
extern	int			NS_GetDirectDrawDeviceNum(						void ) ;										// 有効な DirectDraw デバイスの数を取得する
extern	const void* NS_GetUseDirect3DDevice9(					void ) ;										// 使用中のＤｉｒｅｃｔ３ＤＤｅｖｉｃｅ９オブジェクトを得る
extern	const void* NS_GetUseDirect3D9BackBufferSurface(		void ) ;										// 使用中のバックバッファのDirect3DSurface9オブジェクトを取得する
extern	const void*	NS_GetUseDirect3D11Device(							void ) ;										// 使用中のID3D11Deviceオブジェクトを取得する( 戻り値を ID3D11Device * にキャストして下さい )
extern	const void*	NS_GetUseDirect3D11DeviceContext(					void ) ;										// 使用中のID3D11DeviceContextオブジェクトを取得する( 戻り値を ID3D11DeviceContext * にキャストして下さい )
extern	const void*	NS_GetUseDirect3D11BackBufferTexture2D(			void ) ;										// 使用中のバックバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	const void*	NS_GetUseDirect3D11BackBufferRenderTargetView(		void ) ;										// 使用中のバックバッファのID3D11RenderTargetViewオブジェクトを取得する( 戻り値を ID3D11RenderTargetView * にキャストしてください )
extern	const void*	NS_GetUseDirect3D11DepthStencilTexture2D(			void ) ;										// 使用中の深度ステンシルバッファのID3D11Texture2Dオブジェクトを取得する( 戻り値を ID3D11Texture2D * にキャストしてください )
extern	int			NS_SetDrawScreen_ID3D11RenderTargetView(			const void *pID3D11RenderTargetView, const void *pID3D11DepthStencilView = NULL ) ;		// 指定の ID3D11RenderTargetView を描画対象にする( pID3D11DepthStencilView が NULL の場合はデフォルトの深度ステンシルバッファを使用する )
extern	int			NS_RefreshDxLibDirect3DSetting(				void ) ;										// ＤＸライブラリのＤｉｒｅｃｔ３Ｄ設定をしなおす( 特殊用途 )
#ifndef DX_NON_MEDIA_FOUNDATION
extern	int			NS_SetUseMediaFoundationFlag(						int Flag ) ;									// Media Foundation を使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
#endif // DX_NON_MEDIA_FOUNDATION
#endif // __WINDOWS__
extern	int			NS_RenderVertex(								void ) ;										// 頂点バッファに溜まった頂点データを吐き出す( 特殊用途 )

// 描画パフォーマンス関係関数
extern	int			NS_GetDrawCallCount(							void ) ;										// 前々回の ScreenFlip 呼び出しから、前回の ScreenFlip 呼び出しまでの間に行われた描画コールの回数を取得する
extern	float		NS_GetFPS(										void ) ;										// フレームレート( １秒間に呼ばれる ScreenFlip の回数 )を取得する

#ifndef DX_NON_SAVEFUNCTION

// 描画先画面保存関数
// Jpeg_Quality         = 0:低画質〜100:高画質
// Png_CompressionLevel = 0:無圧縮〜  9:最高圧縮
extern	int			NS_SaveDrawScreen(       int x1, int y1, int x2, int y2, const TCHAR *FileName, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 現在描画対象になっている画面を保存する
extern	int			NS_SaveDrawScreenWithStrLen(       int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 現在描画対象になっている画面をファイルで保存する
extern	int			NS_SaveDrawScreenToBMP(            int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																	// 現在描画対象になっている画面をＢＭＰ形式で保存する
extern	int			NS_SaveDrawScreenToBMPWithStrLen(  int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																	// 現在描画対象になっている画面をＢＭＰ形式で保存する
extern	int			NS_SaveDrawScreenToDDS(            int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																	// 現在描画対象になっている画面をＤＤＳ形式で保存する
extern	int			NS_SaveDrawScreenToDDSWithStrLen(  int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																	// 現在描画対象になっている画面をＤＤＳ形式で保存する
extern	int			NS_SaveDrawScreenToJPEG(           int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 現在描画対象になっている画面をＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			NS_SaveDrawScreenToJPEGWithStrLen( int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 現在描画対象になっている画面をＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			NS_SaveDrawScreenToPNG(            int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int CompressionLevel = -1 ) ;																										// 現在描画対象になっている画面をＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern	int			NS_SaveDrawScreenToPNGWithStrLen(  int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int CompressionLevel = -1 ) ;																										// 現在描画対象になっている画面をＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9

// 描画対象にできるグラフィックハンドル保存関数
// Jpeg_Quality         = 0:低画質〜100:高画質
// Png_CompressionLevel = 0:無圧縮〜  9:最高圧縮
extern	int			NS_SaveDrawValidGraph(       int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 描画対象にできるグラフィックハンドルをファイルで保存する
extern	int			NS_SaveDrawValidGraphWithStrLen(       int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int SaveType = DX_IMAGESAVETYPE_BMP , int Jpeg_Quality = 80 , int Jpeg_Sample2x1 = TRUE , int Png_CompressionLevel = -1 ) ;		// 描画対象にできるグラフィックハンドルをファイルで保存する
extern	int			NS_SaveDrawValidGraphToBMP(            int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																// 描画対象にできるグラフィックハンドルをＢＭＰ形式で保存する
extern	int			NS_SaveDrawValidGraphToBMPWithStrLen(  int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																// 描画対象にできるグラフィックハンドルをＢＭＰ形式で保存する
extern	int			NS_SaveDrawValidGraphToDDS(            int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName                        ) ;																																// 描画対象にできるグラフィックハンドルをＤＤＳ形式で保存する
extern	int			NS_SaveDrawValidGraphToDDSWithStrLen(  int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength ) ;																																// 描画対象にできるグラフィックハンドルをＤＤＳ形式で保存する
extern	int			NS_SaveDrawValidGraphToJPEG(           int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 描画対象にできるグラフィックハンドルをＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			NS_SaveDrawValidGraphToJPEGWithStrLen( int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int Quality = 80 , int Sample2x1 = TRUE ) ;																						// 描画対象にできるグラフィックハンドルをＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			NS_SaveDrawValidGraphToPNG(            int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName,                        int CompressionLevel = -1 ) ;																									// 描画対象にできるグラフィックハンドルをＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern	int			NS_SaveDrawValidGraphToPNGWithStrLen(  int GrHandle, int x1, int y1, int x2, int y2, const TCHAR *FileName, size_t FileNameLength, int CompressionLevel = -1 ) ;																									// 描画対象にできるグラフィックハンドルをＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9

#endif // DX_NON_SAVEFUNCTION

// 頂点バッファ関係関数
extern	int			NS_CreateVertexBuffer(		int VertexNum, int VertexType /* DX_VERTEX_TYPE_NORMAL_3D 等 */ ) ;						// 頂点バッファを作成する( -1:エラー  0以上:頂点バッファハンドル )
extern	int			NS_DeleteVertexBuffer(		int VertexBufHandle ) ;																	// 頂点バッファを削除する
extern	int			NS_InitVertexBuffer(		void ) ;																				// すべての頂点バッファを削除する
extern	int			NS_SetVertexBufferData(	int SetIndex, const void *VertexData, int VertexNum, int VertexBufHandle ) ;					// 頂点バッファに頂点データを転送する
extern	int			NS_CreateIndexBuffer(		int IndexNum, int IndexType /* DX_INDEX_TYPE_16BIT 等 */  ) ;							// インデックスバッファを作成する( -1:エラー　0以上：インデックスバッファハンドル )
extern	int			NS_DeleteIndexBuffer(		int IndexBufHandle ) ;																	// インデックスバッファを削除する
extern	int			NS_InitIndexBuffer(		void ) ;																				// すべてのインデックスバッファを削除する
extern	int			NS_SetIndexBufferData(		int SetIndex, const void *IndexData, int IndexNum, int IndexBufHandle ) ;						// インデックスバッファにインデックスデータを転送する
extern	int			NS_GetMaxPrimitiveCount(	void ) ;																				// グラフィックスデバイスが対応している一度に描画できるプリミティブの最大数を取得する
extern	int			NS_GetMaxVertexIndex(		void ) ;																				// グラフィックスデバイスが対応している一度に使用することのできる最大頂点数を取得する

// シェーダー関係関数
extern	int			NS_GetValidShaderVersion(		void ) ;												// 使用できるシェーダーのバージョンを取得する( 0=使えない  200=シェーダーモデル２．０が使用可能  300=シェーダーモデル３．０が使用可能 )

extern	int			NS_LoadVertexShader(			const TCHAR *FileName ) ;								// 頂点シェーダーバイナリをファイルから読み込み頂点シェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			NS_LoadVertexShaderWithStrLen(	const TCHAR *FileName, size_t FileNameLength ) ;		// 頂点シェーダーバイナリをファイルから読み込み頂点シェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			NS_LoadPixelShader(			const TCHAR *FileName                        ) ;		// ピクセルシェーダーバイナリをファイルから読み込みピクセルシェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			NS_LoadPixelShaderWithStrLen(	const TCHAR *FileName, size_t FileNameLength ) ;		// ピクセルシェーダーバイナリをファイルから読み込みピクセルシェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			NS_LoadVertexShaderFromMem(	const void *ImageAddress, int ImageSize ) ;					// メモリ空間上に存在する頂点シェーダーバイナリから頂点シェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			NS_LoadPixelShaderFromMem(		const void *ImageAddress, int ImageSize ) ;					// メモリ空間上に存在するピクセルシェーダーバイナリからピクセルシェーダーハンドルを作成する( 戻り値 -1:エラー  -1以外:シェーダーハンドル )
extern	int			NS_DeleteShader(				int ShaderHandle ) ;									// シェーダーハンドルの削除
extern	int			NS_InitShader(					void ) ;												// シェーダーハンドルを全て削除する

extern	int			NS_GetConstIndexToShader(           const TCHAR *ConstantName, int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int			NS_GetConstIndexToShaderWithStrLen(           const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の番号を取得する
extern	int			NS_GetConstCountToShader(                     const TCHAR *ConstantName,                            int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	int			NS_GetConstCountToShaderWithStrLen(           const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle ) ;	// 指定の名前を持つ定数が使用するシェーダー定数の数を取得する
extern	const FLOAT4 *NS_GetConstDefaultParamFToShader(           const TCHAR *ConstantName,                            int ShaderHandle ) ;	// 指定の名前を持つ浮動小数点数定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	const FLOAT4 *NS_GetConstDefaultParamFToShaderWithStrLen( const TCHAR *ConstantName, size_t ConstantNameLength, int ShaderHandle ) ;	// 指定の名前を持つ浮動小数点数定数のデフォルトパラメータが格納されているメモリアドレスを取得する
extern	int			NS_SetVSConstSF(         int ConstantIndex, float  Param ) ;							// 頂点シェーダーの float 型定数を設定する
extern	int			NS_SetVSConstF(          int ConstantIndex, FLOAT4 Param ) ;							// 頂点シェーダーの float 型定数を設定する
extern	int			NS_SetVSConstFMtx(       int ConstantIndex, MATRIX Param ) ;							// 頂点シェーダーの float 型定数に行列を設定する
extern	int			NS_SetVSConstFMtxT(      int ConstantIndex, MATRIX Param ) ;							// 頂点シェーダーの float 型定数に転置した行列を設定する
extern	int			NS_SetVSConstSI(         int ConstantIndex, int    Param ) ;							// 頂点シェーダーの int 型定数を設定する
extern	int			NS_SetVSConstI(          int ConstantIndex, INT4   Param ) ;							// 頂点シェーダーの int 型定数を設定する
extern	int			NS_SetVSConstB(          int ConstantIndex, BOOL   Param ) ;							// 頂点シェーダーの BOOL 型定数を設定する
extern	int			NS_SetVSConstSFArray(    int ConstantIndex, const float  *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetVSConstFArray(     int ConstantIndex, const FLOAT4 *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetVSConstFMtxArray(  int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数に行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetVSConstFMtxTArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// 頂点シェーダーの float 型定数に転置した行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetVSConstSIArray(    int ConstantIndex, const int    *ParamArray, int ParamNum ) ;	// 頂点シェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetVSConstIArray(     int ConstantIndex, const INT4   *ParamArray, int ParamNum ) ;	// 頂点シェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetVSConstBArray(     int ConstantIndex, const BOOL   *ParamArray, int ParamNum ) ;	// 頂点シェーダーの BOOL  型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_ResetVSConstF(        int ConstantIndex, int ParamNum ) ;							// 頂点シェーダーの float 型定数の設定をリセットする
extern	int			NS_ResetVSConstI(        int ConstantIndex, int ParamNum ) ;							// 頂点シェーダーの int 型定数の設定をリセットする
extern	int			NS_ResetVSConstB(        int ConstantIndex, int ParamNum ) ;							// 頂点シェーダーの BOOL 型定数の設定をリセットする

extern	int			NS_SetPSConstSF(         int ConstantIndex, float  Param ) ;							// ピクセルシェーダーの float 型定数を設定する
extern	int			NS_SetPSConstF(          int ConstantIndex, FLOAT4 Param ) ;							// ピクセルシェーダーの float 型定数を設定する
extern	int			NS_SetPSConstFMtx(       int ConstantIndex, MATRIX Param ) ;							// ピクセルシェーダーの float 型定数に行列を設定する
extern	int			NS_SetPSConstFMtxT(      int ConstantIndex, MATRIX Param ) ;							// ピクセルシェーダーの float 型定数に転置した行列を設定する
extern	int			NS_SetPSConstSI(         int ConstantIndex, int    Param ) ;							// ピクセルシェーダーの int 型定数を設定する
extern	int			NS_SetPSConstI(          int ConstantIndex, INT4   Param ) ;							// ピクセルシェーダーの int 型定数を設定する
extern	int			NS_SetPSConstB(          int ConstantIndex, BOOL   Param ) ;							// ピクセルシェーダーの BOOL 型定数を設定する
extern	int			NS_SetPSConstSFArray(    int ConstantIndex, const float  *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetPSConstFArray(     int ConstantIndex, const FLOAT4 *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetPSConstFMtxArray(  int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数に行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetPSConstFMtxTArray( int ConstantIndex, const MATRIX *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの float 型定数に転置した行列を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetPSConstSIArray(    int ConstantIndex, const int    *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetPSConstIArray(     int ConstantIndex, const INT4   *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの int   型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_SetPSConstBArray(     int ConstantIndex, const BOOL   *ParamArray, int ParamNum ) ;	// ピクセルシェーダーの BOOL  型定数を設定する( 配列を使って連番インデックスに一度に設定 )
extern	int			NS_ResetPSConstF(        int ConstantIndex, int ParamNum ) ;							// ピクセルシェーダーの float 型定数の設定をリセットする
extern	int			NS_ResetPSConstI(        int ConstantIndex, int ParamNum ) ;							// ピクセルシェーダーの int 型定数の設定をリセットする
extern	int			NS_ResetPSConstB(        int ConstantIndex, int ParamNum ) ;							// ピクセルシェーダーの BOOL 型定数の設定をリセットする

extern	int			NS_SetRenderTargetToShader( int TargetIndex, int DrawScreen, int SurfaceIndex = 0 , int MipLevel = 0 ) ;	// シェーダーを使用した描画での描画先を設定する
extern	int			NS_SetUseTextureToShader(   int StageIndex, int GraphHandle ) ;						// シェーダー描画で使用するグラフィックを設定する
extern	int			NS_SetUseVertexShader(      int ShaderHandle ) ;										// シェーダー描画に使用する頂点シェーダーを設定する( -1を渡すと解除 )
extern	int			NS_SetUsePixelShader(       int ShaderHandle ) ;										// シェーダー描画に使用するピクセルシェーダーを設定する( -1を渡すと解除 )

extern	int			NS_CalcPolygonBinormalAndTangentsToShader(        VERTEX3DSHADER *Vertex, int PolygonNum ) ;													// ポリゴンの頂点の接線と従法線をＵＶ座標から計算してセットする
extern	int			NS_CalcPolygonIndexedBinormalAndTangentsToShader( VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum ) ;		// ポリゴンの頂点の接線と従法線をＵＶ座標から計算してセットする( 頂点インデックスを使用する )

extern	int			NS_DrawBillboard3DToShader( VECTOR Pos, float cx, float cy, float Size, float Angle, int GrHandle, int TransFlag, int ReverseXFlag = FALSE, int ReverseYFlag = FALSE ) ;													// シェーダーを使ってビルボードを描画する
extern	int			NS_DrawPolygon2DToShader(          const VERTEX2DSHADER *Vertex, int PolygonNum ) ;																										// シェーダーを使って２Ｄポリゴンを描画する
extern	int			NS_DrawPolygon3DToShader(          const VERTEX3DSHADER *Vertex, int PolygonNum ) ;																										// シェーダーを使って３Ｄポリゴンを描画する
extern	int			NS_DrawPolygonIndexed2DToShader(   const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum ) ;															// シェーダーを使って２Ｄポリゴンを描画する( 頂点インデックスを使用する )
extern	int			NS_DrawPolygonIndexed3DToShader(   const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int PolygonNum ) ;															// シェーダーを使って３Ｄポリゴンを描画する( 頂点インデックスを使用する )
extern	int			NS_DrawPrimitive2DToShader(        const VERTEX2DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する
extern	int			NS_DrawPrimitive3DToShader(        const VERTEX3DSHADER *Vertex, int VertexNum,                                              int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する
extern	int			NS_DrawPrimitiveIndexed2DToShader( const VERTEX2DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って２Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int			NS_DrawPrimitiveIndexed3DToShader( const VERTEX3DSHADER *Vertex, int VertexNum, const unsigned short *Indices, int IndexNum, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;		// シェーダーを使って３Ｄプリミティブを描画する( 頂点インデックスを使用する )
extern	int			NS_DrawPolygon3DToShader_UseVertexBuffer(           int VertexBufHandle ) ;																												// シェーダーを使って３Ｄポリゴンを描画する( 頂点バッファ使用版 )
extern	int			NS_DrawPolygonIndexed3DToShader_UseVertexBuffer(    int VertexBufHandle, int IndexBufHandle ) ;																							// シェーダーを使って３Ｄポリゴンを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int			NS_DrawPrimitive3DToShader_UseVertexBuffer(         int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;										// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int			NS_DrawPrimitive3DToShader_UseVertexBuffer2(        int VertexBufHandle,                     int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int StartVertex, int UseVertexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファ使用版 )
extern	int			NS_DrawPrimitiveIndexed3DToShader_UseVertexBuffer(  int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */ ) ;										// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )
extern	int			NS_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2( int VertexBufHandle, int IndexBufHandle, int PrimitiveType /* DX_PRIMTYPE_TRIANGLELIST 等 */, int BaseVertex, int StartVertex, int UseVertexNum, int StartIndex, int UseIndexNum ) ;	// シェーダーを使って３Ｄプリミティブを描画する( 頂点バッファとインデックスバッファ使用版 )

// シェーダー用定数バッファ関係関数
extern	int			NS_InitShaderConstantBuffer(		void ) ;																					// 全てのシェーダー用定数バッファハンドルを削除する
extern	int			NS_CreateShaderConstantBuffer(		int BufferSize ) ;																			// シェーダー用定数バッファハンドルを初期化する
extern	int			NS_DeleteShaderConstantBuffer(		int SConstBufHandle ) ;																		// シェーダー用定数バッファハンドルの後始末
extern	void *		NS_GetBufferShaderConstantBuffer(	int SConstBufHandle ) ;																		// シェーダー用定数バッファハンドルの定数バッファのアドレスを取得する
extern	int			NS_UpdateShaderConstantBuffer(		int SConstBufHandle ) ;																		// シェーダー用定数バッファハンドルの定数バッファへの変更を適用する
extern	int			NS_SetShaderConstantBuffer(		int SConstBufHandle, int TargetShader /* DX_SHADERTYPE_VERTEX など */ , int Slot ) ;		// シェーダー用定数バッファハンドルの定数バッファを指定のシェーダーの指定のスロットにセットする

// フィルター関係関数
#ifndef DX_NON_FILTER
extern	int			NS_GraphFilter(        int    GrHandle,                                                                                     int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像にフィルター処理を行う
extern	int			NS_GraphFilterBlt(     int SrcGrHandle, int DestGrHandle,                                                                   int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像のフィルター付き転送を行う
extern	int			NS_GraphFilterRectBlt( int SrcGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int DestX, int DestY, int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像のフィルター付き転送を行う( 矩形指定 )
extern	int			NS_GraphFilterRectBlt2( int SrcGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int DestX1, int DestY1, int DestX2, int DestY2, int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */ , ... ) ;		// 画像のフィルター付き転送を行う( 矩形指定沢山 )
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_MONO, int Cb = 青色差( -255 〜 255 ), int Cr = 赤色差( -255 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_GAUSS, int PixelWidth = 使用ピクセル幅( 8 , 16 , 32 の何れか ), int Param = ぼかしパラメータ( 100 で約1ピクセル分の幅 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_DOWN_SCALE, int DivNum = 元のサイズの何分の１か、という値( 2 , 4 , 8 の何れか ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_BRIGHT_CLIP, int CmpType = クリップタイプ( DX_CMP_LESS:CmpParam以下をクリップ  又は  DX_CMP_GREATER:CmpParam以上をクリップ ), int CmpParam = クリップパラメータ( 0 〜 255 ), int ClipFillFlag = クリップしたピクセルを塗りつぶすかどうか( TRUE:塗りつぶす  FALSE:塗りつぶさない ), int ClipFillColor = クリップしたピクセルに塗る色値( GetColor で取得する )( ClipFillFlag が FALSE の場合は使用しない ), int ClipFillAlpha = クリップしたピクセルに塗るα値( 0 〜 255 )( ClipFillFlag が FALSE の場合は使用しない ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_HSB, int HueType = Hue の意味( 0:相対値  1:絶対値 ), int Hue = 色相パラメータ( HueType が 0 の場合 = ピクセルの色相に対する相対値( -180 〜 180 )   HueType が 1 の場合 = 色相の絶対値( 0 〜 360 ) ), int Saturation = 彩度( -255 〜 ), int Bright = 輝度( -255 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_INVERT ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_LEVEL, int Min = 変換元の下限値( 0 〜 255 ), int Max = 変換元の上限値( 0 〜 255 ), int Gamma = ガンマ値( 100 でガンマ補正無し、0 とそれ以下の値は不可 ), int AfterMin = 変換後の最低値( 0 〜 255 ), int AfterMax = 変換後の最大値( 0 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_TWO_COLOR, int Threshold = 閾値( 0 〜 255 ), int LowColor = 閾値より値が低かったピクセルの変換後の色値( GetColor で取得する ), int LowAlpha = 閾値より値が低かったピクセルの変換後のα値( 0 〜 255 ), int HighColor = 閾値より値が高かったピクセルの変換後の色値( GetColor で取得する ), int HighAlpha = 閾値より値が高かったピクセルの変換後のα値( 0 〜 255 ) ) ;
//		int			GraphFilter( int GrHandle, int FilterType = DX_GRAPH_FILTER_GRADIENT_MAP, int MapGrHandle = グラデーションマップのグラフィックハンドル( 元画像の輝度からグラデーションマップ画像の x 座標を算出しますので縦幅は1dotでもＯＫ ), int Reverse = グラデーションマップ左右反転フラグ( TRUE : グラデーションマップを左右反転して使う  FALSE : 左右反転しない ) ) ;

extern	int			NS_GraphBlend(        int    GrHandle, int BlendGrHandle,                                                                                                             int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */ , int BlendType /* DX_GRAPH_BLEND_ADD 等 */ , ... ) ;	// 二つの画像をブレンドする
extern	int			NS_GraphBlendBlt(     int SrcGrHandle, int BlendGrHandle, int DestGrHandle,                                                                                           int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */ , int BlendType /* DX_GRAPH_BLEND_ADD 等 */ , ... ) ;	// 二つの画像をブレンドして結果を指定の画像に出力する
extern	int			NS_GraphBlendRectBlt( int SrcGrHandle, int BlendGrHandle, int DestGrHandle, int SrcX1, int SrcY1, int SrcX2, int SrcY2, int BlendX, int BlendY, int DestX, int DestY, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */ , int BlendType /* DX_GRAPH_BLEND_ADD 等 */ , ... ) ;	// 二つの画像をブレンドして結果を指定の画像に出力する( 矩形指定 )
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_NORMAL ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_RGBA_SELECT_MIX, int SelectR = ( 出力の赤分となる成分 DX_RGBA_SELECT_SRC_R 等 ), int SelectG = ( 出力の緑成分となる成分 DX_RGBA_SELECT_SRC_R 等 ), int SelectB = ( 出力の青成分となる成分 DX_RGBA_SELECT_SRC_R 等 ), int SelectA = ( 出力のα成分となる成分 DX_RGBA_SELECT_SRC_R 等 ) ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_MULTIPLE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_DIFFERENCE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_ADD ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_SCREEN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_OVERLAY ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_DODGE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_BURN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_DARKEN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_LIGHTEN ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_SOFTLIGHT ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_HARDLIGHT ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_EXCLUSION ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_HUE ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_SATURATION ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_COLOR ) ;
//		int			GraphBlend( int GrHandle, int BlendGrHandle, int BlendRatio, int BlendType = DX_GRAPH_BLEND_BRIGHT ) ;
#endif // DX_NON_FILTER

#ifndef DX_NON_MOVIE
// ムービーグラフィック関係関数
extern	int			NS_PlayMovie(						const TCHAR *FileName, int ExRate, int PlayType ) ;						// 動画ファイルの再生
extern	int			NS_PlayMovieWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int ExRate, int PlayType ) ;	// 動画ファイルの再生
extern	int			NS_GetMovieImageSize_File(             const TCHAR *FileName,                        int *SizeX, int *SizeY ) ;	// 動画ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			NS_GetMovieImageSize_File_WithStrLen(  const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY ) ;	// 動画ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			NS_GetMovieImageSize_Mem(              const void *FileImage, int FileImageSize, int *SizeX, int *SizeY ) ;	// メモリ上に展開された動画ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			NS_OpenMovieToGraph(				const TCHAR *FileName, int FullColor = TRUE ) ;							// ムービーを開く
extern	int			NS_OpenMovieToGraphWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int FullColor = TRUE ) ;		// 動画ファイルを開く
extern	int			NS_PlayMovieToGraph(				int GraphHandle, int PlayType = DX_PLAYTYPE_BACK , int SysPlay = 0 ) ;	// ムービーグラフィックに含まれるムービーの再生を開始する
extern	int			NS_PauseMovieToGraph(				int GraphHandle, int SysPause = 0 ) ;									// ムービーグラフィックに含まれるムービーの再生をストップする
extern	int			NS_AddMovieFrameToGraph(			int GraphHandle, unsigned int FrameNum ) ;								// ムービーのフレームを進める、戻すことは出来ない( ムービーが停止状態で、且つ Ogg Theora のみ有効 )
extern	int			NS_SeekMovieToGraph(				int GraphHandle, int Time ) ;											// ムービーの再生位置を設定する(ミリ秒単位)
extern	int			NS_SetPlaySpeedRateMovieToGraph(		int GraphHandle, double SpeedRate ) ;									// 動画ファイルの再生速度を設定する( 1.0 = 等倍速  2.0 = ２倍速 )、一部のファイルフォーマットのみで有効な機能です
extern	int			NS_GetMovieStateToGraph(			int GraphHandle ) ;														// ムービーの再生状態を得る
extern	int			NS_SetMovieVolumeToGraph(			int Volume, int GraphHandle ) ;											// ムービーのボリュームをセットする(0〜10000)
extern	int			NS_ChangeMovieVolumeToGraph(		int Volume, int GraphHandle ) ;											// ムービーのボリュームをセットする(0〜255)
extern	const BASEIMAGE* NS_GetMovieBaseImageToGraph(	int GraphHandle, int *ImageUpdateFlag , int ImageUpdateFlagSetOnly ) ;	// ムービーの基本イメージデータを取得する
extern	int			NS_GetMovieTotalFrameToGraph(		int GraphHandle ) ;														// ムービーの総フレーム数を得る( Ogg Theora でのみ有効 )
extern	int			NS_TellMovieToGraph(				int GraphHandle ) ;														// ムービーの再生位置を取得する(ミリ秒単位)
extern	int			NS_TellMovieToGraphToFrame(		int GraphHandle ) ;														// ムービーの再生位置を取得する(フレーム単位)
extern	int			NS_SeekMovieToGraphToFrame(		int GraphHandle, int Frame ) ;											// ムービーの再生位置を設定する(フレーム単位)
extern	LONGLONG	NS_GetOneFrameTimeMovieToGraph(	int GraphHandle ) ;														// ムービーの１フレームあたりの時間を得る
extern	int			NS_GetLastUpdateTimeMovieToGraph(	int GraphHandle ) ;														// ムービーのイメージを最後に更新した時間を得る(ミリ秒単位)
extern	int			NS_SetMovieRightImageAlphaFlag(	int Flag ) ;															// 読み込む動画ファイル映像の右半分の赤成分をα情報として扱うかどうかをセットする( TRUE:α情報として扱う  FALSE:α情報として扱わない( デフォルト ) )
extern	int			NS_SetMovieColorA8R8G8B8Flag(		int Flag ) ;															// 読み込む動画ファイルが32bitカラーだった場合、A8R8G8B8 形式として扱うかどうかをセットする、32bitカラーではない動画ファイルに対しては無効( Flag  TRUE:A8R8G8B8として扱う  FALSE:X8R8G8B8として扱う( デフォルト ) )
extern	int			NS_SetMovieUseYUVFormatSurfaceFlag(	int Flag ) ;															// ＹＵＶフォーマットのサーフェスが使用できる場合はＹＵＶフォーマットのサーフェスを使用するかどうかを設定する( TRUE:使用する( デフォルト ) FALSE:ＲＧＢフォーマットのサーフェスを使用する )
#endif

// カメラ関係関数
extern	int			NS_SetCameraNearFar(					float Near, float Far ) ;											// カメラの Nearクリップ面と Farクリップ面の距離を設定する
extern	int			NS_SetCameraNearFarD(					double Near, double Far ) ;											// カメラの Nearクリップ面と Farクリップ面の距離を設定する
extern	int			NS_SetCameraPositionAndTarget_UpVecY(  VECTOR    Position, VECTOR   Target ) ;								// カメラの視点、注視点、アップベクトルを設定する( アップベクトルはＹ軸方向から導き出す )
extern	int			NS_SetCameraPositionAndTarget_UpVecYD( VECTOR_D  Position, VECTOR_D Target ) ;								// カメラの視点、注視点、アップベクトルを設定する( アップベクトルはＹ軸方向から導き出す )
extern	int			NS_SetCameraPositionAndTargetAndUpVec( VECTOR    Position, VECTOR   TargetPosition, VECTOR   UpVector ) ;	// カメラの視点、注視点、アップベクトルを設定する
extern	int			NS_SetCameraPositionAndTargetAndUpVecD( VECTOR_D Position, VECTOR_D TargetPosition, VECTOR_D UpVector ) ;	// カメラの視点、注視点、アップベクトルを設定する
extern	int			NS_SetCameraPositionAndAngle(			VECTOR   Position, float  VRotate, float  HRotate, float  TRotate ) ;	// カメラの視点、注視点、アップベクトルを設定する( 注視点とアップベクトルは垂直回転角度、水平回転角度、捻り回転角度から導き出す )
extern	int			NS_SetCameraPositionAndAngleD(			VECTOR_D Position, double VRotate, double HRotate, double TRotate ) ;	// カメラの視点、注視点、アップベクトルを設定する( 注視点とアップベクトルは垂直回転角度、水平回転角度、捻り回転角度から導き出す )
extern	int			NS_SetCameraViewMatrix(					MATRIX   ViewMatrix ) ;												// ビュー行列を直接設定する
extern	int			NS_SetCameraViewMatrixD(				MATRIX_D ViewMatrix ) ;												// ビュー行列を直接設定する
extern	int			NS_SetCameraScreenCenter(				float x, float y ) ;												// 画面上におけるカメラが見ている映像の中心の座標を設定する
extern	int			NS_SetCameraScreenCenterD(				double x, double y ) ;												// 画面上におけるカメラが見ている映像の中心の座標を設定する

extern	int			NS_SetupCamera_Perspective(			float  Fov ) ;														// 遠近法カメラをセットアップする
extern	int			NS_SetupCamera_PerspectiveD(			double Fov ) ;														// 遠近法カメラをセットアップする
extern	int			NS_SetupCamera_Ortho(					float  Size ) ;														// 正射影カメラをセットアップする
extern	int			NS_SetupCamera_OrthoD(					double Size ) ;														// 正射影カメラをセットアップする
extern	int			NS_SetupCamera_ProjectionMatrix(		MATRIX   ProjectionMatrix ) ;										// 射影行列を直接設定する
extern	int			NS_SetupCamera_ProjectionMatrixD(		MATRIX_D ProjectionMatrix ) ;										// 射影行列を直接設定する
extern	int			NS_SetCameraDotAspect(					float  DotAspect ) ;												// カメラのドットアスペクト比を設定する
extern	int			NS_SetCameraDotAspectD(				double DotAspect ) ;												// カメラのドットアスペクト比を設定する

extern	int			NS_CheckCameraViewClip(				VECTOR   CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )
extern	int			NS_CheckCameraViewClipD(				VECTOR_D CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )
extern	int			NS_CheckCameraViewClip_Dir(			VECTOR   CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する、戻り値で外れている方向も知ることができる( 戻り値 0:視界に入っている  0以外:視界に入っていない( DX_CAMERACLIP_LEFT や DX_CAMERACLIP_RIGHT が or 演算で混合されたもの、and 演算で方向を確認できる ) )
extern	int			NS_CheckCameraViewClip_DirD(			VECTOR_D CheckPos ) ;													// 指定の座標がカメラの視界に入っているかどうかを判定する、戻り値で外れている方向も知ることができる( 戻り値 0:視界に入っている  0以外:視界に入っていない( DX_CAMERACLIP_LEFT や DX_CAMERACLIP_RIGHT が or 演算で混合されたもの、and 演算で方向を確認できる ) )
extern	int			NS_CheckCameraViewClip_Box(			VECTOR   BoxPos1, VECTOR   BoxPos2 ) ;									// 二つの座標で表されるボックスがカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )
extern	int			NS_CheckCameraViewClip_BoxD(			VECTOR_D BoxPos1, VECTOR_D BoxPos2 ) ;									// 二つの座標で表されるボックスがカメラの視界に入っているかどうかを判定する( 戻り値 TRUE:視界に入っていない  FALSE:視界に入っている )

extern	float		NS_GetCameraNear(						void ) ;															// カメラの Near クリップ面の距離を取得する
extern	double		NS_GetCameraNearD(						void ) ;															// カメラの Near クリップ面の距離を取得する
extern	float		NS_GetCameraFar(						void ) ;															// カメラの Far クリップ面の距離を取得する
extern	double		NS_GetCameraFarD(						void ) ;															// カメラの Far クリップ面の距離を取得する

extern	VECTOR		NS_GetCameraPosition(					void ) ;															// カメラの位置を取得する
extern	VECTOR_D	NS_GetCameraPositionD(					void ) ;															// カメラの位置を取得する
extern	VECTOR		NS_GetCameraTarget(					void ) ;															// カメラの注視点を取得する
extern	VECTOR_D	NS_GetCameraTargetD(					void ) ;															// カメラの注視点を取得する
extern	VECTOR		NS_GetCameraUpVector(					void ) ;															// カメラの視線に垂直な上方向のベクトルを取得する
extern	VECTOR_D	NS_GetCameraUpVectorD(					void ) ;															// カメラの視線に垂直な上方向のベクトルを取得する
extern	VECTOR		NS_GetCameraDownVector(				void ) ;															// カメラの視線に垂直な下方向のベクトルを取得する
extern	VECTOR_D	NS_GetCameraDownVectorD(				void ) ;															// カメラの視線に垂直な下方向のベクトルを取得する
extern	VECTOR		NS_GetCameraRightVector(				void ) ;															// カメラの視線に垂直な右方向のベクトルを取得する
extern	VECTOR_D	NS_GetCameraRightVectorD(				void ) ;															// カメラの視線に垂直な右方向のベクトルを取得する
extern	VECTOR		NS_GetCameraLeftVector(				void ) ;															// カメラの視線に垂直な左方向のベクトルを取得する
extern	VECTOR_D	NS_GetCameraLeftVectorD(				void ) ;															// カメラの視線に垂直な左方向のベクトルを取得する
extern	VECTOR		NS_GetCameraFrontVector(				void ) ;															// カメラの向いている方向のベクトルを取得する
extern	VECTOR_D	NS_GetCameraFrontVectorD(				void ) ;															// カメラの向いている方向のベクトルを取得する
extern	VECTOR		NS_GetCameraBackVector(				void ) ;															// カメラの後ろ方向のベクトルを取得する
extern	VECTOR_D	NS_GetCameraBackVectorD(				void ) ;															// カメラの後ろ方向のベクトルを取得する
extern	float		NS_GetCameraAngleHRotate(				void ) ;															// カメラの水平方向の向きを取得する
extern	double		NS_GetCameraAngleHRotateD(				void ) ;															// カメラの水平方向の向きを取得する
extern	float		NS_GetCameraAngleVRotate(				void ) ;															// カメラの垂直方向の向きを取得する
extern	double		NS_GetCameraAngleVRotateD(				void ) ;															// カメラの垂直方向の向きを取得する
extern	float		NS_GetCameraAngleTRotate(				void ) ;															// カメラの向きの捻り角度を取得する
extern	double		NS_GetCameraAngleTRotateD(				void ) ;															// カメラの向きの捻り角度を取得する

extern	MATRIX		NS_GetCameraViewMatrix(				void ) ;															// ビュー行列を取得する
extern	MATRIX_D	NS_GetCameraViewMatrixD(				void ) ;															// ビュー行列を取得する
extern	MATRIX		NS_GetCameraBillboardMatrix(			void ) ;															// ビルボード行列を取得する
extern	MATRIX_D	NS_GetCameraBillboardMatrixD(			void ) ;															// ビルボード行列を取得する
extern	int			NS_GetCameraScreenCenter(				float  *x, float  *y ) ;											// 画面上におけるカメラが見ている映像の中心の座標を取得する
extern	int			NS_GetCameraScreenCenterD(				double *x, double *y ) ;											// 画面上におけるカメラが見ている映像の中心の座標を取得する
extern	float		NS_GetCameraFov(						void ) ;															// カメラの視野角を取得する
extern	double		NS_GetCameraFovD(						void ) ;															// カメラの視野角を取得する
extern	float		NS_GetCameraSize(						void ) ;															// カメラの視野サイズを取得する
extern	double		NS_GetCameraSizeD(						void ) ;															// カメラの視野サイズを取得する
extern	MATRIX		NS_GetCameraProjectionMatrix(			void ) ;															// 射影行列を取得する
extern	MATRIX_D	NS_GetCameraProjectionMatrixD(			void ) ;															// 射影行列を取得する
extern	float		NS_GetCameraDotAspect(					void ) ;															// カメラのドットアスペクト比を得る
extern	double		NS_GetCameraDotAspectD(				void ) ;															// カメラのドットアスペクト比を得る
extern	MATRIX		NS_GetCameraViewportMatrix(			void ) ;															// ビューポート行列を取得する
extern	MATRIX_D	NS_GetCameraViewportMatrixD(			void ) ;															// ビューポート行列を取得する
extern	MATRIX		NS_GetCameraAPIViewportMatrix(			void ) ;															// Direct3Dで自動適用されるビューポート行列を取得する
extern	MATRIX_D	NS_GetCameraAPIViewportMatrixD(		void ) ;															// Direct3Dで自動適用されるビューポート行列を取得する

// ライト関係関数
extern	int			NS_SetUseLighting(				int Flag ) ;																// ライティング計算機能を使用するかどうかを設定する
extern	int			NS_SetMaterialUseVertDifColor( int UseFlag ) ;																// ３Ｄ図形描画のライティング計算に頂点カラーのディフューズカラーを使用するかどうかを設定する
extern	int			NS_SetMaterialUseVertSpcColor( int UseFlag ) ;																// ３Ｄ図形描画のライティング計算に頂点カラーのスペキュラカラーを使用するかどうかを設定する
extern	int			NS_SetMaterialParam(			MATERIALPARAM Material ) ;													// ３Ｄ図形描画のライティング計算に使用するマテリアルパラメータを設定する
extern	int			NS_SetUseSpecular(				int UseFlag ) ;																// ３Ｄ図形描画にスペキュラを使用するかどうかを設定する
extern	int			NS_SetGlobalAmbientLight(		COLOR_F Color ) ;															// グローバルアンビエントライトカラーを設定する

extern	int			NS_ChangeLightTypeDir(			VECTOR Direction ) ;														// ライトのタイプをディレクショナルライトにする
extern	int			NS_ChangeLightTypeSpot(		VECTOR Position, VECTOR Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2 ) ;	// ライトのタイプをスポットライトにする
extern	int			NS_ChangeLightTypePoint(		VECTOR Position, float Range, float Atten0, float Atten1, float Atten2 ) ;	// ライトのタイプをポイントライトにする
extern	int			NS_SetLightEnable(				int EnableFlag ) ;															// ライトの有効、無効をセットする
extern	int			NS_SetLightDifColor(			COLOR_F Color ) ;															// ライトのディフューズカラーを設定する
extern	int			NS_SetLightSpcColor(			COLOR_F Color ) ;															// ライトのスペキュラカラーを設定する
extern	int			NS_SetLightAmbColor(			COLOR_F Color ) ;															// ライトのアンビエントカラーを設定する
extern	int			NS_SetLightDirection(			VECTOR Direction ) ;														// ライトの方向を設定する
extern	int			NS_SetLightPosition(			VECTOR Position ) ;															// ライトの位置を設定する
extern	int			NS_SetLightRangeAtten(			float Range, float Atten0, float Atten1, float Atten2 ) ;					// ライトの距離減衰パラメータを設定する( 有効距離、距離減衰係数０、１、２ )
extern	int			NS_SetLightAngle(				float OutAngle, float InAngle ) ;											// ライトのスポットライトのパラメータを設定する( 外部コーン角度、内部コーン角度 )
extern	int			NS_GetLightType(				void ) ;																	// ライトのタイプを取得する( 戻り値は DX_LIGHTTYPE_DIRECTIONAL 等 )
extern	int			NS_GetLightEnable(				void ) ;																	// ライト効果の有効、無効を取得する( TRUE:有効  FALSE:無効 )
extern	COLOR_F		NS_GetLightDifColor(			void ) ;																	// ライトのディフューズカラーを取得する
extern	COLOR_F		NS_GetLightSpcColor(			void ) ;																	// ライトのスペキュラカラーを取得する
extern	COLOR_F		NS_GetLightAmbColor(			void ) ;																	// ライトのアンビエントカラーを取得する
extern	VECTOR		NS_GetLightDirection(			void ) ;																	// ライトの方向を取得する
extern	VECTOR		NS_GetLightPosition(			void ) ;																	// ライトの位置を取得する
extern	int			NS_GetLightRangeAtten(			float *Range, float *Atten0, float *Atten1, float *Atten2 )	;				// ライトの距離減衰パラメータを取得する( 有効距離、距離減衰係数０、１、２ )
extern	int			NS_GetLightAngle(				float *OutAngle, float *InAngle ) ;											// ライトのスポットライトのパラメータを取得する( 外部コーン角度、内部コーン角度 )
extern	int			NS_SetLightUseShadowMap(		int SmSlotIndex, int UseFlag ) ;											// デフォルトライトに SetUseShadowMap で指定したシャドウマップを適用するかどうかを設定する( SmSlotIndex:シャドウマップスロット( SetUseShadowMap の第一引数に設定する値 ) UseFlag:適用にするかどうかのフラグ( TRUE:適用する( デフォルト )  FALSE:適用しない ) )

extern	int			NS_CreateDirLightHandle(       VECTOR Direction ) ;														// ディレクショナルライトハンドルを作成する
extern	int			NS_CreateSpotLightHandle(      VECTOR Position, VECTOR Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2 ) ;	// スポットライトハンドルを作成する
extern	int			NS_CreatePointLightHandle(     VECTOR Position, float Range, float Atten0, float Atten1, float Atten2 ) ;	// ポイントライトハンドルを作成する
extern	int			NS_DeleteLightHandle(          int LHandle ) ;																// ライトハンドルを削除する
extern	int			NS_DeleteLightHandleAll(       void ) ;																	// ライトハンドルを全て削除する
extern	int			NS_SetLightTypeHandle(         int LHandle, int LightType ) ;												// ライトハンドルのライトのタイプを変更する( DX_LIGHTTYPE_DIRECTIONAL 等 )
extern	int			NS_SetLightEnableHandle(       int LHandle, int EnableFlag ) ;												// ライトハンドルのライト効果の有効、無効を設定する( TRUE:有効  FALSE:無効 )
extern	int			NS_SetLightDifColorHandle(     int LHandle, COLOR_F Color ) ;												// ライトハンドルのライトのディフューズカラーを設定する
extern	int			NS_SetLightSpcColorHandle(     int LHandle, COLOR_F Color ) ;												// ライトハンドルのライトのスペキュラカラーを設定する
extern	int			NS_SetLightAmbColorHandle(     int LHandle, COLOR_F Color ) ;												// ライトハンドルのライトのアンビエントカラーを設定する
extern	int			NS_SetLightDirectionHandle(    int LHandle, VECTOR Direction ) ;											// ライトハンドルのライトの方向を設定する
extern	int			NS_SetLightPositionHandle(     int LHandle, VECTOR Position ) ;											// ライトハンドルのライトの位置を設定する
extern	int			NS_SetLightRangeAttenHandle(   int LHandle, float Range, float Atten0, float Atten1, float Atten2 ) ;		// ライトハンドルのライトの距離減衰パラメータを設定する( 有効距離、距離減衰係数０、１、２ )
extern	int			NS_SetLightAngleHandle(        int LHandle, float OutAngle, float InAngle ) ;								// ライトハンドルのライトのスポットライトのパラメータを設定する( 外部コーン角度、内部コーン角度 )
extern	int			NS_SetLightUseShadowMapHandle(	int LHandle, int SmSlotIndex, int UseFlag ) ;								// ライトハンドルのライトに SetUseShadowMap で指定したシャドウマップを適用するかどうかを設定する( SmSlotIndex:シャドウマップスロット( SetUseShadowMap の第一引数に設定する値 ) UseFlag:適用にするかどうかのフラグ( TRUE:適用する( デフォルト )  FALSE:適用しない ) )
extern	int			NS_GetLightTypeHandle(         int LHandle ) ;																// ライトハンドルのライトのタイプを取得する( 戻り値は DX_LIGHTTYPE_DIRECTIONAL 等 )
extern	int			NS_GetLightEnableHandle(       int LHandle ) ;																// ライトハンドルのライト効果の有効、無効を取得する( TRUE:有効  FALSE:無効 )
extern	COLOR_F		NS_GetLightDifColorHandle(     int LHandle ) ;																// ライトハンドルのライトのディフューズカラーを取得する
extern	COLOR_F		NS_GetLightSpcColorHandle(     int LHandle ) ;																// ライトハンドルのライトのスペキュラカラーを取得する
extern	COLOR_F		NS_GetLightAmbColorHandle(     int LHandle ) ;																// ライトハンドルのライトのアンビエントカラーを取得する
extern	VECTOR		NS_GetLightDirectionHandle(    int LHandle ) ;																// ライトハンドルのライトの方向を取得する
extern	VECTOR		NS_GetLightPositionHandle(     int LHandle ) ;																// ライトハンドルのライトの位置を取得する
extern	int			NS_GetLightRangeAttenHandle(   int LHandle, float *Range, float *Atten0, float *Atten1, float *Atten2 ) ;	// ライトハンドルのライトの距離減衰パラメータを取得する( 有効距離、距離減衰係数０、１、２ )
extern	int			NS_GetLightAngleHandle(        int LHandle, float *OutAngle, float *InAngle ) ;							// ライトハンドルのライトのスポットライトのパラメータを取得する( 外部コーン角度、内部コーン角度 )

extern	int			NS_GetEnableLightHandleNum(	void ) ;																	// 有効になっているライトハンドルの数を取得する
extern	int			NS_GetEnableLightHandle(		int Index ) ;																// 有効になっているライトハンドルを取得する

// 色情報取得用関数
extern	int			NS_GetTexFormatIndex(			const IMAGEFORMATDESC *Format ) ;																								// テクスチャフォーマットのインデックスを得る
extern	int			NS_ColorKaiseki(				const void *PixelData, COLORDATA* ColorData ) ;																					// 色ビット情報解析












// DxMask.cpp 関数プロトタイプ宣言

#ifndef DX_NON_MASK

// マスク関係 
extern	int			NS_CreateMaskScreen(				void ) ;																						// マスクスクリーンを作成する
extern	int			NS_DeleteMaskScreen(				void ) ;																						// マスクスクリーンを削除する
extern	int			NS_DrawMaskToDirectData(			int x, int y, int Width, int Height, const void *MaskData , int TransMode ) ;					// マスクのデータを直接セット
extern	int			NS_DrawFillMaskToDirectData(		int x1, int y1, int x2, int y2,  int Width, int Height, const void *MaskData ) ;				// マスクのデータを直接マスク画面全体に描画する

extern	int			NS_SetUseMaskScreenFlag(			int ValidFlag ) ;																				// マスク使用モードを変更
extern	int			NS_GetUseMaskScreenFlag(			void ) ;																						// マスク使用モードの取得
extern	int			NS_FillMaskScreen(					int Flag ) ;																					// マスクスクリーンを指定の色で塗りつぶす
extern	int			NS_SetMaskScreenGraph(				int GraphHandle ) ;																				// マスクスクリーンとして使用するグラフィックのハンドルを設定する、-1を渡すと解除( 引数で渡すグラフィックハンドルは MakeScreen で作成した「アルファチャンネル付きの描画対象にできるグラフィックハンドル」である必要があります( アルファチャンネルがマスクに使用されます ) )
extern	int			NS_SetMaskScreenGraphUseChannel(	int UseChannel /* DX_MASKGRAPH_CH_A 等 */ ) ;													// マスクスクリーンとして使用するグラフィックの、どのチャンネルをマスクとして使用するかを設定する( デフォルトは DX_MASKGRAPH_CH_A、 尚、DX_MASKGRAPH_CH_A以外を使用する場合はグラフィックスデバイスがシェーダーモデル2.0以降に対応している必要があります )

extern	int			NS_InitMask(						void ) ;																						// マスクデータを初期化する
extern	int			NS_MakeMask(						int Width, int Height ) ;																		// マスクデータの追加
extern	int			NS_GetMaskSize(					int *WidthBuf, int *HeightBuf, int MaskHandle ) ;												// マスクの大きさを得る 
extern	int			NS_SetDataToMask(					int Width, int Height, const void *MaskData, int MaskHandle ) ;										// マスクのデータをマスクに転送する
extern	int			NS_DeleteMask(						int MaskHandle ) ;																				// マスクデータを削除
extern	int			NS_BmpBltToMask(					HBITMAP Bmp, int BmpPointX, int BmpPointY, int MaskHandle ) ;									// マスクデータにＢＭＰデータをマスクデータと見たてて転送
extern	int			NS_GraphImageBltToMask(			const BASEIMAGE *BaseImage, int ImageX, int ImageY, int MaskHandle ) ;								// マスクハンドルにBASEIMAGEデータを転送する
extern	int			NS_LoadMask(						const TCHAR *FileName ) ;																		// マスクデータをロードする
extern	int			NS_LoadMaskWithStrLen(					const TCHAR *FileName, size_t FileNameLength ) ;																			// 画像ファイルを読み込みマスクハンドルを作成する
extern	int			NS_LoadDivMask(						const TCHAR *FileName,                        int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;	// 画像ファイルを分割読み込みしてマスクハンドルを作成する
extern	int			NS_LoadDivMaskWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;	// 画像ファイルを分割読み込みしてマスクハンドルを作成する
extern	int			NS_CreateMaskFromMem(				const void *FileImage, int FileImageSize ) ;																		// メモリ上にある画像ファイルイメージを読み込みマスクハンドルを作成する
extern	int			NS_CreateDivMaskFromMem(			const void *FileImage, int FileImageSize, int AllNum, int XNum, int YNum, int XSize, int YSize, int *HandleArray ) ;	// メモリ上にある画像ファイルイメージを分割読み込みしてマスクハンドルを作成する
extern	int			NS_DrawMask(						int x, int y, int MaskHandle, int TransMode ) ;													// マスクをセットする
#ifndef DX_NON_FONT
extern	int			NS_DrawFormatStringMask(			int x, int y, int Flag,                 const TCHAR *FormatString, ... ) ;						// 書式指定ありの文字列をマスクスクリーンに描画する
extern	int			NS_DrawFormatStringMaskToHandle(	int x, int y, int Flag, int FontHandle, const TCHAR *FormatString, ... ) ;						// 書式指定ありの文字列をマスクスクリーンに描画する(フォントハンドル指定版)( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
extern	int			NS_DrawStringMask(					int x, int y, int Flag,                 const TCHAR *String ) ;									// 文字列をマスクスクリーンに描画する
extern	int			NS_DrawNStringMask(					int x, int y, int Flag,                 const TCHAR *String, size_t StringLength ) ;			// 文字列をマスクスクリーンに描画する
extern	int			NS_DrawStringMaskToHandle(			int x, int y, int Flag, int FontHandle, const TCHAR *String ) ;									// 文字列をマスクスクリーンに描画する( フォントハンドル指定版 )( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
extern	int			NS_DrawNStringMaskToHandle(			int x, int y, int Flag, int FontHandle, const TCHAR *String, size_t StringLength ) ;			// 文字列をマスクスクリーンに描画する( フォントハンドル指定版 )( SetFontCacheToTextureFlag( FALSE ) ; にして作成したフォントハンドルのみ使用可能 )
#endif // DX_NON_FONT
extern	int			NS_DrawFillMask(					int x1, int y1, int x2, int y2, int MaskHandle ) ;												// 指定のマスクを画面いっぱいに展開する	
extern	int			NS_SetMaskReverseEffectFlag(		int ReverseFlag ) ;																				// マスクの数値に対する効果を逆転させる

extern	int			NS_GetMaskScreenData(				int x1, int y1, int x2, int y2, int MaskHandle ) ;												// マスク画面上の描画状態を取得する
extern	int			NS_GetMaskUseFlag(					void ) ;																						// マスクスクリーンを使用中かフラグの取得

#endif // DX_NON_MASK

#endif // DX_NOTUSE_DRAWFUNCTION













// DxFont.cpp 関数プロトタイプ宣言

#ifndef DX_NON_FONT

// フォント、文字列描画関係関数
extern	int			NS_EnumFontName(                           TCHAR *NameBuffer, int NameBufferNum, int JapanOnlyFlag = TRUE ) ;								// 使用可能なフォントの名前をすべて列挙する( NameBuffer に 64バイト区切りで名前が格納されます )
extern	int			NS_EnumFontNameEx(                         TCHAR *NameBuffer, int NameBufferNum, int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;			// 使用可能なフォントの名前をすべて列挙する( NameBuffer に 64バイト区切りで名前が格納されます )
extern	int			NS_EnumFontNameEx2(						TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;	// 指定のフォント名のフォントを列挙する
extern	int			NS_EnumFontNameEx2WithStrLen(				TCHAR *NameBuffer, int NameBufferNum, const TCHAR *EnumFontName, size_t EnumFontNameLength, int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;	// 指定のフォント名のフォントを列挙する
extern	int			NS_CheckFontName(							const TCHAR *FontName,                        int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;												// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )
extern	int			NS_CheckFontNameWithStrLen(				const TCHAR *FontName, size_t FontNameLength, int CharSet = -1 /* DX_CHARSET_DEFAULT 等 */ ) ;												// 指定のフォント名のフォントが存在するかどうかをチェックする( 戻り値  TRUE:存在する  FALSE:存在しない )

extern	int			NS_InitFontToHandle(                       void ) ;																						// フォントのステータスをデフォルトに戻す

extern	int			NS_CreateFontToHandle(                     const TCHAR *FontName, int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE , int Handle = -1 ) ;			// 新しいフォントデータを作成
extern	int			NS_CreateFontToHandleWithStrLen(			const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE , int Handle = -1 ) ;		// フォントハンドルを作成する
extern	int			NS_LoadFontDataToHandle(					const TCHAR *FileName,                            int EdgeSize = 0 ) ;							// フォントデータファイルからフォントハンドルを作成する
extern	int			NS_LoadFontDataToHandleWithStrLen(			const TCHAR *FileName, size_t FileNameLength,     int EdgeSize = 0 ) ;							// フォントデータファイルからフォントハンドルを作成する
extern	int			NS_LoadFontDataFromMemToHandle(			const void *FontDataImage, int FontDataImageSize, int EdgeSize = -1 ) ;		// メモリ上のフォントデータファイルイメージからフォントハンドルを作成する
extern	int			NS_SetFontSpaceToHandle(                   int Pixel, int FontHandle ) ;																	// 字間を変更する
extern	int			NS_SetFontLineSpaceToHandle(               int Pixel, int FontHandle ) ;																	// フォントハンドルの行間を変更する
extern	int			NS_SetFontCharCodeFormatToHandle(			int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ , int FontHandle ) ;											// 指定のフォントハンドルを使用する関数の引数に渡す文字列の文字コード形式を設定する( UNICODE版では無効 )
extern	int			NS_SetDefaultFontState(                    const TCHAR *FontName, int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE ) ;	// デフォルトフォントハンドルの設定を変更する
extern	int			NS_SetDefaultFontStateWithStrLen(          const TCHAR *FontName, size_t FontNameLength, int Size, int Thick, int FontType = -1 , int CharSet = -1 , int EdgeSize = -1 , int Italic = FALSE ) ;	// デフォルトフォントハンドルの設定を変更する
extern	int			NS_DeleteFontToHandle(                     int FontHandle ) ;																				// フォントキャッシュの制御を終了する
extern	int			NS_SetFontLostFlag(                        int FontHandle, int *LostFlag ) ;																// 解放時に TRUE にするフラグへのポインタを設定する
extern	int			NS_AddFontImageToHandle(					int FontHandle, const TCHAR *Char, int GrHandle, int DrawX, int DrawY, int AddX ) ;				// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern	int			NS_AddFontImageToHandleWithStrLen(			int FontHandle, const TCHAR *Char, size_t CharLength, int GrHandle, int DrawX, int DrawY, int AddX ) ;	// 指定の文字の代わりに描画するグラフィックハンドルを登録する
extern	int			NS_SubFontImageToHandle(					int FontHandle, const TCHAR *Char                    ) ;												// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern	int			NS_SubFontImageToHandleWithStrLen(			int FontHandle, const TCHAR *Char, size_t CharLength ) ;												// 指定の文字の代わりに描画するグラフィックハンドルの登録を解除する
extern	int			NS_AddSubstitutionFontToHandle(			int FontHandle, int SubstitutionFontHandle, int DrawX, int DrawY ) ;							// 代替フォントハンドルを登録する
extern	int			NS_SubSubstitutionFontToHandle(			int FontHandle, int SubstitutionFontHandle ) ;													// 代替フォントハンドルの登録を解除する

extern	int			NS_SetFontSize(                            int FontSize ) ;																				// 描画するフォントのサイズをセットする
extern	int			NS_SetFontThickness(                       int ThickPal ) ;																				// フォントの太さをセット
extern	int			NS_SetFontSpace(                           int Pixel ) ;																					// 字間を変更する
extern	int			NS_GetFontSpace(                           void ) ;																						// デフォルトフォントハンドルの字間を取得する
extern	int			NS_SetFontLineSpace(                       int Pixel ) ;																					// デフォルトフォントハンドルの行間を変更する
extern	int			NS_GetFontLineSpace(                       void ) ;																						// デフォルトフォントハンドルの行間を取得する
extern	int			NS_SetFontCharCodeFormat(					int CharCodeFormat /* DX_CHARCODEFORMAT_SHIFTJIS 等 */ ) ;										// デフォルトフォントハンドルを使用する関数の引数に渡す文字列の文字コード形式を設定する( UNICODE版では無効 )

extern	int			NS_SetFontCacheToTextureFlag(              int Flag ) ;																					// フォントのキャッシュにテクスチャを使用するか、フラグをセットする
extern	int			NS_SetFontChacheToTextureFlag(             int Flag ) ;																					// フォントのキャッシュにテクスチャを使用するか、フラグをセットする(誤字版)
extern	int			NS_SetFontCacheCharNum(                    int CharNum ) ;																					// フォントキャッシュでキャッシュできる文字数を指定する
extern	int			NS_ChangeFont(                             const TCHAR *FontName, int CharSet = -1 /* DX_CHARSET_SHFTJIS 等 */ ) ;							// フォントを変更
extern	int			NS_ChangeFontWithStrLen(                   const TCHAR *FontName, size_t FontNameLength, int CharSet = -1 /* DX_CHARSET_SHFTJIS 等 */ ) ;	// デフォルトフォントハンドルで使用するフォントを変更
extern	int			NS_ChangeFontType(                         int FontType ) ;																				// フォントタイプの変更
extern	const TCHAR *NS_GetFontName(							void ) ;																						// デフォルトフォントハンドルのフォント名を取得する
// FontCacheStringDraw は DrawString を使ってください
extern	int			NS_FontCacheStringDrawToHandle(            int x, int y, const TCHAR *StrData, unsigned int Color, unsigned int EdgeColor, BASEIMAGE *DestImage, const RECT *ClipRect /* NULL 可 */ , int FontHandle, int VerticalFlag = FALSE , SIZE *DrawSizeP = NULL ) ;
extern	int			NS_FontCacheStringDrawToHandleWithStrLen(  int x, int y, const TCHAR *StrData, size_t StrDataLength, unsigned int Color, unsigned int EdgeColor, BASEIMAGE *DestImage, const RECT *ClipRect /* NULL 可 */ , int FontHandle, int VerticalFlag = FALSE , SIZE *DrawSizeP = NULL ) ;
extern	int			NS_FontBaseImageBlt(                       int x, int y, const TCHAR *StrData,                       BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage,                 int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			NS_FontBaseImageBltWithStrLen(             int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage,                 int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			NS_FontBaseImageBltToHandle(               int x, int y, const TCHAR *StrData,                       BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する
extern	int			NS_FontBaseImageBltToHandleWithStrLen(     int x, int y, const TCHAR *StrData, size_t StrDataLength, BASEIMAGE *DestImage, BASEIMAGE *DestEdgeImage, int FontHandle, int VerticalFlag = FALSE ) ;	// 基本イメージに文字列を描画する

extern	const TCHAR *NS_GetFontNameToHandle(					int FontHandle ) ;																				// フォントハンドルのフォント名を取得する
extern	int			NS_GetFontMaxWidth(                        void ) ;																						// 文字の最大幅を得る
extern	int			NS_GetFontMaxWidthToHandle(                int FontHandle ) ;																				// 文字の最大幅を得る
extern	int			NS_GetFontAscent(                          void ) ;																						// デフォルトフォントハンドルの描画位置からベースラインまでの高さを取得する
extern	int			NS_GetFontAscentToHandle(                  int FontHandle ) ;																				// フォントハンドルの描画位置からベースラインまでの高さを取得する
extern	int			NS_GetFontCharInfo(                        int FontHandle, const TCHAR *Char, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY ) ;		// 指定の文字の描画情報を取得する
extern	int			NS_GetFontCharInfoWithStrLen(              int FontHandle, const TCHAR *Char, size_t CharLength, int *DrawX, int *DrawY, int *NextCharX, int *SizeX, int *SizeY ) ;	// フォントハンドルの指定の文字の描画情報を取得する
extern	int			NS_GetDrawStringWidth(                     const TCHAR *String, int StrLen, int VerticalFlag = FALSE ) ;									// 文字列の幅を得る
extern	int			NS_GetDrawNStringWidth(                    const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の描画幅を取得する
extern	int			NS_GetDrawFormatStringWidth(               const TCHAR *FormatString, ... ) ;																// 書式付き文字列の描画幅を得る
extern	int			NS_GetDrawStringWidthToHandle(             const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag = FALSE ) ;					// 文字列の幅を得る
extern	int			NS_GetDrawNStringWidthToHandle(            const TCHAR   *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;						// フォントハンドルを使用した文字列の描画幅を取得する
extern	int			NS_GetDrawFormatStringWidthToHandle(       int FontHandle, const TCHAR *FormatString, ... ) ;												// 書式付き文字列の描画幅を得る
extern	int			NS_GetDrawExtendStringWidth(               double ExRateX, const TCHAR *String, int StrLen, int VerticalFlag = FALSE ) ;					// 文字列の幅を得る
extern	int			NS_GetDrawExtendNStringWidth(              double ExRateX, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;					// デフォルトフォントハンドルを使用した文字列の描画幅を取得する( 拡大率付き )
extern	int			NS_GetDrawExtendFormatStringWidth(         double ExRateX, const TCHAR *FormatString, ... ) ;												// 書式付き文字列の描画幅を得る
extern	int			NS_GetDrawExtendStringWidthToHandle(       double ExRateX, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag = FALSE ) ;	// 文字列の幅を得る
extern	int			NS_GetDrawExtendNStringWidthToHandle(      double ExRateX, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;			// フォントハンドルを使用した文字列の描画幅を取得する
extern	int			NS_GetDrawExtendFormatStringWidthToHandle( double ExRateX, int FontHandle, const TCHAR *FormatString, ... ) ;								// 書式付き文字列の描画幅を得る
extern	int			NS_GetDrawStringSize(                      int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, int StrLen, int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawNStringSize(                     int *SizeX, int *SizeY, int *LineCount, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;											// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawFormatStringSize(                int *SizeX, int *SizeY, int *LineCount, const TCHAR *FormatString, ... ) ;																// デフォルトフォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawExtendStringSize(                int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int VerticalFlag = FALSE ) ;	// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern	int			NS_GetDrawExtendNStringSize(               int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;			// デフォルトフォントハンドルを使用した文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern	int			NS_GetDrawExtendFormatStringSize(          int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *FormatString, ... ) ;								// デフォルトフォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する( 拡大率付き )
extern	int			NS_GetDrawStringSizeToHandle(              int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, int StrLen, int FontHandle, int VerticalFlag = FALSE ) ;									// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawNStringSizeToHandle(             int *SizeX, int *SizeY, int *LineCount, const TCHAR   *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;											// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawFormatStringSizeToHandle(        int *SizeX, int *SizeY, int *LineCount, int FontHandle, const TCHAR *FormatString, ... ) ;																// フォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawExtendStringSizeToHandle(        int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag = FALSE ) ;	// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawExtendNStringSizeToHandle(       int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;				// フォントハンドルを使用した文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawExtendFormatStringSizeToHandle(  int *SizeX, int *SizeY, int *LineCount, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... ) ;								// フォントハンドルを使用した書式付き文字列の描画幅・高さ・行数を取得する
extern	int			NS_GetDrawStringCharInfo(                  DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen, int VerticalFlag = FALSE ) ;													// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawNStringCharInfo(                 DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;									// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawFormatStringCharInfo(            DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawExtendStringCharInfo(            DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int VerticalFlag = FALSE ) ;					// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawNStringCharInfoToHandle(         DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;									// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawExtendFormatStringCharInfo(      DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *FormatString, ... ) ;											// デフォルトフォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawStringCharInfoToHandle(          DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag = FALSE ) ;												// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawExtendNStringCharInfoToHandle(   DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle, int VerticalFlag = FALSE ) ;	// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawFormatStringCharInfoToHandle(    DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, int FontHandle, const TCHAR *FormatString, ... ) ;																		// フォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawExtendStringCharInfoToHandle(    DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, int StrLen, int FontHandle, int VerticalFlag = FALSE ) ;				// フォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawExtendNStringCharInfo(           DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int VerticalFlag = FALSE ) ;	// デフォルトフォントハンドルを使用した文字列の１文字毎の情報を取得する
extern	int			NS_GetDrawExtendFormatStringCharInfoToHandle(  DRAWCHARINFO *InfoBuffer, size_t InfoBufferSize, double ExRateX, double ExRateY, int FontHandle, const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用した書式付き文字列の１文字毎の情報を取得する

extern	int			NS_GetFontStateToHandle(                   TCHAR *FontName, int *Size, int *Thick, int FontHandle, int *FontType = NULL , int *CharSet = NULL , int *EdgeSize = NULL , int *Italic = NULL ) ;		// フォントハンドルの情報を取得する
extern	int			NS_GetDefaultFontHandle(                   void ) ;																						// デフォルトのフォントのハンドルを得る
extern	int			NS_GetFontChacheToTextureFlag(             void ) ;																						// フォントにテクスチャキャッシュを使用するかどうかを取得する(誤字版)
extern	int			NS_SetFontCacheTextureColorBitDepth(		int ColorBitDepth ) ;																			// フォントのキャッシュとして使用するテクスチャのカラービット深度を設定する( 16 又は 32 のみ指定可能  デフォルトは 32 )
extern	int			NS_GetFontCacheTextureColorBitDepth(		void ) ;																						// フォントのキャッシュとして使用するテクスチャのカラービット深度を取得する
extern	int			NS_GetFontCacheToTextureFlag(              void ) ;																						// フォントにテクスチャキャッシュを使用するかどうかを取得する
extern	int			NS_CheckFontChacheToTextureFlag(           int FontHandle ) ;																				// 指定のフォントがテクスチャキャッシュを使用しているかどうかを得る(誤字版)
extern	int			NS_CheckFontCacheToTextureFlag(            int FontHandle ) ;																				// 指定のフォントがテクスチャキャッシュを使用しているかどうかを得る
extern	int			NS_CheckFontHandleValid(                   int FontHandle ) ;																				// 指定のフォントハンドルが有効か否か調べる
extern	int			NS_ClearFontCacheToHandle(					int FontHandle ) ;																				// フォントハンドルのキャッシュ情報を初期化する
extern	int			NS_MultiByteCharCheck(                     const char *Buf, int CharSet /* DX_CHARSET_SHFTJIS */ ) ;										// ２バイト文字か調べる( TRUE:２バイト文字  FALSE:１バイト文字 )
extern	int			NS_GetFontCacheCharNum(                    void ) ;																						// フォントキャッシュでキャッシュできる文字数を取得する( 戻り値  0:デフォルト  1以上:指定文字数 )
extern	int			NS_GetFontSize(                            void ) ;																						// フォントのサイズを得る
extern	int			NS_GetFontEdgeSize(                        void ) ;																						// デフォルトフォントハンドルの縁サイズを取得する
extern	int			NS_GetFontSizeToHandle(                    int FontHandle ) ;																				// フォントのサイズを得る
extern	int			NS_GetFontEdgeSizeToHandle(                int FontHandle ) ;																				// フォントハンドルの縁サイズを取得する
extern	int			NS_GetFontSpaceToHandle(                   int FontHandle ) ;																				// フォントハンドルの字間を取得する
extern	int			NS_GetFontLineSpaceToHandle(               int FontHandle ) ;																				// フォントハンドルの行間を取得する
extern	int			NS_SetFontCacheUsePremulAlphaFlag(         int Flag ) ;																					// フォントキャッシュとして保存する画像の形式を乗算済みαチャンネル付き画像にするかどうかを設定する( TRUE:乗算済みαを使用する  FLASE:乗算済みαを使用しない( デフォルト ) )
extern	int			NS_GetFontCacheUsePremulAlphaFlag(         void ) ;																						// フォントキャッシュとして保存する画像の形式を乗算済みαチャンネル付き画像にするかどうかを取得する
extern	int			NS_SetFontUseAdjustSizeFlag(               int Flag ) ;																					// フォントのサイズを補正する処理を行うかどうかを設定する( Flag  TRUE:行う( デフォルト )  FALSE:行わない )
extern	int			NS_GetFontUseAdjustSizeFlag(               void ) ;																						// フォントのサイズを補正する処理を行うかどうかを取得する

extern	HANDLE		NS_AddFontFile( const TCHAR *FontFilePath ) ;																			// 指定のフォントファイルをシステムに追加する( 戻り値  NULL:失敗  NULL以外:フォントハンドル( WindowsOS のものなので、ＤＸライブラリのフォントハンドルとは別物です ) )
extern	HANDLE		NS_AddFontFileWithStrLen(			const TCHAR *FontFilePath, size_t FontFilePathLength ) ;										// 指定のフォントファイルをシステムに追加する( 戻り値  NULL:失敗  NULL以外:フォントハンドル( WindowsOS のものなので、ＤＸライブラリのフォントハンドルとは別物です ) )
extern	HANDLE		NS_AddFontFileFromMem( const void *FontFileImage, int FontFileImageSize ) ;											// 指定のメモリアドレスに展開したフォントファイルイメージをシステムに追加する( 戻り値  NULL:失敗  NULL以外:フォントハンドル( WindowsOS のものなので、ＤＸライブラリのフォントハンドルとは別物です ) )
extern	int			NS_RemoveFontFile( HANDLE FontHandle ) ;																				// 指定のフォントハンドルをシステムから削除する( 引数は AddFontFile や AddFontFileFromMem の戻り値 )

#ifndef DX_NON_SAVEFUNCTION

extern	int			NS_CreateFontDataFile(						const TCHAR *SaveFilePath, const TCHAR *FontName, int Size, int BitDepth /* DX_FONTIMAGE_BIT_1等 */ , int Thick, int Italic = FALSE , int CharSet = -1 , const TCHAR *SaveCharaList = NULL ) ;				// フォントデータファイルを作成する
extern	int			NS_CreateFontDataFileWithStrLen(	const TCHAR *SaveFilePath, size_t SaveFilePathLength, const TCHAR *FontName, size_t FontNameLength, int Size, int BitDepth /* DX_FONTIMAGE_BIT_1等 */ , int Thick, int Italic = FALSE , int CharSet = -1 , const TCHAR *SaveCharaList = NULL , size_t SaveCharaListLength = 0 ) ;				// フォントデータファイルを作成する

#endif

// 文字列描画関数
extern	int			NS_DrawString(                             int x, int y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する
extern	int			NS_DrawNString(                            int x, int y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する
extern	int			NS_DrawVString(                            int x, int y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			NS_DrawNVString(                           int x, int y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			NS_DrawFormatString(                       int x, int y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を描画する
extern	int			NS_DrawFormatVString(                      int x, int y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を描画する( 縦書き )
extern	int			NS_DrawExtendString(                       int x, int y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画
extern	int			NS_DrawExtendNString(                      int x, int y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画
extern	int			NS_DrawExtendVString(                      int x, int y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )
extern	int			NS_DrawExtendNVString(                     int x, int y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;							// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )
extern	int			NS_DrawExtendFormatString(                 int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する
extern	int			NS_DrawExtendFormatVString(                int x, int y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																			// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )
extern	int			NS_DrawRotaString(							int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する
extern	int			NS_DrawRotaNString(						int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する
extern	int			NS_DrawRotaFormatString(					int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する
extern	int			NS_DrawModiString(							int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する
extern	int			NS_DrawModiNString(						int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する
extern	int			NS_DrawModiFormatString(					int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する

extern	int			NS_DrawStringF(                            float x, float y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			NS_DrawNStringF(                           float x, float y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			NS_DrawVStringF(                           float x, float y,                                              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawNVStringF(                          float x, float y,                                              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawFormatStringF(                      float x, float y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を描画する( 座標指定が float 版 )
extern	int			NS_DrawFormatVStringF(                     float x, float y,                                 unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawExtendStringF(                      float x, float y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 座標指定が float 版 )
extern	int			NS_DrawExtendNStringF(                     float x, float y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 座標指定が float 版 )
extern	int			NS_DrawExtendVStringF(                     float x, float y, double ExRateX, double ExRateY,              const TCHAR *String,                      unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawExtendNVStringF(                    float x, float y, double ExRateX, double ExRateY,              const TCHAR *String, size_t StringLength, unsigned int Color, unsigned int EdgeColor = 0 ) ;						// デフォルトフォントハンドルを使用して文字列の拡大描画( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawExtendFormatStringF(                float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する( 座標指定が float 版 )
extern	int			NS_DrawExtendFormatVStringF(               float x, float y, double ExRateX, double ExRateY, unsigned int Color, const TCHAR *FormatString, ... ) ;																		// デフォルトフォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawRotaStringF(						float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			NS_DrawRotaNStringF(						float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			NS_DrawRotaFormatStringF(					float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を回転描画する( 座標指定が float 版 )
extern	int			NS_DrawModiStringF(						float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			NS_DrawModiNStringF(						float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// デフォルトフォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			NS_DrawModiFormatStringF(					float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// デフォルトフォントハンドルを使用して書式指定文字列を変形描画する( 座標指定が float 版 )

extern	int			NS_DrawNumberToI(                          int x, int y,                          int    Num, int RisesNum, unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して整数型の数値を描画する
extern	int			NS_DrawNumberToF(                          int x, int y,                          double Num, int Length,   unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して浮動小数点型の数値を描画する
extern	int			NS_DrawNumberPlusToI(                      int x, int y, const TCHAR *NoteString, int    Num, int RisesNum, unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して整数型の数値とその説明の文字列を一度に描画する
extern 	int			NS_DrawNumberPlusToF(                      int x, int y, const TCHAR *NoteString, double Num, int Length,   unsigned int Color, unsigned int EdgeColor = 0 ) ;											// デフォルトフォントハンドルを使用して浮動小数点型の数値とその説明の文字列を一度に描画する

extern	int			NS_DrawStringToZBuffer(                    int x, int y, const TCHAR *String,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			NS_DrawNStringToZBuffer(                   int x, int y, const TCHAR *String, size_t StringLength,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			NS_DrawVStringToZBuffer(                   int x, int y, const TCHAR *String,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			NS_DrawNVStringToZBuffer(                  int x, int y, const TCHAR *String, size_t StringLength,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			NS_DrawFormatStringToZBuffer(              int x, int y,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;							// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を描画する
extern	int			NS_DrawFormatVStringToZBuffer(             int x, int y,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;							// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を描画する( 縦書き )
extern	int			NS_DrawExtendStringToZBuffer(              int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			NS_DrawExtendNStringToZBuffer(             int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			NS_DrawExtendVStringToZBuffer(             int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			NS_DrawExtendNVStringToZBuffer(            int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;										// デフォルトフォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			NS_DrawExtendFormatStringToZBuffer(        int x, int y, double ExRateX, double ExRateY,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;							// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する
extern	int			NS_DrawExtendFormatVStringToZBuffer(       int x, int y, double ExRateX, double ExRateY,                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;							// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する( 縦書き )
extern	int			NS_DrawRotaStringToZBuffer(				int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String                            ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			NS_DrawRotaNStringToZBuffer(				int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String,       size_t StringLength ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			NS_DrawRotaFormatStringToZBuffer(			int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *FormatString , ...                ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を回転描画する
extern	int			NS_DrawModiStringToZBuffer(				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String                            ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			NS_DrawModiNStringToZBuffer(				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *String,       size_t StringLength ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			NS_DrawModiFormatStringToZBuffer(			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4,                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag , const TCHAR *FormatString , ...                ) ;	// デフォルトフォントハンドルを使用してＺバッファに対して書式指定文字列を変形描画する


extern	int			NS_DrawStringToHandle(                     int x, int y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;											// フォントハンドルを使用して文字列を描画する
extern	int			NS_DrawNStringToHandle(                    int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;											// フォントハンドルを使用して文字列を描画する
extern	int			NS_DrawVStringToHandle(                    int x, int y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																		// フォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			NS_DrawNVStringToHandle(                   int x, int y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																		// フォントハンドルを使用して文字列を描画する( 縦書き )
extern	int			NS_DrawFormatStringToHandle(               int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																											// フォントハンドルを使用して書式指定文字列を描画する
extern	int			NS_DrawFormatVStringToHandle(              int x, int y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																											// フォントハンドルを使用して書式指定文字列を描画する( 縦書き )
extern	int			NS_DrawExtendStringToHandle(               int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;			// フォントハンドルを使用して文字列を拡大描画する
extern	int			NS_DrawExtendNStringToHandle(              int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;			// フォントハンドルを使用して文字列を拡大描画する
extern	int			NS_DrawExtendVStringToHandle(              int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;										// フォントハンドルを使用して文字列を拡大描画する( 縦書き )
extern	int			NS_DrawExtendNVStringToHandle(             int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;										// フォントハンドルを使用して文字列を拡大描画する( 縦書き )
extern	int			NS_DrawExtendFormatStringToHandle(         int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																			// フォントハンドルを使用して書式指定文字列を拡大描画する
extern	int			NS_DrawExtendFormatVStringToHandle(        int x, int y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																			// フォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )
extern	int			NS_DrawRotaStringToHandle(					int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String                            ) ;		// フォントハンドルを使用して文字列を回転描画する
extern	int			NS_DrawRotaNStringToHandle(				int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;		// フォントハンドルを使用して文字列を回転描画する
extern	int			NS_DrawRotaFormatStringToHandle(			int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ...                 ) ;		// フォントハンドルを使用して書式指定文字列を回転描画する
extern	int			NS_DrawModiStringToHandle(					int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String                            ) ;	// フォントハンドルを使用して文字列を変形描画する
extern	int			NS_DrawModiNStringToHandle(				int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;	// フォントハンドルを使用して文字列を変形描画する
extern	int			NS_DrawModiFormatStringToHandle(			int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ...                 ) ;	// フォントハンドルを使用して書式指定文字列を変形描画する

extern	int			NS_DrawStringFToHandle(                    float x, float y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;										// フォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			NS_DrawNStringFToHandle(                   float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;										// フォントハンドルを使用して文字列を描画する( 座標指定が float 版 )
extern	int			NS_DrawVStringFToHandle(                   float x, float y, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																	// フォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawNVStringFToHandle(                  float x, float y, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;																	// フォントハンドルを使用して文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawFormatStringFToHandle(              float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																										// フォントハンドルを使用して書式指定文字列を描画する( 座標指定が float 版 )
extern	int			NS_DrawFormatVStringFToHandle(             float x, float y, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																										// フォントハンドルを使用して書式指定文字列を描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawExtendStringFToHandle(              float x, float y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;		// フォントハンドルを使用して文字列を拡大描画する( 座標指定が float 版 )
extern	int			NS_DrawExtendNStringFToHandle(             float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE ) ;		// フォントハンドルを使用して文字列を拡大描画する( 座標指定が float 版 )
extern	int			NS_DrawExtendVStringFToHandle(             float x, float y, double ExRateX, double ExRateY, const TCHAR *String,                      unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;									// フォントハンドルを使用して文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawExtendNVStringFToHandle(            float x, float y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;									// フォントハンドルを使用して文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawExtendFormatStringFToHandle(        float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																		// フォントハンドルを使用して書式指定文字列を拡大描画する( 座標指定が float 版 )
extern	int			NS_DrawExtendFormatVStringFToHandle(       float x, float y, double ExRateX, double ExRateY, unsigned int Color, int FontHandle, const TCHAR *FormatString, ... ) ;																		// フォントハンドルを使用して書式指定文字列を拡大描画する( 縦書き )( 座標指定が float 版 )
extern	int			NS_DrawRotaStringFToHandle(				float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL                           ) ;		// フォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			NS_DrawRotaNStringFToHandle(				float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *String       = NULL , size_t StringLength = 0 ) ;		// フォントハンドルを使用して文字列を回転描画する( 座標指定が float 版 )
extern	int			NS_DrawRotaFormatStringFToHandle(			float x, float y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 , int VerticalFlag = FALSE , const TCHAR *FormatString = NULL , ...                     ) ;		// フォントハンドルを使用して書式指定文字列を回転描画する( 座標指定が float 版 )
extern	int			NS_DrawModiStringFToHandle(				float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String                            ) ;		// フォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			NS_DrawModiNStringFToHandle(				float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;		// フォントハンドルを使用して文字列を変形描画する( 座標指定が float 版 )
extern	int			NS_DrawModiFormatStringFToHandle(			float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, unsigned int Color, int FontHandle, unsigned int EdgeColor, int VerticalFlag, const TCHAR *FormatString, ...                 ) ;		// フォントハンドルを使用して書式指定文字列を変形描画する( 座標指定が float 版 )

extern	int			NS_DrawNumberToIToHandle(                  int x, int y,                          int    Num, int RisesNum, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して整数型の数値を描画する
extern	int			NS_DrawNumberToFToHandle(                  int x, int y,                          double Num, int Length,   unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して浮動小数点型の数値を描画する
extern	int			NS_DrawNumberPlusToIToHandle(              int x, int y, const TCHAR *NoteString, int    Num, int RisesNum, unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して整数型の数値とその説明の文字列を一度に描画する
extern	int			NS_DrawNumberPlusToFToHandle(              int x, int y, const TCHAR *NoteString, double Num, int Length,   unsigned int Color, int FontHandle, unsigned int EdgeColor = 0 ) ;											// フォントハンドルを使用して浮動小数点型の数値とその説明の文字列を一度に描画する

extern	int			NS_DrawStringToHandleToZBuffer(            int x, int y, const TCHAR *String,                      int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			NS_DrawNStringToHandleToZBuffer(           int x, int y, const TCHAR *String, size_t StringLength, int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を描画する
extern	int			NS_DrawVStringToHandleToZBuffer(           int x, int y, const TCHAR *String,                      int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			NS_DrawNVStringToHandleToZBuffer(          int x, int y, const TCHAR *String, size_t StringLength, int FontHandle,                                                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を描画する( 縦書き )
extern	int			NS_DrawFormatStringToHandleToZBuffer(      int x, int y, int FontHandle,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を描画する
extern	int			NS_DrawFormatVStringToHandleToZBuffer(     int x, int y, int FontHandle,                                                                                        int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を描画する( 縦書き )
extern	int			NS_DrawExtendStringToHandleToZBuffer(      int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			NS_DrawExtendNStringToHandleToZBuffer(     int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag = FALSE ) ;							// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する
extern	int			NS_DrawExtendVStringToHandleToZBuffer(     int x, int y, double ExRateX, double ExRateY, const TCHAR *String,                      int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			NS_DrawExtendNVStringToHandleToZBuffer(    int x, int y, double ExRateX, double ExRateY, const TCHAR *String, size_t StringLength, int FontHandle,                                   int WriteZMode /* DX_ZWRITE_MASK 等 */ ) ;													// フォントハンドルを使用してＺバッファに対して文字列を拡大描画する( 縦書き )
extern	int			NS_DrawExtendFormatStringToHandleToZBuffer(   int x, int y, double ExRateX, double ExRateY, int FontHandle,                                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する
extern	int			NS_DrawExtendFormatVStringToHandleToZBuffer(  int x, int y, double ExRateX, double ExRateY, int FontHandle,                                                      int WriteZMode /* DX_ZWRITE_MASK 等 */ , const TCHAR *FormatString, ... ) ;										// フォントハンドルを使用してＺバッファに対して書式指定文字列を拡大描画する( 縦書き )
extern	int			NS_DrawRotaStringToHandleToZBuffer(		int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String                            ) ;		// フォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			NS_DrawRotaNStringToHandleToZBuffer(		int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;		// フォントハンドルを使用してＺバッファに対して文字列を回転描画する
extern	int			NS_DrawRotaFormatStringToHandleToZBuffer(	int x, int y, double ExRateX, double ExRateY, double RotCenterX, double RotCenterY, double RotAngle, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *FormatString, ...                 ) ;		// フォントハンドルを使用してＺバッファに対して書式指定文字列を回転描画する
extern	int			NS_DrawModiStringToHandleToZBuffer(		int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String                            ) ;											// フォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			NS_DrawModiNStringToHandleToZBuffer(		int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *String,       size_t StringLength ) ;											// フォントハンドルを使用してＺバッファに対して文字列を変形描画する
extern	int			NS_DrawModiFormatStringToHandleToZBuffer(	int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int FontHandle, int WriteZMode /* DX_ZWRITE_MASK 等 */ , int VerticalFlag, const TCHAR *FormatString, ...                 ) ;											// フォントハンドルを使用してＺバッファに対して書式指定文字列を変形描画する

#endif // DX_NON_FONT













// DxMath.cpp 関数プロトタイプ宣言

// 演算ライブラリ
















// DxBaseImage.cpp 関数プロトタイプ宣言

// 基本イメージデータのロード＋ＤＩＢ関係
extern	int			NS_CreateGraphImageOrDIBGraph(    const TCHAR *FileName, const void *DataImage, int DataImageSize, int DataImageType /* LOADIMAGE_TYPE_FILE 等 */ , int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータ若しくはＢＭＰデータを構築する
extern	int			NS_CreateGraphImageOrDIBGraphWithStrLen(  const TCHAR *FileName, size_t FileNameLength, const void *DataImage, int DataImageSize, int DataImageType /* LOADIMAGE_TYPE_FILE 等 */ , int BmpFlag, int ReverseFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータ若しくはＢＭＰデータを構築する
extern	int			NS_CreateGraphImageType2(         STREAMDATA *Src, BASEIMAGE *Dest ) ;																																															// 画像データから構築したストリーム読み込み用データを使用して基本イメージデータを構築する
extern	int			NS_CreateBmpInfo(                 BITMAPINFO *BmpInfo, int Width, int Height, int Pitch, const void *SrcGrData, void **DestGrData ) ;																													// 24bitカラー形式のビットマップデータからＢＭＰデータを構築する
extern	int			NS_GetImageSize_File(             const TCHAR *FileName, int *SizeX, int *SizeY ) ;																																											// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			NS_GetImageSize_FileWithStrLen(   const TCHAR *FileName, size_t FileNameLength, int *SizeX, int *SizeY ) ;																																													// 画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern	int			NS_GetImageSize_Mem(              const void *FileImage, int FileImageSize, int *SizeX, int *SizeY ) ;																																							// メモリ上に展開された画像ファイルの横ピクセル数と縦ピクセル数を取得する
extern	unsigned int NS_GetGraphImageFullColorCode(   const BASEIMAGE *GraphImage, int x, int y ) ;																																												// 基本イメージデータの指定座標のフルカラーコードを取得する
extern	int			NS_CreateGraphImage_plus_Alpha(           const TCHAR *FileName,                        const void *RgbImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag ) ;	// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータを構築する
extern	int			NS_CreateGraphImage_plus_AlphaWithStrLen( const TCHAR *FileName, size_t FileNameLength, const void *RgbImage, int RgbImageSize, int RgbImageType, const void *AlphaImage, int AlphaImageSize, int AlphaImageType, BASEIMAGE *RgbGraphImage, BASEIMAGE *AlphaGraphImage, int ReverseFlag ) ;	// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージを読み込み、基本イメージデータを構築する
extern	int			NS_ReverseGraphImage(             BASEIMAGE *GraphImage ) ;																																																	// 基本イメージデータを左右反転する
#ifdef __WINDOWS__
extern	HBITMAP		NS_CreateDIBGraph(                const TCHAR *FileName,                                                                                                                                        int ReverseFlag,          COLORDATA *SrcColor ) ;				// 画像ファイルからＤＩＢデータを作成する
extern	HBITMAP		NS_CreateDIBGraphWithStrLen(					const TCHAR *FileName, size_t FileNameLength,                                                                                                                                        int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイルからＤＩＢデータを作成する
extern	HBITMAP		NS_CreateDIBGraphToMem(           const BITMAPINFO *BmpInfo, const void *GraphData,                                                                                                             int ReverseFlag,          COLORDATA *SrcColor ) ;				// ＢＭＰデータからＤＩＢデータクを作成する
extern	int			NS_CreateDIBGraph_plus_Alpha(     const TCHAR *FileName, HBITMAP *RGBBmp, HBITMAP *AlphaBmp,                                                                                                    int ReverseFlag = FALSE , COLORDATA *SrcColor = NULL ) ;		// 画像ファイルからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	int			NS_CreateDIBGraph_plus_AlphaWithStrLen(		const TCHAR *FileName, size_t FileNameLength, HBITMAP *RGBBmp, HBITMAP *AlphaBmp,                                                                                                    int ReverseFlag = FALSE , COLORDATA *SrcColor = NULL ) ;			// 画像ファイルからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	HBITMAP		NS_CreateDIBGraphVer2(            const TCHAR *FileName, const void *MemImage, int MemImageSize,                                             int ImageType,                                     int ReverseFlag,          COLORDATA *SrcColor ) ;				// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータを作成する
extern	HBITMAP		NS_CreateDIBGraphVer2WithStrLen(				const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize,                                             int ImageType,                                     int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータを作成する
extern	int			NS_CreateDIBGraphVer2_plus_Alpha(				const TCHAR *FileName,                        const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	int			NS_CreateDIBGraphVer2_plus_AlphaWithStrLen(	const TCHAR *FileName, size_t FileNameLength, const void *MemImage, int MemImageSize, const void *AlphaImage, int AlphaImageSize, int ImageType, HBITMAP *RGBBmp, HBITMAP *AlphaBmp, int ReverseFlag,          COLORDATA *SrcColor ) ;					// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージからＤＩＢデータとマスク用ＤＩＢデータを作成する
extern	int			NS_ConvBitmapToGraphImage(        const BITMAPINFO *BmpInfo, void *GraphData, BASEIMAGE *GraphImage, int CopyFlag ) ;																																		// ＢＭＰデータから基本イメージデータを構築する( Ret 0:正常終了  1:コピーを行った  -1:エラー )
extern	int			NS_ConvGraphImageToBitmap(        const BASEIMAGE *GraphImage, BITMAPINFO *BmpInfo, void **GraphData, int CopyFlag, int FullColorConv = TRUE ) ;																												// 基本イメージデータをＢＭＰデータに変換するGraphImage を ＢＭＰ に変換する(アルファデータはあっても無視される)( 戻り値　0:正常終了  1:コピーを行った  -1:エラー )
#endif // __WINDOWS__

//extern int		NS_AddUserGraphLoadFunction( int ( *UserLoadFunc )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ) ) ;																					// ユーザー定義の画像データ読み込み関数を登録する
//extern int		NS_AddUserGraphLoadFunction2( int ( *UserLoadFunc )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ) ) ;												// ユーザー定義の画像データ読み込み関数を登録する
//extern int		NS_AddUserGraphLoadFunction3( int ( *UserLoadFunc )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ) ;	// ユーザー定義の画像データ読み込み関数Ver3を登録する
extern	int			NS_AddUserGraphLoadFunction4( int (* UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ) ) ; 																								// ユーザー定義の画像データ読み込み関数Ver4を登録する
//extern int		NS_SubUserGraphLoadFunction( int ( *UserLoadFunc )( FILE *fp, BITMAPINFO **BmpInfo, void **GraphData ) ) ;																					// ユーザー定義の画像データ読み込み関数を登録から抹消する
//extern int		NS_SubUserGraphLoadFunction2( int ( *UserLoadFunc )( void *Image, int ImageSize, int ImageType, BITMAPINFO **BmpInfo, void **GraphData ) ) ;												// ユーザー定義の画像データ読み込み関数を登録から抹消する
//extern int		NS_SubUserGraphLoadFunction3( int ( *UserLoadFunc )( void *DataImage, int DataImageSize, int DataImageType, int BmpFlag, BASEIMAGE *BaseImage, BITMAPINFO **BmpInfo, void **GraphData ) ) ;	// ユーザー定義の画像データ読み込み関数Ver3を登録から抹消する
extern	int			NS_SubUserGraphLoadFunction4( int (* UserLoadFunc )( STREAMDATA *Src, BASEIMAGE *BaseImage ) ) ; 																								// ユーザー定義の画像データ読み込み関数Ver4を登録から抹消する

extern	int			NS_SetUseFastLoadFlag(            int Flag ) ;														// 高速読み込みルーチンを使用するかどうかを設定する( TRUE:使用する( デフォルト )  FALSE:使用しない )
extern	int			NS_SetGraphDataShavedMode(        int ShavedMode /* DX_SHAVEDMODE_NONE 等 */ ) ;					// 画像減色時の画像劣化緩和処理モードを設定する( デフォルトでは緩和処理無しの DX_SHAVEDMODE_NONE )
extern	int			NS_GetGraphDataShavedMode(        void ) ;															// 画像減色時の画像劣化緩和処理モードを取得する
extern	int			NS_SetUsePremulAlphaConvertLoad(  int UseFlag ) ;													// 画像ファイル読み込み時に乗算済みアルファ画像に変換するかどうかを設定する( TRUE:変換処理を行う  FALSE:変換処理を行わない( デフォルト ) )

// 基本イメージデータ構造体関係
extern	int			NS_CreateBaseImage(               const TCHAR *FileName, const void *FileImage, int FileImageSize, int DataType /*=LOADIMAGE_TYPE_FILE*/ , BASEIMAGE *BaseImage,  int ReverseFlag ) ;			// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern	int			NS_CreateBaseImageWithStrLen(       const TCHAR *FileName, size_t FileNameLength, const void *FileImage, int FileImageSize, int DataType /*=LOADIMAGE_TYPE_FILE*/ , BASEIMAGE *BaseImage,  int ReverseFlag ) ;			// 画像ファイル若しくはメモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern	int			NS_CreateGraphImage(                const TCHAR *FileName,                        const void *DataImage, int DataImageSize, int DataImageType,                      BASEIMAGE *GraphImage, int ReverseFlag ) ;			// CreateBaseImage の旧名称
extern	int			NS_CreateBaseImageToFile(           const TCHAR *FileName,                                                                                                          BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;	// 画像ファイルから基本イメージデータを構築する
extern	int			NS_CreateBaseImageToFileWithStrLen( const TCHAR *FileName, size_t FileNameLength,                                                                                   BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;	// 画像ファイルから基本イメージデータを構築する
extern	int			NS_CreateBaseImageToMem(                                 const void *FileImage, int FileImageSize,                                         BASEIMAGE *BaseImage,  int ReverseFlag = FALSE ) ;	// メモリ上に展開された画像ファイルイメージから基本イメージデータを構築する
extern	int			NS_CreateARGBF32ColorBaseImage(   int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// ＡＲＧＢ各チャンネル 32bit 浮動小数点型 カラーの基本イメージデータを作成する
extern	int			NS_CreateARGBF16ColorBaseImage(   int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// ＡＲＧＢ各チャンネル 16bit 浮動小数点型 カラーの基本イメージデータを作成する
extern	int			NS_CreateARGB8ColorBaseImage(     int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// ＡＲＧＢ８カラーの基本イメージデータを作成する
extern	int			NS_CreateXRGB8ColorBaseImage(     int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// ＸＲＧＢ８カラーの基本イメージデータを作成する
extern	int			NS_CreateRGB8ColorBaseImage(      int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// ＲＧＢ８カラーの基本イメージデータを作成する
extern	int			NS_CreateARGB4ColorBaseImage(     int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// ＡＲＧＢ４カラーの基本イメージデータを作成する
extern	int			NS_CreateA1R5G5B5ColorBaseImage(  int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// Ａ１Ｒ５Ｇ５Ｂ５カラーの基本イメージデータを作成する
extern	int			NS_CreateX1R5G5B5ColorBaseImage(  int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// Ｘ１Ｒ５Ｇ５Ｂ５カラーの基本イメージデータを作成する
extern	int			NS_CreateR5G5B5A1ColorBaseImage(  int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// Ｒ５Ｇ５Ｂ５Ａ１カラーの基本イメージデータを作成する
extern	int			NS_CreateR5G6B5ColorBaseImage(    int SizeX, int SizeY,                             BASEIMAGE *BaseImage ) ;																					// Ｒ５Ｇ６Ｂ５カラーの基本イメージデータを作成する
extern	int			NS_CreatePAL8ColorBaseImage(      int SizeX, int SizeY,                             BASEIMAGE *BaseImage, int UseAlpha = FALSE ) ;																// パレット８ビットカラーの基本イメージデータを作成する
extern	int			NS_CreateColorDataBaseImage(      int SizeX, int SizeY, const COLORDATA *ColorData, BASEIMAGE *BaseImage ) ;																					// 指定のカラーフォーマットの基本イメージデータを作成する
extern	int			NS_GetBaseImageGraphDataSize(     const BASEIMAGE *BaseImage ) ;																																// 基本イメージデータのイメージサイズを取得する( 単位：byte )
extern	int			NS_DerivationBaseImage(           const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, BASEIMAGE *NewBaseImage ) ;																		// 基本イメージデータの指定の部分だけを使う基本イメージデータの情報を作成する

extern	int			NS_ReleaseBaseImage(              BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータの後始末を行う
extern	int			NS_ReleaseGraphImage(             BASEIMAGE *GraphImage ) ;																																	// ReleaseBaseImage の旧名称

extern	int			NS_ConvertNormalFormatBaseImage(  BASEIMAGE *BaseImage, int ReleaseOrigGraphData = TRUE ) ;																									// DX_BASEIMAGE_FORMAT_NORMAL 以外の形式のイメージを DX_BASEIMAGE_FORMAT_NORMAL 形式のイメージに変換する
extern	int			NS_ConvertPremulAlphaBaseImage(   BASEIMAGE *BaseImage ) ;																																		// 通常のαチャンネル付き画像を乗算済みαチャンネル付き画像に変換する( ピクセルフォーマットが ARGB8 以外の場合は ARGB8 に変換されます )
extern	int			NS_ConvertInterpAlphaBaseImage(   BASEIMAGE *BaseImage ) ;																																		// 乗算済みαチャンネル付き画像を通常のαチャンネル付き画像に変換する( ピクセルフォーマットが ARGB8 以外の場合は ARGB8 に変換されます )

extern	int			NS_GetDrawScreenBaseImage(        int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage ) ;																										// 描画対象の画面から指定領域を基本イメージデータに転送する
extern	int			NS_GetDrawScreenBaseImageDestPos( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY ) ;																				// 描画対象の画面から指定領域を基本イメージデータに転送する( 転送先座標指定版 )
#ifdef __WINDOWS__
extern	int			NS_UpdateLayerdWindowForBaseImage(                    const BASEIMAGE *BaseImage ) ;																											// 基本イメージデータを使用して UpdateLayerdWindow を行う
extern	int			NS_UpdateLayerdWindowForBaseImageRect(                   const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 ) ;																		// 基本イメージデータの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int			NS_UpdateLayerdWindowForPremultipliedAlphaBaseImage(  const BASEIMAGE *BaseImage ) ;																											// 乗算済みアルファの基本イメージデータを使用して UpdateLayerdWindow を行う
extern	int			NS_UpdateLayerdWindowForPremultipliedAlphaBaseImageRect( const BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2 ) ;																		// 乗算済みアルファの基本イメージデータの指定の範囲を使用して UpdateLayerdWindow を行う

// デスクトップキャプチャ
extern	int			NS_GetDesktopScreenBaseImage( int x1, int y1, int x2, int y2, BASEIMAGE *BaseImage, int DestX, int DestY ) ;						// デスクトップの指定の領域を基本イメージデータに転送する
#endif // __WINDOWS__
extern	int			NS_FillBaseImage(                       BASEIMAGE *BaseImage, int r, int g, int b, int a ) ;																											// 基本イメージデータを指定の色で塗りつぶす
extern	int			NS_FillRectBaseImage(                   BASEIMAGE *BaseImage, int x, int y, int w, int h, int r, int g, int b, int a ) ;																				// 基本イメージデータの指定の領域を指定の色で塗りつぶす
extern	int			NS_ClearRectBaseImage(                  BASEIMAGE *BaseImage, int x, int y, int w, int h ) ;																											// 基本イメージデータの指定の領域を０クリアする
extern	int			NS_GetPaletteBaseImage(           const BASEIMAGE *BaseImage, int PaletteNo, int *r, int *g, int *b, int *a ) ;																						// 基本イメージデータのパレットを取得する
extern	int			NS_SetPaletteBaseImage(                 BASEIMAGE *BaseImage, int PaletteNo, int  r, int  g, int  b, int  a ) ;																						// 基本イメージデータのパレットをセットする
extern	int			NS_SetPixelPalCodeBaseImage(            BASEIMAGE *BaseImage, int x, int y, int palNo ) ;																												// 基本イメージデータの指定の座標の色コードを変更する(パレット画像用)
extern	int			NS_GetPixelPalCodeBaseImage(      const BASEIMAGE *BaseImage, int x, int y ) ;																															// 基本イメージデータの指定の座標の色コードを取得する(パレット画像用)
extern	int			NS_SetPixelBaseImage(                   BASEIMAGE *BaseImage, int x, int y, int  r, int  g, int  b, int  a ) ;																							// 基本イメージデータの指定の座標の色を変更する(各色要素は０〜２５５)
extern	int			NS_SetPixelBaseImageF(                  BASEIMAGE *BaseImage, int x, int y, float  r, float  g, float  b, float  a ) ;																					// 基本イメージデータの指定の座標の色を変更する(各色要素は浮動小数点数)
extern	int			NS_GetPixelBaseImage(             const BASEIMAGE *BaseImage, int x, int y, int *r, int *g, int *b, int *a ) ;																							// 基本イメージデータの指定の座標の色を取得する(各色要素は０〜２５５)
extern	int			NS_GetPixelBaseImageF(            const BASEIMAGE *BaseImage, int x, int y, float *r, float *g, float *b, float *a ) ;																					// 基本イメージデータの指定の座標の色を取得する(各色要素は浮動小数点数)
extern	int			NS_DrawLineBaseImage(                   BASEIMAGE *BaseImage, int x1, int y1, int x2, int y2, int r, int g, int b, int a ) ;																			// 基本イメージデータの指定の座標に線を描画する(各色要素は０〜２５５)
extern	int			NS_DrawCircleBaseImage(                 BASEIMAGE *BaseImage, int x, int y, int radius, int r, int g, int b, int a, int FillFlag = TRUE ) ;															// 基本イメージデータの指定の座標に円を描画する(各色要素は０〜２５５)
extern	int			NS_BltBaseImage(                        int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage ) ;									// 基本イメージデータを別の基本イメージデータに転送する
extern	int			NS_BltBaseImage(                                                                        int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage ) ;									// 基本イメージデータを別の基本イメージデータに転送する
extern	int			NS_BltBaseImageWithTransColor(          int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Tr, int Tg, int Tb, int Ta ) ;	// 基本イメージデータを別の基本イメージデータに透過色処理付きで転送する
extern	int			NS_BltBaseImageWithAlphaBlend(          int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int DestX, int DestY, BASEIMAGE *SrcBaseImage, BASEIMAGE *DestBaseImage, int Opacity = 255 ) ;					// 基本イメージデータを別の基本イメージデータにアルファ値のブレンドを考慮した上で転送する( Opacity は透明度 : 0( 完全透明 ) 〜 255( 完全不透明 ) )( 出力先が ARGB8 形式以外の場合はエラーになります )
extern	int			NS_ReverseBaseImageH(                   BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータを左右反転する
extern	int			NS_ReverseBaseImageV(                   BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータを上下反転する
extern	int			NS_ReverseBaseImage(                    BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータを上下左右反転する
extern	int			NS_CheckPixelAlphaBaseImage(      const BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータに含まれるピクセルのアルファ値をチェックする( 戻り値   -1:エラー  0:画像にアルファ成分が無い  1:画像にアルファ成分があり、すべて最大(255)値  2:画像にアルファ成分があり、存在するアルファ値は最小(0)と最大(255)もしくは最小(0)のみ　3:画像にアルファ成分があり、最小と最大以外の中間の値がある )  
extern	int			NS_GetBaseImageUseMaxPaletteNo(   const BASEIMAGE *BaseImage ) ;																																		// 基本イメージデータで使用されているパレット番号の最大値を取得する( パレット画像では無い場合は -1 が返る )

#ifndef DX_NON_JPEGREAD
extern	int			NS_ReadJpegExif(                  const TCHAR *JpegFilePath, BYTE *ExifBuffer, size_t ExifBufferSize ) ;																									// JPEGファイルの Exif情報を取得する、ExifBuffer を NULL に渡すと、戻り値の情報のサイズのみ取得できます( 戻り値  -1:エラー  -1以外：Exif情報のサイズ )
extern	int			NS_ReadJpegExifWithStrLen(          const TCHAR *JpegFilePath, size_t JpegFilePathLength, BYTE *ExifBuffer_Array, size_t ExifBufferSize ) ;																		// JPEGファイルの Exif情報を取得する、ExifBuffer を NULL に渡すと、戻り値の情報のサイズのみ取得できます( 戻り値  -1:エラー  -1以外：Exif情報のサイズ )
#endif // DX_NON_JPEGREAD

#ifndef DX_NON_SAVEFUNCTION

extern	int			NS_SaveBaseImageToBmp(            const TCHAR *FilePath, const BASEIMAGE *BaseImage ) ;																										// 基本イメージデータをＢＭＰ画像として保存する
extern	int			NS_SaveBaseImageToBmpWithStrLen(    const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage ) ;																										// 基本イメージデータをＢＭＰ画像として保存する
extern	int			NS_SaveBaseImageToDds(              const TCHAR *FilePath,                        const BASEIMAGE *BaseImage ) ;																										// 基本イメージデータをＤＤＳ画像として保存する
extern	int			NS_SaveBaseImageToDdsWithStrLen(    const TCHAR *FilePath, size_t FilePathLength, const BASEIMAGE *BaseImage ) ;																										// 基本イメージデータをＤＤＳ画像として保存する
#ifndef DX_NON_PNGREAD
extern	int			NS_SaveBaseImageToPng(              const TCHAR *FilePath,                        BASEIMAGE *BaseImage, int CompressionLevel ) ;																					// 基本イメージデータをＰＮＧ画像として保存する
extern	int			NS_SaveBaseImageToPngWithStrLen(    const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int CompressionLevel ) ;																					// 基本イメージデータをＰＮＧ画像として保存する
#endif // DX_NON_PNGREAD
#ifndef DX_NON_JPEGREAD
extern	int			NS_SaveBaseImageToJpeg(             const TCHAR *FilePath,                        BASEIMAGE *BaseImage, int Quality, int Sample2x1 ) ;																			// 基本イメージデータをＪＰＥＧ画像として保存する
extern	int			NS_SaveBaseImageToJpegWithStrLen(   const TCHAR *FilePath, size_t FilePathLength, BASEIMAGE *BaseImage, int Quality, int Sample2x1 ) ;																			// 基本イメージデータをＪＰＥＧ画像として保存する
#endif // DX_NON_JPEGREAD

#endif // DX_NON_SAVEFUNCTION

// 基本イメージ描画
extern	int			NS_DrawBaseImage(					int x, int y, BASEIMAGE *BaseImage ) ;																												// 基本イメージデータを描画する

// カラーマッチングしながらイメージデータ間転送を行う Ver2
extern int			NS_GraphColorMatchBltVer2(       void *DestGraphData, int DestPitch,  const COLORDATA *DestColorData,
											const void *SrcGraphData,  int SrcPitch,   const COLORDATA *SrcColorData,
											const void *AlphaMask,     int AlphaPitch, const COLORDATA *AlphaColorData,
											POINT DestPoint, const RECT *SrcRect, int ReverseFlag,
											int TransColorAlphaTestFlag, unsigned int TransColor,
											int ImageShavedMode, int AlphaOnlyFlag = FALSE ,
											int RedIsAlphaFlag = FALSE , int TransColorNoMoveFlag = FALSE ,
											int Pal8ColorMatch = FALSE ) ;


// 色情報取得関係
extern	COLOR_F			NS_GetColorF(              float Red, float Green, float Blue, float Alpha ) ;												// 浮動小数点型のカラー値を作成する
extern	COLOR_U8		NS_GetColorU8(             int Red, int Green, int Blue, int Alpha ) ;														// 符号なし整数８ビットのカラー値を作成する
extern	unsigned int	NS_GetColor(               int Red, int Green, int Blue ) ;																	// DrawPixel 等の描画関数で使用するカラー値を取得する
extern	int				NS_GetColor2(              unsigned int Color, int *Red, int *Green, int *Blue ) ;													// カラー値から赤、緑、青の値を取得する
extern	unsigned int	NS_GetColor3(              const COLORDATA *ColorData, int Red, int Green, int Blue, int Alpha = 255 ) ;					// 指定のピクセルフォーマットに対応したカラー値を得る
extern	unsigned int	NS_GetColor4(              const COLORDATA *DestColorData, const COLORDATA* SrcColorData, unsigned int SrcColor ) ;				// 指定のカラーフォーマットのカラー値を別のカラーフォーマットのカラー値に変換する
extern	int				NS_GetColor5(              const COLORDATA *ColorData, unsigned int Color, int *Red, int *Green, int *Blue, int *Alpha = NULL ) ;	// 指定のカラーフォーマットのカラー値を赤、緑、青、アルファの値を取得する
extern	int				NS_CreatePaletteColorData( COLORDATA *ColorDataBuf ) ;																		// パレットカラーのカラーフォーマットを構築する
extern	int				NS_CreateARGBF32ColorData( COLORDATA *ColorDataBuf ) ;																		// ＡＲＧＢ各チャンネル 32bit 浮動小数点型カラーのカラーフォーマットを構築する
extern	int				NS_CreateARGBF16ColorData( COLORDATA *ColorDataBuf ) ;																		// ＡＲＧＢ各チャンネル 16bit 浮動小数点型カラーのカラーフォーマットを構築する
extern	int				NS_CreateXRGB8ColorData(   COLORDATA *ColorDataBuf ) ;																		// ＸＲＧＢ８カラーのカラーフォーマットを構築する
extern	int				NS_CreateARGB8ColorData(   COLORDATA *ColorDataBuf ) ;																		// ＡＲＧＢ８カラーのカラーフォーマットを構築する
extern	int				NS_CreateARGB4ColorData(   COLORDATA *ColorDataBuf ) ;																		// ＡＲＧＢ４カラーのカラーフォーマットを構築する
extern	int				NS_CreateA1R5G5B5ColorData( COLORDATA *ColorDataBuf ) ;																		// Ａ１Ｒ５Ｇ５Ｂ５カラーのカラーフォーマットを構築する
extern	int				NS_CreateX1R5G5B5ColorData( COLORDATA *ColorDataBuf ) ;																		// Ｘ１Ｒ５Ｇ５Ｂ５カラーのカラーフォーマットを構築する
extern	int				NS_CreateR5G5B5A1ColorData( COLORDATA *ColorDataBuf ) ;																		// Ｒ５Ｇ５Ｂ５Ａ１カラーのカラーフォーマットを構築する
extern	int				NS_CreateR5G6B5ColorData(   COLORDATA *ColorDataBuf ) ;																		// Ｒ５Ｇ６Ｂ５カラーのカラーフォーマットを構築する
extern	int				NS_CreateFullColorData(    COLORDATA *ColorDataBuf ) ;																		// ２４ビットカラーのカラーフォーマットを構築する
extern	int				NS_CreateGrayColorData(    COLORDATA *ColorDataBuf ) ;																		// グレースケールのカラーフォーマットを構築する
extern	int				NS_CreatePal8ColorData(    COLORDATA *ColorDataBuf, int UseAlpha = FALSE ) ;													// パレット２５６色のカラーフォーマットを構築する
extern	int				NS_CreateColorData(        COLORDATA *ColorDataBuf, int ColorBitDepth,
													DWORD RedMask, DWORD GreenMask, DWORD BlueMask, DWORD AlphaMask,
													int ChannelNum = 0, int ChannelBitDepth = 0, int FloatTypeFlag = FALSE ) ;						// カラーフォーマットを作成する
extern	void			NS_SetColorDataNoneMask(   COLORDATA *ColorData ) ;																		// NoneMask 以外の要素を埋めた COLORDATA 構造体の情報を元に NoneMask をセットする
extern	int				NS_CmpColorData(           const COLORDATA *ColorData1, const COLORDATA *ColorData2 ) ;									// 二つのカラーフォーマットが等しいかどうか調べる( 戻り値　TRUE:等しい  FALSE:等しくない )














// DxSoftImage.cpp関数プロトタイプ宣言
#ifndef DX_NON_SOFTIMAGE
extern	int			NS_InitSoftImage(                   void ) ;																			// ソフトウエアで扱うイメージを全て解放する
extern	int			NS_LoadSoftImage(                   const TCHAR *FileName ) ;															// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern	int			NS_LoadSoftImageWithStrLen(              const TCHAR *FileName, size_t FileNameLength ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern	int			NS_LoadARGB8ColorSoftImage(              const TCHAR *FileName                        ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int			NS_LoadARGB8ColorSoftImageWithStrLen(    const TCHAR *FileName, size_t FileNameLength ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int			NS_LoadXRGB8ColorSoftImage(              const TCHAR *FileName                        ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern	int			NS_LoadXRGB8ColorSoftImageWithStrLen(    const TCHAR *FileName, size_t FileNameLength ) ;									// 画像ファイルを読み込みソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern	int			NS_LoadSoftImageToMem(              const void *FileImage, int FileImageSize ) ;										// メモリ上に展開された画像ファイルイメージからソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )
extern	int			NS_LoadARGB8ColorSoftImageToMem(    const void *FileImage, int FileImageSize ) ;										// メモリ上に展開された画像ファイルイメージからソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が RGBA8 以外のフォーマットだった場合は RGBA8 カラーに変換 )
extern	int			NS_LoadXRGB8ColorSoftImageToMem(    const void *FileImage, int FileImageSize ) ;										// メモリ上に展開された画像ファイルイメージからソフトウエアイメージハンドルを作成する( -1:エラー  -1以外:イメージハンドル )( 読み込んだ画像が XGBA8 以外のフォーマットだった場合は XGBA8 カラーに変換 )
extern	int			NS_MakeSoftImage(                   int SizeX, int SizeY ) ;															// ソフトウエアで扱うイメージの作成( -1:エラー  -1以外:イメージハンドル )
extern	int			NS_MakeARGBF32ColorSoftImage(       int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( RGBA 各チャンネル 32bit 浮動小数点型 カラー )
extern	int			NS_MakeARGBF16ColorSoftImage(       int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( RGBA 各チャンネル 16bit 浮動小数点型 カラー )
extern	int			NS_MakeARGB8ColorSoftImage(         int SizeX, int SizeY ) ;															// ソフトウエアで扱うイメージの作成( RGBA8 カラー )
extern	int			NS_MakeXRGB8ColorSoftImage(         int SizeX, int SizeY ) ;															// ソフトウエアで扱うイメージの作成( XRGB8 カラー )
extern	int			NS_MakeARGB4ColorSoftImage(         int SizeX, int SizeY ) ;															// ソフトウエアで扱うイメージの作成( ARGB4 カラー )
extern	int			NS_MakeA1R5G5B5ColorSoftImage(      int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( A1R5G5B5 カラー )
extern	int			NS_MakeX1R5G5B5ColorSoftImage(      int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( X1R5G5B5 カラー )
extern	int			NS_MakeR5G5B5A1ColorSoftImage(      int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( R5G5B5A1 カラー )
extern	int			NS_MakeR5G6B5ColorSoftImage(        int SizeX, int SizeY ) ;															// ソフトウエアイメージハンドルの作成( R5G6B5 カラー )
extern	int			NS_MakeRGB8ColorSoftImage(          int SizeX, int SizeY ) ;															// ソフトウエアで扱うイメージの作成( RGB8 カラー )
extern	int			NS_MakePAL8ColorSoftImage(          int SizeX, int SizeY, int UseAlpha = FALSE ) ;										// ソフトウエアで扱うイメージの作成( パレット２５６色 カラー )

extern	int			NS_DeleteSoftImage(                 int SIHandle ) ;																	// ソフトウエアで扱うイメージの解放

extern	int			NS_GetSoftImageSize(                int SIHandle, int *Width, int *Height ) ;											// ソフトウエアで扱うイメージのサイズを取得する
extern	int			NS_CheckPaletteSoftImage(           int SIHandle ) ;																	// ソフトウエアで扱うイメージがパレット画像かどうかを取得する( TRUE:パレット画像  FALSE:パレット画像じゃない )
extern	int			NS_CheckAlphaSoftImage(             int SIHandle ) ;																	// ソフトウエアで扱うイメージのフォーマットにα要素があるかどうかを取得する( TRUE:ある  FALSE:ない )
extern	int			NS_CheckPixelAlphaSoftImage(        int SIHandle ) ;																	// ソフトウエアで扱うイメージに含まれるピクセルのアルファ値をチェックする( 戻り値   -1:エラー  0:画像にアルファ成分が無い  1:画像にアルファ成分があり、すべて最大(255)値  2:画像にアルファ成分があり、存在するアルファ値は最小(0)と最大(255)もしくは最小(0)のみ　3:画像にアルファ成分があり、最小と最大以外の中間の値がある )  

extern	int			NS_GetDrawScreenSoftImage(          int x1, int y1, int x2, int y2, int SIHandle ) ;									// 描画対象の画面から指定領域のグラフィックをソフトウエアで扱うイメージに転送する
extern	int			NS_GetDrawScreenSoftImageDestPos(   int x1, int y1, int x2, int y2, int SIHandle, int DestX, int DestY ) ;				// 描画対象の画面から指定領域をソフトウエアイメージハンドルに転送する( 転送先座標指定版 )
#ifdef __WINDOWS__
extern	int			NS_UpdateLayerdWindowForSoftImage(                       int SIHandle ) ;												// ソフトウエアイメージハンドルを使用して UpdateLayerdWindow を行う
extern	int			NS_UpdateLayerdWindowForSoftImageRect(                   int SIHandle, int x1, int y1, int x2, int y2 ) ;				// ソフトウエアイメージハンドルの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int			NS_UpdateLayerdWindowForPremultipliedAlphaSoftImage(     int SIHandle ) ;												// 乗算済みアルファのソフトウエアイメージハンドルを使用して UpdateLayerdWindow を行う
extern	int			NS_UpdateLayerdWindowForPremultipliedAlphaSoftImageRect( int SIHandle, int x1, int y1, int x2, int y2 ) ;				// 乗算済みアルファのソフトウエアイメージハンドルの指定の範囲を使用して UpdateLayerdWindow を行う
extern	int			NS_GetDesktopScreenSoftImage( int x1, int y1, int x2, int y2, int SIHandle,         int DestX, int DestY ) ;						// デスクトップの指定の領域をソフトウエアイメージハンドルに転送する
#endif // __WINDOWS__
extern	int			NS_FillSoftImage(                   int SIHandle, int r, int g, int b, int a ) ;										// ソフトウエアで扱うイメージを指定色で塗りつぶす(各色要素は０〜２５５)
extern	int			NS_ClearRectSoftImage(              int SIHandle, int x, int y, int w, int h ) ;										// ソフトウエアで扱うイメージの指定の領域を０クリアする
extern	int			NS_GetPaletteSoftImage(             int SIHandle, int PaletteNo, int *r, int *g, int *b, int *a ) ;					// ソフトウエアで扱うイメージのパレットを取得する(各色要素は０〜２５５)
extern	int			NS_SetPaletteSoftImage(             int SIHandle, int PaletteNo, int  r, int  g, int  b, int  a ) ;					// ソフトウエアで扱うイメージのパレットをセットする(各色要素は０〜２５５)
extern	int			NS_DrawPixelPalCodeSoftImage(       int SIHandle, int x, int y, int palNo ) ;											// ソフトウエアで扱うイメージの指定座標にドットを描画する(パレット画像用、有効値は０〜２５５)
extern	int			NS_GetPixelPalCodeSoftImage(        int SIHandle, int x, int y ) ;														// ソフトウエアで扱うイメージの指定座標の色コードを取得する(パレット画像用、戻り値は０〜２５５)
extern	void		*NS_GetImageAddressSoftImage(       int SIHandle ) ;																	// ソフトウエアで扱うイメージの実イメージが格納されているメモリ領域の先頭アドレスを取得する
extern	int			NS_GetPitchSoftImage(               int SIHandle ) ;																	// ソフトウエアイメージハンドルのメモリに格納されている画像のピッチ( 1ライン辺りのバイト数 )を取得する
extern	int			NS_DrawPixelSoftImage(              int SIHandle, int x, int y, int  r, int  g, int  b, int  a ) ;						// ソフトウエアで扱うイメージの指定座標にドットを描画する(各色要素は０〜２５５)
extern	int			NS_DrawPixelSoftImageF(             int SIHandle, int x, int y, float  r, float  g, float  b, float  a ) ;				// ソフトウエアイメージハンドルの指定座標にドットを描画する(各色要素は浮動小数点数)
extern	void		NS_DrawPixelSoftImage_Unsafe_XRGB8( int SIHandle, int x, int y, int  r, int  g, int  b ) ;								// ソフトウエアで扱うイメージの指定座標にドットを描画する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や ARGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	void		NS_DrawPixelSoftImage_Unsafe_ARGB8( int SIHandle, int x, int y, int  r, int  g, int  b, int  a ) ;						// ソフトウエアで扱うイメージの指定座標にドットを描画する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や XRGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	int			NS_GetPixelSoftImage(               int SIHandle, int x, int y, int *r, int *g, int *b, int *a ) ;						// ソフトウエアで扱うイメージの指定座標の色を取得する(各色要素は０〜２５５)
extern	int			NS_GetPixelSoftImageF(              int SIHandle, int x, int y, float *r, float *g, float *b, float *a ) ;				// ソフトウエアイメージハンドルの指定座標の色を取得する(各色要素は浮動小数点数)
extern	void		NS_GetPixelSoftImage_Unsafe_XRGB8(  int SIHandle, int x, int y, int *r, int *g, int *b ) ;								// ソフトウエアで扱うイメージの指定座標の色を取得する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や XRGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	void		NS_GetPixelSoftImage_Unsafe_ARGB8(  int SIHandle, int x, int y, int *r, int *g, int *b, int *a ) ;						// ソフトウエアで扱うイメージの指定座標の色を取得する(各色要素は０〜２５５)、エラーチェックをしない代わりに高速ですが、範囲外の座標や ARGB8 以外のフォーマットのソフトハンドルを渡すと不正なメモリアクセスで強制終了します
extern	int			NS_DrawLineSoftImage(               int SIHandle, int x1, int y1, int x2, int y2, int r, int g, int b, int a ) ;		// ソフトウエアで扱うイメージの指定座標に線を描画する(各色要素は０〜２５５)
extern	int			NS_DrawCircleSoftImage(             int SIHandle, int x, int y, int radius, int r, int g, int b, int a, int FillFlag = TRUE ) ;	// ソフトウエアイメージハンドルの指定座標に円を描画する(各色要素は０〜２５５)
extern	int			NS_BltSoftImage(                    int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle ) ;									// ソフトウエアで扱うイメージを別のイメージ上に転送する
extern	int			NS_BltSoftImageWithTransColor(      int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Tr, int Tg, int Tb, int Ta ) ;	// ソフトウエアで扱うイメージを透過色処理付きで転送する
extern	int			NS_BltSoftImageWithAlphaBlend(      int SrcX, int SrcY, int SrcSizeX, int SrcSizeY, int SrcSIHandle, int DestX, int DestY, int DestSIHandle, int Opacity = 255 ) ;					// ソフトウエアで扱うイメージをアルファ値のブレンドを考慮した上で転送する( Opacity は透明度 : 0( 完全透明 ) 〜 255( 完全不透明 ) )( 出力先が ARGB8 形式以外の場合はエラーになります )
extern	int			NS_ReverseSoftImageH(               int SIHandle ) ;																	// ソフトウエアで扱うイメージの左右を反転する
extern	int			NS_ReverseSoftImageV(               int SIHandle ) ;																	// ソフトウエアで扱うイメージの上下を反転する
extern	int			NS_ReverseSoftImage(                int SIHandle ) ;																	// ソフトウエアで扱うイメージの上下左右を反転する

#ifndef DX_NON_FONT
extern	int			NS_BltStringSoftImage(              int x, int y, const TCHAR *StrData, int DestSIHandle, int DestEdgeSIHandle = -1 ,                                        int VerticalFlag = FALSE ) ;		// ソフトウエアで扱うイメージに文字列イメージを転送する
extern	int			NS_BltStringSoftImageWithStrLen(         int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle = -1 ,                                        int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( デフォルトフォントハンドルを使用する )
extern	int			NS_BltStringSoftImageToHandle(           int x, int y, const TCHAR *StrData,                       int DestSIHandle, int DestEdgeSIHandle /* 縁が必要ない場合は -1 */ , int FontHandle, int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( フォントハンドル使用版 )
extern	int			NS_BltStringSoftImageToHandleWithStrLen( int x, int y, const TCHAR *StrData, size_t StrDataLength, int DestSIHandle, int DestEdgeSIHandle /* 縁が必要ない場合は -1 */ , int FontHandle, int VerticalFlag = FALSE ) ;		// ソフトウエアイメージハンドルに文字列を描画する( フォントハンドル使用版 )
#endif // DX_NON_FONT

extern	int			NS_DrawSoftImage(                   int x, int y, int SIHandle ) ;														// ソフトウエアで扱うイメージを画面に描画する

#ifndef DX_NON_SAVEFUNCTION

extern	int			NS_SaveSoftImageToBmp(              const TCHAR *FilePath, int SIHandle ) ;											// ソフトウエアで扱うイメージをＢＭＰ画像ファイルとして保存する
extern	int			NS_SaveSoftImageToBmpWithStrLen(         const TCHAR *FilePath, size_t FilePathLength, int SIHandle ) ;											// ソフトウエアイメージハンドルをＢＭＰ画像ファイルとして保存する
extern	int			NS_SaveSoftImageToDds(                   const TCHAR *FilePath,                        int SIHandle ) ;											// ソフトウエアイメージハンドルをＤＤＳ画像ファイルとして保存する
extern	int			NS_SaveSoftImageToDdsWithStrLen(         const TCHAR *FilePath, size_t FilePathLength, int SIHandle ) ;											// ソフトウエアイメージハンドルをＤＤＳ画像ファイルとして保存する
#ifndef DX_NON_PNGREAD
extern	int			NS_SaveSoftImageToPng(              const TCHAR *FilePath, int SIHandle, int CompressionLevel ) ;						// ソフトウエアで扱うイメージをＰＮＧ画像ファイルとして保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
extern	int			NS_SaveSoftImageToPngWithStrLen(         const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int CompressionLevel ) ;						// ソフトウエアイメージハンドルをＰＮＧ画像ファイルとして保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0〜9
#endif
#ifndef DX_NON_JPEGREAD
extern	int			NS_SaveSoftImageToJpeg(             const TCHAR *FilePath, int SIHandle, int Quality, int Sample2x1 ) ;				// ソフトウエアで扱うイメージをＪＰＥＧ画像ファイルとして保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
extern	int			NS_SaveSoftImageToJpegWithStrLen(        const TCHAR *FilePath, size_t FilePathLength, int SIHandle, int Quality, int Sample2x1 ) ;				// ソフトウエアイメージハンドルをＪＰＥＧ画像ファイルとして保存する Quality = 画質、値が大きいほど低圧縮高画質,0〜100 
#endif

#endif // DX_NON_SAVEFUNCTION

#endif // DX_NON_SOFTIMAGE















#ifndef DX_NON_SOUND

// DxSound.cpp関数プロトタイプ宣言

// サウンドデータ管理系関数
extern	int			NS_InitSoundMem(                        int LogOutFlag = FALSE ) ;																	// 全てのサウンドハンドルを削除する

extern	int			NS_AddSoundData(                        int Handle = -1 ) ;																										// サウンドハンドルを作成する
extern	int			NS_AddStreamSoundMem(                   STREAMDATA *Stream, int LoopNum,  int SoundHandle, int StreamDataType, int *CanStreamCloseFlag, int UnionHandle = -1 ) ;	// ストリーム再生タイプのサウンドハンドルにストリームデータを再生対象に追加する
extern	int			NS_AddStreamSoundMemToMem(              const void *FileImage, int FileImageSize, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;		// ストリーム再生タイプのサウンドハンドルにメモリ上に展開したサウンドファイルイメージを再生対象に追加する
extern	int			NS_AddStreamSoundMemToFile(             const TCHAR *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;							// ストリーム再生タイプのサウンドハンドルにサウンドファイルを再生対象に追加する
extern	int			NS_AddStreamSoundMemToFileWithStrLen(   const TCHAR *WaveFile, size_t WaveFilePathLength, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle = -1 ) ;							// ストリーム再生タイプのサウンドハンドルにサウンドファイルを再生対象に追加する
extern	int			NS_SetupStreamSoundMem(                 int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの再生準備をする
extern	int			NS_PlayStreamSoundMem(                  int SoundHandle, int PlayType = DX_PLAYTYPE_LOOP , int TopPositionFlag = TRUE ) ;			// ストリーム再生タイプのサウンドハンドルの再生を開始する
extern	int			NS_CheckStreamSoundMem(                 int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの再生状態を取得する
extern	int			NS_StopStreamSoundMem(                  int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの再生を停止する
extern	int			NS_SetStreamSoundCurrentPosition(       int Byte, int SoundHandle ) ;																// サウンドハンドルの再生位置をバイト単位で変更する(再生が止まっている時のみ有効)
extern	int			NS_GetStreamSoundCurrentPosition(       int SoundHandle ) ;																		// サウンドハンドルの再生位置をバイト単位で取得する
extern	int			NS_SetStreamSoundCurrentTime(           int Time, int SoundHandle ) ;																// サウンドハンドルの再生位置をミリ秒単位で設定する(圧縮形式の場合は正しく設定されない場合がある)
extern	int			NS_GetStreamSoundCurrentTime(           int SoundHandle ) ;																		// サウンドハンドルの再生位置をミリ秒単位で取得する(圧縮形式の場合は正しい値が返ってこない場合がある)
extern	int			NS_ProcessStreamSoundMem(               int SoundHandle ) ;																		// ストリーム再生タイプのサウンドハンドルの周期的な処理を行う関数( 内部で自動的に呼ばれます )
extern	int			NS_ProcessStreamSoundMemAll(            void ) ;																					// 有効なストリーム再生タイプのサウンドハンドルに対して ProcessStreamSoundMem を実行する( 内部で自動的に呼ばれます )


extern	int			NS_LoadSoundMem2(                       const TCHAR *FileName1, const TCHAR *FileName2 ) ;											// 前奏部とループ部に分かれたサウンドファイルを読み込みサウンドハンドルを作成する
extern	int			NS_LoadSoundMem2WithStrLen(             const TCHAR *FileName1, size_t FileName1Length, const TCHAR *FileName2, size_t FileName2Length ) ;	// 前奏部とループ部に分かれたサウンドファイルを読み込みサウンドハンドルを作成する
extern	int			NS_LoadBGM(                             const TCHAR *FileName                        ) ;													// 主にＢＧＭを読み込みサウンドハンドルを作成するのに適した関数
extern	int			NS_LoadBGMWithStrLen(                   const TCHAR *FileName, size_t FileNameLength ) ;													// 主にＢＧＭを読み込みサウンドハンドルを作成するのに適した関数

extern	int			NS_LoadSoundMemBase(                    const TCHAR *FileName, int BufferNum,      int UnionHandle = -1 ) ;						// サウンドファイルからサウンドハンドルを作成する
extern	int			NS_LoadSoundMemBaseWithStrLen(          const TCHAR *FileName, size_t FileNameLength, int BufferNum,      int UnionHandle = -1 ) ;			// サウンドファイルからサウンドハンドルを作成する
extern	int			NS_LoadSoundMem(                        const TCHAR *FileName,                        int BufferNum = 3 , int UnionHandle = -1 ) ;			// LoadSoundMemBase の別名関数
extern	int			NS_LoadSoundMemWithStrLen(              const TCHAR *FileName, size_t FileNameLength, int BufferNum = 3 , int UnionHandle = -1 ) ;			// LoadSoundMemBase の別名関数
extern	int			NS_LoadSoundMemToBufNumSitei(           const TCHAR *FileName,                        int BufferNum ) ;									// LoadSoundMem を使用して下さい
extern	int			NS_LoadSoundMemToBufNumSiteiWithStrLen( const TCHAR *FileName, size_t FileNameLength, int BufferNum ) ;									// LoadSoundMem を使用して下さい
extern	int			NS_LoadSoundMemByResource(              const TCHAR *ResourceName, const TCHAR *ResourceType, int BufferNum = 1 ) ;				// サウンドリソースからサウンドハンドルを作成する
extern	int			NS_LoadSoundMemByResourceWithStrLen(    const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int BufferNum = 1 ) ;				// サウンドリソースからサウンドハンドルを作成する
extern	int			NS_DuplicateSoundMem(                   int SrcSoundHandle, int BufferNum = 3 ) ;													// 同じサウンドデータを使用するサウンドハンドルを作成する( DX_SOUNDDATATYPE_MEMNOPRESS タイプのサウンドハンドルのみ可能 )

extern	int			NS_LoadSoundMemByMemImageBase(          const void *FileImage, int FileImageSize, int BufferNum,     int UnionHandle = -1 ) ;			// メモリ上に展開されたサウンドファイルイメージからサウンドハンドルを作成する
extern	int			NS_LoadSoundMemByMemImage(              const void *FileImage, int FileImageSize, int BufferNum = 3, int UnionHandle = -1 ) ;			/// LoadSoundMemByMemImageBase の別名関数
extern	int			NS_LoadSoundMemByMemImage2(             const void *WaveImage, int WaveImageSize, const WAVEFORMATEX *WaveFormat, int WaveHeaderSize ) ;	// メモリ上に展開されたＰＣＭデータからサウンドハンドルを作成する
extern	int			NS_LoadSoundMemByMemImageToBufNumSitei( const void *FileImage, int FileImageSize, int BufferNum ) ;								// LoadSoundMemByMemImageBase を使用して下さい
extern	int			NS_LoadSoundMem2ByMemImage(             const void *FileImage1, int FileImageSize1, const void *FileImage2, int FileImageSize2 ) ;	// 前奏部とループ部に分かれた二つのメモリ上に展開されたサウンドファイルイメージからサウンドハンドルを作成する
extern	int			NS_LoadSoundMemFromSoftSound(           int SoftSoundHandle, int BufferNum = 3 ) ;													// ソフトウエアサウンドハンドルが持つサウンドデータからサウンドハンドルを作成する

extern	int			NS_DeleteSoundMem(                      int SoundHandle, int LogOutFlag = FALSE ) ;												// サウンドハンドルを削除する

extern	int			NS_PlaySoundMem(                        int SoundHandle, int PlayType, int TopPositionFlag = TRUE ) ;								// サウンドハンドルを再生する
extern	int			NS_StopSoundMem(                                                                        int SoundHandle ) ;						// サウンドハンドルの再生を停止する
extern	int			NS_CheckSoundMem(                                                                       int SoundHandle ) ;						// サウンドハンドルが再生中かどうかを取得する
extern	int			NS_SetPanSoundMem(                      int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルのパンを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			NS_ChangePanSoundMem(                   int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルのパンを設定する( -255 〜 255 )
extern	int			NS_GetPanSoundMem(                                                                      int SoundHandle ) ;						// サウンドハンドルのパンを取得する
extern	int			NS_SetVolumeSoundMem(                   int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			NS_ChangeVolumeSoundMem(                int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルのボリュームを設定する( 0 〜 255 )
extern	int			NS_GetVolumeSoundMem(                                                                   int SoundHandle ) ;						// サウンドハンドルのボリュームを取得する
extern	int			NS_GetVolumeSoundMem2(                                                                  int SoundHandle ) ;						// サウンドハンドルのボリュームを取得する( 0 〜 255 )
extern	int			NS_SetChannelVolumeSoundMem(            int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			NS_ChangeChannelVolumeSoundMem(         int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを設定する( 0 〜 255 )
extern	int			NS_GetChannelVolumeSoundMem(            int Channel,                                    int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを取得する
extern	int			NS_GetChannelVolumeSoundMem2(           int Channel,                                    int SoundHandle ) ;						// サウンドハンドルの指定のチャンネルのボリュームを取得する( 0 〜 255 )
extern	int			NS_SetFrequencySoundMem(                int FrequencyPal,                               int SoundHandle ) ;						// サウンドハンドルの再生周波数を設定する
extern	int			NS_GetFrequencySoundMem(                                                                int SoundHandle ) ;						// サウンドハンドルの再生周波数を取得する
extern	int			NS_ResetFrequencySoundMem(                                                              int SoundHandle ) ;						// サウンドハンドルの再生周波数を読み込み直後の状態に戻す

extern	int			NS_SetNextPlayPanSoundMem(              int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するパンを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			NS_ChangeNextPlayPanSoundMem(           int PanPal,                                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するパンを設定する( -255 〜 255 )
extern	int			NS_SetNextPlayVolumeSoundMem(           int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			NS_ChangeNextPlayVolumeSoundMem(        int VolumePal,                                  int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するボリュームを設定する( 0 〜 255 )
extern	int			NS_SetNextPlayChannelVolumeSoundMem(    int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するチャンネルのボリュームを設定する( 100分の1デシベル単位 0 〜 10000 )
extern	int			NS_ChangeNextPlayChannelVolumeSoundMem( int Channel, int VolumePal,                     int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用するチャンネルのボリュームを設定する( 0 〜 255 )
extern	int			NS_SetNextPlayFrequencySoundMem(        int FrequencyPal,                               int SoundHandle ) ;						// サウンドハンドルの次の再生にのみ使用する再生周波数を設定する

extern	int			NS_SetCurrentPositionSoundMem(          int SamplePosition,                             int SoundHandle ) ;						// サウンドハンドルの再生位置をサンプル単位で設定する(再生が止まっている時のみ有効)
extern	int			NS_GetCurrentPositionSoundMem(                                                          int SoundHandle ) ;						// サウンドハンドルの再生位置をサンプル単位で取得する
extern	int			NS_SetSoundCurrentPosition(             int Byte,                                       int SoundHandle ) ;						// サウンドハンドルの再生位置をバイト単位で設定する(再生が止まっている時のみ有効)
extern	int			NS_GetSoundCurrentPosition(                                                             int SoundHandle ) ;						// サウンドハンドルの再生位置をバイト単位で取得する
extern	int			NS_SetSoundCurrentTime(                 int Time,                                       int SoundHandle ) ;						// サウンドハンドルの再生位置をミリ秒単位で設定する(圧縮形式の場合は正しく設定されない場合がある)
extern	int			NS_GetSoundCurrentTime(                                                                 int SoundHandle ) ;						// サウンドハンドルの再生位置をミリ秒単位で取得する(圧縮形式の場合は正しい値が返ってこない場合がある)
extern	int			NS_GetSoundTotalSample(                                                                 int SoundHandle ) ;						// サウンドハンドルの音の総時間をサンプル単位で取得する
extern	int			NS_GetSoundTotalTime(                                                                   int SoundHandle ) ;						// サウンドハンドルの音の総時間をミリ秒単位で取得する

extern	int			NS_SetLoopPosSoundMem(                  int LoopTime,                                   int SoundHandle ) ;						// SetLoopTimePosSoundMem の別名関数
extern	int			NS_SetLoopTimePosSoundMem(              int LoopTime,                                   int SoundHandle ) ;						// サウンドハンドルにループ位置を設定する(ミリ秒単位)
extern	int			NS_SetLoopSamplePosSoundMem(            int LoopSamplePosition,                         int SoundHandle ) ;						// サウンドハンドルにループ位置を設定する(サンプル単位)

extern	int			NS_SetLoopStartTimePosSoundMem(         int LoopStartTime,                              int SoundHandle ) ;						// サウンドハンドルにループ開始位置を設定する(ミリ秒単位)
extern	int			NS_SetLoopStartSamplePosSoundMem(       int LoopStartSamplePosition,                    int SoundHandle ) ;						// サウンドハンドルにループ開始位置を設定する(サンプル単位)

extern	int			NS_SetLoopAreaTimePosSoundMem(          int LoopStartTime, int LoopEndTime,             int SoundHandle ) ;						// サウンドハンドルにループ範囲を設定する(ミリ秒単位)
extern	int			NS_GetLoopAreaTimePosSoundMem(          int *LoopStartTime, int *LoopEndTime,                     int SoundHandle ) ;				// サウンドハンドルにループ範囲を取得する(ミリ秒単位)
extern	int			NS_SetLoopAreaSamplePosSoundMem(        int  LoopStartSamplePosition, int  LoopEndSamplePosition, int SoundHandle ) ;				// サウンドハンドルにループ範囲を設定する(サンプル単位)
extern	int			NS_GetLoopAreaSamplePosSoundMem(        int *LoopStartSamplePosition, int *LoopEndSamplePosition, int SoundHandle ) ;				// サウンドハンドルにループ範囲を取得する(サンプル単位)

extern	int			NS_SetPlayFinishDeleteSoundMem(         int DeleteFlag,                                 int SoundHandle ) ;						// サウンドハンドルの再生が終了したら自動的にハンドルを削除するかどうかを設定する

extern	int			NS_Set3DReverbParamSoundMem(            const SOUND3D_REVERB_PARAM *Param,                    int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用のリバーブパラメータを設定する
extern	int			NS_Set3DPresetReverbParamSoundMem(      int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ , int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用のリバーブパラメータをプリセットを使用して設定する
extern	int			NS_Set3DReverbParamSoundMemAll(         const SOUND3D_REVERB_PARAM *Param, int PlaySoundOnly = FALSE ) ;							// 全ての３Ｄサウンドのサウンドハンドルにリバーブパラメータを設定する( PlaySoundOnly TRUE:再生中のサウンドにのみ設定する  FALSE:再生していないサウンドにも設定する )
extern	int			NS_Set3DPresetReverbParamSoundMemAll(   int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ , int PlaySoundOnly = FALSE  ) ;			// 全ての３Ｄサウンドのサウンドハンドルにリバーブパラメータをプリセットを使用して設定する( PlaySoundOnly TRUE:再生中のサウンドにのみ設定する  FALSE:再生していないサウンドにも設定する )
extern	int			NS_Get3DReverbParamSoundMem(            SOUND3D_REVERB_PARAM *ParamBuffer,              int SoundHandle ) ;						// サウンドハンドルに設定されている３Ｄサウンド用のリバーブパラメータを取得する
extern	int			NS_Get3DPresetReverbParamSoundMem(      SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT 等 */ ) ;		// プリセットの３Ｄサウンド用のリバーブパラメータを取得する

extern	int			NS_Set3DPositionSoundMem(               VECTOR Position,                                int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用の再生位置を設定する
extern	int			NS_Set3DRadiusSoundMem(                 float Radius,                                   int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用の音が聞こえる距離を設定する
extern	int			NS_Set3DVelocitySoundMem(               VECTOR Velocity,                                int SoundHandle ) ;						// サウンドハンドルの３Ｄサウンド用の移動速度を設定する

extern	int			NS_SetNextPlay3DPositionSoundMem(       VECTOR Position,                                int SoundHandle ) ;						// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の再生位置を設定する
extern	int			NS_SetNextPlay3DRadiusSoundMem(         float Radius,                                   int SoundHandle ) ;						// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の音が聞こえる距離を設定する
extern	int			NS_SetNextPlay3DVelocitySoundMem(       VECTOR Velocity,                                int SoundHandle ) ;						// サウンドハンドルの次の再生のみに使用する３Ｄサウンド用の移動速度を設定する


// 特殊関数
extern	int			NS_GetMP3TagInfo(           const TCHAR *FileName,                        TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle ) ;		// MP3ファイルのタグ情報を取得する
extern	int			NS_GetMP3TagInfoWithStrLen( const TCHAR *FileName, size_t FileNameLength, TCHAR *TitleBuffer, size_t TitleBufferBytes, TCHAR *ArtistBuffer, size_t ArtistBufferBytes, TCHAR *AlbumBuffer, size_t AlbumBufferBytes, TCHAR *YearBuffer, size_t YearBufferBytes, TCHAR *CommentBuffer, size_t CommentBufferBytes, TCHAR *TrackBuffer, size_t TrackBufferBytes, TCHAR *GenreBuffer, size_t GenreBufferBytes, int *PictureGrHandle ) ;		// MP3ファイルのタグ情報を取得する


// 設定関係関数
extern	int			NS_SetCreateSoundDataType(              int SoundDataType ) ;																		// 作成するサウンドハンドルの再生タイプを設定する( DX_SOUNDDATATYPE_MEMNOPRESS 等 )
extern	int			NS_GetCreateSoundDataType(              void ) ;																					// 作成するサウンドハンドルの再生タイプを取得する( DX_SOUNDDATATYPE_MEMNOPRESS 等 )
extern	int			NS_SetCreateSoundPitchRate(             float Cents ) ;																				// 作成するサウンドハンドルのピッチレートを設定する( 単位はセント( 100.0fで半音、1200.0fで１オクターブ )、プラスの値で音程が高く、マイナスの値で音程が低くなります )
extern	float		NS_GetCreateSoundPitchRate(             void ) ;																					// 作成するサウンドハンドルのピッチレートを取得する( 単位はセント( 100.0fで半音、1200.0fで１オクターブ )、プラスの値で音程が高く、マイナスの値で音程が低くなります )
extern	int			NS_SetCreateSoundTimeStretchRate(       float Rate ) ;																				// 作成するサウンドハンドルのタイムストレッチ( 音程を変えずに音の長さを変更する )レートを設定する( 単位は倍率、2.0f で音の長さが２倍に、0.5f で音の長さが半分になります )
extern	float		NS_GetCreateSoundTimeStretchRate(       void ) ;																					// 作成するサウンドハンドルのタイムストレッチ( 音程を変えずに音の長さを変更する )レートを取得する( 単位は倍率、2.0f で音の長さが２倍に、0.5f で音の長さが半分になります )
extern	int			NS_SetCreateSoundLoopAreaTimePos(       int  LoopStartTime,           int  LoopEndTime ) ;											// 作成するサウンドハンドルのループ範囲を設定する( ミリ秒単位 )
extern	int			NS_GetCreateSoundLoopAreaTimePos(       int *LoopStartTime,           int *LoopEndTime ) ;											// 作成するサウンドハンドルのループ範囲を取得する( ミリ秒単位 )
extern	int			NS_SetCreateSoundLoopAreaSamplePos(     int  LoopStartSamplePosition, int  LoopEndSamplePosition ) ;								// 作成するサウンドハンドルのループ範囲を設定する( サンプル単位 )
extern	int			NS_GetCreateSoundLoopAreaSamplePos(     int *LoopStartSamplePosition, int *LoopEndSamplePosition ) ;								// 作成するサウンドハンドルのループ範囲を取得する( サンプル単位 )
extern	int			NS_SetCreateSoundIgnoreLoopAreaInfo(    int IgnoreFlag ) ;																			// LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視するかどうかを設定する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern	int			NS_GetCreateSoundIgnoreLoopAreaInfo(    void ) ;																					// LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視するかどうかを取得する( TRUE:無視する  FALSE:無視しない( デフォルト ) )
extern	int			NS_SetDisableReadSoundFunctionMask(     int Mask ) ;																				// 使用しないサウンドデータ読み込み処理のマスクを設定する( DX_READSOUNDFUNCTION_PCM 等 )
extern	int			NS_GetDisableReadSoundFunctionMask(     void ) ;																					// 使用しないサウンドデータ読み込み処理のマスクを取得する( DX_READSOUNDFUNCTION_PCM 等 )
extern	int			NS_SetEnableSoundCaptureFlag(           int Flag ) ;																				// サウンドキャプチャを前提とした動作をするかどうかを設定する
extern	int			NS_SetUseSoftwareMixingSoundFlag(       int Flag ) ;																				// サウンドの処理をソフトウエアで行うかどうかを設定する( TRUE:ソフトウエア  FALSE:ハードウエア( デフォルト ) )
extern	int			NS_SetEnableXAudioFlag(                 int Flag ) ;																				// サウンドの再生にXAudioを使用するかどうかを設定する( TRUE:使用する  FALSE:使用しない( デフォルト ) )
extern	int			NS_SetUseOldVolumeCalcFlag(             int Flag ) ;																				// ChangeVolumeSoundMem, ChangeNextPlayVolumeSoundMem, ChangeMovieVolumeToGraph の音量計算式を Ver3.10c以前のものを使用するかどうかを設定する( TRUE:Ver3.10c以前の計算式を使用  FALSE:3.10d以降の計算式を使用( デフォルト ) )

extern	int			NS_SetCreate3DSoundFlag(                     int Flag ) ;																			// 次に作成するサウンドハンドルを３Ｄサウンド用にするかどうかを設定する( TRUE:３Ｄサウンド用にする  FALSE:３Ｄサウンド用にしない( デフォルト ) )
extern	int			NS_Set3DSoundOneMetre(                       float Distance ) ;																		// ３Ｄ空間の１メートルに相当する距離を設定する、DxLib_Init を呼び出す前でのみ呼び出し可能( デフォルト:1.0f )
extern	int			NS_Set3DSoundListenerPosAndFrontPos_UpVecY(  VECTOR Position, VECTOR FrontPosition ) ;												// ３Ｄサウンドのリスナーの位置とリスナーの前方位置を設定する( リスナーの上方向はＹ軸固定 )
extern	int			NS_Set3DSoundListenerPosAndFrontPosAndUpVec( VECTOR Position, VECTOR FrontPosition, VECTOR UpVector ) ;								// ３Ｄサウンドのリスナーの位置とリスナーの前方位置とリスナーの上方向を設定する
extern	int			NS_Set3DSoundListenerVelocity(               VECTOR Velocity ) ;																	// ３Ｄサウンドのリスナーの移動速度を設定する
extern	int			NS_Set3DSoundListenerConeAngle(              float InnerAngle, float OuterAngle ) ;													// ３Ｄサウンドのリスナーの可聴角度範囲を設定する
extern	int			NS_Set3DSoundListenerConeVolume(             float InnerAngleVolume, float OuterAngleVolume ) ;										// ３Ｄサウンドのリスナーの可聴角度範囲の音量倍率を設定する

// 情報取得系関数
extern	const void*	NS_GetDSoundObj(                        void ) ;	/* 戻り値を IDirectSound * にキャストして下さい */								// ＤＸライブラリが使用している DirectSound オブジェクトを取得する

#ifndef DX_NON_BEEP
// BEEP音再生用命令
extern	int			NS_SetBeepFrequency(					int Freq ) ;																				// ビープ音周波数設定関数
extern	int			NS_PlayBeep(							void ) ;																					// ビープ音を再生する
extern	int			NS_StopBeep(							void ) ;																					// ビープ音を止める
#endif // DX_NON_BEEP

// ラッパー関数
extern	int			NS_PlaySoundFile(						const TCHAR *FileName, int PlayType ) ;														// サウンドファイルを再生する
extern	int			NS_PlaySoundFileWithStrLen(			const TCHAR *FileName, size_t FileNameLength, int PlayType ) ;														// サウンドファイルを再生する
extern	int			NS_PlaySound(							const TCHAR *FileName,                        int PlayType ) ;														// PlaySoundFile の旧名称
extern	int			NS_PlaySoundWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int PlayType ) ;														// PlaySoundFile の旧名称
extern	int			NS_CheckSoundFile(						void ) ;																					// サウンドファイルの再生中かどうかを取得する
extern	int			NS_CheckSound(							void ) ;																					// CheckSoundFile の旧名称
extern	int			NS_StopSoundFile(						void ) ;																					// サウンドファイルの再生を停止する
extern	int			NS_StopSound(							void ) ;																					// StopSoundFile の旧名称
extern	int			NS_SetVolumeSoundFile(					int VolumePal ) ;																			// サウンドファイルの音量を設定する
extern	int			NS_SetVolumeSound(						int VolumePal ) ;																			// SetVolumeSound の旧名称

// ソフトウエア制御サウンド系関数
extern	int			NS_InitSoftSound(						void ) ;																					// ソフトウエアで扱う波形データハンドルをすべて削除する
extern	int			NS_LoadSoftSound(						const TCHAR *FileName ) ;																	// ソフトウエアで扱う波形データハンドルをサウンドファイルから作成する
extern	int			NS_LoadSoftSoundWithStrLen(			const TCHAR *FileName, size_t FileNameLength ) ;											// ソフトウエアで扱う波形データハンドルをサウンドファイルから作成する
extern	int			NS_LoadSoftSoundFromMemImage(			const void *FileImage, int FileImageSize ) ;												// ソフトウエアで扱う波形データハンドルをメモリ上に展開されたサウンドファイルイメージから作成する
extern	int			NS_MakeSoftSound(						int UseFormat_SoftSoundHandle, int SampleNum ) ;											// ソフトウエアで扱う空の波形データハンドルを作成する( フォーマットは引数のソフトウエアサウンドハンドルと同じものにする )
extern	int			NS_MakeSoftSound2Ch16Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSound2Ch16Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSound2Ch8Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSound2Ch8Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSound1Ch16Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSound1Ch16Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSound1Ch8Bit44KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSound1Ch8Bit22KHz(			int SampleNum ) ;																			// ソフトウエアで扱う空の波形データハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSoundCustom(				int ChannelNum, int BitsPerSample, int SamplesPerSec, int SampleNum, int IsFloatType ) ;		// ソフトウエアで扱う空の波形データハンドルを作成する
extern	int			NS_DeleteSoftSound(					int SoftSoundHandle ) ;																		// ソフトウエアで扱う波形データハンドルを削除する
#ifndef DX_NON_SAVEFUNCTION
extern	int			NS_SaveSoftSound(						int SoftSoundHandle, const TCHAR *FileName ) ;												// ソフトウエアで扱う波形データハンドルをWAVEファイル(PCM)形式で保存する
extern	int			NS_SaveSoftSoundWithStrLen(			int SoftSoundHandle, const TCHAR *FileName, size_t FileNameLength ) ;						// ソフトウエアで扱う波形データハンドルをWAVEファイル(PCM)形式で保存する
#endif // DX_NON_SAVEFUNCTION
extern	int			NS_GetSoftSoundSampleNum(				int SoftSoundHandle ) ;																		// ソフトウエアで扱う波形データハンドルのサンプル数を取得する
extern	int			NS_GetSoftSoundFormat(					int SoftSoundHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec, int *IsFloatType ) ;				// ソフトウエアで扱う波形データハンドルのフォーマットを取得する
extern	int			NS_ReadSoftSoundData(					int SoftSoundHandle, int SamplePosition, int *Channel1, int *Channel2 ) ;					// ソフトウエアで扱う波形データハンドルのサンプルを読み取る
extern	int			NS_ReadSoftSoundDataF(					int SoftSoundHandle, int SamplePosition, float *Channel1, float *Channel2 ) ;				// ソフトウエアで扱う波形データハンドルのサンプルを読み取る( float型版 )
extern	int			NS_WriteSoftSoundData(					int SoftSoundHandle, int SamplePosition, int Channel1, int Channel2 ) ;						// ソフトウエアで扱う波形データハンドルのサンプルを書き込む
extern	int			NS_WriteSoftSoundDataF(					int SoftSoundHandle, int SamplePosition, float Channel1, float Channel2 ) ;					// ソフトウエアで扱う波形データハンドルのサンプルを書き込む( float型版 )
extern	int			NS_WriteTimeStretchSoftSoundData(		int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;											// ソフトウエアで扱う波形データハンドルの波形データを音程を変えずにデータの長さを変更する
extern	int			NS_WritePitchShiftSoftSoundData(		int SrcSoftSoundHandle, int DestSoftSoundHandle ) ;											// ソフトウエアで扱う波形データハンドルの波形データの長さを変更する
extern	void*		NS_GetSoftSoundDataImage(				int SoftSoundHandle ) ;																		// ソフトウエアで扱う波形データハンドルの波形イメージが格納されているメモリアドレスを取得する
extern	int			NS_GetFFTVibrationSoftSound(			int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *Buffer, int BufferLength ) ;		// ソフトウエアで扱う波形データハンドルの指定の範囲を高速フーリエ変換を行い、各周波数域の振幅を取得する( SampleNum は 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 の何れかである必要があります )
extern	int			NS_GetFFTVibrationSoftSoundBase(		int SoftSoundHandle, int Channel, int SamplePosition, int SampleNum, float *RealBuffer_Array, float *ImagBuffer_Array, int BufferLength ) ;	// ソフトウエアで扱う波形データハンドルの指定の範囲を高速フーリエ変換を行い、各周波数域の振幅を取得する、結果の実数と虚数を別々に取得することができるバージョン( SampleNum は 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 の何れかである必要があります、Channel を -1 にすると二つのチャンネルを合成した結果になります )

extern	int			NS_InitSoftSoundPlayer(				void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルをすべて解放する
extern	int			NS_MakeSoftSoundPlayer(				int UseFormat_SoftSoundHandle ) ;															// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( フォーマットは引数のソフトウエアサウンドハンドルと同じものにする )
extern	int			NS_MakeSoftSoundPlayer2Ch16Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSoundPlayer2Ch16Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSoundPlayer2Ch8Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSoundPlayer2Ch8Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:2 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSoundPlayer1Ch16Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSoundPlayer1Ch16Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数:16bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSoundPlayer1Ch8Bit44KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:44.1KHz )
extern	int			NS_MakeSoftSoundPlayer1Ch8Bit22KHz(	void ) ;																					// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する( チャンネル数:1 量子化ビット数: 8bit サンプリング周波数:22KHz )
extern	int			NS_MakeSoftSoundPlayerCustom(			int ChannelNum, int BitsPerSample, int SamplesPerSec ) ;									// ソフトウエアで扱う波形データのプレイヤーハンドルを作成する
extern	int			NS_DeleteSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルを削除する
extern	int			NS_AddDataSoftSoundPlayer(				int SSoundPlayerHandle, int SoftSoundHandle, int AddSamplePosition, int AddSampleNum ) ;	// ソフトウエアで扱う波形データのプレイヤーハンドルに波形データを追加する( フォーマットが同じではない場合はエラー )
extern	int			NS_AddDirectDataSoftSoundPlayer(		int SSoundPlayerHandle, const void *SoundData, int AddSampleNum ) ;							// ソフトウエアで扱う波形データのプレイヤーハンドルにプレイヤーが対応したフォーマットの生波形データを追加する
extern	int			NS_AddOneDataSoftSoundPlayer(			int SSoundPlayerHandle, int Channel1, int Channel2 ) ;										// ソフトウエアで扱う波形データのプレイヤーハンドルに波形データを一つ追加する
extern	int			NS_GetSoftSoundPlayerFormat(			int SSoundPlayerHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec ) ;			// ソフトウエアで扱う波形データのプレイヤーハンドルが扱うデータフォーマットを取得する
extern	int			NS_StartSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの再生処理を開始する
extern	int			NS_CheckStartSoftSoundPlayer(			int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの再生処理が開始されているか取得する( TRUE:開始している  FALSE:停止している )
extern	int			NS_StopSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの再生処理を停止する
extern	int			NS_ResetSoftSoundPlayer(				int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルの状態を初期状態に戻す( 追加された波形データは削除され、再生状態だった場合は停止する )
extern	int			NS_GetStockDataLengthSoftSoundPlayer(	int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルに追加した波形データでまだ再生用サウンドバッファに転送されていない波形データのサンプル数を取得する
extern	int			NS_CheckSoftSoundPlayerNoneData(		int SSoundPlayerHandle ) ;																	// ソフトウエアで扱う波形データのプレイヤーハンドルに再生用サウンドバッファに転送していない波形データが無く、再生用サウンドバッファにも無音データ以外無いかどうかを取得する( TRUE:無音データ以外無い  FALSE:有効データがある )




// ＭＩＤＩ制御関数
extern	int			NS_DeleteMusicMem(						int MusicHandle ) ;																			// ＭＩＤＩハンドルを削除する
extern	int			NS_LoadMusicMem(						const TCHAR *FileName ) ;																	// ＭＩＤＩファイルを読み込む
extern	int			NS_LoadMusicMemWithStrLen(				const TCHAR *FileName, size_t FileNameLength ) ;											// ＭＩＤＩファイルを読み込みＭＩＤＩハンドルを作成する
extern	int			NS_LoadMusicMemByMemImage(				const void *FileImage, int FileImageSize ) ;														// メモリ上に展開されたＭＩＤＩファイルを読み込む
extern	int			NS_LoadMusicMemByResource(				const TCHAR *ResourceName, const TCHAR *ResourceType ) ;									// リソース上のＭＩＤＩファイルを読み込む
extern	int			NS_LoadMusicMemByResourceWithStrLen(	const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength ) ;									// リソース上のＭＩＤＩファイルからＭＩＤＩハンドルを作成する
extern	int			NS_PlayMusicMem(						int MusicHandle, int PlayType ) ;															// 読み込んだＭＩＤＩデータの演奏を開始する
extern	int			NS_StopMusicMem(						int MusicHandle ) ;																			// ＭＩＤＩデータの演奏を停止する
extern	int			NS_CheckMusicMem(						int MusicHandle ) ;																			// ＭＩＤＩデータが演奏中かどうかを取得する( TRUE:演奏中  FALSE:停止中 )
extern	int			NS_SetVolumeMusicMem(					int Volume, int MusicHandle ) ;																// ＭＩＤＩデータの再生音量をセットする
extern	int			NS_GetMusicMemPosition(				int MusicHandle ) ;																			// ＭＩＤＩデータの現在の再生位置を取得する
extern	int			NS_InitMusicMem(						void ) ;																					// ＭＩＤＩデータハンドルをすべて削除する
extern	int			NS_ProcessMusicMem(					void ) ;																					// ＭＩＤＩデータの周期的処理

extern	int			NS_PlayMusic(							const TCHAR *FileName, int PlayType ) ;														// ＭＩＤＩファイルを演奏する
extern	int			NS_PlayMusicWithStrLen(				const TCHAR *FileName, size_t FileNameLength, int PlayType ) ;								// ＭＩＤＩファイルを演奏する
extern	int			NS_PlayMusicByMemImage(				const void *FileImage, int FileImageSize, int PlayType ) ;										// メモリ上に展開されているＭＩＤＩファイルを演奏する
extern	int			NS_PlayMusicByResource(				const TCHAR *ResourceName, const TCHAR *ResourceType, int PlayType ) ;						// リソースからＭＩＤＩファイルを読み込んで演奏する
extern	int			NS_PlayMusicByResourceWithStrLen(		const TCHAR *ResourceName, size_t ResourceNameLength, const TCHAR *ResourceType, size_t ResourceTypeLength, int PlayType ) ;	// リソースからＭＩＤＩファイルを読み込んで演奏する
extern	int			NS_SetVolumeMusic(						int Volume ) ;																				// ＭＩＤＩの再生音量をセットする
extern	int			NS_StopMusic(							void ) ;																					// ＭＩＤＩファイルの演奏停止
extern	int			NS_CheckMusic(							void ) ;																					// ＭＩＤＩファイルが演奏中か否か情報を取得する
extern	int			NS_GetMusicPosition(					void ) ;																					// ＭＩＤＩの現在の再生位置を取得する

extern	int			NS_SelectMidiMode(						int Mode ) ;																				// ＭＩＤＩの再生形式をセットする

#endif // DX_NON_SOUND










// DxArchive_.cpp 関数 プロトタイプ宣言
extern	int			NS_SetUseDXArchiveFlag(		int Flag ) ;													// ＤＸアーカイブファイルの読み込み機能を使うかどうかを設定する( FALSE:使用しない  TRUE:使用する )
extern	int			NS_SetDXArchivePriority(					int Priority = 0 ) ;																// ＤＸアーカイブファイルと通常のフォルダのどちらも存在した場合、どちらを優先させるかを設定する( 1:フォルダを優先 0:ＤＸアーカイブファイルを優先(デフォルト) )
extern	int			NS_SetDXArchiveExtension(					const TCHAR *Extension = NULL ) ;													// 検索するＤＸアーカイブファイルの拡張子を変更する
extern	int			NS_SetDXArchiveExtensionWithStrLen(	const TCHAR *Extension = NULL , size_t ExtensionLength = 0 ) ;	// 検索するＤＸアーカイブファイルの拡張子を設定する( Extension:拡張子名文字列 )
extern	int			NS_SetDXArchiveKeyString(				const TCHAR *KeyString = NULL ) ;								// ＤＸアーカイブファイルの鍵文字列を設定する( KeyString:鍵文字列 )
extern	int			NS_SetDXArchiveKeyStringWithStrLen(	const TCHAR *KeyString = NULL , size_t KeyStringLength = 0 ) ;	// ＤＸアーカイブファイルの鍵文字列を設定する( KeyString:鍵文字列 )

extern	int			NS_DXArchivePreLoad(			const TCHAR *FilePath , int ASync = FALSE ) ;					// 指定のＤＸＡファイルを丸ごとメモリに読み込む( 戻り値: -1=エラー  0=成功 )
extern	int			NS_DXArchivePreLoadWithStrLen(			const TCHAR *FilePath, size_t FilePathLength, int ASync = FALSE ) ;		// 指定のＤＸＡファイルを丸ごとメモリに読み込む( 戻り値  -1:エラー  0:成功 )
extern	int			NS_DXArchiveCheckIdle(					const TCHAR *FilePath                        ) ;						// 指定のＤＸＡファイルの事前読み込みが完了したかどうかを取得する( 戻り値  TRUE:完了した FALSE:まだ )
extern	int			NS_DXArchiveCheckIdleWithStrLen(		const TCHAR *FilePath, size_t FilePathLength ) ;						// 指定のＤＸＡファイルの事前読み込みが完了したかどうかを取得する( 戻り値  TRUE:完了した FALSE:まだ )
extern	int			NS_DXArchiveRelease(					const TCHAR *FilePath                        ) ;						// 指定のＤＸＡファイルをメモリから解放する
extern	int			NS_DXArchiveReleaseWithStrLen(			const TCHAR *FilePath, size_t FilePathLength ) ;						// 指定のＤＸＡファイルをメモリから解放する
extern	int			NS_DXArchiveCheckFile(					const TCHAR *FilePath,                        const TCHAR *TargetFilePath                              ) ;	// ＤＸＡファイルの中に指定のファイルが存在するかどうかを調べる、TargetFilePath はＤＸＡファイルをカレントフォルダとした場合のパス( 戻り値:  -1=エラー  0:無い  1:ある )
extern	int			NS_DXArchiveCheckFileWithStrLen(		const TCHAR *FilePath, size_t FilePathLength, const TCHAR *TargetFilePath, size_t TargetFilePathLength ) ;	// ＤＸＡファイルの中に指定のファイルが存在するかどうかを調べる、TargetFilePath はＤＸＡファイルをカレントフォルダとした場合のパス( 戻り値:  -1=エラー  0:無い  1:ある )
extern	int			NS_DXArchiveSetMemImage(				void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath,                               int ArchiveImageCopyFlag = FALSE , int ArchiveImageReadOnly = TRUE ) ;	// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする( EmulateFilePath は見立てる dxa ファイルのパス、例えばＤＸＡファイルイメージを Image.dxa というファイル名で c:\Temp にあることにしたい場合は EmulateFilePath に "c:\\Temp\\Image.dxa" を渡す、SetDXArchiveExtension で拡張子を変更している場合は EmulateFilePath に渡すファイルパスの拡張子もそれに合わせる必要あり )
extern	int			NS_DXArchiveSetMemImageWithStrLen(		void *ArchiveImage, int ArchiveImageSize, const TCHAR *EmulateFilePath, size_t EmulateFilePathLength, int ArchiveImageCopyFlag = FALSE , int ArchiveImageReadOnly = TRUE ) ;	// メモリ上に展開されたＤＸＡファイルを指定のファイルパスにあることにする( EmulateFilePath は見立てる dxa ファイルのパス、例えばＤＸＡファイルイメージを Image.dxa というファイル名で c:\Temp にあることにしたい場合は EmulateFilePath に "c:\\Temp\\Image.dxa" を渡す、SetDXArchiveExtension で拡張子を変更している場合は EmulateFilePath に渡すファイルパスの拡張子もそれに合わせる必要あり )
extern	int			NS_DXArchiveReleaseMemImage(	void *ArchiveImage ) ;											// DXArchiveSetMemImage の設定を解除する































#else  // DX_THREAD_SAFE


// DxSystem.cpp関数プロトタイプ宣言

// ウエイト系関数
#define NS_WaitTimer							WaitTimer
#ifndef DX_NON_INPUT
#define NS_WaitKey								WaitKey
#endif // DX_NON_INPUT

// カウンタ及び時刻取得系関数
#define NS_GetNowCount							GetNowCount
#define NS_GetNowHiPerformanceCount				GetNowHiPerformanceCount
#define NS_GetDateTime							GetDateTime

// 乱数取得
#define NS_GetRand								GetRand
#define NS_SRand								SRand

// バッテリー関連
#define NS_GetBatteryLifePercent				GetBatteryLifePercent

// クリップボード関係
#define NS_GetClipboardText						GetClipboardText
#define NS_SetClipboardText						SetClipboardText
#define NS_SetClipboardTextWithStrLen			SetClipboardTextWithStrLen










// DxLog.cpp関数プロトタイプ宣言

#ifndef DX_NON_LOG

// ログ出力設定関数
#define NS_SetOutApplicationLogValidFlag				SetOutApplicationLogValidFlag
#define NS_SetApplicationLogFileName					SetApplicationLogFileName
#define NS_SetApplicationLogFileNameWithStrLen			SetApplicationLogFileNameWithStrLen
#define NS_SetApplicationLogSaveDirectory				SetApplicationLogSaveDirectory
#define NS_SetApplicationLogSaveDirectoryWithStrLen		SetApplicationLogSaveDirectoryWithStrLen
#define NS_SetUseDateNameLogFile						SetUseDateNameLogFile

// エラー処理関数
#define NS_LogFileAdd							LogFileAdd
#define NS_LogFileAddWithStrLen					LogFileAddWithStrLen
#define NS_LogFileFmtAdd						LogFileFmtAdd
#define NS_LogFileTabAdd						LogFileTabAdd
#define NS_LogFileTabSub						LogFileTabSub
#define NS_ErrorLogAdd							ErrorLogAdd
#define NS_ErrorLogFmtAdd						ErrorLogFmtAdd
#define NS_ErrorLogTabAdd						ErrorLogTabAdd
#define NS_ErrorLogTabSub						ErrorLogTabSub
#define NS_SetUseTimeStampFlag					SetUseTimeStampFlag
#define NS_AppLogAdd							AppLogAdd

#ifndef DX_NON_PRINTF_DX

// ログ出力機能関数
#define NS_SetLogDrawOutFlag					SetLogDrawOutFlag
#define NS_GetLogDrawFlag						GetLogDrawFlag
#define NS_SetLogFontSize						SetLogFontSize
#define NS_SetLogFontHandle						SetLogFontHandle
#define NS_SetLogDrawArea						SetLogDrawArea

// 簡易画面出力関数
#define NS_printfDx								printfDx
#define NS_putsDx								putsDx
#define NS_putsDxWithStrLen						putsDxWithStrLen
#define NS_clsDx								clsDx
#endif

#endif // DX_NON_LOG













#ifndef DX_NON_ASYNCLOAD

// DxASyncLoad.cpp関数プロトタイプ宣言

// 非同期読み込み関係
#define NS_SetUseASyncLoadFlag					SetUseASyncLoadFlag
#define NS_GetUseASyncLoadFlag					GetUseASyncLoadFlag
#define NS_CheckHandleASyncLoad					CheckHandleASyncLoad
#define NS_GetHandleASyncLoadResult				GetHandleASyncLoadResult
#define NS_SetASyncLoadFinishDeleteFlag			SetASyncLoadFinishDeleteFlag
#define NS_GetASyncLoadNum						GetASyncLoadNum
#define NS_SetASyncLoadThreadNum				SetASyncLoadThreadNum

#endif // DX_NON_ASYNCLOAD










// DxHandle.cpp関数プロトタイプ宣言

#define NS_SetDeleteHandleFlag					SetDeleteHandleFlag














// DxWindow.cpp関数プロトタイプ宣言

#ifdef __WINDOWS__

// 便利関数
#define NS_GetResourceInfo						GetResourceInfo
#define NS_GetResourceInfoWithStrLen			GetResourceInfoWithStrLen
#define NS_GetResourceIDString					GetResourceIDString

// ウインドウ関係情報取得関数
#define NS_GetWindowCRect						GetWindowCRect
#define NS_GetWindowClientRect					GetWindowClientRect
#define NS_GetWindowFrameRect					GetWindowFrameRect
#define NS_GetWindowActiveFlag					GetWindowActiveFlag
#define NS_GetWindowMinSizeFlag					GetWindowMinSizeFlag
#define NS_GetWindowMaxSizeFlag					GetWindowMaxSizeFlag
#define NS_GetMainWindowHandle					GetMainWindowHandle
#define NS_GetWindowModeFlag					GetWindowModeFlag
#define NS_GetDefaultState						GetDefaultState
#define NS_GetActiveFlag						GetActiveFlag
#define NS_GetNoActiveState						GetNoActiveState
#define NS_GetMouseDispFlag						GetMouseDispFlag
#define NS_GetAlwaysRunFlag						GetAlwaysRunFlag
#define NS__GetSystemInfo						_GetSystemInfo
#define NS_GetPcInfo							GetPcInfo
#define NS_GetUseMMXFlag						GetUseMMXFlag
#define NS_GetUseSSEFlag						GetUseSSEFlag
#define NS_GetUseSSE2Flag						GetUseSSE2Flag
#define NS_GetWindowCloseFlag					GetWindowCloseFlag
#define NS_GetTaskInstance						GetTaskInstance
#define NS_GetUseWindowRgnFlag					GetUseWindowRgnFlag
#define NS_GetWindowSizeChangeEnableFlag		GetWindowSizeChangeEnableFlag
#define NS_GetWindowSizeExtendRate				GetWindowSizeExtendRate
#define NS_GetWindowSize						GetWindowSize
#define NS_GetWindowEdgeWidth					GetWindowEdgeWidth
#define NS_GetWindowPosition					GetWindowPosition
#define NS_GetWindowUserCloseFlag				GetWindowUserCloseFlag
#define NS_GetNotDrawFlag						GetNotDrawFlag
#define NS_GetPaintMessageFlag					GetPaintMessageFlag
#define NS_GetValidHiPerformanceCounter			GetValidHiPerformanceCounter
#define NS_GetInputSystemChar					GetInputSystemChar

// 設定系関数
#define NS_ChangeWindowMode						ChangeWindowMode
#define NS_SetUseCharSet						SetUseCharSet
#define NS_LoadPauseGraph						LoadPauseGraph
#define NS_LoadPauseGraphWithStrLen				LoadPauseGraphWithStrLen
#define NS_LoadPauseGraphFromMem				LoadPauseGraphFromMem
#define NS_SetActiveStateChangeCallBackFunction	SetActiveStateChangeCallBackFunction
#define NS_SetWindowText						SetWindowText
#define NS_SetWindowTextWithStrLen				SetWindowTextWithStrLen
#define NS_SetMainWindowText					SetMainWindowText
#define NS_SetMainWindowTextWithStrLen			SetMainWindowTextWithStrLen
#define NS_SetMainWindowClassName				SetMainWindowClassName
#define NS_SetMainWindowClassNameWithStrLen		SetMainWindowClassNameWithStrLen
#define NS_SetAlwaysRunFlag						SetAlwaysRunFlag
#define NS_SetWindowIconID						SetWindowIconID
#define NS_SetWindowIconHandle					SetWindowIconHandle
#define NS_SetUseASyncChangeWindowModeFunction	SetUseASyncChangeWindowModeFunction
#define NS_SetShutdownCallbackFunction			SetShutdownCallbackFunction
#define NS_SetWindowStyleMode					SetWindowStyleMode
#define NS_SetWindowZOrder						SetWindowZOrder
#define NS_SetWindowSizeChangeEnableFlag		SetWindowSizeChangeEnableFlag
#define NS_SetWindowSizeExtendRate				SetWindowSizeExtendRate
#define NS_SetWindowSize						SetWindowSize
#define NS_SetWindowMaxSize						SetWindowMaxSize
#define NS_SetWindowMinSize						SetWindowMinSize
#define NS_SetWindowPosition					SetWindowPosition
#define NS_SetSysCommandOffFlag					SetSysCommandOffFlag
#define NS_SetSysCommandOffFlagWithStrLen		SetSysCommandOffFlagWithStrLen
#define NS_SetHookWinProc						SetHookWinProc
#define NS_SetUseHookWinProcReturnValue			SetUseHookWinProcReturnValue
#define NS_SetDoubleStartValidFlag				SetDoubleStartValidFlag
#define NS_CheckDoubleStart						CheckDoubleStart
#define NS_AddMessageTakeOverWindow				AddMessageTakeOverWindow
#define NS_SubMessageTakeOverWindow				SubMessageTakeOverWindow

#define NS_SetWindowInitPosition				SetWindowInitPosition
#define NS_SetNotWinFlag						SetNotWinFlag
#define NS_SetNotDrawFlag						SetNotDrawFlag
#define NS_SetNotSoundFlag						SetNotSoundFlag
#define NS_SetNotInputFlag						SetNotInputFlag
#define NS_SetDialogBoxHandle					SetDialogBoxHandle
#define NS_SetWindowVisibleFlag					SetWindowVisibleFlag
#define NS_SetWindowMinimizeFlag				SetWindowMinimizeFlag
#define NS_SetWindowUserCloseEnableFlag			SetWindowUserCloseEnableFlag
#define NS_SetDxLibEndPostQuitMessageFlag		SetDxLibEndPostQuitMessageFlag
#define NS_SetUserWindow						SetUserWindow
#define NS_SetUserChildWindow					SetUserChildWindow
#define NS_SetUserWindowMessageProcessDXLibFlag	SetUserWindowMessageProcessDXLibFlag
#define NS_SetUseFPUPreserveFlag				SetUseFPUPreserveFlag
#define NS_SetValidMousePointerWindowOutClientAreaMoveFlag		SetValidMousePointerWindowOutClientAreaMoveFlag
#define NS_SetUseBackBufferTransColorFlag		SetUseBackBufferTransColorFlag
#define NS_SetUseUpdateLayerdWindowFlag			SetUseUpdateLayerdWindowFlag
#define NS_SetResourceModule					SetResourceModule
#define NS_SetUseDxLibWM_PAINTProcess			SetUseDxLibWM_PAINTProcess

// ドラッグ＆ドロップされたファイル関係
#define NS_SetDragFileValidFlag					SetDragFileValidFlag
#define NS_DragFileInfoClear					DragFileInfoClear
#define NS_GetDragFilePath						GetDragFilePath
#define NS_GetDragFileNum						GetDragFileNum

// ウインドウ描画領域設定系関数
#define NS_CreateRgnFromGraph					CreateRgnFromGraph
#define NS_CreateRgnFromBaseImage				CreateRgnFromBaseImage
#define NS_SetWindowRgnGraph					SetWindowRgnGraph
#define NS_SetWindowRgnGraphWithStrLen			SetWindowRgnGraphWithStrLen
#define NS_UpdateTransColorWindowRgn			UpdateTransColorWindowRgn

// ツールバー関係
#define NS_SetupToolBar							SetupToolBar
#define NS_SetupToolBarWithStrLen				SetupToolBarWithStrLen
#define NS_AddToolBarButton						AddToolBarButton
#define NS_AddToolBarSep						AddToolBarSep
#define NS_GetToolBarButtonState				GetToolBarButtonState
#define NS_SetToolBarButtonState				SetToolBarButtonState
#define NS_DeleteAllToolBarButton				DeleteAllToolBarButton

// メニュー関係
#define NS_SetUseMenuFlag						SetUseMenuFlag
#define NS_SetUseKeyAccelFlag					SetUseKeyAccelFlag

#define NS_AddKeyAccel							AddKeyAccel
#define NS_AddKeyAccelWithStrLen				AddKeyAccelWithStrLen
#define NS_AddKeyAccel_Name						AddKeyAccel_Name
#define NS_AddKeyAccel_NameWithStrLen			AddKeyAccel_NameWithStrLen
#define NS_AddKeyAccel_ID						AddKeyAccel_ID
#define NS_ClearKeyAccel						ClearKeyAccel

#define NS_AddMenuItem							AddMenuItem
#define NS_AddMenuItemWithStrLen				AddMenuItemWithStrLen
#define NS_DeleteMenuItem						DeleteMenuItem
#define NS_DeleteMenuItemWithStrLen				DeleteMenuItemWithStrLen
#define NS_CheckMenuItemSelect					CheckMenuItemSelect
#define NS_CheckMenuItemSelectWithStrLen		CheckMenuItemSelectWithStrLen
#define NS_SetMenuItemEnable					SetMenuItemEnable
#define NS_SetMenuItemEnableWithStrLen			SetMenuItemEnableWithStrLen
#define NS_SetMenuItemMark						SetMenuItemMark
#define NS_SetMenuItemMarkWithStrLen			SetMenuItemMarkWithStrLen
#define NS_CheckMenuItemSelectAll				CheckMenuItemSelectAll

#define NS_AddMenuItem_Name						AddMenuItem_Name
#define NS_AddMenuItem_NameWithStrLen			AddMenuItem_NameWithStrLen
#define NS_AddMenuLine_Name						AddMenuLine_Name
#define NS_AddMenuLine_NameWithStrLen			AddMenuLine_NameWithStrLen
#define NS_InsertMenuItem_Name					InsertMenuItem_Name
#define NS_InsertMenuItem_NameWithStrLen		InsertMenuItem_NameWithStrLen
#define NS_InsertMenuLine_Name					InsertMenuLine_Name
#define NS_InsertMenuLine_NameWithStrLen		InsertMenuLine_NameWithStrLen
#define NS_DeleteMenuItem_Name					DeleteMenuItem_Name
#define NS_DeleteMenuItem_NameWithStrLen		DeleteMenuItem_NameWithStrLen
#define NS_CheckMenuItemSelect_Name				CheckMenuItemSelect_Name
#define NS_CheckMenuItemSelect_NameWithStrLen	CheckMenuItemSelect_NameWithStrLen
#define NS_SetMenuItemEnable_Name				SetMenuItemEnable_Name
#define NS_SetMenuItemEnable_NameWithStrLen		SetMenuItemEnable_NameWithStrLen
#define NS_SetMenuItemMark_Name					SetMenuItemMark_Name
#define NS_SetMenuItemMark_NameWithStrLen		SetMenuItemMark_NameWithStrLen

#define NS_AddMenuItem_ID						AddMenuItem_ID
#define NS_AddMenuItem_IDWithStrLen				AddMenuItem_IDWithStrLen
#define NS_AddMenuLine_ID						AddMenuLine_ID
#define NS_InsertMenuItem_ID					InsertMenuItem_ID
#define NS_InsertMenuLine_ID					InsertMenuLine_ID
#define NS_DeleteMenuItem_ID					DeleteMenuItem_ID
#define NS_CheckMenuItemSelect_ID				CheckMenuItemSelect_ID
#define NS_SetMenuItemEnable_ID					SetMenuItemEnable_ID
#define NS_SetMenuItemMark_ID					SetMenuItemMark_ID

#define NS_DeleteMenuItemAll					DeleteMenuItemAll
#define NS_ClearMenuItemSelect					ClearMenuItemSelect
#define NS_GetMenuItemID						GetMenuItemID
#define NS_GetMenuItemIDWithStrLen				GetMenuItemIDWithStrLen
#define NS_GetMenuItemName						GetMenuItemName
#define NS_LoadMenuResource						LoadMenuResource
#define NS_SetMenuItemSelectCallBackFunction	SetMenuItemSelectCallBackFunction

#define NS_SetWindowMenu						SetWindowMenu
#define NS_SetDisplayMenuFlag					SetDisplayMenuFlag
#define NS_GetDisplayMenuFlag					GetDisplayMenuFlag
#define NS_GetUseMenuFlag						GetUseMenuFlag
#define NS_SetAutoMenuDisplayFlag				SetAutoMenuDisplayFlag

#endif // __WINDOWS__

// マウス関係関数
#define NS_SetMouseDispFlag						SetMouseDispFlag
#ifndef DX_NON_INPUT
#define NS_GetMousePoint						GetMousePoint
#define NS_SetMousePoint						SetMousePoint
#define NS_GetMouseInput						GetMouseInput
#define NS_GetMouseWheelRotVol					GetMouseWheelRotVol
#define NS_GetMouseHWheelRotVol					GetMouseHWheelRotVol
#define NS_GetMouseWheelRotVolF					GetMouseWheelRotVolF
#define NS_GetMouseHWheelRotVolF				GetMouseHWheelRotVolF
#define NS_GetMouseInputLog						GetMouseInputLog
#define NS_GetMouseInputLog2					GetMouseInputLog2
#endif // DX_NON_INPUT













// タッチパネル入力関係関数
#ifndef DX_NON_INPUT
#define NS_GetTouchInputNum						GetTouchInputNum
#define NS_GetTouchInput						GetTouchInput

#define NS_GetTouchInputLogNum					GetTouchInputLogNum
#define NS_ClearTouchInputLog					ClearTouchInputLog
#define NS_GetTouchInputLogOne					GetTouchInputLogOne
#define NS_GetTouchInputLog						GetTouchInputLog

#define NS_GetTouchInputDownLogNum				GetTouchInputDownLogNum
#define NS_ClearTouchInputDownLog				ClearTouchInputDownLog
#define NS_GetTouchInputDownLogOne				GetTouchInputDownLogOne
#define NS_GetTouchInputDownLog					GetTouchInputDownLog

#define NS_GetTouchInputUpLogNum				GetTouchInputUpLogNum
#define NS_ClearTouchInputUpLog					ClearTouchInputUpLog
#define NS_GetTouchInputUpLogOne				GetTouchInputUpLogOne
#define NS_GetTouchInputUpLog					GetTouchInputUpLog
#endif // DX_NON_INPUT












// DxMemory.cpp関数プロトタイプ宣言

// メモリ確保系関数
#define NS_DxAlloc								DxAlloc
#define NS_DxCalloc								DxCalloc
#define NS_DxRealloc							DxRealloc
#define NS_DxFree								DxFree
#define NS_DxSetAllocSizeTrap					DxSetAllocSizeTrap
#define NS_DxSetAllocPrintFlag					DxSetAllocPrintFlag
#define NS_DxGetAllocSize						DxGetAllocSize
#define NS_DxGetAllocNum						DxGetAllocNum
#define NS_DxDumpAlloc							DxDumpAlloc
#define NS_DxDrawAlloc							DxDrawAlloc
#define NS_DxErrorCheckAlloc					DxErrorCheckAlloc
#define NS_DxSetAllocSizeOutFlag				DxSetAllocSizeOutFlag
#define NS_DxSetAllocMemoryErrorCheckFlag		DxSetAllocMemoryErrorCheckFlag














// DxBaseFunc.cpp 関数プロトタイプ宣言

// 文字コード関係
#define NS_GetCharBytes							GetCharBytes
#define NS_ConvertStringCharCodeFormat			ConvertStringCharCodeFormat
#define NS_SetUseCharCodeFormat					SetUseCharCodeFormat
#define NS_Get_wchar_t_CharCodeFormat			Get_wchar_t_CharCodeFormat

// 文字列関係
#define NS_strcpyDx								strcpyDx
#define NS_strcpy_sDx							strcpy_sDx
#define NS_strpcpyDx							strpcpyDx
#define NS_strpcpy_sDx							strpcpy_sDx
#define NS_strpcpy2Dx							strpcpy2Dx
#define NS_strpcpy2_sDx							strpcpy2_sDx
#define NS_strncpyDx							strncpyDx
#define NS_strncpy_sDx							strncpy_sDx
#define NS_strncpy2Dx							strncpy2Dx
#define NS_strncpy2_sDx							strncpy2_sDx
#define NS_strrncpyDx							strrncpyDx
#define NS_strrncpy_sDx							strrncpy_sDx
#define NS_strrncpy2Dx							strrncpy2Dx
#define NS_strrncpy2_sDx						strrncpy2_sDx
#define NS_strpncpyDx							strpncpyDx
#define NS_strpncpy_sDx							strpncpy_sDx
#define NS_strpncpy2Dx							strpncpy2Dx
#define NS_strpncpy2_sDx						strpncpy2_sDx
#define NS_strcatDx								strcatDx
#define NS_strcat_sDx							strcat_sDx
#define NS_strlenDx								strlenDx
#define NS_strlen2Dx							strlen2Dx
#define NS_strcmpDx								strcmpDx
#define NS_stricmpDx							stricmpDx
#define NS_strncmpDx							strncmpDx
#define NS_strncmp2Dx							strncmp2Dx
#define NS_strpncmpDx							strpncmpDx
#define NS_strpncmp2Dx							strpncmp2Dx
#define NS_strgetchrDx							strgetchrDx
#define NS_strgetchr2Dx							strgetchr2Dx
#define NS_strputchrDx							strputchrDx
#define NS_strputchr2Dx							strputchr2Dx
#define NS_strposDx								strposDx
#define NS_strpos2Dx							strpos2Dx
#define NS_strstrDx								strstrDx
#define NS_strstr2Dx							strstr2Dx
#define NS_strrstrDx							strrstrDx
#define NS_strrstr2Dx							strrstr2Dx
#define NS_strchrDx								strchrDx
#define NS_strchr2Dx							strchr2Dx
#define NS_strrchrDx							strrchrDx
#define NS_strrchr2Dx							strrchr2Dx
#define NS_struprDx								struprDx
#define NS_vsprintfDx							vsprintfDx
#define NS_vsnprintfDx							vsnprintfDx
#define NS_sprintfDx							sprintfDx
#define NS_snprintfDx							snprintfDx
#define NS_itoaDx								itoaDx
#define NS_itoa_sDx								itoa_sDx
#define NS_atoiDx								atoiDx
#define NS_atofDx								atofDx
#define NS_vsscanfDx							vsscanfDx
#define NS_sscanfDx								sscanfDx









// DxInputString.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUTSTRING

// 文字コードバッファ操作関係
#define NS_StockInputChar						StockInputChar
#define NS_ClearInputCharBuf					ClearInputCharBuf
#define NS_GetInputChar							GetInputChar
#define NS_GetInputCharWait						GetInputCharWait

#define NS_GetOneChar							GetOneChar
#define NS_GetOneCharWait						GetOneCharWait
#define NS_GetCtrlCodeCmp						GetCtrlCodeCmp

#ifndef DX_NON_KEYEX

#define NS_DrawIMEInputString					DrawIMEInputString
#define NS_SetUseIMEFlag						SetUseIMEFlag
#define NS_GetUseIMEFlag						GetUseIMEFlag
#define NS_SetInputStringMaxLengthIMESync		SetInputStringMaxLengthIMESync
#define NS_SetIMEInputStringMaxLength			SetIMEInputStringMaxLength
#define NS_SetUseTSFFlag						SetUseTSFFlag

#endif

#define NS_GetStringPoint						GetStringPoint
#define NS_GetStringPointWithStrLen				GetStringPointWithStrLen
#define NS_GetStringPoint2						GetStringPoint2
#define NS_GetStringPoint2WithStrLen			GetStringPoint2WithStrLen
#define NS_GetStringLength						GetStringLength

#ifndef DX_NON_FONT
#define NS_DrawObtainsString						DrawObtainsString
#define NS_DrawObtainsNString						DrawObtainsNString
#define NS_DrawObtainsString_CharClip				DrawObtainsString_CharClip
#define NS_DrawObtainsNString_CharClip				DrawObtainsNString_CharClip
#define NS_GetObtainsStringCharPosition				GetObtainsStringCharPosition
#define NS_GetObtainsStringCharPosition_CharClip	GetObtainsStringCharPosition_CharClip
#endif // DX_NON_FONT
#define NS_DrawObtainsBox						DrawObtainsBox

#ifndef DX_NON_KEYEX

#define NS_InputStringToCustom					InputStringToCustom

#define NS_KeyInputString						KeyInputString
#define NS_KeyInputSingleCharString				KeyInputSingleCharString
#define NS_KeyInputNumber						KeyInputNumber

#define NS_GetIMEInputModeStr					GetIMEInputModeStr
#define NS_GetIMEInputData						GetIMEInputData
#define NS_SetKeyInputStringColor				SetKeyInputStringColor
#define NS_SetKeyInputStringColor2				SetKeyInputStringColor2
#define NS_ResetKeyInputStringColor2			ResetKeyInputStringColor2
#define NS_SetKeyInputStringFont				SetKeyInputStringFont
#define NS_SetKeyInputStringEndCharaMode		SetKeyInputStringEndCharaMode
#define NS_DrawKeyInputModeString				DrawKeyInputModeString

#define NS_InitKeyInput							InitKeyInput
#define NS_MakeKeyInput							MakeKeyInput
#define NS_DeleteKeyInput						DeleteKeyInput
#define NS_SetActiveKeyInput					SetActiveKeyInput
#define NS_GetActiveKeyInput					GetActiveKeyInput
#define NS_CheckKeyInput						CheckKeyInput
#define NS_ReStartKeyInput						ReStartKeyInput
#define NS_ProcessActKeyInput					ProcessActKeyInput
#define NS_DrawKeyInputString					DrawKeyInputString
#define NS_SetKeyInputDrawArea					SetKeyInputDrawArea

#define NS_SetKeyInputSelectArea				SetKeyInputSelectArea
#define NS_GetKeyInputSelectArea				GetKeyInputSelectArea
#define NS_SetKeyInputDrawStartPos				SetKeyInputDrawStartPos
#define NS_GetKeyInputDrawStartPos				GetKeyInputDrawStartPos
#define NS_SetKeyInputCursorBrinkTime			SetKeyInputCursorBrinkTime
#define NS_SetKeyInputCursorBrinkFlag			SetKeyInputCursorBrinkFlag
#define NS_SetKeyInputString					SetKeyInputString
#define NS_SetKeyInputStringWithStrLen			SetKeyInputStringWithStrLen
#define NS_SetKeyInputNumber					SetKeyInputNumber
#define NS_SetKeyInputNumberToFloat				SetKeyInputNumberToFloat
#define NS_GetKeyInputString					GetKeyInputString
#define NS_GetKeyInputNumber					GetKeyInputNumber
#define NS_GetKeyInputNumberToFloat				GetKeyInputNumberToFloat
#define NS_SetKeyInputCursorPosition			SetKeyInputCursorPosition
#define NS_GetKeyInputCursorPosition			GetKeyInputCursorPosition

#endif // DX_NON_KEYEX

#endif // DX_NON_INPUTSTRING












// DxFile.cpp関数プロトタイプ宣言

// ファイルアクセス関数
#define NS_FileRead_open						FileRead_open
#define NS_FileRead_open_WithStrLen				FileRead_open_WithStrLen
#define NS_FileRead_open_mem					FileRead_open_mem
#define NS_FileRead_size						FileRead_size
#define NS_FileRead_size_WithStrLen				FileRead_size_WithStrLen
#define NS_FileRead_close						FileRead_close
#define NS_FileRead_tell						FileRead_tell
#define NS_FileRead_seek						FileRead_seek
#define NS_FileRead_read						FileRead_read
#define NS_FileRead_idle_chk					FileRead_idle_chk
#define NS_FileRead_eof							FileRead_eof
#define NS_FileRead_set_format					FileRead_set_format
#define NS_FileRead_gets						FileRead_gets
#define NS_FileRead_getc						FileRead_getc
#define NS_FileRead_scanf						FileRead_scanf

#define NS_FileRead_createInfo					FileRead_createInfo
#define NS_FileRead_createInfo_WithStrLen		FileRead_createInfo_WithStrLen
#define NS_FileRead_getInfoNum					FileRead_getInfoNum
#define NS_FileRead_getInfo						FileRead_getInfo
#define NS_FileRead_deleteInfo					FileRead_deleteInfo

#define NS_FileRead_findFirst					FileRead_findFirst
#define NS_FileRead_findFirst_WithStrLen		FileRead_findFirst_WithStrLen
#define NS_FileRead_findNext					FileRead_findNext
#define NS_FileRead_findClose					FileRead_findClose

#define NS_FileRead_fullyLoad					FileRead_fullyLoad
#define NS_FileRead_fullyLoad_WithStrLen		FileRead_fullyLoad_WithStrLen
#define NS_FileRead_fullyLoad_check				FileRead_fullyLoad_check
#define NS_FileRead_fullyLoad_delete			FileRead_fullyLoad_delete
#define NS_FileRead_fullyLoad_getImage			FileRead_fullyLoad_getImage
#define NS_FileRead_fullyLoad_getSize			FileRead_fullyLoad_getSize

// 設定関係関数
#define NS_GetStreamFunctionDefault				GetStreamFunctionDefault
#define NS_ChangeStreamFunction					ChangeStreamFunction
#define NS_ChangeStreamFunctionW				ChangeStreamFunctionW

// 補助関係関数
#define NS_ConvertFullPath						ConvertFullPath
#define NS_ConvertFullPathWithStrLen			ConvertFullPathWithStrLen









// DxNetwork.cpp関数プロトタイプ宣言

#ifndef DX_NON_NETWORK
#define NS_GetWinSockLastError					GetWinSockLastError
#endif // DX_NON_NETWORK







// DxInput.cpp関数プロトタイプ宣言

#ifndef DX_NON_INPUT

// 入力状態取得関数
#define NS_CheckHitKey							CheckHitKey
#define NS_CheckHitKeyAll						CheckHitKeyAll
#define NS_GetHitKeyStateAll					GetHitKeyStateAll
#define NS_SetKeyExclusiveCooperativeLevelFlag	SetKeyExclusiveCooperativeLevelFlag
#define NS_GetJoypadNum							GetJoypadNum
#define NS_GetJoypadInputState					GetJoypadInputState
#define NS_GetJoypadAnalogInput					GetJoypadAnalogInput
#define NS_GetJoypadAnalogInputRight			GetJoypadAnalogInputRight
#define NS_GetJoypadDirectInputState			GetJoypadDirectInputState
#define NS_CheckJoypadXInput					CheckJoypadXInput
#define NS_GetJoypadXInputState					GetJoypadXInputState
#define NS_GetJoypadGUID						GetJoypadGUID
#define NS_ConvertKeyCodeToVirtualKey			ConvertKeyCodeToVirtualKey
#define NS_ConvertVirtualKeyToKeyCode			ConvertVirtualKeyToKeyCode
#define NS_SetJoypadInputToKeyInput				SetJoypadInputToKeyInput
#define NS_SetJoypadDeadZone					SetJoypadDeadZone
#define NS_GetJoypadDeadZone					GetJoypadDeadZone
#define NS_StartJoypadVibration					StartJoypadVibration
#define NS_StopJoypadVibration					StopJoypadVibration
#define NS_GetJoypadPOVState					GetJoypadPOVState
#define NS_GetJoypadName						GetJoypadName
#define NS_ReSetupJoypad						ReSetupJoypad

#define NS_SetKeyboardNotDirectInputFlag		SetKeyboardNotDirectInputFlag
#define NS_SetUseDirectInputFlag				SetUseDirectInputFlag
#define NS_SetUseXInputFlag						SetUseXInputFlag
#define NS_SetUseJoypadVibrationFlag			SetUseJoypadVibrationFlag

#endif // DX_NON_INPUT













#ifndef DX_NOTUSE_DRAWFUNCTION

// グラフィックハンドル作成関係関数
#define NS_MakeGraph							MakeGraph
#define NS_MakeScreen							MakeScreen
#define NS_DerivationGraph						DerivationGraph
#define NS_DerivationGraphF						DerivationGraphF
#define NS_DeleteGraph							DeleteGraph
#define NS_DeleteSharingGraph					DeleteSharingGraph
#define NS_GetGraphNum							GetGraphNum
#define NS_FillGraph							FillGraph
#define NS_FillRectGraph						FillRectGraph
#define NS_SetGraphLostFlag						SetGraphLostFlag
#define NS_InitGraph							InitGraph
#define NS_ReloadFileGraphAll					ReloadFileGraphAll

// シャドウマップハンドル関係関数
#define NS_MakeShadowMap						MakeShadowMap
#define NS_DeleteShadowMap						DeleteShadowMap
#define NS_SetShadowMapLightDirection			SetShadowMapLightDirection
#define NS_ShadowMap_DrawSetup					ShadowMap_DrawSetup
#define NS_ShadowMap_DrawEnd					ShadowMap_DrawEnd
#define NS_SetUseShadowMap						SetUseShadowMap
#define NS_SetShadowMapDrawArea					SetShadowMapDrawArea
#define NS_ResetShadowMapDrawArea				ResetShadowMapDrawArea
#define NS_SetShadowMapAdjustDepth				SetShadowMapAdjustDepth
#define NS_GetShadowMapViewProjectionMatrix		GetShadowMapViewProjectionMatrix
#define NS_SetShadowMapBlurParam				SetShadowMapBlurParam
#define NS_SetShadowMapGradationParam			SetShadowMapGradationParam
#define NS_TestDrawShadowMap					TestDrawShadowMap

// グラフィックハンドルへの画像転送関数
#define NS_BltBmpToGraph						BltBmpToGraph
#define NS_BltBmpToDivGraph						BltBmpToDivGraph
#define NS_BltBmpOrGraphImageToGraph			BltBmpOrGraphImageToGraph
#define NS_BltBmpOrGraphImageToGraph2			BltBmpOrGraphImageToGraph2
#define NS_BltBmpOrGraphImageToDivGraph			BltBmpOrGraphImageToDivGraph
#define NS_BltBmpOrGraphImageToDivGraphF		BltBmpOrGraphImageToDivGraphF

// 画像からグラフィックハンドルを作成する関数
#define NS_LoadBmpToGraph						LoadBmpToGraph
#define NS_LoadBmpToGraphWithStrLen				LoadBmpToGraphWithStrLen
#define NS_LoadGraph							LoadGraph
#define NS_LoadGraphWithStrLen					LoadGraphWithStrLen
#define NS_LoadReverseGraph						LoadReverseGraph
#define NS_LoadReverseGraphWithStrLen			LoadReverseGraphWithStrLen
#define NS_LoadDivGraph							LoadDivGraph
#define NS_LoadDivGraphWithStrLen				LoadDivGraphWithStrLen
#define NS_LoadDivGraphF						LoadDivGraphF
#define NS_LoadDivGraphFWithStrLen				LoadDivGraphFWithStrLen
#define NS_LoadDivBmpToGraph					LoadDivBmpToGraph
#define NS_LoadDivBmpToGraphWithStrLen			LoadDivBmpToGraphWithStrLen
#define NS_LoadDivBmpToGraphF					LoadDivBmpToGraphF
#define NS_LoadDivBmpToGraphFWithStrLen			LoadDivBmpToGraphFWithStrLen
#define NS_LoadReverseDivGraph					LoadReverseDivGraph
#define NS_LoadReverseDivGraphWithStrLen		LoadReverseDivGraphWithStrLen
#define NS_LoadReverseDivGraphF					LoadReverseDivGraphF
#define NS_LoadReverseDivGraphFWithStrLen		LoadReverseDivGraphFWithStrLen
#define NS_LoadBlendGraph						LoadBlendGraph
#define NS_LoadBlendGraphWithStrLen				LoadBlendGraphWithStrLen
#ifdef __WINDOWS__
#define NS_LoadGraphToResource					LoadGraphToResource
#define NS_LoadDivGraphToResource				LoadDivGraphToResource
#define NS_LoadDivGraphFToResource				LoadDivGraphFToResource
#define NS_LoadGraphToResource					LoadGraphToResource
#define NS_LoadGraphToResourceWithStrLen		LoadGraphToResourceWithStrLen
#define NS_LoadDivGraphToResource				LoadDivGraphToResource
#define NS_LoadDivGraphToResourceWithStrLen		LoadDivGraphToResourceWithStrLen
#define NS_LoadDivGraphFToResource				LoadDivGraphFToResource
#define NS_LoadDivGraphFToResourceWithStrLen	LoadDivGraphFToResourceWithStrLen
#define NS_CreateGraphFromID3D11Texture2D		CreateGraphFromID3D11Texture2D
#endif // __WINDOWS__

#define NS_CreateGraphFromMem					CreateGraphFromMem
#define NS_ReCreateGraphFromMem					ReCreateGraphFromMem
#define NS_CreateDivGraphFromMem				CreateDivGraphFromMem
#define NS_CreateDivGraphFFromMem				CreateDivGraphFFromMem
#define NS_ReCreateDivGraphFromMem				ReCreateDivGraphFromMem
#define NS_ReCreateDivGraphFFromMem				ReCreateDivGraphFFromMem
#define NS_CreateGraphFromBmp					CreateGraphFromBmp
#define NS_ReCreateGraphFromBmp					ReCreateGraphFromBmp
#define NS_CreateDivGraphFromBmp				CreateDivGraphFromBmp
#define NS_CreateDivGraphFFromBmp				CreateDivGraphFFromBmp
#define NS_ReCreateDivGraphFromBmp				ReCreateDivGraphFromBmp
#define NS_ReCreateDivGraphFFromBmp				ReCreateDivGraphFFromBmp
#define NS_CreateDXGraph						CreateDXGraph
#define NS_CreateGraphFromGraphImage			CreateGraphFromGraphImage
#define NS_ReCreateGraphFromGraphImage			ReCreateGraphFromGraphImage
#define NS_CreateDivGraphFromGraphImage			CreateDivGraphFromGraphImage
#define NS_CreateDivGraphFFromGraphImage		CreateDivGraphFFromGraphImage
#define NS_ReCreateDivGraphFromGraphImage		ReCreateDivGraphFromGraphImage
#define NS_ReCreateDivGraphFFromGraphImage		ReCreateDivGraphFFromGraphImage
#define NS_CreateGraph							CreateGraph
#define NS_CreateDivGraph						CreateDivGraph
#define NS_CreateDivGraphF						CreateDivGraphF
#define NS_ReCreateGraph						ReCreateGraph
#ifndef DX_NON_SOFTIMAGE
#define NS_CreateBlendGraphFromSoftImage		CreateBlendGraphFromSoftImage
#define NS_CreateGraphFromSoftImage				CreateGraphFromSoftImage
#define NS_CreateGraphFromRectSoftImage			CreateGraphFromRectSoftImage
#define NS_ReCreateGraphFromSoftImage			ReCreateGraphFromSoftImage
#define NS_ReCreateGraphFromRectSoftImage		ReCreateGraphFromRectSoftImage
#define NS_CreateDivGraphFromSoftImage			CreateDivGraphFromSoftImage
#define NS_CreateDivGraphFFromSoftImage			CreateDivGraphFFromSoftImage
#endif // DX_NON_SOFTIMAGE
#define NS_CreateGraphFromBaseImage				CreateGraphFromBaseImage
#define NS_CreateGraphFromRectBaseImage			CreateGraphFromRectBaseImage
#define NS_ReCreateGraphFromBaseImage			ReCreateGraphFromBaseImage
#define NS_ReCreateGraphFromRectBaseImage		ReCreateGraphFromRectBaseImage
#define NS_CreateDivGraphFromBaseImage			CreateDivGraphFromBaseImage
#define NS_CreateDivGraphFFromBaseImage			CreateDivGraphFFromBaseImage
#define NS_ReloadGraph							ReloadGraph
#define NS_ReloadGraphWithStrLen				ReloadGraphWithStrLen
#define NS_ReloadDivGraph						ReloadDivGraph
#define NS_ReloadDivGraphWithStrLen				ReloadDivGraphWithStrLen
#define NS_ReloadDivGraphF						ReloadDivGraphF
#define NS_ReloadDivGraphFWithStrLen			ReloadDivGraphFWithStrLen
#define NS_ReloadReverseGraph					ReloadReverseGraph
#define NS_ReloadReverseGraphWithStrLen			ReloadReverseGraphWithStrLen
#define NS_ReloadReverseDivGraph				ReloadReverseDivGraph
#define NS_ReloadReverseDivGraphWithStrLen		ReloadReverseDivGraphWithStrLen
#define NS_ReloadReverseDivGraphF				ReloadReverseDivGraphF
#define NS_ReloadReverseDivGraphFWithStrLen		ReloadReverseDivGraphFWithStrLen

// グラフィックハンドル作成時設定係関数
#define NS_SetGraphColorBitDepth				SetGraphColorBitDepth
#define NS_GetGraphColorBitDepth				GetGraphColorBitDepth
#define NS_SetCreateGraphColorBitDepth			SetCreateGraphColorBitDepth
#define NS_GetCreateGraphColorBitDepth			GetCreateGraphColorBitDepth
#define NS_SetCreateGraphChannelBitDepth		SetCreateGraphChannelBitDepth
#define NS_GetCreateGraphChannelBitDepth		GetCreateGraphChannelBitDepth
#define NS_SetDrawValidGraphCreateFlag			SetDrawValidGraphCreateFlag
#define NS_GetDrawValidGraphCreateFlag			GetDrawValidGraphCreateFlag
#define NS_SetDrawValidFlagOf3DGraph			SetDrawValidFlagOf3DGraph
#define NS_SetLeftUpColorIsTransColorFlag		SetLeftUpColorIsTransColorFlag
#define NS_SetUsePaletteGraphFlag				SetUsePaletteGraphFlag
#define NS_SetUseSystemMemGraphCreateFlag		SetUseSystemMemGraphCreateFlag
#define NS_GetUseSystemMemGraphCreateFlag		GetUseSystemMemGraphCreateFlag
#define NS_SetUseBlendGraphCreateFlag			SetUseBlendGraphCreateFlag
#define NS_GetUseBlendGraphCreateFlag			GetUseBlendGraphCreateFlag
#define NS_SetUseAlphaTestGraphCreateFlag		SetUseAlphaTestGraphCreateFlag
#define NS_GetUseAlphaTestGraphCreateFlag		GetUseAlphaTestGraphCreateFlag
#define NS_SetUseAlphaTestFlag					SetUseAlphaTestFlag
#define NS_GetUseAlphaTestFlag					GetUseAlphaTestFlag
#define NS_SetCubeMapTextureCreateFlag			SetCubeMapTextureCreateFlag
#define NS_GetCubeMapTextureCreateFlag			GetCubeMapTextureCreateFlag
#define NS_SetUseNoBlendModeParam				SetUseNoBlendModeParam
#define NS_SetDrawValidAlphaChannelGraphCreateFlag		SetDrawValidAlphaChannelGraphCreateFlag
#define NS_GetDrawValidAlphaChannelGraphCreateFlag		GetDrawValidAlphaChannelGraphCreateFlag
#define NS_SetDrawValidFloatTypeGraphCreateFlag			SetDrawValidFloatTypeGraphCreateFlag
#define NS_GetDrawValidFloatTypeGraphCreateFlag			GetDrawValidFloatTypeGraphCreateFlag
#define NS_SetDrawValidGraphCreateZBufferFlag			SetDrawValidGraphCreateZBufferFlag
#define NS_GetDrawValidGraphCreateZBufferFlag			GetDrawValidGraphCreateZBufferFlag
#define NS_SetCreateDrawValidGraphZBufferBitDepth		SetCreateDrawValidGraphZBufferBitDepth
#define NS_GetCreateDrawValidGraphZBufferBitDepth		GetCreateDrawValidGraphZBufferBitDepth
#define NS_SetCreateDrawValidGraphMipLevels				SetCreateDrawValidGraphMipLevels
#define NS_GetCreateDrawValidGraphMipLevels				GetCreateDrawValidGraphMipLevels
#define NS_SetCreateDrawValidGraphChannelNum	SetCreateDrawValidGraphChannelNum
#define NS_GetCreateDrawValidGraphChannelNum	GetCreateDrawValidGraphChannelNum
#define NS_SetCreateDrawValidGraphMultiSample	SetCreateDrawValidGraphMultiSample
#define NS_SetDrawValidMultiSample				SetDrawValidMultiSample
#define NS_GetMultiSampleQuality				GetMultiSampleQuality
#define NS_SetUseTransColor						SetUseTransColor
#define NS_SetUseTransColorGraphCreateFlag		SetUseTransColorGraphCreateFlag
#define NS_SetUseGraphAlphaChannel				SetUseGraphAlphaChannel
#define NS_GetUseGraphAlphaChannel				GetUseGraphAlphaChannel
#define NS_SetUseAlphaChannelGraphCreateFlag	SetUseAlphaChannelGraphCreateFlag
#define NS_GetUseAlphaChannelGraphCreateFlag	GetUseAlphaChannelGraphCreateFlag
#define NS_SetUseNotManageTextureFlag			SetUseNotManageTextureFlag
#define NS_GetUseNotManageTextureFlag			GetUseNotManageTextureFlag
#define NS_SetUsePlatformTextureFormat			SetUsePlatformTextureFormat
#define NS_GetUsePlatformTextureFormat			GetUsePlatformTextureFormat
#define NS_SetTransColor						SetTransColor
#define NS_GetTransColor						GetTransColor
#define NS_SetUseDivGraphFlag					SetUseDivGraphFlag
#define NS_SetUseAlphaImageLoadFlag				SetUseAlphaImageLoadFlag
#define NS_SetUseMaxTextureSize					SetUseMaxTextureSize
#define NS_SetUseGraphBaseDataBackup			SetUseGraphBaseDataBackup
#define NS_GetUseGraphBaseDataBackup			GetUseGraphBaseDataBackup

// 画像情報関係関数
#define NS_GetFullColorImage					GetFullColorImage

#define NS_GraphLock							GraphLock
#define NS_GraphUnLock							GraphUnLock

#define NS_SetUseGraphZBuffer					SetUseGraphZBuffer
#define NS_CopyGraphZBufferImage				CopyGraphZBufferImage

#define NS_SetDeviceLostDeleteGraphFlag			SetDeviceLostDeleteGraphFlag

#define NS_GetGraphSize							GetGraphSize
#define NS_GetGraphSizeF						GetGraphSizeF
#define NS_GetGraphTextureSize					GetGraphTextureSize
#define NS_GetGraphUseBaseGraphArea				GetGraphUseBaseGraphArea
#define NS_GetGraphMipmapCount					GetGraphMipmapCount
#define NS_GetGraphFilePath						GetGraphFilePath
#define NS_CheckDrawValidGraph					CheckDrawValidGraph

#ifdef __WINDOWS__
#define NS_GetGraphID3D11Texture2D				GetGraphID3D11Texture2D
#define NS_GetGraphID3D11RenderTargetView		GetGraphID3D11RenderTargetView
#define NS_GetGraphID3D11DepthStencilView		GetGraphID3D11DepthStencilView
#endif

#define NS_GetTexColorData						GetTexColorData
#define NS_GetTexColorData						GetTexColorData
#define NS_GetTexColorData						GetTexColorData
#define NS_GetMaxGraphTextureSize				GetMaxGraphTextureSize
#define NS_GetValidRestoreShredPoint			GetValidRestoreShredPoint
#define NS_GetCreateGraphColorData				GetCreateGraphColorData

// パレット操作関係関数
#define NS_GetGraphPalette						GetGraphPalette
#define NS_GetGraphOriginalPalette				GetGraphOriginalPalette
#define NS_SetGraphPalette						SetGraphPalette
#define NS_ResetGraphPalette					ResetGraphPalette

// 図形描画関数
#define NS_DrawLine										DrawLine
#define NS_DrawLineAA									DrawLineAA
#define NS_DrawBox										DrawBox
#define NS_DrawBoxAA									DrawBoxAA
#define NS_DrawFillBox									DrawFillBox
#define NS_DrawLineBox									DrawLineBox
#define NS_DrawCircle									DrawCircle
#define NS_DrawCircleAA									DrawCircleAA
#define NS_DrawOval										DrawOval
#define NS_DrawOvalAA									DrawOvalAA
#define NS_DrawOval_Rect								DrawOval_Rect
#define NS_DrawTriangle									DrawTriangle
#define NS_DrawTriangleAA								DrawTriangleAA
#define NS_DrawQuadrangle								DrawQuadrangle
#define NS_DrawQuadrangleAA								DrawQuadrangleAA
#define NS_DrawRoundRect								DrawRoundRect
#define NS_DrawRoundRectAA								DrawRoundRectAA
#define NS_BeginAADraw									BeginAADraw
#define NS_EndAADraw									EndAADraw
#define NS_DrawPixel									DrawPixel

#define NS_Paint										Paint

#define NS_DrawPixelSet									DrawPixelSet
#define NS_DrawLineSet									DrawLineSet

#define NS_DrawPixel3D									DrawPixel3D
#define NS_DrawPixel3DD									DrawPixel3DD
#define NS_DrawLine3D									DrawLine3D
#define NS_DrawLine3DD									DrawLine3DD
#define NS_DrawTriangle3D								DrawTriangle3D
#define NS_DrawTriangle3DD								DrawTriangle3DD
#define NS_DrawCube3D									DrawCube3D
#define NS_DrawCube3DD									DrawCube3DD
#define NS_DrawSphere3D									DrawSphere3D
#define NS_DrawSphere3DD								DrawSphere3DD
#define NS_DrawCapsule3D								DrawCapsule3D
#define NS_DrawCapsule3DD								DrawCapsule3DD
#define NS_DrawCone3D									DrawCone3D
#define NS_DrawCone3DD									DrawCone3DD

// 画像描画関数
#define NS_LoadGraphScreen								LoadGraphScreen
#define NS_LoadGraphScreenWithStrLen					LoadGraphScreenWithStrLen

#define NS_DrawGraph									DrawGraph
#define NS_DrawExtendGraph								DrawExtendGraph
#define NS_DrawRotaGraph								DrawRotaGraph
#define NS_DrawRotaGraph2								DrawRotaGraph2
#define NS_DrawRotaGraph3								DrawRotaGraph3
#define NS_DrawRotaGraphFast							DrawRotaGraphFast
#define NS_DrawRotaGraphFast2							DrawRotaGraphFast2
#define NS_DrawRotaGraphFast3							DrawRotaGraphFast3
#define NS_DrawModiGraph								DrawModiGraph
#define NS_DrawTurnGraph								DrawTurnGraph
#define NS_DrawReverseGraph								DrawReverseGraph

#define NS_DrawGraphF									DrawGraphF
#define NS_DrawExtendGraphF								DrawExtendGraphF
#define NS_DrawRotaGraphF								DrawRotaGraphF
#define NS_DrawRotaGraph2F								DrawRotaGraph2F
#define NS_DrawRotaGraph3F								DrawRotaGraph3F
#define NS_DrawRotaGraphFastF							DrawRotaGraphFastF
#define NS_DrawRotaGraphFast2F							DrawRotaGraphFast2F
#define NS_DrawRotaGraphFast3F							DrawRotaGraphFast3F
#define NS_DrawModiGraphF								DrawModiGraphF
#define NS_DrawTurnGraphF								DrawTurnGraphF
#define NS_DrawReverseGraphF							DrawReverseGraphF

#define NS_DrawChipMap									DrawChipMap
#define NS_DrawTile										DrawTile

#define NS_DrawRectGraph								DrawRectGraph
#define NS_DrawRectExtendGraph							DrawRectExtendGraph
#define NS_DrawRectRotaGraph							DrawRectRotaGraph
#define NS_DrawRectRotaGraph2							DrawRectRotaGraph2
#define NS_DrawRectRotaGraph3							DrawRectRotaGraph3
#define NS_DrawRectRotaGraphFast						DrawRectRotaGraphFast
#define NS_DrawRectRotaGraphFast2						DrawRectRotaGraphFast2
#define NS_DrawRectRotaGraphFast3						DrawRectRotaGraphFast3
#define NS_DrawRectModiGraph							DrawRectModiGraph

#define NS_DrawRectGraphF								DrawRectGraphF
#define NS_DrawRectExtendGraphF							DrawRectExtendGraphF
#define NS_DrawRectRotaGraphF							DrawRectRotaGraphF
#define NS_DrawRectRotaGraph2F							DrawRectRotaGraph2F
#define NS_DrawRectRotaGraph3F							DrawRectRotaGraph3F
#define NS_DrawRectRotaGraphFastF						DrawRectRotaGraphFastF
#define NS_DrawRectRotaGraphFast2F						DrawRectRotaGraphFast2F
#define NS_DrawRectRotaGraphFast3F						DrawRectRotaGraphFast3F
#define NS_DrawRectModiGraphF							DrawRectModiGraphF

#define NS_DrawBlendGraph								DrawBlendGraph
#define NS_DrawBlendGraphPos							DrawBlendGraphPos

#define NS_DrawCircleGauge								DrawCircleGauge

#define NS_DrawGraphToZBuffer							DrawGraphToZBuffer
#define NS_DrawTurnGraphToZBuffer						DrawTurnGraphToZBuffer
#define NS_DrawReverseGraphToZBuffer					DrawReverseGraphToZBuffer
#define NS_DrawExtendGraphToZBuffer						DrawExtendGraphToZBuffer
#define NS_DrawRotaGraphToZBuffer						DrawRotaGraphToZBuffer
#define NS_DrawRotaGraph2ToZBuffer						DrawRotaGraph2ToZBuffer
#define NS_DrawRotaGraph3ToZBuffer						DrawRotaGraph3ToZBuffer
#define NS_DrawRotaGraphFastToZBuffer					DrawRotaGraphFastToZBuffer
#define NS_DrawRotaGraphFast2ToZBuffer					DrawRotaGraphFast2ToZBuffer
#define NS_DrawRotaGraphFast3ToZBuffer					DrawRotaGraphFast3ToZBuffer
#define NS_DrawModiGraphToZBuffer						DrawModiGraphToZBuffer
#define NS_DrawBoxToZBuffer								DrawBoxToZBuffer
#define NS_DrawCircleToZBuffer							DrawCircleToZBuffer
#define NS_DrawTriangleToZBuffer						DrawTriangleToZBuffer
#define NS_DrawQuadrangleToZBuffer						DrawQuadrangleToZBuffer
#define NS_DrawRoundRectToZBuffer						DrawRoundRectToZBuffer

#define NS_DrawPolygon									DrawPolygon
#define NS_DrawPolygon2D								DrawPolygon2D
#define NS_DrawPolygon3D								DrawPolygon3D
#define NS_DrawPolygonIndexed2D							DrawPolygonIndexed2D
#define NS_DrawPolygonIndexed3D							DrawPolygonIndexed3D
#define NS_DrawPolygonIndexed3DBase						DrawPolygonIndexed3DBase
#define NS_DrawPolygon3DBase							DrawPolygon3DBase
#define NS_DrawPolygon3D								DrawPolygon3D

#define NS_DrawPolygonBase								DrawPolygonBase
#define NS_DrawPrimitive2D								DrawPrimitive2D
#define NS_DrawPrimitive3D								DrawPrimitive3D
#define NS_DrawPrimitiveIndexed2D						DrawPrimitiveIndexed2D
#define NS_DrawPrimitiveIndexed3D						DrawPrimitiveIndexed3D

#define NS_DrawPolygon3D_UseVertexBuffer				DrawPolygon3D_UseVertexBuffer
#define NS_DrawPrimitive3D_UseVertexBuffer				DrawPrimitive3D_UseVertexBuffer
#define NS_DrawPrimitive3D_UseVertexBuffer2				DrawPrimitive3D_UseVertexBuffer2
#define NS_DrawPolygonIndexed3D_UseVertexBuffer			DrawPolygonIndexed3D_UseVertexBuffer
#define NS_DrawPrimitiveIndexed3D_UseVertexBuffer		DrawPrimitiveIndexed3D_UseVertexBuffer
#define NS_DrawPrimitiveIndexed3D_UseVertexBuffer2		DrawPrimitiveIndexed3D_UseVertexBuffer2

#define NS_DrawGraph3D									DrawGraph3D
#define NS_DrawExtendGraph3D							DrawExtendGraph3D
#define NS_DrawRotaGraph3D								DrawRotaGraph3D
#define NS_DrawRota2Graph3D								DrawRota2Graph3D
#define NS_DrawModiBillboard3D							DrawModiBillboard3D
#define NS_DrawBillboard3D								DrawBillboard3D

// 描画設定関係関数
#define NS_SetDrawMode							SetDrawMode
#define NS_GetDrawMode							GetDrawMode
#define NS_SetDrawBlendMode						SetDrawBlendMode
#define NS_GetDrawBlendMode						GetDrawBlendMode
#define NS_SetDrawAlphaTest						SetDrawAlphaTest
#define NS_SetBlendGraph						SetBlendGraph
#define NS_SetBlendGraphParam					SetBlendGraphParam
#define NS_SetBlendGraphPosition				SetBlendGraphPosition
#define NS_SetBlendGraphPositionMode			SetBlendGraphPositionMode
#define NS_SetDrawBright						SetDrawBright
#define NS_GetDrawBright						GetDrawBright
#define NS_SetIgnoreDrawGraphColor				SetIgnoreDrawGraphColor
#define NS_SetMaxAnisotropy						SetMaxAnisotropy
#define NS_SetUseLarge3DPositionSupport			SetUseLarge3DPositionSupport

#define NS_SetUseZBufferFlag					SetUseZBufferFlag
#define NS_SetWriteZBufferFlag					SetWriteZBufferFlag
#define NS_SetZBufferCmpType					SetZBufferCmpType
#define NS_SetZBias								SetZBias
#define NS_SetUseZBuffer3D						SetUseZBuffer3D
#define NS_SetWriteZBuffer3D					SetWriteZBuffer3D
#define NS_SetZBufferCmpType3D					SetZBufferCmpType3D
#define NS_SetZBias3D							SetZBias3D
#define NS_SetDrawZ								SetDrawZ

#define NS_SetDrawArea							SetDrawArea
#define NS_GetDrawArea							GetDrawArea
#define NS_SetDraw3DScale						SetDraw3DScale
#define NS_SetDrawAreaFull						SetDrawAreaFull

#define NS_SetRestoreShredPoint					SetRestoreShredPoint
#define NS_SetRestoreGraphCallback				SetRestoreGraphCallback
#define NS_RunRestoreShred						RunRestoreShred
#define NS_SetGraphicsDeviceRestoreCallbackFunction		SetGraphicsDeviceRestoreCallbackFunction
#define NS_SetGraphicsDeviceLostCallbackFunction		SetGraphicsDeviceLostCallbackFunction

#define NS_SetTransformTo2D						SetTransformTo2D
#define NS_SetTransformTo2DD					SetTransformTo2DD
#define NS_ResetTransformTo2D					ResetTransformTo2D
#define NS_SetTransformToWorld					SetTransformToWorld
#define NS_SetTransformToWorldD					SetTransformToWorldD
#define NS_GetTransformToWorldMatrix			GetTransformToWorldMatrix
#define NS_GetTransformToWorldMatrixD			GetTransformToWorldMatrixD
#define NS_SetTransformToView					SetTransformToView
#define NS_SetTransformToViewD					SetTransformToViewD
#define NS_GetTransformToViewMatrix				GetTransformToViewMatrix
#define NS_GetTransformToViewMatrixD			GetTransformToViewMatrixD
#define NS_SetTransformToProjection				SetTransformToProjection
#define NS_SetTransformToProjectionD			SetTransformToProjectionD
#define NS_GetTransformToProjectionMatrix		GetTransformToProjectionMatrix
#define NS_GetTransformToProjectionMatrixD		GetTransformToProjectionMatrixD
#define NS_SetTransformToViewport				SetTransformToViewport
#define NS_SetTransformToViewportD				SetTransformToViewportD
#define NS_GetTransformToViewportMatrix			GetTransformToViewportMatrix
#define NS_GetTransformToViewportMatrixD		GetTransformToViewportMatrixD
#define NS_GetTransformToAPIViewportMatrix		GetTransformToAPIViewportMatrix
#define NS_GetTransformToAPIViewportMatrixD		GetTransformToAPIViewportMatrixD
#define NS_SetDefTransformMatrix				SetDefTransformMatrix
#define NS_SetDefTransformMatrixD				SetDefTransformMatrixD
#define NS_GetTransformPosition					GetTransformPosition
#define NS_GetTransformPositionD				GetTransformPositionD
#define NS_GetBillboardPixelSize				GetBillboardPixelSize
#define NS_GetBillboardPixelSizeD				GetBillboardPixelSizeD
#define	NS_ConvWorldPosToViewPos				ConvWorldPosToViewPos
#define	NS_ConvWorldPosToViewPosD				ConvWorldPosToViewPosD
#define NS_ConvWorldPosToScreenPos				ConvWorldPosToScreenPos
#define NS_ConvWorldPosToScreenPosD				ConvWorldPosToScreenPosD
#define NS_ConvWorldPosToScreenPosPlusW			ConvWorldPosToScreenPosPlusW
#define NS_ConvWorldPosToScreenPosPlusWD		ConvWorldPosToScreenPosPlusWD
#define NS_ConvScreenPosToWorldPos				ConvScreenPosToWorldPos
#define NS_ConvScreenPosToWorldPosD				ConvScreenPosToWorldPosD
#define NS_ConvScreenPosToWorldPos_ZLinear		ConvScreenPosToWorldPos_ZLinear
#define NS_ConvScreenPosToWorldPos_ZLinearD		ConvScreenPosToWorldPos_ZLinearD

#define NS_SetUseCullingFlag					SetUseCullingFlag
#define NS_SetUseBackCulling					SetUseBackCulling
#define NS_GetUseBackCulling					GetUseBackCulling

#define NS_SetTextureAddressMode				SetTextureAddressMode
#define NS_SetTextureAddressModeUV				SetTextureAddressModeUV
#define NS_SetTextureAddressTransform			SetTextureAddressTransform
#define NS_SetTextureAddressTransformMatrix		SetTextureAddressTransformMatrix
#define NS_ResetTextureAddressTransform			ResetTextureAddressTransform

#define NS_SetFogEnable							SetFogEnable
#define NS_GetFogEnable							GetFogEnable
#define NS_SetFogMode							SetFogMode
#define NS_GetFogMode							GetFogMode
#define NS_SetFogColor							SetFogColor
#define NS_GetFogColor							GetFogColor
#define NS_SetFogStartEnd						SetFogStartEnd
#define NS_GetFogStartEnd						GetFogStartEnd
#define NS_SetFogDensity						SetFogDensity
#define NS_GetFogDensity						GetFogDensity

// 画面関係関数
#define NS_GetPixel								GetPixel
#define NS_GetPixelF							GetPixelF
#define NS_SetBackgroundColor					SetBackgroundColor
#define NS_GetBackgroundColor					GetBackgroundColor
#define NS_GetDrawScreenGraph					GetDrawScreenGraph
#define NS_BltDrawValidGraph					BltDrawValidGraph
#define NS_ScreenFlip							ScreenFlip
#define NS_ScreenCopy							ScreenCopy
#define NS_WaitVSync							WaitVSync
#define NS_ClearDrawScreen						ClearDrawScreen
#define NS_ClearDrawScreenZBuffer				ClearDrawScreenZBuffer
#define NS_ClsDrawScreen						ClsDrawScreen
#define NS_SetDrawScreen						SetDrawScreen
#define NS_GetDrawScreen						GetDrawScreen
#define NS_GetActiveGraph						GetActiveGraph
#define NS_SetUseSetDrawScreenSettingReset		SetUseSetDrawScreenSettingReset
#define NS_GetUseSetDrawScreenSettingReset		GetUseSetDrawScreenSettingReset
#define NS_SetDrawZBuffer						SetDrawZBuffer
#ifdef __WINDOWS__
#define NS_BltBackScreenToWindow				BltBackScreenToWindow
#define NS_BltRectBackScreenToWindow			BltRectBackScreenToWindow
#define NS_SetScreenFlipTargetWindow			SetScreenFlipTargetWindow
#define NS_GetDesktopScreenGraph				GetDesktopScreenGraph
#endif // __WINDOWS__
#define NS_SetGraphMode							SetGraphMode
#define NS_SetFullScreenResolutionMode			SetFullScreenResolutionMode
#define NS_GetFullScreenResolutionMode			GetFullScreenResolutionMode
#define NS_SetFullScreenScalingMode				SetFullScreenScalingMode
#define NS_SetEmulation320x240					SetEmulation320x240
#define NS_SetZBufferSize						SetZBufferSize
#define NS_SetZBufferBitDepth					SetZBufferBitDepth
#define NS_SetWaitVSyncFlag						SetWaitVSyncFlag
#define NS_GetWaitVSyncFlag						GetWaitVSyncFlag
#define NS_SetFullSceneAntiAliasingMode			SetFullSceneAntiAliasingMode
#define NS_SetGraphDisplayArea					SetGraphDisplayArea
#define NS_SetChangeScreenModeGraphicsSystemResetFlag	SetChangeScreenModeGraphicsSystemResetFlag
#define NS_GetScreenState						GetScreenState
#define NS_GetDrawScreenSize					GetDrawScreenSize
#define NS_GetScreenBitDepth					GetScreenBitDepth
#define NS_GetColorBitDepth						GetColorBitDepth
#define NS_GetChangeDisplayFlag					GetChangeDisplayFlag
#define NS_GetVideoMemorySize					GetVideoMemorySize
#define NS_GetRefreshRate						GetRefreshRate
#define NS_GetDisplayNum						GetDisplayNum
#define NS_GetDisplayInfo						GetDisplayInfo
#define NS_GetDisplayModeNum					GetDisplayModeNum
#define NS_GetDisplayMode						GetDisplayMode
#define NS_GetDisplayMaxResolution				GetDisplayMaxResolution
#define NS_GetDispColorData						GetDispColorData
#define NS_GetMultiDrawScreenNum				GetMultiDrawScreenNum
#define NS_GetDrawFloatCoordType				GetDrawFloatCoordType

// その他設定関係関数
#define NS_SetUseNormalDrawShader				SetUseNormalDrawShader
#define NS_SetUseSoftwareRenderModeFlag			SetUseSoftwareRenderModeFlag
#define NS_SetNotUse3DFlag						SetNotUse3DFlag
#define NS_SetUse3DFlag							SetUse3DFlag
#define NS_GetUse3DFlag							GetUse3DFlag
#define NS_SetScreenMemToVramFlag				SetScreenMemToVramFlag
#define NS_GetScreenMemToSystemMemFlag			GetScreenMemToSystemMemFlag

#define NS_SetUseBasicGraphDraw3DDeviceMethodFlag	SetUseBasicGraphDraw3DDeviceMethodFlag
#define NS_SetUseDisplayIndex					SetUseDisplayIndex

#define NS_SetWindowDrawRect					SetWindowDrawRect
#define NS_RestoreGraphSystem					RestoreGraphSystem
#define NS_SetUseHardwareVertexProcessing		SetUseHardwareVertexProcessing
#define NS_SetUsePixelLighting					SetUsePixelLighting
#define NS_SetUseOldDrawModiGraphCodeFlag		SetUseOldDrawModiGraphCodeFlag
#define NS_SetUseVramFlag						SetUseVramFlag
#define NS_GetUseVramFlag						GetUseVramFlag
#define NS_SetBasicBlendFlag					SetBasicBlendFlag
#ifdef __WINDOWS__
#define NS_SetMultiThreadFlag					SetMultiThreadFlag
#define NS_SetUseDirectDrawDeviceIndex			SetUseDirectDrawDeviceIndex
#define NS_SetAeroDisableFlag					SetAeroDisableFlag
#define NS_SetUseDirect3D9Ex					SetUseDirect3D9Ex
#define NS_SetUseDirect3D11						SetUseDirect3D11
#define NS_SetUseDirect3D11MinFeatureLevel		SetUseDirect3D11MinFeatureLevel
#define NS_SetUseDirect3D11WARPDriver			SetUseDirect3D11WARPDriver
#define NS_SetUseDirect3DVersion				SetUseDirect3DVersion
#define NS_GetUseDirect3DVersion				GetUseDirect3DVersion
#define NS_GetUseDirect3D11FeatureLevel			GetUseDirect3D11FeatureLevel
#define NS_SetUseDirectDrawFlag					SetUseDirectDrawFlag
#define NS_SetUseGDIFlag						SetUseGDIFlag
#define NS_GetUseGDIFlag						GetUseGDIFlag
#define NS_SetDDrawUseGuid						SetDDrawUseGuid
#define NS_GetUseDDrawObj						GetUseDDrawObj
#define NS_GetDirectDrawDeviceGUID				GetDirectDrawDeviceGUID
#define NS_GetDirectDrawDeviceDescription		GetDirectDrawDeviceDescription
#define NS_GetDirectDrawDeviceNum				GetDirectDrawDeviceNum
#define NS_GetUseDirect3DDevice9				GetUseDirect3DDevice9
#define NS_GetUseDirect3D9BackBufferSurface		GetUseDirect3D9BackBufferSurface
#define NS_GetUseDirect3D11Device						GetUseDirect3D11Device
#define NS_GetUseDirect3D11DeviceContext				GetUseDirect3D11DeviceContext
#define NS_GetUseDirect3D11BackBufferTexture2D			GetUseDirect3D11BackBufferTexture2D
#define NS_GetUseDirect3D11BackBufferRenderTargetView	GetUseDirect3D11BackBufferRenderTargetView
#define NS_GetUseDirect3D11DepthStencilTexture2D		GetUseDirect3D11DepthStencilTexture2D
#define NS_SetDrawScreen_ID3D11RenderTargetView	SetDrawScreen_ID3D11RenderTargetView
#define NS_RefreshDxLibDirect3DSetting			RefreshDxLibDirect3DSetting
#ifndef DX_NON_MEDIA_FOUNDATION
#define NS_SetUseMediaFoundationFlag			SetUseMediaFoundationFlag
#endif // DX_NON_MEDIA_FOUNDATION
#endif // __WINDOWS__
#define NS_RenderVertex							RenderVertex
#define NS_GetDrawCallCount						GetDrawCallCount
#define NS_GetFPS								GetFPS

#ifndef DX_NON_SAVEFUNCTION

// 画面画像保存関数
// Jpeg_Quality         = 0:低画質〜100:高画質
// Png_CompressionLevel = 0:無圧縮〜  9:最高圧縮
#define NS_SaveDrawScreen						SaveDrawScreen
#define NS_SaveDrawScreenWithStrLen				SaveDrawScreenWithStrLen
#define NS_SaveDrawScreenToBMP					SaveDrawScreenToBMP
#define NS_SaveDrawScreenToBMPWithStrLen		SaveDrawScreenToBMPWithStrLen
#define NS_SaveDrawScreenToDDS					SaveDrawScreenToDDS
#define NS_SaveDrawScreenToDDSWithStrLen		SaveDrawScreenToDDSWithStrLen
#define NS_SaveDrawScreenToJPEG					SaveDrawScreenToJPEG
#define NS_SaveDrawScreenToJPEGWithStrLen		SaveDrawScreenToJPEGWithStrLen
#define NS_SaveDrawScreenToPNG					SaveDrawScreenToPNG
#define NS_SaveDrawScreenToPNGWithStrLen		SaveDrawScreenToPNGWithStrLen

// 描画対象にできるグラフィックハンドル保存関数
// Jpeg_Quality         = 0:低画質〜100:高画質
// Png_CompressionLevel = 0:無圧縮〜  9:最高圧縮
#define NS_SaveDrawValidGraph					SaveDrawValidGraph
#define NS_SaveDrawValidGraphWithStrLen			SaveDrawValidGraphWithStrLen
#define NS_SaveDrawValidGraphToBMP				SaveDrawValidGraphToBMP
#define NS_SaveDrawValidGraphToBMPWithStrLen	SaveDrawValidGraphToBMPWithStrLen
#define NS_SaveDrawValidGraphToDDS				SaveDrawValidGraphToDDS
#define NS_SaveDrawValidGraphToDDSWithStrLen	SaveDrawValidGraphToDDSWithStrLen
#define NS_SaveDrawValidGraphToJPEG				SaveDrawValidGraphToJPEG
#define NS_SaveDrawValidGraphToJPEGWithStrLen	SaveDrawValidGraphToJPEGWithStrLen
#define NS_SaveDrawValidGraphToPNG				SaveDrawValidGraphToPNG
#define NS_SaveDrawValidGraphToPNGWithStrLen	SaveDrawValidGraphToPNGWithStrLen

#endif // DX_NON_SAVEFUNCTION

// 頂点バッファ関係関数
#define NS_CreateVertexBuffer					CreateVertexBuffer
#define NS_DeleteVertexBuffer					DeleteVertexBuffer
#define NS_InitVertexBuffer						InitVertexBuffer
#define NS_SetVertexBufferData					SetVertexBufferData
#define NS_CreateIndexBuffer					CreateIndexBuffer
#define NS_DeleteIndexBuffer					DeleteIndexBuffer
#define NS_InitIndexBuffer						InitIndexBuffer
#define NS_SetIndexBufferData					SetIndexBufferData
#define NS_GetMaxPrimitiveCount					GetMaxPrimitiveCount
#define NS_GetMaxVertexIndex					GetMaxVertexIndex

// シェーダー関係関数
#define NS_GetValidShaderVersion				GetValidShaderVersion

#define NS_LoadVertexShader						LoadVertexShader
#define NS_LoadVertexShaderWithStrLen			LoadVertexShaderWithStrLen
#define NS_LoadPixelShader						LoadPixelShader
#define NS_LoadPixelShaderWithStrLen			LoadPixelShaderWithStrLen
#define NS_LoadVertexShaderFromMem				LoadVertexShaderFromMem
#define NS_LoadPixelShaderFromMem				LoadPixelShaderFromMem
#define NS_DeleteShader							DeleteShader
#define NS_InitShader							InitShader

#define NS_GetConstIndexToShader				GetConstIndexToShader
#define NS_GetConstIndexToShaderWithStrLen		GetConstIndexToShaderWithStrLen
#define NS_GetConstCountToShader				GetConstCountToShader
#define NS_GetConstCountToShaderWithStrLen		GetConstCountToShaderWithStrLen
#define NS_GetConstDefaultParamFToShader		GetConstDefaultParamFToShader
#define NS_GetConstDefaultParamFToShaderWithStrLen GetConstDefaultParamFToShaderWithStrLen
#define NS_SetVSConstSF							SetVSConstSF
#define NS_SetVSConstF							SetVSConstF
#define NS_SetVSConstFMtx						SetVSConstFMtx
#define NS_SetVSConstFMtxT						SetVSConstFMtxT
#define NS_SetVSConstSI							SetVSConstSI
#define NS_SetVSConstI							SetVSConstI
#define NS_SetVSConstB							SetVSConstB
#define NS_SetVSConstSFArray					SetVSConstSFArray
#define NS_SetVSConstFArray						SetVSConstFArray
#define NS_SetVSConstFMtxArray					SetVSConstFMtxArray
#define NS_SetVSConstFMtxTArray					SetVSConstFMtxTArray
#define NS_SetVSConstSIArray					SetVSConstSIArray
#define NS_SetVSConstIArray						SetVSConstIArray
#define NS_SetVSConstBArray						SetVSConstBArray
#define NS_ResetVSConstF						ResetVSConstF
#define NS_ResetVSConstI						ResetVSConstI
#define NS_ResetVSConstB						ResetVSConstB

#define NS_SetPSConstSF							SetPSConstSF
#define NS_SetPSConstF							SetPSConstF
#define NS_SetPSConstFMtx						SetPSConstFMtx
#define NS_SetPSConstFMtxT						SetPSConstFMtxT
#define NS_SetPSConstSI							SetPSConstSI
#define NS_SetPSConstI							SetPSConstI
#define NS_SetPSConstB							SetPSConstB
#define NS_SetPSConstSFArray					SetPSConstSFArray
#define NS_SetPSConstFArray						SetPSConstFArray
#define NS_SetPSConstFMtxArray					SetPSConstFMtxArray
#define NS_SetPSConstFMtxTArray					SetPSConstFMtxTArray
#define NS_SetPSConstSIArray					SetPSConstSIArray
#define NS_SetPSConstIArray						SetPSConstIArray
#define NS_SetPSConstBArray						SetPSConstBArray
#define NS_ResetPSConstF						ResetPSConstF
#define NS_ResetPSConstI						ResetPSConstI
#define NS_ResetPSConstB						ResetPSConstB

#define NS_SetRenderTargetToShader				SetRenderTargetToShader
#define NS_SetUseTextureToShader				SetUseTextureToShader
#define NS_SetUseVertexShader					SetUseVertexShader
#define NS_SetUsePixelShader					SetUsePixelShader

#define NS_CalcPolygonBinormalAndTangentsToShader				CalcPolygonBinormalAndTangentsToShader
#define NS_CalcPolygonIndexedBinormalAndTangentsToShader		CalcPolygonIndexedBinormalAndTangentsToShader

#define NS_DrawBillboard3DToShader								DrawBillboard3DToShader
#define NS_DrawPolygon2DToShader								DrawPolygon2DToShader
#define NS_DrawPolygon3DToShader								DrawPolygon3DToShader
#define NS_DrawPolygonIndexed2DToShader							DrawPolygonIndexed2DToShader
#define NS_DrawPolygonIndexed3DToShader							DrawPolygonIndexed3DToShader
#define NS_DrawPrimitive2DToShader								DrawPrimitive2DToShader
#define NS_DrawPrimitive3DToShader								DrawPrimitive3DToShader
#define NS_DrawPrimitiveIndexed2DToShader						DrawPrimitiveIndexed2DToShader
#define NS_DrawPrimitiveIndexed3DToShader						DrawPrimitiveIndexed3DToShader
#define NS_DrawPolygon3DToShader_UseVertexBuffer				DrawPolygon3DToShader_UseVertexBuffer
#define NS_DrawPolygonIndexed3DToShader_UseVertexBuffer			DrawPolygonIndexed3DToShader_UseVertexBuffer
#define NS_DrawPrimitive3DToShader_UseVertexBuffer				DrawPrimitive3DToShader_UseVertexBuffer
#define NS_DrawPrimitive3DToShader_UseVertexBuffer2				DrawPrimitive3DToShader_UseVertexBuffer2
#define NS_DrawPrimitiveIndexed3DToShader_UseVertexBuffer		DrawPrimitiveIndexed3DToShader_UseVertexBuffer
#define NS_DrawPrimitiveIndexed3DToShader_UseVertexBuffer2		DrawPrimitiveIndexed3DToShader_UseVertexBuffer2

// シェーダー用定数バッファ関係関数
#define NS_InitShaderConstantBuffer						InitShaderConstantBuffer
#define NS_CreateShaderConstantBuffer					CreateShaderConstantBuffer
#define NS_DeleteShaderConstantBuffer					DeleteShaderConstantBuffer
#define NS_GetBufferShaderConstantBuffer				GetBufferShaderConstantBuffer
#define NS_UpdateShaderConstantBuffer					UpdateShaderConstantBuffer
#define NS_SetShaderConstantBuffer						SetShaderConstantBuffer

// フィルター関係関数
#ifndef DX_NON_FILTER

#define NS_GraphFilter							GraphFilter
#define NS_GraphFilterBlt						GraphFilterBlt
#define NS_GraphFilterRectBlt					GraphFilterRectBlt
#define NS_GraphBlend							GraphBlend
#define NS_GraphBlendBlt						GraphBlendBlt
#define NS_GraphBlendRectBlt					GraphBlendRectBlt

#endif // DX_NON_FILTER


#ifndef DX_NON_MOVIE

// ムービーグラフィック関係関数
#define NS_PlayMovie							PlayMovie
#define NS_PlayMovieWithStrLen					PlayMovieWithStrLen
#define NS_GetMovieImageSize_File				GetMovieImageSize_File
#define NS_GetMovieImageSize_File_WithStrLen	GetMovieImageSize_File_WithStrLen
#define NS_GetMovieImageSize_Mem				GetMovieImageSize_Mem
#define NS_OpenMovieToGraph						OpenMovieToGraph
#define NS_OpenMovieToGraphWithStrLen			OpenMovieToGraphWithStrLen
#define NS_PlayMovieToGraph						PlayMovieToGraph
#define NS_PauseMovieToGraph					PauseMovieToGraph
#define NS_AddMovieFrameToGraph					AddMovieFrameToGraph
#define NS_SeekMovieToGraph						SeekMovieToGraph
#define NS_SetPlaySpeedRateMovieToGraph			SetPlaySpeedRateMovieToGraph
#define NS_GetMovieStateToGraph					GetMovieStateToGraph
#define NS_SetMovieVolumeToGraph				SetMovieVolumeToGraph
#define NS_ChangeMovieVolumeToGraph				ChangeMovieVolumeToGraph
#define NS_GetMovieBaseImageToGraph				GetMovieBaseImageToGraph
#define NS_GetMovieTotalFrameToGraph			GetMovieTotalFrameToGraph
#define NS_TellMovieToGraph						TellMovieToGraph
#define NS_TellMovieToGraphToFrame				TellMovieToGraphToFrame
#define NS_SeekMovieToGraphToFrame				SeekMovieToGraphToFrame
#define NS_GetOneFrameTimeMovieToGraph			GetOneFrameTimeMovieToGraph
#define NS_GetLastUpdateTimeMovieToGraph		GetLastUpdateTimeMovieToGraph
#define NS_SetMovieRightImageAlphaFlag			SetMovieRightImageAlphaFlag
#define NS_SetMovieColorA8R8G8B8Flag			SetMovieColorA8R8G8B8Flag
#define NS_SetMovieUseYUVFormatSurfaceFlag		SetMovieUseYUVFormatSurfaceFlag

#endif

// カメラ関係関数
#define NS_SetCameraNearFar						SetCameraNearFar
#define NS_SetCameraNearFarD					SetCameraNearFarD
#define NS_SetCameraPositionAndTarget_UpVecY	SetCameraPositionAndTarget_UpVecY
#define NS_SetCameraPositionAndTarget_UpVecYD	SetCameraPositionAndTarget_UpVecYD
#define NS_SetCameraPositionAndTargetAndUpVec	SetCameraPositionAndTargetAndUpVec
#define NS_SetCameraPositionAndTargetAndUpVecD	SetCameraPositionAndTargetAndUpVecD
#define NS_SetCameraPositionAndAngle			SetCameraPositionAndAngle
#define NS_SetCameraPositionAndAngleD			SetCameraPositionAndAngleD
#define NS_SetCameraViewMatrix					SetCameraViewMatrix
#define NS_SetCameraViewMatrixD					SetCameraViewMatrixD
#define NS_SetCameraScreenCenter				SetCameraScreenCenter
#define NS_SetCameraScreenCenterD				SetCameraScreenCenterD

#define NS_SetupCamera_Perspective				SetupCamera_Perspective
#define NS_SetupCamera_PerspectiveD				SetupCamera_PerspectiveD
#define NS_SetupCamera_Ortho					SetupCamera_Ortho
#define NS_SetupCamera_OrthoD					SetupCamera_OrthoD
#define NS_SetupCamera_ProjectionMatrix			SetupCamera_ProjectionMatrix
#define NS_SetupCamera_ProjectionMatrixD		SetupCamera_ProjectionMatrixD
#define NS_SetCameraDotAspect					SetCameraDotAspect
#define NS_SetCameraDotAspectD					SetCameraDotAspectD

#define NS_CheckCameraViewClip					CheckCameraViewClip
#define NS_CheckCameraViewClipD					CheckCameraViewClipD
#define NS_CheckCameraViewClip_Dir				CheckCameraViewClip_Dir
#define NS_CheckCameraViewClip_DirD				CheckCameraViewClip_DirD
#define NS_CheckCameraViewClip_Box				CheckCameraViewClip_Box
#define NS_CheckCameraViewClip_BoxD				CheckCameraViewClip_BoxD

#define NS_GetCameraNear						GetCameraNear
#define NS_GetCameraNearD						GetCameraNearD
#define NS_GetCameraFar							GetCameraFar
#define NS_GetCameraFarD						GetCameraFarD

#define NS_GetCameraPosition					GetCameraPosition
#define NS_GetCameraPositionD					GetCameraPositionD
#define NS_GetCameraTarget						GetCameraTarget
#define NS_GetCameraTargetD						GetCameraTargetD
#define NS_GetCameraUpVector					GetCameraUpVector
#define NS_GetCameraUpVectorD					GetCameraUpVectorD
#define NS_GetCameraDownVector					GetCameraDownVector
#define NS_GetCameraDownVectorD					GetCameraDownVectorD
#define NS_GetCameraRightVector					GetCameraRightVector
#define NS_GetCameraRightVectorD				GetCameraRightVectorD
#define NS_GetCameraLeftVector					GetCameraLeftVector
#define NS_GetCameraLeftVectorD					GetCameraLeftVectorD
#define NS_GetCameraFrontVector					GetCameraFrontVector
#define NS_GetCameraFrontVectorD				GetCameraFrontVectorD
#define NS_GetCameraBackVector					GetCameraBackVector
#define NS_GetCameraBackVectorD					GetCameraBackVectorD
#define NS_GetCameraAngleHRotate				GetCameraAngleHRotate
#define NS_GetCameraAngleHRotateD				GetCameraAngleHRotateD
#define NS_GetCameraAngleVRotate				GetCameraAngleVRotate
#define NS_GetCameraAngleVRotateD				GetCameraAngleVRotateD
#define NS_GetCameraAngleTRotate				GetCameraAngleTRotate
#define NS_GetCameraAngleTRotateD				GetCameraAngleTRotateD

#define NS_GetCameraViewMatrix					GetCameraViewMatrix
#define NS_GetCameraViewMatrixD					GetCameraViewMatrixD
#define NS_GetCameraBillboardMatrix				GetCameraBillboardMatrix
#define NS_GetCameraBillboardMatrixD			GetCameraBillboardMatrixD
#define NS_GetCameraScreenCenter				GetCameraScreenCenter
#define NS_GetCameraScreenCenterD				GetCameraScreenCenterD
#define NS_GetCameraFov							GetCameraFov
#define NS_GetCameraFovD						GetCameraFovD
#define NS_GetCameraSize						GetCameraSize
#define NS_GetCameraSizeD						GetCameraSizeD
#define NS_GetCameraProjectionMatrix			GetCameraProjectionMatrix
#define NS_GetCameraProjectionMatrixD			GetCameraProjectionMatrixD
#define NS_GetCameraDotAspect					GetCameraDotAspect
#define NS_GetCameraDotAspectD					GetCameraDotAspectD
#define NS_GetCameraViewportMatrix				GetCameraViewportMatrix
#define NS_GetCameraViewportMatrixD				GetCameraViewportMatrixD
#define NS_GetCameraAPIViewportMatrix			GetCameraAPIViewportMatrix
#define NS_GetCameraAPIViewportMatrixD			GetCameraAPIViewportMatrixD

// ライト関係関数
#define NS_SetUseLighting						SetUseLighting
#define NS_SetMaterialUseVertDifColor			SetMaterialUseVertDifColor
#define NS_SetMaterialUseVertSpcColor			SetMaterialUseVertSpcColor
#define NS_SetMaterialParam						SetMaterialParam
#define NS_SetUseSpecular						SetUseSpecular
#define NS_SetGlobalAmbientLight				SetGlobalAmbientLight

#define NS_ChangeLightTypeDir					ChangeLightTypeDir
#define NS_ChangeLightTypeSpot					ChangeLightTypeSpot
#define NS_ChangeLightTypePoint					ChangeLightTypePoint
#define NS_SetLightEnable						SetLightEnable
#define NS_SetLightDifColor						SetLightDifColor
#define NS_SetLightSpcColor						SetLightSpcColor
#define NS_SetLightAmbColor						SetLightAmbColor
#define NS_SetLightDirection					SetLightDirection
#define NS_SetLightPosition						SetLightPosition
#define NS_SetLightRangeAtten					SetLightRangeAtten
#define NS_SetLightAngle						SetLightAngle
#define NS_GetLightType							GetLightType
#define NS_GetLightEnable						GetLightEnable
#define NS_GetLightDifColor						GetLightDifColor
#define NS_GetLightSpcColor						GetLightSpcColor
#define NS_GetLightAmbColor						GetLightAmbColor
#define NS_GetLightDirection					GetLightDirection
#define NS_GetLightPosition						GetLightPosition
#define NS_GetLightRangeAtten					GetLightRangeAtten
#define NS_GetLightAngle						GetLightAngle
#define NS_SetLightUseShadowMap					SetLightUseShadowMap

#define NS_CreateDirLightHandle					CreateDirLightHandle
#define NS_CreateSpotLightHandle				CreateSpotLightHandle
#define NS_CreatePointLightHandle				CreatePointLightHandle
#define NS_DeleteLightHandle					DeleteLightHandle
#define NS_DeleteLightHandleAll					DeleteLightHandleAll
#define NS_SetLightTypeHandle					SetLightTypeHandle
#define NS_SetLightEnableHandle					SetLightEnableHandle
#define NS_SetLightDifColorHandle				SetLightDifColorHandle
#define NS_SetLightSpcColorHandle				SetLightSpcColorHandle
#define NS_SetLightAmbColorHandle				SetLightAmbColorHandle
#define NS_SetLightDirectionHandle				SetLightDirectionHandle
#define NS_SetLightPositionHandle				SetLightPositionHandle
#define NS_SetLightRangeAttenHandle				SetLightRangeAttenHandle
#define NS_SetLightAngleHandle					SetLightAngleHandle
#define NS_SetLightUseShadowMapHandle			SetLightUseShadowMapHandle
#define NS_GetLightTypeHandle					GetLightTypeHandle
#define NS_GetLightEnableHandle					GetLightEnableHandle
#define NS_GetLightDifColorHandle				GetLightDifColorHandle
#define NS_GetLightSpcColorHandle				GetLightSpcColorHandle
#define NS_GetLightAmbColorHandle				GetLightAmbColorHandle
#define NS_GetLightDirectionHandle				GetLightDirectionHandle
#define NS_GetLightPositionHandle				GetLightPositionHandle
#define NS_GetLightRangeAttenHandle				GetLightRangeAttenHandle
#define NS_GetLightAngleHandle					GetLightAngleHandle

#define NS_GetEnableLightHandleNum				GetEnableLightHandleNum
#define NS_GetEnableLightHandle					GetEnableLightHandle

// 色情報取得用関数
#define NS_GetTexFormatIndex					GetTexFormatIndex
#define NS_ColorKaiseki							ColorKaiseki


















// DxMask.cpp 関数プロトタイプ宣言

#ifndef DX_NON_MASK

// マスク関係 
#define NS_CreateMaskScreen						CreateMaskScreen
#define NS_DeleteMaskScreen						DeleteMaskScreen
#define NS_DrawMaskToDirectData					DrawMaskToDirectData
#define NS_DrawFillMaskToDirectData				DrawFillMaskToDirectData

#define NS_SetUseMaskScreenFlag					SetUseMaskScreenFlag
#define NS_GetUseMaskScreenFlag					GetUseMaskScreenFlag
#define NS_FillMaskScreen						FillMaskScreen
#define NS_SetMaskScreenGraph					SetMaskScreenGraph
#define NS_SetMaskScreenGraphUseChannel			SetMaskScreenGraphUseChannel

#define NS_InitMask								InitMask
#define NS_MakeMask								MakeMask
#define NS_GetMaskSize							GetMaskSize
#define NS_SetDataToMask						SetDataToMask
#define NS_DeleteMask							DeleteMask
#define NS_BmpBltToMask							BmpBltToMask
#define NS_GraphImageBltToMask					GraphImageBltToMask
#define NS_LoadMask								LoadMask
#define NS_LoadMaskWithStrLen					LoadMaskWithStrLen
#define NS_LoadDivMask							LoadDivMask
#define NS_LoadDivMaskWithStrLen				LoadDivMaskWithStrLen
#define NS_CreateMaskFromMem					CreateMaskFromMem
#define NS_CreateDivMaskFromMem					CreateDivMaskFromMem
#define NS_DrawMask								DrawMask
#ifndef DX_NON_FONT
#define NS_DrawFormatStringMask					DrawFormatStringMask
#define NS_DrawFormatStringMaskToHandle			DrawFormatStringMaskToHandle
#define NS_DrawStringMask						DrawStringMask
#define NS_DrawNStringMask						DrawNStringMask
#define NS_DrawStringMaskToHandle				DrawStringMaskToHandle
#define NS_DrawNStringMaskToHandle				DrawNStringMaskToHandle
#endif // DX_NON_FONT
#define NS_DrawFillMask							DrawFillMask
#define NS_SetMaskReverseEffectFlag				SetMaskReverseEffectFlag

#define NS_GetMaskScreenData					GetMaskScreenData
#define NS_GetMaskUseFlag						GetMaskUseFlag

#endif // DX_NON_MASK

#endif // DX_NOTUSE_DRAWFUNCTION












// DxFont.cpp 関数プロトタイプ宣言

#ifndef DX_NON_FONT

// フォント、文字列描画関係関数
#define NS_EnumFontName								EnumFontName
#define NS_EnumFontNameEx							EnumFontNameEx
#define NS_EnumFontNameEx2							EnumFontNameEx2
#define NS_EnumFontNameEx2WithStrLen				EnumFontNameEx2WithStrLen
#define NS_CheckFontName							CheckFontName
#define NS_CheckFontNameWithStrLen					CheckFontNameWithStrLen

#define NS_InitFontToHandle							InitFontToHandle

#define NS_CreateFontToHandle						CreateFontToHandle
#define NS_CreateFontToHandleWithStrLen				CreateFontToHandleWithStrLen
#define NS_LoadFontDataToHandle						LoadFontDataToHandle
#define NS_LoadFontDataToHandleWithStrLen			LoadFontDataToHandleWithStrLen
#define NS_LoadFontDataFromMemToHandle				LoadFontDataFromMemToHandle
#define NS_SetFontSpaceToHandle						SetFontSpaceToHandle
#define NS_SetFontLineSpaceToHandle					SetFontLineSpaceToHandle
#define NS_SetFontCharCodeFormatToHandle			SetFontCharCodeFormatToHandle
#define NS_SetDefaultFontState						SetDefaultFontState
#define NS_SetDefaultFontStateWithStrLen			SetDefaultFontStateWithStrLen
#define NS_DeleteFontToHandle						DeleteFontToHandle
#define NS_SetFontLostFlag							SetFontLostFlag
#define NS_AddFontImageToHandle						AddFontImageToHandle
#define NS_AddFontImageToHandleWithStrLen			AddFontImageToHandleWithStrLen
#define NS_SubFontImageToHandle						SubFontImageToHandle
#define NS_SubFontImageToHandleWithStrLen			SubFontImageToHandleWithStrLen
#define NS_AddSubstitutionFontToHandle				AddSubstitutionFontToHandle
#define NS_SubSubstitutionFontToHandle				SubSubstitutionFontToHandle

#define NS_SetFontSize								SetFontSize
#define NS_SetFontThickness							SetFontThickness
#define NS_SetFontSpace								SetFontSpace
#define NS_GetFontSpace								GetFontSpace
#define NS_SetFontLineSpace							SetFontLineSpace
#define NS_GetFontLineSpace							GetFontLineSpace
#define NS_SetFontCharCodeFormat					SetFontCharCodeFormat

#define NS_SetFontCacheToTextureFlag				SetFontCacheToTextureFlag
#define NS_SetFontChacheToTextureFlag				SetFontChacheToTextureFlag
#define NS_SetFontCacheCharNum						SetFontCacheCharNum
#define NS_ChangeFont								ChangeFont
#define NS_ChangeFontWithStrLen						ChangeFontWithStrLen
#define NS_ChangeFontType							ChangeFontType
#define NS_GetFontName								GetFontName

#define NS_FontCacheStringDrawToHandle				FontCacheStringDrawToHandle
#define NS_FontCacheStringDrawToHandleWithStrLen	FontCacheStringDrawToHandleWithStrLen
#define NS_FontBaseImageBlt							FontBaseImageBlt
#define NS_FontBaseImageBltWithStrLen				FontBaseImageBltWithStrLen
#define NS_FontBaseImageBltToHandle					FontBaseImageBltToHandle
#define NS_FontBaseImageBltToHandleWithStrLen		FontBaseImageBltToHandleWithStrLen

#define NS_GetFontNameToHandle						GetFontNameToHandle
#define NS_GetFontMaxWidth							GetFontMaxWidth
#define NS_GetFontMaxWidthToHandle					GetFontMaxWidthToHandle
#define NS_GetFontAscent							GetFontAscent
#define NS_GetFontAscentToHandle					GetFontAscentToHandle
#define NS_GetFontCharInfo							GetFontCharInfo
#define NS_GetFontCharInfoWithStrLen				GetFontCharInfoWithStrLen
#define NS_GetDrawStringWidth						GetDrawStringWidth
#define NS_GetDrawNStringWidth						GetDrawNStringWidth
#define NS_GetDrawFormatStringWidth					GetDrawFormatStringWidth
#define NS_GetDrawStringWidthToHandle				GetDrawStringWidthToHandle
#define NS_GetDrawNStringWidthToHandle				GetDrawNStringWidthToHandle
#define NS_GetDrawFormatStringWidthToHandle			GetDrawFormatStringWidthToHandle
#define NS_GetDrawExtendStringWidth					GetDrawExtendStringWidth
#define NS_GetDrawExtendNStringWidth				GetDrawExtendNStringWidth
#define NS_GetDrawExtendFormatStringWidth			GetDrawExtendFormatStringWidth
#define NS_GetDrawExtendStringWidthToHandle			GetDrawExtendStringWidthToHandle
#define NS_GetDrawExtendNStringWidthToHandle		GetDrawExtendNStringWidthToHandle
#define NS_GetDrawExtendFormatStringWidthToHandle	GetDrawExtendFormatStringWidthToHandle
#define NS_GetDrawStringSize						GetDrawStringSize
#define NS_GetDrawNStringSize						GetDrawNStringSize
#define NS_GetDrawFormatStringSize					GetDrawFormatStringSize
#define NS_GetDrawExtendStringSize					GetDrawExtendStringSize
#define NS_GetDrawExtendNStringSize					GetDrawExtendNStringSize
#define NS_GetDrawExtendFormatStringSize			GetDrawExtendFormatStringSize
#define NS_GetDrawStringSizeToHandle				GetDrawStringSizeToHandle
#define NS_GetDrawNStringSizeToHandle				GetDrawNStringSizeToHandle
#define NS_GetDrawFormatStringSizeToHandle			GetDrawFormatStringSizeToHandle
#define NS_GetDrawExtendStringSizeToHandle			GetDrawExtendStringSizeToHandle
#define NS_GetDrawExtendNStringSizeToHandle			GetDrawExtendNStringSizeToHandle
#define NS_GetDrawExtendFormatStringSizeToHandle	GetDrawExtendFormatStringSizeToHandle
#define NS_GetDrawStringCharInfo					GetDrawStringCharInfo
#define NS_GetDrawNStringCharInfo					GetDrawNStringCharInfo
#define NS_GetDrawFormatStringCharInfo				GetDrawFormatStringCharInfo
#define NS_GetDrawExtendStringCharInfo				GetDrawExtendStringCharInfo
#define NS_GetDrawExtendNStringCharInfo				GetDrawExtendNStringCharInfo
#define NS_GetDrawExtendFormatStringCharInfo		GetDrawExtendFormatStringCharInfo
#define NS_GetDrawStringCharInfoToHandle			GetDrawStringCharInfoToHandle
#define NS_GetDrawNStringCharInfoToHandle			GetDrawNStringCharInfoToHandle
#define NS_GetDrawFormatStringCharInfoToHandle		GetDrawFormatStringCharInfoToHandle
#define NS_GetDrawExtendStringCharInfoToHandle		GetDrawExtendStringCharInfoToHandle
#define NS_GetDrawExtendNStringCharInfoToHandle		GetDrawExtendNStringCharInfoToHandle
#define NS_GetDrawExtendFormatStringCharInfoToHandle	GetDrawExtendFormatStringCharInfoToHandle

#define NS_GetFontStateToHandle						GetFontStateToHandle
#define NS_GetDefaultFontHandle						GetDefaultFontHandle
#define NS_GetFontChacheToTextureFlag				GetFontChacheToTextureFlag
#define NS_SetFontCacheTextureColorBitDepth			SetFontCacheTextureColorBitDepth
#define NS_GetFontCacheTextureColorBitDepth			GetFontCacheTextureColorBitDepth
#define NS_GetFontCacheToTextureFlag				GetFontCacheToTextureFlag
#define NS_CheckFontChacheToTextureFlag				CheckFontChacheToTextureFlag
#define NS_CheckFontCacheToTextureFlag				CheckFontCacheToTextureFlag
#define NS_CheckFontHandleValid						CheckFontHandleValid
#define NS_ClearFontCacheToHandle					ClearFontCacheToHandle
#define NS_MultiByteCharCheck						MultiByteCharCheck
#define NS_GetFontCacheCharNum						GetFontCacheCharNum
#define NS_GetFontSize								GetFontSize
#define NS_GetFontEdgeSize							GetFontEdgeSize
#define NS_GetFontSizeToHandle						GetFontSizeToHandle
#define NS_GetFontEdgeSizeToHandle					GetFontEdgeSizeToHandle
#define NS_GetFontSpaceToHandle						GetFontSpaceToHandle
#define NS_GetFontLineSpaceToHandle					GetFontLineSpaceToHandle
#define	NS_SetFontCacheUsePremulAlphaFlag			SetFontCacheUsePremulAlphaFlag
#define NS_GetFontCacheUsePremulAlphaFlag			GetFontCacheUsePremulAlphaFlag
#define NS_SetFontUseAdjustSizeFlag					SetFontUseAdjustSizeFlag
#define NS_GetFontUseAdjustSizeFlag					GetFontUseAdjustSizeFlag

#define NS_AddFontFile								AddFontFile
#define NS_AddFontFileWithStrLen					AddFontFileWithStrLen
#define NS_AddFontFileFromMem						AddFontFileFromMem
#define NS_RemoveFontFile							RemoveFontFile

#define NS_CreateFontDataFile						CreateFontDataFile
#define NS_CreateFontDataFileWithStrLen				CreateFontDataFileWithStrLen

// 文字列描画関数
#define NS_DrawString								DrawString
#define NS_DrawNString								DrawNString
#define NS_DrawVString								DrawVString
#define NS_DrawNVString								DrawNVString
#define NS_DrawFormatString							DrawFormatString
#define NS_DrawFormatVString						DrawFormatVString
#define NS_DrawExtendString							DrawExtendString
#define NS_DrawExtendNString						DrawExtendNString
#define NS_DrawExtendVString						DrawExtendVString
#define NS_DrawExtendNVString						DrawExtendNVString
#define NS_DrawExtendFormatString					DrawExtendFormatString
#define NS_DrawExtendFormatVString					DrawExtendFormatVString
#define NS_DrawRotaString							DrawRotaString
#define NS_DrawRotaNString							DrawRotaNString
#define NS_DrawRotaFormatString						DrawRotaFormatString
#define NS_DrawModiString							DrawModiString
#define NS_DrawModiNString							DrawModiNString
#define NS_DrawModiFormatString						DrawModiFormatString

#define NS_DrawStringF								DrawStringF
#define NS_DrawNStringF								DrawNStringF
#define NS_DrawVStringF								DrawVStringF
#define NS_DrawNVStringF							DrawNVStringF
#define NS_DrawFormatStringF						DrawFormatStringF
#define NS_DrawFormatVStringF						DrawFormatVStringF
#define NS_DrawExtendStringF						DrawExtendStringF
#define NS_DrawExtendNStringF						DrawExtendNStringF
#define NS_DrawExtendVStringF						DrawExtendVStringF
#define NS_DrawExtendNVStringF						DrawExtendNVStringF
#define NS_DrawExtendFormatStringF					DrawExtendFormatStringF
#define NS_DrawExtendFormatVStringF					DrawExtendFormatVStringF
#define NS_DrawRotaStringF							DrawRotaStringF
#define NS_DrawRotaNStringF							DrawRotaNStringF
#define NS_DrawRotaFormatStringF					DrawRotaFormatStringF
#define NS_DrawModiStringF							DrawModiStringF
#define NS_DrawModiNStringF							DrawModiNStringF
#define NS_DrawModiFormatStringF					DrawModiFormatStringF

#define NS_DrawNumberToI							DrawNumberToI
#define NS_DrawNumberToF							DrawNumberToF
#define NS_DrawNumberPlusToI						DrawNumberPlusToI
#define NS_DrawNumberPlusToF						DrawNumberPlusToF

#define NS_DrawStringToZBuffer						DrawStringToZBuffer
#define NS_DrawNStringToZBuffer						DrawNStringToZBuffer
#define NS_DrawVStringToZBuffer						DrawVStringToZBuffer
#define NS_DrawNVStringToZBuffer					DrawNVStringToZBuffer
#define NS_DrawFormatStringToZBuffer				DrawFormatStringToZBuffer
#define NS_DrawFormatVStringToZBuffer				DrawFormatVStringToZBuffer
#define NS_DrawExtendStringToZBuffer				DrawExtendStringToZBuffer
#define NS_DrawExtendNStringToZBuffer				DrawExtendNStringToZBuffer
#define NS_DrawExtendVStringToZBuffer				DrawExtendVStringToZBuffer
#define NS_DrawExtendNVStringToZBuffer				DrawExtendNVStringToZBuffer
#define NS_DrawExtendFormatStringToZBuffer			DrawExtendFormatStringToZBuffer
#define NS_DrawExtendFormatVStringToZBuffer			DrawExtendFormatVStringToZBuffer
#define NS_DrawRotaStringToZBuffer					DrawRotaStringToZBuffer
#define NS_DrawRotaNStringToZBuffer					DrawRotaNStringToZBuffer
#define NS_DrawRotaFormatStringToZBuffer			DrawRotaFormatStringToZBuffer
#define NS_DrawModiStringToZBuffer					DrawModiStringToZBuffer
#define NS_DrawModiNStringToZBuffer					DrawModiNStringToZBuffer
#define NS_DrawModiFormatStringToZBuffer			DrawModiFormatStringToZBuffer


#define NS_DrawStringToHandle						DrawStringToHandle
#define NS_DrawNStringToHandle						DrawNStringToHandle
#define NS_DrawVStringToHandle						DrawVStringToHandle
#define NS_DrawNVStringToHandle						DrawNVStringToHandle
#define NS_DrawFormatStringToHandle					DrawFormatStringToHandle
#define NS_DrawFormatVStringToHandle				DrawFormatVStringToHandle
#define NS_DrawExtendStringToHandle					DrawExtendStringToHandle
#define NS_DrawExtendNStringToHandle				DrawExtendNStringToHandle
#define NS_DrawExtendVStringToHandle				DrawExtendVStringToHandle
#define NS_DrawExtendNVStringToHandle				DrawExtendNVStringToHandle
#define NS_DrawExtendFormatStringToHandle			DrawExtendFormatStringToHandle
#define NS_DrawExtendFormatVStringToHandle			DrawExtendFormatVStringToHandle
#define NS_DrawRotaStringToHandle					DrawRotaStringToHandle
#define NS_DrawRotaNStringToHandle					DrawRotaNStringToHandle
#define NS_DrawRotaFormatStringToHandle				DrawRotaFormatStringToHandle
#define NS_DrawModiStringToHandle					DrawModiStringToHandle
#define NS_DrawModiNStringToHandle					DrawModiNStringToHandle
#define NS_DrawModiFormatStringToHandle				DrawModiFormatStringToHandle

#define NS_DrawStringFToHandle						DrawStringFToHandle
#define NS_DrawNStringFToHandle						DrawNStringFToHandle
#define NS_DrawVStringFToHandle						DrawVStringFToHandle
#define NS_DrawNVStringFToHandle					DrawNVStringFToHandle
#define NS_DrawFormatStringFToHandle				DrawFormatStringFToHandle
#define NS_DrawFormatVStringFToHandle				DrawFormatVStringFToHandle
#define NS_DrawExtendStringFToHandle				DrawExtendStringFToHandle
#define NS_DrawExtendNStringFToHandle				DrawExtendNStringFToHandle
#define NS_DrawExtendVStringFToHandle				DrawExtendVStringFToHandle
#define NS_DrawExtendNVStringFToHandle				DrawExtendNVStringFToHandle
#define NS_DrawExtendFormatStringFToHandle			DrawExtendFormatStringFToHandle
#define NS_DrawExtendFormatVStringFToHandle			DrawExtendFormatVStringFToHandle
#define NS_DrawRotaStringFToHandle					DrawRotaStringFToHandle
#define NS_DrawRotaNStringFToHandle					DrawRotaNStringFToHandle
#define NS_DrawRotaFormatStringFToHandle			DrawRotaFormatStringFToHandle
#define NS_DrawModiStringFToHandle					DrawModiStringFToHandle
#define NS_DrawModiNStringFToHandle					DrawModiNStringFToHandle
#define NS_DrawModiFormatStringFToHandle			DrawModiFormatStringFToHandle

#define NS_DrawNumberToIToHandle					DrawNumberToIToHandle
#define NS_DrawNumberToFToHandle					DrawNumberToFToHandle
#define NS_DrawNumberPlusToIToHandle				DrawNumberPlusToIToHandle
#define NS_DrawNumberPlusToFToHandle				DrawNumberPlusToFToHandle

#define NS_DrawStringToHandleToZBuffer				DrawStringToHandleToZBuffer
#define NS_DrawNStringToHandleToZBuffer				DrawNStringToHandleToZBuffer
#define NS_DrawVStringToHandleToZBuffer				DrawVStringToHandleToZBuffer
#define NS_DrawNVStringToHandleToZBuffer			DrawNVStringToHandleToZBuffer
#define NS_DrawFormatStringToHandleToZBuffer		DrawFormatStringToHandleToZBuffer
#define NS_DrawFormatVStringToHandleToZBuffer		DrawFormatVStringToHandleToZBuffer
#define NS_DrawExtendStringToHandleToZBuffer		DrawExtendStringToHandleToZBuffer
#define NS_DrawExtendNStringToHandleToZBuffer		DrawExtendNStringToHandleToZBuffer
#define NS_DrawExtendVStringToHandleToZBuffer		DrawExtendVStringToHandleToZBuffer
#define NS_DrawExtendNVStringToHandleToZBuffer		DrawExtendNVStringToHandleToZBuffer
#define NS_DrawExtendFormatStringToHandleToZBuffer	DrawExtendFormatStringToHandleToZBuffer
#define NS_DrawExtendFormatVStringToHandleToZBuffer	DrawExtendFormatVStringToHandleToZBuffer
#define NS_DrawRotaStringToHandleToZBuffer			DrawRotaStringToHandleToZBuffer
#define NS_DrawRotaNStringToHandleToZBuffer			DrawRotaNStringToHandleToZBuffer
#define NS_DrawRotaFormatStringToHandleToZBuffer	DrawRotaFormatStringToHandleToZBuffer
#define NS_DrawModiStringToHandleToZBuffer			DrawModiStringToHandleToZBuffer
#define NS_DrawModiNStringToHandleToZBuffer			DrawModiNStringToHandleToZBuffer
#define NS_DrawModiFormatStringToHandleToZBuffer	DrawModiFormatStringToHandleToZBuffer

#endif // DX_NON_FONT










// DxMath.cpp 関数プロトタイプ宣言

// 演算ライブラリ









// DxBaseImage.cpp 関数プロトタイプ宣言

// 基本イメージデータのロード＋ＤＩＢ関係
#define NS_CreateGraphImageOrDIBGraph			CreateGraphImageOrDIBGraph
#define NS_CreateGraphImageOrDIBGraphWithStrLen	CreateGraphImageOrDIBGraphWithStrLen
#define NS_CreateGraphImageType2				CreateGraphImageType2
#define NS_CreateBmpInfo						CreateBmpInfo
#define NS_GetImageSize_File					GetImageSize_File
#define NS_GetImageSize_FileWithStrLen			GetImageSize_FileWithStrLen
#define NS_GetImageSize_Mem						GetImageSize_Mem
#define NS_GetGraphImageFullColorCode			GetGraphImageFullColorCode
#define NS_CreateGraphImage_plus_Alpha			CreateGraphImage_plus_Alpha
#define NS_CreateGraphImage_plus_AlphaWithStrLen CreateGraphImage_plus_AlphaWithStrLen
#define NS_ReverseGraphImage					ReverseGraphImage
#ifdef __WINDOWS__
#define NS_CreateDIBGraph						CreateDIBGraph
#define NS_CreateDIBGraphWithStrLen				CreateDIBGraphWithStrLen
#define NS_CreateDIBGraphToMem					CreateDIBGraphToMem
#define NS_CreateDIBGraph_plus_Alpha			CreateDIBGraph_plus_Alpha
#define NS_CreateDIBGraph_plus_AlphaWithStrLen	CreateDIBGraph_plus_AlphaWithStrLen
#define NS_CreateDIBGraphVer2					CreateDIBGraphVer2
#define NS_CreateDIBGraphVer2WithStrLen			CreateDIBGraphVer2WithStrLen
#define NS_CreateDIBGraphVer2_plus_Alpha		CreateDIBGraphVer2_plus_Alpha
#define NS_CreateDIBGraphVer2_plus_AlphaWithStrLen CreateDIBGraphVer2_plus_AlphaWithStrLen
#define NS_ConvBitmapToGraphImage				ConvBitmapToGraphImage
#define NS_ConvGraphImageToBitmap				ConvGraphImageToBitmap
#endif // __WINDOWS__

//#define NS_AddUserGraphLoadFunction			S_AddUserGraphLoadFunction
//#define NS_AddUserGraphLoadFunction2			S_AddUserGraphLoadFunction2
//#define NS_AddUserGraphLoadFunction3			S_AddUserGraphLoadFunction3
#define NS_AddUserGraphLoadFunction4			AddUserGraphLoadFunction4
//#define NS_SubUserGraphLoadFunction			S_SubUserGraphLoadFunction
//#define NS_SubUserGraphLoadFunction2			S_SubUserGraphLoadFunction2
//#define NS_SubUserGraphLoadFunction3			S_SubUserGraphLoadFunction3
#define NS_SubUserGraphLoadFunction4			SubUserGraphLoadFunction4

#define NS_SetUseFastLoadFlag					SetUseFastLoadFlag
#define NS_GetGraphDataShavedMode				GetGraphDataShavedMode
#define NS_SetGraphDataShavedMode				SetGraphDataShavedMode
#define NS_SetUsePremulAlphaConvertLoad			SetUsePremulAlphaConvertLoad

//基本イメージデータ構造体関係
#define NS_CreateBaseImage						CreateBaseImage
#define NS_CreateBaseImageWithStrLen			CreateBaseImageWithStrLen
#define NS_CreateGraphImage						CreateGraphImage
#define NS_CreateBaseImageToFile				CreateBaseImageToFile
#define NS_CreateBaseImageToFileWithStrLen		CreateBaseImageToFileWithStrLen
#define NS_CreateBaseImageToMem					CreateBaseImageToMem
#define NS_CreateARGBF32ColorBaseImage			CreateARGBF32ColorBaseImage
#define NS_CreateARGBF16ColorBaseImage			CreateARGBF16ColorBaseImage
#define NS_CreateARGB8ColorBaseImage			CreateARGB8ColorBaseImage
#define NS_CreateXRGB8ColorBaseImage			CreateXRGB8ColorBaseImage
#define NS_CreateRGB8ColorBaseImage				CreateRGB8ColorBaseImage
#define NS_CreateARGB4ColorBaseImage			CreateARGB4ColorBaseImage
#define NS_CreateA1R5G5B5ColorBaseImage			CreateA1R5G5B5ColorBaseImage
#define NS_CreateX1R5G5B5ColorBaseImage			CreateX1R5G5B5ColorBaseImage
#define NS_CreateR5G5B5A1ColorBaseImage			CreateR5G5B5A1ColorBaseImage
#define NS_CreateR5G6B5ColorBaseImage			CreateR5G6B5ColorBaseImage
#define NS_CreatePAL8ColorBaseImage				CreatePAL8ColorBaseImage
#define NS_CreateColorDataBaseImage				CreateColorDataBaseImage
#define NS_GetBaseImageGraphDataSize			GetBaseImageGraphDataSize
#define NS_DerivationBaseImage					DerivationBaseImage

#define NS_ReleaseBaseImage						ReleaseBaseImage
#define NS_ReleaseGraphImage					ReleaseGraphImage

#define NS_ConvertNormalFormatBaseImage			ConvertNormalFormatBaseImage
#define NS_ConvertPremulAlphaBaseImage			ConvertPremulAlphaBaseImage
#define NS_ConvertInterpAlphaBaseImage			ConvertInterpAlphaBaseImage

#define NS_GetDrawScreenBaseImage				GetDrawScreenBaseImage
#define NS_GetDrawScreenBaseImageDestPos		GetDrawScreenBaseImageDestPos
#ifdef __WINDOWS__
#define NS_UpdateLayerdWindowForBaseImage		UpdateLayerdWindowForBaseImage
#define NS_UpdateLayerdWindowForBaseImageRect	UpdateLayerdWindowForBaseImageRect
#define NS_UpdateLayerdWindowForPremultipliedAlphaBaseImage			UpdateLayerdWindowForPremultipliedAlphaBaseImage
#define NS_UpdateLayerdWindowForPremultipliedAlphaBaseImageRect		UpdateLayerdWindowForPremultipliedAlphaBaseImageRect
#define NS_GetDesktopScreenBaseImage			GetDesktopScreenBaseImage
#endif // __WINDOWS__
#define NS_FillBaseImage						FillBaseImage
#define NS_FillRectBaseImage					FillRectBaseImage
#define NS_ClearRectBaseImage					ClearRectBaseImage
#define NS_GetPaletteBaseImage					GetPaletteBaseImage
#define NS_SetPaletteBaseImage					SetPaletteBaseImage
#define NS_SetPixelPalCodeBaseImage				SetPixelPalCodeBaseImage
#define NS_GetPixelPalCodeBaseImage				GetPixelPalCodeBaseImage
#define NS_SetPixelBaseImage					SetPixelBaseImage
#define NS_SetPixelBaseImageF					SetPixelBaseImageF
#define NS_GetPixelBaseImage					GetPixelBaseImage
#define NS_GetPixelBaseImageF					GetPixelBaseImageF
#define NS_DrawLineBaseImage					DrawLineBaseImage
#define NS_DrawCircleBaseImage					DrawCircleBaseImage
#define NS_BltBaseImage							BltBaseImage
#define NS_BltBaseImage							BltBaseImage
#define NS_BltBaseImageWithTransColor			BltBaseImageWithTransColor
#define NS_BltBaseImageWithAlphaBlend			BltBaseImageWithAlphaBlend
#define NS_ReverseBaseImageH					ReverseBaseImageH
#define NS_ReverseBaseImageV					ReverseBaseImageV
#define NS_ReverseBaseImage						ReverseBaseImage
#define NS_CheckPixelAlphaBaseImage				CheckPixelAlphaBaseImage
#define NS_GetBaseImageUseMaxPaletteNo			GetBaseImageUseMaxPaletteNo

#ifndef DX_NON_JPEGREAD
#define NS_ReadJpegExif							ReadJpegExif
#define NS_ReadJpegExifWithStrLen				ReadJpegExifWithStrLen
#endif // DX_NON_JPEGREAD

#ifndef DX_NON_SAVEFUNCTION

#define NS_SaveBaseImageToBmp					SaveBaseImageToBmp
#define NS_SaveBaseImageToBmpWithStrLen			SaveBaseImageToBmpWithStrLen
#define NS_SaveBaseImageToDds					SaveBaseImageToDds
#define NS_SaveBaseImageToDdsWithStrLen			SaveBaseImageToDdsWithStrLen
#ifndef DX_NON_PNGREAD
#define NS_SaveBaseImageToPng					SaveBaseImageToPng
#define NS_SaveBaseImageToPngWithStrLen			SaveBaseImageToPngWithStrLen
#endif
#ifndef DX_NON_JPEGREAD
#define NS_SaveBaseImageToJpeg					SaveBaseImageToJpeg
#define NS_SaveBaseImageToJpegWithStrLen		SaveBaseImageToJpegWithStrLen
#endif

#endif // DX_NON_SAVEFUNCTION

// 基本イメージ描画
#define NS_DrawBaseImage						DrawBaseImage

// カラーマッチングしながらグラフィックデータ間転送を行う Ver2
#define NS_GraphColorMatchBltVer2				GraphColorMatchBltVer2







// 色情報取得関係
#define NS_GetColorF							GetColorF
#define NS_GetColorU8							GetColorU8
#define NS_GetColor								GetColor
#define NS_GetColor2							GetColor2
#define NS_GetColor3							GetColor3
#define NS_GetColor4							GetColor4
#define NS_GetColor5							GetColor5
#define NS_CreatePaletteColorData				CreatePaletteColorData
#define NS_CreateARGBF32ColorData				CreateARGBF32ColorData
#define NS_CreateARGBF16ColorData				CreateARGBF16ColorData
#define NS_CreateXRGB8ColorData					CreateXRGB8ColorData
#define NS_CreateARGB8ColorData					CreateARGB8ColorData
#define NS_CreateARGB4ColorData					CreateARGB4ColorData
#define NS_CreateA1R5G5B5ColorData				CreateA1R5G5B5ColorData
#define NS_CreateX1R5G5B5ColorData				CreateX1R5G5B5ColorData
#define NS_CreateR5G5B5A1ColorData				CreateR5G5B5A1ColorData
#define NS_CreateR5G6B5ColorData				CreateR5G6B5ColorData
#define NS_CreateFullColorData					CreateFullColorData
#define NS_CreateGrayColorData					CreateGrayColorData
#define NS_CreatePal8ColorData					CreatePal8ColorData
#define NS_CreateColorData						CreateColorData
#define NS_SetColorDataNoneMask					SetColorDataNoneMask
#define NS_CmpColorData							CmpColorData









// DxSoftImage.cpp関数プロトタイプ宣言
#ifndef DX_NON_SOFTIMAGE
#define NS_InitSoftImage						InitSoftImage
#define	NS_LoadSoftImage						LoadSoftImage
#define	NS_LoadSoftImageWithStrLen				LoadSoftImageWithStrLen
#define	NS_LoadARGB8ColorSoftImage				LoadARGB8ColorSoftImage
#define	NS_LoadARGB8ColorSoftImageWithStrLen	LoadARGB8ColorSoftImageWithStrLen
#define	NS_LoadXRGB8ColorSoftImage				LoadXRGB8ColorSoftImage
#define	NS_LoadXRGB8ColorSoftImageWithStrLen	LoadXRGB8ColorSoftImageWithStrLen
#define	NS_LoadSoftImageToMem					LoadSoftImageToMem
#define	NS_LoadARGB8ColorSoftImageToMem			LoadARGB8ColorSoftImageToMem
#define	NS_LoadXRGB8ColorSoftImageToMem			LoadXRGB8ColorSoftImageToMem
#define NS_MakeSoftImage						MakeSoftImage
#define NS_MakeARGBF32ColorSoftImage			MakeARGBF32ColorSoftImage
#define NS_MakeARGBF16ColorSoftImage			MakeARGBF16ColorSoftImage
#define NS_MakeARGB8ColorSoftImage				MakeARGB8ColorSoftImage
#define NS_MakeXRGB8ColorSoftImage				MakeXRGB8ColorSoftImage
#define NS_MakeARGB4ColorSoftImage				MakeARGB4ColorSoftImage
#define NS_MakeA1R5G5B5ColorSoftImage			MakeA1R5G5B5ColorSoftImage
#define NS_MakeX1R5G5B5ColorSoftImage			MakeX1R5G5B5ColorSoftImage
#define NS_MakeR5G5B5A1ColorSoftImage			MakeR5G5B5A1ColorSoftImage
#define NS_MakeR5G6B5ColorSoftImage				MakeR5G6B5ColorSoftImage
#define NS_MakeRGB8ColorSoftImage				MakeRGB8ColorSoftImage
#define NS_MakePAL8ColorSoftImage				MakePAL8ColorSoftImage

#define NS_DeleteSoftImage						DeleteSoftImage

#define NS_GetSoftImageSize						GetSoftImageSize
#define NS_CheckPaletteSoftImage				CheckPaletteSoftImage
#define NS_CheckAlphaSoftImage					CheckAlphaSoftImage
#define NS_CheckPixelAlphaSoftImage				CheckPixelAlphaSoftImage

#define NS_GetDrawScreenSoftImage				GetDrawScreenSoftImage
#define NS_GetDrawScreenSoftImageDestPos		GetDrawScreenSoftImageDestPos
#ifdef __WINDOWS__
#define NS_UpdateLayerdWindowForSoftImage		UpdateLayerdWindowForSoftImage
#define NS_UpdateLayerdWindowForSoftImageRect	UpdateLayerdWindowForSoftImageRect
#define NS_UpdateLayerdWindowForPremultipliedAlphaSoftImage			UpdateLayerdWindowForPremultipliedAlphaSoftImage
#define NS_UpdateLayerdWindowForPremultipliedAlphaSoftImageRect		UpdateLayerdWindowForPremultipliedAlphaSoftImageRect
#define NS_GetDesktopScreenSoftImage			GetDesktopScreenSoftImage
#endif // __WINDOWS__
#define NS_FillSoftImage						FillSoftImage
#define NS_ClearRectSoftImage					ClearRectSoftImage
#define NS_GetPaletteSoftImage					GetPaletteSoftImage
#define NS_SetPaletteSoftImage					SetPaletteSoftImage
#define NS_DrawPixelPalCodeSoftImage			DrawPixelPalCodeSoftImage
#define NS_GetPixelPalCodeSoftImage				GetPixelPalCodeSoftImage
#define NS_GetImageAddressSoftImage				GetImageAddressSoftImage
#define NS_GetPitchSoftImage					GetPitchSoftImage
#define NS_DrawPixelSoftImage					DrawPixelSoftImage
#define NS_DrawPixelSoftImageF					DrawPixelSoftImageF
#define NS_DrawPixelSoftImage_Unsafe_XRGB8		DrawPixelSoftImage_Unsafe_XRGB8
#define NS_DrawPixelSoftImage_Unsafe_ARGB8		DrawPixelSoftImage_Unsafe_ARGB8
#define NS_GetPixelSoftImage					GetPixelSoftImage
#define NS_GetPixelSoftImageF					GetPixelSoftImageF
#define NS_GetPixelSoftImage_Unsafe_XRGB8		GetPixelSoftImage_Unsafe_XRGB8
#define NS_GetPixelSoftImage_Unsafe_ARGB8		GetPixelSoftImage_Unsafe_ARGB8
#define NS_DrawLineSoftImage					DrawLineSoftImage
#define NS_DrawCircleSoftImage					DrawCircleSoftImage
#define NS_BltSoftImage							BltSoftImage
#define NS_BltSoftImageWithTransColor			BltSoftImageWithTransColor
#define NS_BltSoftImageWithAlphaBlend			BltSoftImageWithAlphaBlend
#define NS_ReverseSoftImageH					ReverseSoftImageH
#define NS_ReverseSoftImageV					ReverseSoftImageV
#define NS_ReverseSoftImage						ReverseSoftImage

#ifndef DX_NON_FONT
#define NS_BltStringSoftImage					BltStringSoftImage
#define NS_BltStringSoftImageWithStrLen			BltStringSoftImageWithStrLen
#define NS_BltStringSoftImageToHandle			BltStringSoftImageToHandle
#define NS_BltStringSoftImageToHandleWithStrLen	BltStringSoftImageToHandleWithStrLen
#endif  // DX_NON_FONT

#define NS_DrawSoftImage						DrawSoftImage

#ifndef DX_NON_SAVEFUNCTION

#define NS_SaveSoftImageToBmp					SaveSoftImageToBmp
#define NS_SaveSoftImageToBmpWithStrLen			SaveSoftImageToBmpWithStrLen
#define NS_SaveSoftImageToDds					SaveSoftImageToDds
#define NS_SaveSoftImageToDdsWithStrLen			SaveSoftImageToDdsWithStrLen
#ifndef DX_NON_PNGREAD
#define NS_SaveSoftImageToPng					SaveSoftImageToPng
#define NS_SaveSoftImageToPngWithStrLen			SaveSoftImageToPngWithStrLen
#endif
#ifndef DX_NON_JPEGREAD
#define NS_SaveSoftImageToJpeg					SaveSoftImageToJpeg
#define NS_SaveSoftImageToJpegWithStrLen		SaveSoftImageToJpegWithStrLen
#endif

#endif // DX_NON_SAVEFUNCTION

#endif // DX_NON_SOFTIMAGE
















#ifndef DX_NON_SOUND

// DxSound.cpp関数プロトタイプ宣言

// サウンドデータ管理系関数
#define NS_InitSoundMem							InitSoundMem

#define NS_AddSoundData							AddSoundData
#define NS_AddStreamSoundMem					AddStreamSoundMem
#define NS_AddStreamSoundMemToMem				AddStreamSoundMemToMem
#define NS_AddStreamSoundMemToFile				AddStreamSoundMemToFile
#define NS_AddStreamSoundMemToFileWithStrLen	AddStreamSoundMemToFileWithStrLen
#define NS_SetupStreamSoundMem					SetupStreamSoundMem
#define NS_PlayStreamSoundMem					PlayStreamSoundMem
#define NS_CheckStreamSoundMem					CheckStreamSoundMem
#define NS_StopStreamSoundMem					StopStreamSoundMem
#define NS_SetStreamSoundCurrentPosition		SetStreamSoundCurrentPosition
#define NS_GetStreamSoundCurrentPosition		GetStreamSoundCurrentPosition
#define NS_SetStreamSoundCurrentTime			SetStreamSoundCurrentTime
#define NS_GetStreamSoundCurrentTime			GetStreamSoundCurrentTime
#define NS_ProcessStreamSoundMem				ProcessStreamSoundMem
#define NS_ProcessStreamSoundMemAll				ProcessStreamSoundMemAll


#define NS_LoadSoundMem2						LoadSoundMem2
#define NS_LoadSoundMem2WithStrLen				LoadSoundMem2WithStrLen
#define NS_LoadBGM								LoadBGM
#define NS_LoadBGMWithStrLen					LoadBGMWithStrLen

#define NS_LoadSoundMemBase						LoadSoundMemBase
#define NS_LoadSoundMemBaseWithStrLen			LoadSoundMemBaseWithStrLen
#define NS_LoadSoundMem							LoadSoundMem
#define NS_LoadSoundMemWithStrLen				LoadSoundMemWithStrLen
#define NS_LoadSoundMemToBufNumSitei			LoadSoundMemToBufNumSitei
#define NS_LoadSoundMemToBufNumSiteiWithStrLen	LoadSoundMemToBufNumSiteiWithStrLen
#define NS_LoadSoundMemByResource				LoadSoundMemByResource
#define NS_LoadSoundMemByResourceWithStrLen		LoadSoundMemByResourceWithStrLen
#define NS_DuplicateSoundMem					DuplicateSoundMem

#define NS_LoadSoundMemByMemImageBase			LoadSoundMemByMemImageBase
#define NS_LoadSoundMemByMemImage				LoadSoundMemByMemImage
#define NS_LoadSoundMemByMemImage2				LoadSoundMemByMemImage2
#define NS_LoadSoundMemByMemImageToBufNumSitei	LoadSoundMemByMemImageToBufNumSitei
#define NS_LoadSoundMem2ByMemImage				LoadSoundMem2ByMemImage
#define NS_LoadSoundMemFromSoftSound			LoadSoundMemFromSoftSound

#define NS_DeleteSoundMem						DeleteSoundMem

#define NS_PlaySoundMem							PlaySoundMem
#define NS_StopSoundMem							StopSoundMem
#define NS_CheckSoundMem						CheckSoundMem
#define NS_SetPanSoundMem						SetPanSoundMem
#define NS_ChangePanSoundMem					ChangePanSoundMem
#define NS_GetPanSoundMem						GetPanSoundMem
#define NS_SetVolumeSoundMem					SetVolumeSoundMem
#define NS_ChangeVolumeSoundMem					ChangeVolumeSoundMem
#define NS_GetVolumeSoundMem					GetVolumeSoundMem
#define NS_GetVolumeSoundMem2					GetVolumeSoundMem2
#define NS_SetChannelVolumeSoundMem				SetChannelVolumeSoundMem
#define NS_ChangeChannelVolumeSoundMem			ChangeChannelVolumeSoundMem
#define NS_GetChannelVolumeSoundMem				GetChannelVolumeSoundMem
#define NS_GetChannelVolumeSoundMem2			GetChannelVolumeSoundMem2
#define NS_SetFrequencySoundMem					SetFrequencySoundMem
#define NS_GetFrequencySoundMem					GetFrequencySoundMem
#define NS_ResetFrequencySoundMem				ResetFrequencySoundMem

#define NS_SetNextPlayPanSoundMem				SetNextPlayPanSoundMem
#define NS_ChangeNextPlayPanSoundMem			ChangeNextPlayPanSoundMem
#define NS_SetNextPlayVolumeSoundMem			SetNextPlayVolumeSoundMem
#define NS_ChangeNextPlayVolumeSoundMem			ChangeNextPlayVolumeSoundMem
#define NS_SetNextPlayChannelVolumeSoundMem		SetNextPlayChannelVolumeSoundMem
#define NS_ChangeNextPlayChannelVolumeSoundMem	ChangeNextPlayChannelVolumeSoundMem
#define NS_SetNextPlayFrequencySoundMem			SetNextPlayFrequencySoundMem

#define NS_SetCurrentPositionSoundMem			SetCurrentPositionSoundMem
#define NS_GetCurrentPositionSoundMem			GetCurrentPositionSoundMem
#define NS_SetSoundCurrentPosition				SetSoundCurrentPosition
#define NS_GetSoundCurrentPosition				GetSoundCurrentPosition
#define NS_SetSoundCurrentTime					SetSoundCurrentTime
#define NS_GetSoundCurrentTime					GetSoundCurrentTime
#define NS_GetSoundTotalSample					GetSoundTotalSample
#define NS_GetSoundTotalTime					GetSoundTotalTime

#define NS_SetLoopPosSoundMem					SetLoopPosSoundMem
#define NS_SetLoopTimePosSoundMem				SetLoopTimePosSoundMem
#define NS_SetLoopSamplePosSoundMem				SetLoopSamplePosSoundMem

#define NS_SetLoopStartTimePosSoundMem			SetLoopStartTimePosSoundMem
#define NS_SetLoopStartSamplePosSoundMem		SetLoopStartSamplePosSoundMem

#define NS_SetLoopAreaTimePosSoundMem			SetLoopAreaTimePosSoundMem
#define NS_GetLoopAreaTimePosSoundMem			GetLoopAreaTimePosSoundMem
#define NS_SetLoopAreaSamplePosSoundMem			SetLoopAreaSamplePosSoundMem
#define NS_GetLoopAreaSamplePosSoundMem			GetLoopAreaSamplePosSoundMem

#define NS_SetPlayFinishDeleteSoundMem			SetPlayFinishDeleteSoundMem

#define NS_Set3DReverbParamSoundMem				Set3DReverbParamSoundMem
#define NS_Set3DPresetReverbParamSoundMem		Set3DPresetReverbParamSoundMem
#define NS_Set3DReverbParamSoundMemAll			Set3DReverbParamSoundMemAll
#define NS_Set3DPresetReverbParamSoundMemAll	Set3DPresetReverbParamSoundMemAll
#define NS_Get3DReverbParamSoundMem				Get3DReverbParamSoundMem
#define NS_Get3DPresetReverbParamSoundMem		Get3DPresetReverbParamSoundMem

#define NS_Set3DPositionSoundMem				Set3DPositionSoundMem
#define NS_Set3DRadiusSoundMem					Set3DRadiusSoundMem
//#define NS_Set3DInnerRadiusSoundMem			Set3DInnerRadiusSoundMem
#define NS_Set3DVelocitySoundMem				Set3DVelocitySoundMem
//#define NS_Set3DFrontPosition_UpVecYSoundMem	Set3DFrontPosition_UpVecYSoundMem
//#define NS_Set3DFrontPositionSoundMem			Set3DFrontPositionSoundMem
//#define NS_Set3DConeAngleSoundMem				Set3DConeAngleSoundMem
//#define NS_Set3DConeVolumeSoundMem			Set3DConeVolumeSoundMem

#define NS_SetNextPlay3DPositionSoundMem		SetNextPlay3DPositionSoundMem
#define NS_SetNextPlay3DRadiusSoundMem			SetNextPlay3DRadiusSoundMem
#define NS_SetNextPlay3DVelocitySoundMem		SetNextPlay3DVelocitySoundMem


// 特殊関数
#define NS_GetMP3TagInfo						GetMP3TagInfo
#define NS_GetMP3TagInfoWithStrLen				GetMP3TagInfoWithStrLen


// 設定関係関数
#define NS_SetCreateSoundDataType				SetCreateSoundDataType
#define NS_GetCreateSoundDataType				GetCreateSoundDataType
#define NS_SetCreateSoundPitchRate				SetCreateSoundPitchRate
#define NS_GetCreateSoundPitchRate				GetCreateSoundPitchRate
#define NS_SetCreateSoundTimeStretchRate		SetCreateSoundTimeStretchRate
#define NS_GetCreateSoundTimeStretchRate		GetCreateSoundTimeStretchRate
#define NS_SetCreateSoundLoopAreaTimePos		SetCreateSoundLoopAreaTimePos
#define NS_GetCreateSoundLoopAreaTimePos		GetCreateSoundLoopAreaTimePos
#define NS_SetCreateSoundLoopAreaSamplePos		SetCreateSoundLoopAreaSamplePos
#define NS_GetCreateSoundLoopAreaSamplePos		GetCreateSoundLoopAreaSamplePos
#define NS_SetCreateSoundIgnoreLoopAreaInfo		SetCreateSoundIgnoreLoopAreaInfo
#define NS_GetCreateSoundIgnoreLoopAreaInfo		GetCreateSoundIgnoreLoopAreaInfo
#define NS_SetDisableReadSoundFunctionMask		SetDisableReadSoundFunctionMask
#define NS_GetDisableReadSoundFunctionMask		GetDisableReadSoundFunctionMask
#define NS_SetEnableSoundCaptureFlag			SetEnableSoundCaptureFlag
#define NS_SetUseSoftwareMixingSoundFlag		SetUseSoftwareMixingSoundFlag
#define NS_SetEnableXAudioFlag					SetEnableXAudioFlag
#define NS_SetUseOldVolumeCalcFlag				SetUseOldVolumeCalcFlag

#define NS_SetCreate3DSoundFlag					SetCreate3DSoundFlag
#define NS_Set3DSoundOneMetre					Set3DSoundOneMetre
#define NS_Set3DSoundListenerPosAndFrontPos_UpVecY	Set3DSoundListenerPosAndFrontPos_UpVecY
#define NS_Set3DSoundListenerPosAndFrontPosAndUpVec		Set3DSoundListenerPosAndFrontPosAndUpVec
#define NS_Set3DSoundListenerVelocity			Set3DSoundListenerVelocity
#define NS_Set3DSoundListenerConeAngle			Set3DSoundListenerConeAngle
#define NS_Set3DSoundListenerConeVolume			Set3DSoundListenerConeVolume


// 情報取得系関数
#define NS_GetDSoundObj							GetDSoundObj

#ifndef DX_NON_BEEP
// BEEP音再生用命令
#define NS_SetBeepFrequency						SetBeepFrequency
#define NS_PlayBeep								PlayBeep
#define NS_StopBeep								StopBeep
#endif

// ラッパー関数
#define NS_PlaySoundFile						PlaySoundFile
#define NS_PlaySoundFileWithStrLen				PlaySoundFileWithStrLen
#define NS_PlaySound							PlaySound
#define NS_PlaySoundWithStrLen					PlaySoundWithStrLen
#define NS_CheckSoundFile						CheckSoundFile
#define NS_CheckSound							CheckSound
#define NS_StopSoundFile						StopSoundFile
#define NS_StopSound							StopSound
#define NS_SetVolumeSoundFile					SetVolumeSoundFile
#define NS_SetVolumeSound						SetVolumeSound

// ソフトウエア制御サウンド系関数
#define NS_InitSoftSound						InitSoftSound
#define NS_LoadSoftSound						LoadSoftSound
#define NS_LoadSoftSoundWithStrLen				LoadSoftSoundWithStrLen
#define NS_LoadSoftSoundFromMemImage			LoadSoftSoundFromMemImage
#define NS_MakeSoftSound						MakeSoftSound
#define NS_MakeSoftSound2Ch16Bit44KHz			MakeSoftSound2Ch16Bit44KHz
#define NS_MakeSoftSound2Ch16Bit22KHz			MakeSoftSound2Ch16Bit22KHz
#define NS_MakeSoftSound2Ch8Bit44KHz			MakeSoftSound2Ch8Bit44KHz
#define NS_MakeSoftSound2Ch8Bit22KHz			MakeSoftSound2Ch8Bit22KHz
#define NS_MakeSoftSound1Ch16Bit44KHz			MakeSoftSound1Ch16Bit44KHz
#define NS_MakeSoftSound1Ch16Bit22KHz			MakeSoftSound1Ch16Bit22KHz
#define NS_MakeSoftSound1Ch8Bit44KHz			MakeSoftSound1Ch8Bit44KHz
#define NS_MakeSoftSound1Ch8Bit22KHz			MakeSoftSound1Ch8Bit22KHz
#define NS_MakeSoftSoundCustom					MakeSoftSoundCustom
#define NS_DeleteSoftSound						DeleteSoftSound
#ifndef DX_NON_SAVEFUNCTION
#define NS_SaveSoftSound						SaveSoftSound
#define NS_SaveSoftSoundWithStrLen				SaveSoftSoundWithStrLen
#endif // DX_NON_SAVEFUNCTION
#define NS_GetSoftSoundSampleNum				GetSoftSoundSampleNum
#define NS_GetSoftSoundFormat					GetSoftSoundFormat
#define NS_ReadSoftSoundData					ReadSoftSoundData
#define NS_ReadSoftSoundDataF					ReadSoftSoundDataF
#define NS_WriteSoftSoundData					WriteSoftSoundData
#define NS_WriteSoftSoundDataF					WriteSoftSoundDataF
#define NS_WriteTimeStretchSoftSoundData		WriteTimeStretchSoftSoundData
#define NS_WritePitchShiftSoftSoundData			WritePitchShiftSoftSoundData
#define NS_GetSoftSoundDataImage				GetSoftSoundDataImage
#define NS_GetFFTVibrationSoftSound				GetFFTVibrationSoftSound
#define NS_GetFFTVibrationSoftSoundBase			GetFFTVibrationSoftSoundBase

#define NS_InitSoftSoundPlayer					InitSoftSoundPlayer
#define NS_MakeSoftSoundPlayer					MakeSoftSoundPlayer
#define NS_MakeSoftSoundPlayer2Ch16Bit44KHz		MakeSoftSoundPlayer2Ch16Bit44KHz
#define NS_MakeSoftSoundPlayer2Ch16Bit22KHz		MakeSoftSoundPlayer2Ch16Bit22KHz
#define NS_MakeSoftSoundPlayer2Ch8Bit44KHz		MakeSoftSoundPlayer2Ch8Bit44KHz
#define NS_MakeSoftSoundPlayer2Ch8Bit22KHz		MakeSoftSoundPlayer2Ch8Bit22KHz
#define NS_MakeSoftSoundPlayer1Ch16Bit44KHz		MakeSoftSoundPlayer1Ch16Bit44KHz
#define NS_MakeSoftSoundPlayer1Ch16Bit22KHz		MakeSoftSoundPlayer1Ch16Bit22KHz
#define NS_MakeSoftSoundPlayer1Ch8Bit44KHz		MakeSoftSoundPlayer1Ch8Bit44KHz
#define NS_MakeSoftSoundPlayer1Ch8Bit22KHz		MakeSoftSoundPlayer1Ch8Bit22KHz
#define NS_MakeSoftSoundPlayerCustom			MakeSoftSoundPlayerCustom
#define NS_DeleteSoftSoundPlayer				DeleteSoftSoundPlayer
#define NS_AddDataSoftSoundPlayer				AddDataSoftSoundPlayer
#define NS_AddDirectDataSoftSoundPlayer			AddDirectDataSoftSoundPlayer
#define NS_AddOneDataSoftSoundPlayer			AddOneDataSoftSoundPlayer
#define NS_GetSoftSoundPlayerFormat				GetSoftSoundPlayerFormat
#define NS_StartSoftSoundPlayer					StartSoftSoundPlayer
#define NS_CheckStartSoftSoundPlayer			CheckStartSoftSoundPlayer
#define NS_StopSoftSoundPlayer					StopSoftSoundPlayer
#define NS_ResetSoftSoundPlayer					ResetSoftSoundPlayer
#define NS_GetStockDataLengthSoftSoundPlayer	GetStockDataLengthSoftSoundPlayer
#define NS_CheckSoftSoundPlayerNoneData			CheckSoftSoundPlayerNoneData


// ＭＩＤＩ制御関数
#define NS_DeleteMusicMem						DeleteMusicMem
#define NS_LoadMusicMem							LoadMusicMem
#define NS_LoadMusicMemWithStrLen				LoadMusicMemWithStrLen
#define NS_LoadMusicMemByMemImage				LoadMusicMemByMemImage
#define NS_LoadMusicMemByResource				LoadMusicMemByResource
#define NS_LoadMusicMemByResourceWithStrLen		LoadMusicMemByResourceWithStrLen
#define NS_PlayMusicMem							PlayMusicMem
#define NS_StopMusicMem							StopMusicMem
#define NS_CheckMusicMem						CheckMusicMem
#define NS_SetVolumeMusicMem					SetVolumeMusicMem
#define NS_GetMusicMemPosition					GetMusicMemPosition
#define NS_InitMusicMem							InitMusicMem
#define NS_ProcessMusicMem						ProcessMusicMem

#define NS_PlayMusic							PlayMusic
#define NS_PlayMusicWithStrLen					PlayMusicWithStrLen
#define NS_PlayMusicByMemImage					PlayMusicByMemImage
#define NS_PlayMusicByResource					PlayMusicByResource
#define NS_PlayMusicByResourceWithStrLen		PlayMusicByResourceWithStrLen
#define NS_SetVolumeMusic						SetVolumeMusic
#define NS_StopMusic							StopMusic
#define NS_CheckMusic							CheckMusic
#define NS_GetMusicPosition						GetMusicPosition

#define NS_SelectMidiMode						SelectMidiMode

#endif // DX_NON_SOUND










// DxArchive_.cpp 関数 プロトタイプ宣言
#define NS_SetUseDXArchiveFlag					SetUseDXArchiveFlag
#define NS_SetDXArchivePriority					SetDXArchivePriority
#define NS_SetDXArchivePriorityWithStrLen		SetDXArchivePriorityWithStrLen
#define NS_SetDXArchiveExtension				SetDXArchiveExtension
#define NS_SetDXArchiveExtensionWithStrLen		SetDXArchiveExtensionWithStrLen
#define NS_SetDXArchiveKeyString				SetDXArchiveKeyString
#define NS_SetDXArchiveKeyStringWithStrLen		SetDXArchiveKeyStringWithStrLen

#define NS_DXArchivePreLoad						DXArchivePreLoad
#define NS_DXArchivePreLoadWithStrLen			DXArchivePreLoadWithStrLen
#define NS_DXArchiveCheckIdle					DXArchiveCheckIdle
#define NS_DXArchiveCheckIdleWithStrLen			DXArchiveCheckIdleWithStrLen
#define NS_DXArchiveRelease						DXArchiveRelease
#define NS_DXArchiveReleaseWithStrLen			DXArchiveReleaseWithStrLen
#define NS_DXArchiveCheckFile					DXArchiveCheckFile
#define NS_DXArchiveCheckFileWithStrLen			DXArchiveCheckFileWithStrLen
#define NS_DXArchiveSetMemImage					DXArchiveSetMemImage
#define NS_DXArchiveSetMemImageWithStrLen		DXArchiveSetMemImageWithStrLen
#define NS_DXArchiveReleaseMemImage				DXArchiveReleaseMemImage
















// DxModel.cpp 関数 プロトタイプ宣言

#ifndef DX_NON_MODEL


#endif






#endif  // DX_THREAD_SAFE

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif

