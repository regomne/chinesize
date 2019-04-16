// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		通信プログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXNETWORK_H__
#define __DXNETWORK_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_NETWORK

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxHandle.h"
#include "DxRingBuffer.h"
#include "DxThread.h"
#include "DxFile.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define MAX_HTTPHANDLE_NUM			(100)				// 同時に使用できる HTTP ハンドルの数

// HTTP の状態一覧
enum HTTP_STATE
{
	HTTP_ST_CONNECT = 0,				// 接続中
	HTTP_ST_GET = 1,					// GET でファイル受信中
	HTTP_ST_HEAD = 2,					// HEAD でヘッダを受信中
	HTTP_ST_POST = 3,					// POST で主にＣＧＩなどから出力受信中
} ;

// HTTP の処理一覧
enum HTTP_PROC
{
	HTTP_PR_GET,						// GET でファイル取得
	HTTP_PR_POST,						// POST でファイル取得
	HTTP_PR_HEAD,						// HEAD でヘッダ取得
} ;

// 保存先一覧
enum HTTP_SAVEPLACE
{
	HTTP_SV_FILE = 0,					// 保存先はファイル
	HTTP_SV_MEM = 1,					// 保存先はメモリ
} ;

// FTP の状態一覧
enum FTP_STATE
{
	FTP_ST_CONNECT = 0,					// 接続中
	FTP_ST_LOGIN_NAME = 1,				// ログイン中(ユーザーネーム入力中)
	FTP_ST_LOGIN_PASS = 2,				// ログイン中(パスワード入力中)
	FTP_ST_TYPE_A = 3,					// TYPE A 送信中
	FTP_ST_TYPE_B = 4,					// TYPE B 送信中
	FTP_ST_SIZE = 5,					// サイズ取得中
	FTP_ST_QUIT = 6,					// 終了中
	FTP_ST_PASV = 7,					// Passive モード以降中
	FTP_ST_RETR = 8,					// 取得したいファイル名を送信中
	FTP_ST_STOR = 9,					// 送信したいファイル名を送信中
	FTP_ST_DATA_GET = 10,				// データ受信中
	FTP_ST_DATA_SET = 11,				// データ送信中
	FTP_ST_CWD = 12,					// ディレクトリ変更中
} ;

// FTP の処理一覧
enum FTP_PROC
{
	FTP_PR_LOGIN = 0,					// ログイン
	FTP_PR_IDLE = 1,					// 待機状態
	FTP_PR_CHDIR = 2,					// ディレクトリ変更
	FTP_PR_FILE_SET = 3,				// ファイル送信
	FTP_PR_FILE_GET = 4,				// ファイル取得
} ;

// 構造体定義 --------------------------------------------------------------------

// WinSock2.0 で使用する定義と構造体

struct _addrinfo
{
	int						ai_flags ;							// AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
	int						ai_family ;							// PF_xxx
	int						ai_socktype ;						// SOCK_xxx
	int						ai_protocol ;						// 0 or IPPROTO_xxx for IPv4 and IPv6
	size_t					ai_addrlen ;						// Length of ai_addr
	char					*ai_canonname ;						// Canonical name for nodename
	struct sockaddr			*ai_addr ;							// Binary address
	struct _addrinfo		*ai_next ;							// Next structure in linked list
} ;

struct _sockaddr_in6 
{
	short					sin6_family ;						// AF_INET6
	unsigned short			sin6_port ;							// Transport level port number
	unsigned long			sin6_flowinfo ;						// IPv6 flow information
	IPDATA_IPv6				sin6_addr ;							// IPv6 address
//	unsigned long			sin6_scope_id ;						// set of interfaces for a scope
} ;

// ConnectNetWorkBase で使用するグローバルデータを纏めたもの
struct CONNECTNETWORK_GPARAM
{
	int						NonUseDXProtocolFlag ;				// ＤＸライブラリのプロトコルを使用しないかどうかのフラグ
	int						NonCloseAfterLostFlag ;				// 接続が切れた時に自動的にハンドルを解放しないかどうかのフラグ( TRUE:直後に解放はしない  FALSE:直後に解放する )
} ;

// ソケット単体データ構造体
struct SOCKETDATA
{
	HANDLEINFO				HandleInfo ;						// ハンドル共通データ

	int						IsUDP ;								// ＵＤＰ接続かどうか
	int						IsUDPBroadCast ;					// ＵＤＰ接続かつブロードキャスト設定かどうか
	int						UseFlag ;							// データを使用中か、フラグ
	int						IsIPv6 ;							// IPv6を使用しているかどうか

	int						ErrorFlag ;							// 重大なエラーが発生したかフラグ( TRUE:発生した  FALSE:していない )
	int						PreConnectionFlag ;					// 接続前フラグ( TRUE:接続前  FALSE:接続後 )
	int						ConnectionFlag ;					// 接続しているかフラグ( TRUE:している  FALSE:していない )
	int						ConnectionLostFlag ;				// ライブラリユーザーが切断を確認したかフラグ、AcceptedFlagが FALSE の時のみ有効( TRUE:した  FALSE:していない )
	int						AccepteConfirFlag ;					// ライブラリユーザーが接続を確認したかフラグ( TRUE:した  FALSE:していない )
	int						AcceptFlag ;						// 接続されたのか、したのか、フラグ( TRUE:された  FALSE:した )

	int						DXProtocolFlag  ;					// ＤＸライブラリのプロトコルで通信するかどうかのフラグ( TRUE:サイズヘッダを付ける FALSE:WinSockをそのまま使う )
	int						CloseAfterLostFlag ;				// 切断直後にハンドルを解放するかどうかのフラグ( TRUE:直後に解放  FALSE:ユーザーから CloseNetWork されるまで解放しない )

//	int						ID ;								// エラーチェック用ＩＤ
	SOCKET					Socket ;							// ソケット識別番号

	int						RecvComDataVol ;					// 内部バッファにＷｉｎＳｏｃｋバッファの内容がすべて入らない場合、読まなくてはならない残り容量
	RINGBUF					RecvBufferToUserR ;					// ライブラリ利用者に公開する受信データのリングバッファ
	int						RecvComDataOriginalVol ;			// 一度の受信で送信されてきたデータ容量、RecvComDataVol が 0 になった時に RecvBufferToUserOpenSize に加算される
	int						RecvBufferToUserOpenSize ;			// ライブラリ利用者に公開する受信データのサイズ

	RINGBUF					SendBufferR ;						// 送信バッファ用リングデータ
	int						SendComDataVol ;					// 一度に送信できなかった場合の残りデータ量
	int						SendComDataComVol ;					// 一度に送信するデータ量の送信が一度に送信できなかった場合の残り容量

	IPDATA					AcceptedIP ;						// 接続先のＩＰ(切断後も有効)
	IPDATA_IPv6				AcceptedIP_IPv6 ;					// 接続先のＩＰ(切断後も有効)
	unsigned int			Port ;								// 接続先のポート

	int						UDPReadFlag ;						// 受信データが存在するか、フラグ( TRUE:存在する  FALSE:しない )
	int						UDPWriteFlag ;						// データ送信が可能な状態か、フラグ( TRUE:可能  FALSE:不可能 )

//	HANDLELIST				List ;								// リスト処理用データ
} ;

// Winsockets関係構造体
struct WINSOCKDATA
{
	int						ListenHandle ;						// 接続待ち処理用ハンドル
/*
	SOCKETDATA				*CSocket[ MAX_SOCKET_NUM + 1 ] ;	// コネクト用ソケットデータ
	HANDLELIST				SocketListFirst ;					// ソケットリストの先頭
	HANDLELIST				SocketListLast ;					// ソケットリストの終端
*/
//	int						SocketNum ;							// ソケットの数
//	int						HandleID ;							// ハンドルに割り当てるＩＤ
	int						InitializeFlag ;					// 初期化フラグ
//	int						MaxSockets ;						// 最大接続数
	int						MyIPv4Num ;							// 自分のIPv4アドレスの数
	IPDATA					*MyIPv4 ;							// 自分のIPv4アドレス
	int						MyIPv6Num ;							// 自分のIPv6アドレスの数
	IPDATA_IPv6				*MyIPv6 ;							// 自分のIPv6アドレス
	int						TimeOutWait ;						// タイムアウトするまでの時間(ミリ秒単位)
	HWND					ParentWindow ;						// 親ウインドウ
	HWND					MessageWindow ;						// ウインドウハンドル
	int						NonUseDXProtocolFlag ;				// ＤＸライブラリのプロトコルを使用しないかどうかのフラグ
	int						NonCloseAfterLostFlag ;				// 接続が切れた時に自動的にハンドルを解放しないかどうかのフラグ( TRUE:直後に解放はしない  FALSE:直後に解放する )
	int						DestroyFlag ;						// メッセージウインドウが WM_DESTROY を受けたかどうかのフラグ

//	DX_CRITICAL_SECTION		CriticalSection ;					// データアクセス時用クリティカルセクション

	HANDLE					ProcessNetMessageThreadHandle ;		// ProcessNetMessage をひたすら呼びつづけるプロセスのハンドル
	DWORD					ProcessNetMessageThreadID ;			// ProcessNetMessage をひたすら呼びつづけるプロセスのＩＤ
	DWORD					ProcessNetMessageThreadExitFlag ;	// ProcessNetMessage をひたすら呼びつづけるプロセスが終了時に立てるフラグ
	DWORD					ProcessNetMessageThreadEndRequest ;	// ProcessNetMessage をひたすら呼びつづけるプロセスの終了リクエストフラグ
} ;

// HTTP通信関係構造体
struct HTTPDATA
{
	HTTP_PROC				Process ;							// 処理
	HTTP_STATE				State ;								// 状態
	int						Step ;								// 推移状態
	int 					Counter ;							// 汎用カウンタ
	int						Result ;							// 処理結果
	int						Error ;								// エラーの場合のエラー内容

	int						NetHandle ;							// HTTPサーバとの通信に使用するネットワークハンドル

	HTTP_SAVEPLACE			SavePlace ;							// 保存場所
	HANDLE					FilePoint ;							// 受信先がファイルの時に使用するファイルポインタ
	void					**BufferPoint ;						// 受信先がメモリの時に使用するポインタのポインタ

	int						FileSize ;							// ファイルサイズ
	int						RecvFileSize ;						// 受信したデータのサイズ
	char					FileName[128] ;						// 受信するファイル名
	
	char					Host[128] ;							// 接続先のホスト名
	char 					Path[256] ;							// 受信するファイルのホスト上のパス
	char					*Param ;							// POST メソッドで長いパラメータを渡す際の文字列が格納されているポインタ
	int						ParamLength ;						// パラメータ文字列の長さ
	IPDATA					HostIP ;							// 接続先ホストのＩＰ
	int						Port ;								// 接続先ポート
} ;

// HTTP 関係管理データ
struct HTTPSYS
{
	HTTPDATA				*Data[MAX_HTTPHANDLE_NUM] ;			// 処理用データへのポインタの配列
	int						Num ;								// 処理用データの数

	int						UseProxy ;							// プロキシを使用するかどうか
	char					ProxyHost[256] ;					// プロキシのホスト名
	int						ProxyPort ;							// プロキシのポート
//	int						NonUseIEProxy ;						// ＩＥのプロキシサーバー設定を使用しないかどうかのフラグ
} ;

// FTP通信関係構造体
struct FTPDATA
{
	int						UseFlag ;							// 使用中フラグ
	
	FTP_PROC				Process ;							// 処理
	FTP_STATE				State ;								// 状態

	int						ComNetHandle ;						// コマンドの送受信に使用するネットワークハンドル
	int						DatNetHandle ;						// データの送受信に使用するネットワークハンドル

	DWORD					Fp ;								// 送受信に使用するファイルポインタ
	int						FileSize ;							// ファイルサイズ
	int						FilePoint ;							// 送受信したファイルのサイズ
	char					FileName[FILEPATH_MAX] ;			// 送受信するファイル名
	char					FileDir[FILEPATH_MAX] ;				// 送受信するファイルが存在するパス
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

extern WINSOCKDATA SockData ;									// ＷｉｎＳｏｃｋｅｔｓデータ
extern HTTPSYS HttpData ;										// HTTP 関係処理用データ

// 関数プロトタイプ宣言-----------------------------------------------------------

extern 	int			InitializeNetWork( HWND WindowHandle = NULL ) ;							// 通信機能を初期化する
extern 	int			TerminateNetWork( void ) ;												// 通信機能を終了する

extern	int			WinSockProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;	// ＷｉｎＳｏｃｋメッセージのコールバック関数

extern	int			AcceptNetWork( void ) ;													// アクセプトする

extern	int			InitializeNetworkHandle( HANDLEINFO *HandleInfo ) ;						// ネットワークハンドルを初期化する
extern	int			TerminateNetworkHandle( HANDLEINFO *HandleInfo ) ;						// ネットワークハンドルの後始末を行う

extern	void		InitConnectNetWorkBaseGParam( CONNECTNETWORK_GPARAM *GParam ) ;			// CONNECTNETWORK_GPARAM を初期化する
extern	int			ConnectNetWorkBase_UseGParam( CONNECTNETWORK_GPARAM *GParam, IPDATA *IPData_IPv4, IPDATA_IPv6 *IPData_IPv6, int Port, int ASync, int ASyncLoadFlag = FALSE ) ;		// ConnectNetWorkBase のグローバルデータにアクセスしない版
extern	int			NetWorkRecv_UseGParam( int NetHandle,       void *Buffer, int Length, int Peek, int ASyncLoadFlag = FALSE ) ;		// NetWorkRecv のグローバルデータにアクセスしないバージョン
extern	int			NetWorkSend_UseGParam( int NetHandle, const void *Buffer, int Length, int ASyncLoadFlag = FALSE ) ;					// NetWorkSend のグローバルデータにアクセスしないバージョン
extern	int			MakeUDPSocketBase_UseGParam( int IsIPv6, int RecvPort, int ASyncLoadFlag = FALSE ) ;								// MakeUDPSocketBase のグローバルデータにアクセスしないバージョン
extern	int			NetWorkSendUDP_UseGParam( int NetUDPHandle, IPDATA SendIP_IPv4, IPDATA_IPv6 SendIP_IPv6, int SendPort, const void *Buffer, int Length, int ASyncLoadFlag = FALSE ) ;	// NetWorkSendUDP のグローバルデータにアクセスしないバージョン
extern	int			NetWorkRecvUDP_UseGParam( int NetUDPHandle, void *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek, int ASyncLoadFlag = FALSE ) ;						// NetWorkRecvUDP のグローバルデータにアクセスしないバージョン

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_NETWORK

#endif // __DXNETWORK_H__
