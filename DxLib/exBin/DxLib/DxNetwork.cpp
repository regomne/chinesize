// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		通信関連プログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxNetwork.h"

#ifndef DX_NON_NETWORK

// インクルード ------------------------------------------------------------------
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxMemory.h"
#include "DxMath.h"
#include "DxUseCLib.h"
#include "DxASyncLoad.h"
#include "DxLog.h"
#include "Windows/DxWinAPI.h"
#include "Windows/DxWindow.h"

#ifndef DX_GCC_COMPILE
#include <winnls32.h>
#endif

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ネットワークハンドルの有効性チェック
#define NETHCHK( HAND, NPOINT )			HANDLECHK(       DX_HANDLETYPE_NETWORK, HAND, *( ( HANDLEINFO ** )&NPOINT ) )
#define NETHCHK_ASYNC( HAND, NPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_NETWORK, HAND, *( ( HANDLEINFO ** )&NPOINT ) )

// ネットワークハンドルの有効性チェック( ＴＣＰ )
#define TCPNETHCHK( HAND, NPOINT )			( ( NETHCHK( HAND, NPOINT ) ) || (NPOINT)->IsUDP != 0 )
#define TCPNETHCHK_ASYNC( HAND, NPOINT )	( ( NETHCHK_ASYNC( HAND, NPOINT ) ) || (NPOINT)->IsUDP != 0 )

// ネットワークハンドルの有効性チェック( ＵＤＰ )
#define UDPNETHCHK( HAND, NPOINT )			( ( NETHCHK( HAND, NPOINT ) ) || (NPOINT)->IsUDP == 0 )
#define UDPNETHCHK_ASYNC( HAND, NPOINT )	( ( NETHCHK_ASYNC( HAND, NPOINT ) ) || (NPOINT)->IsUDP == 0 )


// WinSock用定義
#define WSA_DEFAULTPORT				(10001)
#define WSA_TIMEOUTWAIT				(2000)

#define	WSA_ACCEPT					(WM_USER + 256)
#define	WSA_CONNECT					(WM_USER + 257)
#define	WSA_READ					(WM_USER + 258)
#define	WSA_WRITE					(WM_USER + 259)
#define	WSA_CLOSE					(WM_USER + 260)

#define WINFILEACCESS_BUFFERSIZE	(64 * 1024)			// ファイルの一時記憶バッファのサイズ

// ＩＥのプロキシサーバアドレスが格納されているレジストリアドレス
#define IEPROXY						"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

WINSOCKDATA SockData ;									// ＷｉｎＳｏｃｋｅｔｓデータ
HTTPSYS HttpData ;										// HTTP 関係処理用データ

// 関数プロトタイプ宣言-----------------------------------------------------------

// 通信関係
static	int			ConnectNetWorkBase( IPDATA *IPData_IPv4, IPDATA_IPv6 *IPData_IPv6, int Port, int ASync ) ;	// ConnectNetWork の処理を行う関数
static	int			PreparationListenNetWork_Base( int IsIPv6, int Port ) ;					// 接続を受けられる状態にする
static	int			MakeUDPSocketBase( int IsIPv6, int RecvPort ) ;							// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )

static	int			RecvSocket( int NetHandle ) ;											// データの受信処理
static	int			SendSocket( int NetHandle ) ;											// 溜まったデータの送信処理

static	int			ErrorNetWork( const char *ErrorStr, ... ) ;								// 通信エラー処理関数
static	int			ErrorNetLogAdd( const char *String ) ;									// 通信メッセージ出力関数
static	int			ErrorNetLogTabAdd( void ) ;												// 通信メッセージにタブを追加する関数
static	int			ErrorNetLogTabSub( void ) ;												// 通信メッセージのタブを減らす関数

extern	LRESULT		CALLBACK WinSockWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) ;	// ＷｉｎＳｏｃｋ専用のウインドウのメッセージコールバック関数

DWORD	WINAPI		ProcessNetMessageThreadFunction( LPVOID ) ;									// ProcessNetMessage をひたすら呼び続けるスレッド

#if 0

static	int			HTTP_Initialize( void ) ;													// HTTP 関係の処理の初期化を行う
static	int			HTTP_Terminate( void ) ;													// HTTP 関係の処理の後始末を行う

static	int			HTTP_GetResult( const char *ResStr ) ;										// HTTP メッセージの結果を得る

static	int			HTTP_GetConnectInfo( const char *URL, int UseProxy, char *HostBuf,
											char *PathBuf, char *FileNameBuf, int *PortBuf ) ;	// 指定のＵＲＬからホストパス、ホストのアクセスパス、アクセスポートを取得する
static	int			HTTP_ConnectHost( const char *URL, int UseProxy, IPDATA *HostIPBuf = NULL,
									 char *HostBuf = NULL, char *PathBuf = NULL, char *FileNameBuf = NULL, int *PortBuf = NULL ) ;	// 指定の HTTP ホストに接続する( 戻り値: 0以上 = ネットワークハンドル  -1 = エラー )
static	int			HTTP_AddHandle( void ) ;													// 新しくHTTPハンドルを追加する( 戻り値: -1 = エラー  0以上 = HTTPハンドル )
static	int			HTTP_DelHandle( int HttpHandle ) ;											// HTTPハンドルの後始末を行う
static	int			HTTP_ReleaseResource( int HttpHandle, int Result = HTTP_RES_COMPLETE, int Error = HTTP_ERR_NONE ) ;		// HTTP の処理に使用しているリソースを解放する
static	int			HTTP_Process( int HttpHandle ) ;											// HTTP の処理を行う
static	int			HTTP_ProcessAll( void ) ;													// HTTP_Proess を全てのハンドルに対して行う
static	int			HTTP_ProcessForGET( int HttpHandle ) ;										// HTTP の GET メソッドの処理をする
static	int			HTTP_ProcessForPOST( int HttpHandle ) ;										// HTTP の POST メソッドの処理をする
static	int			HTTP_ProcessForHEAD( int HttpHandle ) ;										// HTTP の HEAD メソッドの処理をする

static	char		GetDataSizeString( int ByteSize, char *SizeString ) ;						// サイズに見合った単位で表した文字列とその単位を表す一文字を返す
static	int			GetTimeLengthString( int MillSec, char *LengthString ) ;					// ミリ秒の時間長から時間長に合った単位の文字列に変換する

#endif

extern	int			HTTP_FileDownload( const char *FileURL, const char *SavePath = NULL,
										 void **SaveBufferP = NULL, int *FileSize = NULL,
										 char **ParamList = NULL ) ;						// HTTP を使用してネットワーク上のファイルをダウンロードする
extern	int			HTTP_GetFileSize( const char *FileURL ) ;								// HTTP を使用してネットワーク上のファイルのサイズを得る

extern	int			HTTP_StartFileDownload( const char *FileURL, const char *SavePath, void **SaveBufferP = NULL, char **ParamList = NULL ) ;	// HTTP を使用したネットワーク上のファイルをダウンロードする処理を開始する
extern	int			HTTP_StartGetFileSize( const char *FileURL ) ;							// HTTP を使用したネットワーク上のファイルのサイズを得る処理を開始する
extern	int			HTTP_Close( int HttpHandle ) ;											// HTTP の処理を終了し、ハンドルを解放する
extern	int			HTTP_CloseAll( void ) ;													// 全てのハンドルに対して HTTP_Close を行う
extern	int			HTTP_GetState( int HttpHandle ) ;										// HTTP 処理の現在の状態を得る( NET_RES_COMPLETE 等 )
extern	int			HTTP_GetError( int HttpHandle ) ;										// HTTP 処理でエラーが発生した場合、エラーの内容を得る( HTTP_ERR_NONE 等 )
extern	int			HTTP_GetDownloadFileSize( int HttpHandle ) ;							// HTTP 処理で対象となっているファイルのサイズを得る( 戻り値: -1 = エラー・若しくはまだファイルのサイズを取得していない  0以上 = ファイルのサイズ )
extern	int			HTTP_GetDownloadedFileSize( int HttpHandle ) ;							// HTTP 処理で既にダウンロードしたファイルのサイズを取得する

extern	int			fgetsForNetHandle( int NetHandle, char *strbuffer ) ;					// fgets のネットワークハンドル版( -1:取得できず 0:取得できた )
extern	int			URLAnalys( const char *URL, char *HostBuf = NULL, char *PathBuf = NULL,
												 char *FileNameBuf = NULL, int *PortBuf = NULL ) ;	// ＵＲＬを解析する
extern	int			URLConvert( char *URL, int ParamConvert = TRUE, int NonConvert = FALSE ) ;	// HTTP に渡せない記号が使われた文字列を渡せるような文字列に変換する( 戻り値: -1 = エラー  0以上 = 変換後の文字列のサイズ )
extern	int			URLParamAnalysis( char **ParamList, char **ParamStringP ) ;				// HTTP 用パラメータリストから一つのパラメータ文字列を作成する( 戻り値:  -1 = エラー  0以上 = パラメータの文字列の長さ )

// プログラム --------------------------------------------------------------------

// 通信関係

// 通信エラー処理関数
static	int ErrorNetWork( const char *ErrorStr, ... )
{
	va_list VaList ;
	char String[ 1024 ] ;
	int ErrorNum ;

	// ログ出力用のリストをセットする
	va_start( VaList, ErrorStr ) ;

	// 編集後の文字列を取得する
	CL_vsnprintf( DX_CHARCODEFORMAT_UTF16LE, TRUE, CHAR_CHARCODEFORMAT, WCHAR_T_CHARCODEFORMAT, String, sizeof( String ) / 2, ErrorStr, VaList ) ;
	
	// 可変長リストのポインタをリセットする
	va_end( VaList ) ;

//	if( SockData.MessageWindow != NULL ) return -1 ;

	SETUP_WIN_API

	ErrorNum = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
	DXST_LOGFILEFMT_ADDUTF16LE(( "\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xed\x30\xb0\x30\x02\x30\x25\x00\x73\x00\x5b\x00\x25\x00\x64\x00\x5d\x00\x00"/*@ L"ネットワークログ。%s[%d]" @*/, String, ErrorNum )) ;

	// 終了
	return -1 ;
}


// 通信メッセージ出力関数
static int ErrorNetLogAdd( const char *String )
{
//	if( SockData.MessageWindow != NULL ) return 0 ;

	return ErrorNetWork( String ) ;
}

// 通信メッセージにタブを追加する関数
static int ErrorNetLogTabAdd( void )
{
//	if( SockData.MessageWindow != NULL ) return 0 ;

	return DXST_LOGFILE_TABADD ;
}

// 通信メッセージのタブを減らす関数
static int ErrorNetLogTabSub( void )
{
//	if( SockData.MessageWindow != NULL ) return 0 ;

	return DXST_LOGFILE_TABSUB ;
}

// 通信メッセージの処理をする関数
extern int NS_ProcessNetMessage( int RunReleaseProcess )
{
	SETUP_WIN_API

	// 通信関係が初期化されていない場合は何もせず終了
	if( SockData.MessageWindow == NULL || SockData.InitializeFlag == FALSE ) return 0 ;

	// クリティカルセクションの取得
	if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ハンドル解放処理をする指定がある場合はハンドル解放処理を行う
	if( RunReleaseProcess == TRUE )
	{
		SOCKETDATA * Sock ;
		HANDLELIST *List, *temp ;

		// 切断直後に解放するフラグが立っていて
		// 相手から接続し、相手から切断された場合、自動的にハンドルの解放を行う
		// 若しくは重大なエラーが発生している場合も自動的にハンドルを解放する
		for( List = HandleManageArray[ DX_HANDLETYPE_NETWORK ].ListFirst.Next ; List->Next != NULL ; List = temp )
		{
			temp = List->Next ;
			Sock = (SOCKETDATA *)List->Data ;
			if( Sock->UseFlag == TRUE &&					// ハンドルが有効で
				( ( 
					Sock->ConnectionFlag == FALSE &&			// 接続が断たれていて
					Sock->ConnectionLostFlag == TRUE &&			// 接続が断たれたことも確認済みで
//					Sock->AcceptFlag == TRUE &&					// 相手から接続されていて
					Sock->CloseAfterLostFlag == TRUE ) ||		// 自動的に解放するフラグが立っている場合
				  Sock->ErrorFlag == TRUE ) )					// もしくは重大なエラーが発生している場合
			{
				// ハンドルを解放
				NS_CloseNetWork( List->Handle ) ;
			}
		}
	}

	// 通信用ウインドウメッセージを処理する
	{
		MSG msg;

		// メッセージが何もないかあった場合はﾒｯｾｰｼﾞの処理が終わるまでループする（制限あり）
		while( WinAPIData.Win32Func.PeekMessageWFunc( &msg, SockData.MessageWindow, 0, 0, PM_REMOVE ) )
		{
			WinAPIData.Win32Func.TranslateMessageFunc( &msg );
			WinAPIData.Win32Func.DispatchMessageWFunc( &msg );
		}
	}

	// クリティカルセクションの解放
	if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return 0 ;
}


// 通信機能を初期化する
extern 	int InitializeNetWork( HWND WindowHandle )
{
	WSADATA wsaData;

	if( SockData.InitializeFlag ) return 0 ;

	SETUP_WIN_API

	DXERRORNETLOG_ADD( "\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xa2\x95\x23\x90\x92\x30\x1d\x52\x1f\x67\x16\x53\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"ネットワーク関連を初期化します\n" @*/ ) ;
	ErrorNetLogTabAdd() ;

	// ハンドル管理情報の初期化
	InitializeHandleManage( DX_HANDLETYPE_NETWORK, sizeof( SOCKETDATA ), MAX_SOCKET_NUM, InitializeNetworkHandle, TerminateNetworkHandle, L"Network" ) ;

	// WinSockets初期化
	if( WinAPIData.WinSockFunc.WSAStartupFunc( MAKEWORD( 2 , 2 ), &wsaData ) != 0 ) 
		return DXERRORNETLOG_ADD( "\x77\x00\x73\x00\x32\x00\x5f\x00\x33\x00\x32\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x4c\x30\x0e\x5c\x65\x51\x55\x30\x8c\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"ws2_32.dllが導入されていません\n" @*/ ) ;

	// 指定バージョンで初期化されなかった場合は終了
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 )
	{
		WinAPIData.WinSockFunc.WSACleanupFunc();

		DXERRORNETLOG_ADD( "\x77\x00\x73\x00\x32\x00\x5f\x00\x33\x00\x32\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x6e\x30\xd0\x30\xfc\x30\xb8\x30\xe7\x30\xf3\x30\x4c\x30\x55\x90\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"ws2_32.dllのバージョンが違います\n" @*/ ) ;
		return -1;
	}

	// ＩＰアドレスを取得する
	{
		char HostName[ 1024 ] ;
		HOSTENT *Host ;

//		_MEMSET( &SockData.MyIPv4, 0, sizeof( IPDATA ) ) ;
		if( WinAPIData.WinSockFunc.gethostnameFunc( HostName, 256 ) == 0 )
		{
			// IPv4 アドレスを取得
			{
				Host = WinAPIData.WinSockFunc.gethostbynameFunc( HostName ) ;
				if( Host != NULL )
				{
					// ＩＰアドレスの数を数える
					for( SockData.MyIPv4Num = 0 ; Host->h_addr_list[ SockData.MyIPv4Num ] != NULL ; SockData.MyIPv4Num ++ ){}

					// ０個だった場合は NULL をセットして終了
					if( SockData.MyIPv4Num == 0 )
					{
						SockData.MyIPv4 = NULL ;
					}
					else
					{
						int i ;

						// ＩＰアドレスを保存するメモリを確保する
						SockData.MyIPv4 = ( IPDATA * )DXALLOC( sizeof( IPDATA ) * SockData.MyIPv4Num ) ;
						if( SockData.MyIPv4 == NULL )
						{
							WinAPIData.WinSockFunc.WSACleanupFunc() ;

							DXERRORNETLOG_ADD( "\x49\x00\x50\x00\x76\x00\x34\x00\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"IPv4アドレスを保存するメモリの確保に失敗しました\n" @*/ ) ;
							return -1 ;
						}

						// ＩＰアドレスを保存
						for( i = 0 ; i < SockData.MyIPv4Num ; i ++ )
						{
							SockData.MyIPv4[ i ].d1 = ( unsigned char )Host->h_addr_list[ i ][ 0 ] ;
							SockData.MyIPv4[ i ].d2 = ( unsigned char )Host->h_addr_list[ i ][ 1 ] ;
							SockData.MyIPv4[ i ].d3 = ( unsigned char )Host->h_addr_list[ i ][ 2 ] ;
							SockData.MyIPv4[ i ].d4 = ( unsigned char )Host->h_addr_list[ i ][ 3 ] ;
						}
					}
				}
			}

			// IPv6 アドレスを取得
			{
				_addrinfo hints ;
				_addrinfo *ainfo = NULL ;

				_MEMSET( &hints, 0, sizeof( hints ) ) ;
				hints.ai_family = 23/*AF_INET6 は 23*/ ;
				if( WinAPIData.WinSockFunc.getaddrinfoFunc( HostName, NULL, &hints, &ainfo ) == 0 )
				{
					// IPv6 のアドレスかチェック
					if( ainfo->ai_family == 23/*AF_INET6 は 23*/ && ainfo->ai_addr != NULL )
					{
						_addrinfo *tmp_ainfo ;

						// アドレスの数を数える
						for( SockData.MyIPv6Num = 0, tmp_ainfo = ainfo ; tmp_ainfo != NULL ; SockData.MyIPv6Num++, tmp_ainfo = tmp_ainfo->ai_next ){}

						// ０個だった場合は NULL をセットして終了
						if( SockData.MyIPv6Num == 0 )
						{
							SockData.MyIPv6 = NULL ;
						}
						else
						{
							int i ;

							// ＩＰアドレスを保存するメモリを確保する
							SockData.MyIPv6 = ( IPDATA_IPv6 * )DXALLOC( sizeof( IPDATA_IPv6 ) * SockData.MyIPv6Num ) ;
							if( SockData.MyIPv6 == NULL )
							{
								WinAPIData.WinSockFunc.WSACleanupFunc() ;

								DXERRORNETLOG_ADD( "\x49\x00\x50\x00\x76\x00\x36\x00\xa2\x30\xc9\x30\xec\x30\xb9\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"IPv6アドレスを保存するメモリの確保に失敗しました\n" @*/ ) ;
								return -1 ;
							}

							// IPv6アドレスを保存
							for( i = 0, tmp_ainfo = ainfo ; tmp_ainfo != NULL ; i ++, tmp_ainfo = tmp_ainfo->ai_next )
							{
								_sockaddr_in6 *addrv6 ;
								addrv6 = ( _sockaddr_in6 * )tmp_ainfo->ai_addr ;
								SockData.MyIPv6[ i ] = addrv6->sin6_addr ;
							}
						}
					}

					// メモリの解放
					WinAPIData.WinSockFunc.freeaddrinfoFunc( ainfo ) ;
				}
			}
		}
	}

	// ソケットデータゼロ初期化
//	_MEMSET( SockData.CSocket, 0, sizeof( SockData.CSocket ) ) ;

	// 接続最大数をセットする、初期化フラグを立てる
//	SockData.MaxSockets = MAX_SOCKET_NUM ;
	SockData.InitializeFlag = TRUE ;

	// ウインドウハンドルを保存する
	SockData.ParentWindow = WindowHandle ;

	// メッセージ処理用子ウインドウを作成する
	{
		WNDCLASSEXW wc ;
		HWND ParentWindow ;
		HINSTANCE hInst = WinAPIData.Win32Func.GetModuleHandleWFunc( NULL ) ;
		const wchar_t *Name = L"WinSockProc" ;

		// 子ウインドウのウインドウクラスを登録
		_MEMSET( &wc, 0, sizeof( wc ) ) ;
		{
			wc.style			= 0 ;
			wc.lpfnWndProc		= WinSockWindowProc ;
			wc.cbClsExtra		= 0 ;
			wc.cbWndExtra		= 0 ;
			wc.hInstance		= hInst ;
			wc.hIcon			= NULL ;
			wc.hCursor			= WinAPIData.Win32Func.LoadCursorWFunc( NULL , ( LPCWSTR )IDC_ARROW ) ;
			wc.hbrBackground	= (HBRUSH)WinAPIData.Win32Func.GetStockObjectFunc(NULL_BRUSH);
			wc.lpszMenuName		= NULL ;
			wc.lpszClassName	= Name ;
			wc.cbSize			= sizeof( WNDCLASSEX );
			wc.hIconSm			= NULL ;

			if( !WinAPIData.Win32Func.RegisterClassExWFunc( &wc ) )
			{
				DXST_LOGFILE_ADDUTF16LE( "\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\x6e\x30\x50\x5b\xa6\x30\xa4\x30\xf3\x30\xc9\x30\xa6\x30\xaf\x30\xe9\x30\xb9\x30\x6e\x30\x7b\x76\x32\x93\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ネットワーク処理用の子ウインドウクラスの登録に失敗しました\n" @*/ ) ;
				return -1 ;
			}
		}

		// 親ウインドウの確定
		ParentWindow = SockData.ParentWindow != NULL ? SockData.ParentWindow : WinData.MainWindow ;

		// 子ウインドウを生成
		SockData.MessageWindow = 
			WinAPIData.Win32Func.CreateWindowExWFunc(
				WS_EX_TRANSPARENT,
				Name,
				Name ,
				( DWORD )( ( ParentWindow ? WS_CHILD : 0 ) | WS_MINIMIZE ),
				0, 0, 0, 0,
				ParentWindow,
				NULL,
				hInst,
				NULL );
		if( SockData.MessageWindow == NULL )
		{
			DXST_LOGFILE_ADDUTF16LE( "\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xe6\x51\x06\x74\x28\x75\x6e\x30\x50\x5b\xa6\x30\xa4\x30\xf3\x30\xc9\x30\xa6\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ネットワーク処理用の子ウインドウの作成に失敗しました\n" @*/ ) ;
			return -1 ;
		}
		SockData.DestroyFlag = FALSE ;
	}

	// ハンドルリストの初期化
//	InitializeHandleList( &SockData.SocketListFirst, &SockData.SocketListLast ) ;

	// ProcessNetMessage をひたすら呼ぶスレッドを立てる
	SockData.ProcessNetMessageThreadHandle = WinAPIData.Win32Func.CreateThreadFunc( NULL, 0, ProcessNetMessageThreadFunction, NULL, CREATE_SUSPENDED, &SockData.ProcessNetMessageThreadID ) ;
	WinAPIData.Win32Func.ResumeThreadFunc( SockData.ProcessNetMessageThreadHandle ) ;

	ErrorNetLogTabSub() ;
	DXERRORNETLOG_ADD( "\x1d\x52\x1f\x67\x16\x53\x92\x30\x8c\x5b\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x0a\x00\x00"/*@ L"初期化を完了しました \n" @*/ ) ;

	// 終了
	return 0 ;
}

// 通信機能を終了する
extern int TerminateNetWork( void )
{
//	int i, Handle ;

	if( !SockData.InitializeFlag ) return 0 ;

	SETUP_WIN_API

	DXERRORNETLOG_ADD( "\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xa2\x95\x23\x90\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x20\x00\x0a\x00\x00"/*@ L"ネットワーク関連の終了処理を行います \n" @*/ ) ;

	// ProcessNetMessage をひたすら呼びつづけるスレッドを閉じる
	if( SockData.ProcessNetMessageThreadHandle != NULL )
	{
		// スレッドが終了するまで待つ
		SockData.ProcessNetMessageThreadEndRequest = TRUE ;
		while( SockData.ProcessNetMessageThreadExitFlag == 0 )
		{
			Thread_Sleep( 1 ) ;
		}

		// スレッドのハンドルを閉じる
		WinAPIData.Win32Func.CloseHandleFunc( SockData.ProcessNetMessageThreadHandle ) ;
		SockData.ProcessNetMessageThreadHandle = NULL ;
	}

	// ハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_NETWORK ) ;
/*
	// 終了処理
	for( i = 0 ; i < MAX_SOCKET_NUM + 1 ; i ++ )
	{
		if( SockData.CSocket[i] != NULL )
		{
			Handle = i | DX_HANDLETYPE_MASK_NETWORK | ( SockData.CSocket[i]->ID << DX_HANDLECHECK_ADDRESS ) ;
			if( SockData.CSocket[i]->IsUDP )
			{
				NS_DeleteUDPSocket( Handle ) ;
			}
			else
			{
				NS_CloseNetWork( Handle ) ;
			}
		}
	}
*/
	// ＩＰアドレスを保存するためにメモリを確保していたら解放する
	if( SockData.MyIPv4 != NULL )
	{
		DXFREE( SockData.MyIPv4 ) ;
		SockData.MyIPv4 = NULL ;
	}

	// ウインドウを削除する
	if( SockData.MessageWindow != NULL )
	{
		WinAPIData.Win32Func.PostMessageWFunc( SockData.MessageWindow, WM_CLOSE, 0, 0 );
		while( SockData.DestroyFlag == FALSE )
		{
			if( NS_ProcessNetMessage() <= 0 ) break ;
		}
	}

	// ゼロクリア
	_MEMSET( &SockData, 0, sizeof( SockData ) ) ;

	// WinSocketsの使用を終了する
	WinAPIData.WinSockFunc.WSACleanupFunc() ;

	DXERRORNETLOG_ADD( "\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xa2\x95\x23\x90\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x6f\x30\x8c\x5b\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ネットワーク関連の終了処理は完了しました\n" @*/ ) ;

	// 終了
	return 0 ;
}

// ＷｉｎＳｏｃｋメッセージのコールバック関数
extern int WinSockProc( HWND /*hWnd*/, UINT /*message*/, WPARAM wParam, LPARAM lParam )
{
	WORD _event , error ;
	int NetHandle ;
	SOCKET Sock ;
	SOCKETDATA * SockD ;
	int ReturnValue = TRUE ;
	HANDLELIST *List ;

	// クリティカルセクションの取得
	if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// どのハンドルへのメッセージなのかを取得
	Sock = ( SOCKET )wParam ;
	for( List = HandleManageArray[ DX_HANDLETYPE_NETWORK ].ListFirst.Next ;
		List->Next != NULL &&
		((SOCKETDATA *)List->Data)->Socket != ( unsigned int )Sock ;
		List = List->Next ){}
	SockD = (SOCKETDATA *)List->Data ;
	if( List->Next == NULL )
	{
		// クリティカルセクションの解放
		if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

		DXST_LOGFILE_ADDUTF16LE( "\x1a\x90\xe1\x4f\xa8\x30\xe9\x30\xfc\x30\x20\x00\x4e\x00\x6f\x00\x2e\x00\x31\x00\x0a\x00\x00"/*@ L"通信エラー No.1\n" @*/ ) ;
		return FALSE ;
	}
	NetHandle = SockD->HandleInfo.Handle ;

	// メッセージの取得
	_event = WSAGETSELECTEVENT( (LPARAM) lParam ) ;
	error = WSAGETSELECTERROR( (LPARAM) lParam ) ;

	// UDPかどうかで処理を分岐
	if( SockD->IsUDP )
	{
		switch( _event )
		{
		// データ受信
		case FD_READ:
			// 受信データ存在フラグを立てる
			SockD->UDPReadFlag = TRUE ;
			break ;

		// データ送信準備完了
		case FD_WRITE:
			// 送信可能フラグを立てる
			SockD->UDPWriteFlag = TRUE ;
			break;
		}
	}
	else
	{
		switch( _event )
		{
		// コネクトメッセージ
		case FD_CONNECT :
			// エラーが発生したらコネクト失敗
			if( error != 0 ) break ;
			
			// 接続フラグを立てる
			SockD->ConnectionFlag = TRUE ;

			// 接続前フラグを倒す
			SockD->PreConnectionFlag = FALSE ;
			break ;

		// アクセプトメッセージ
		case FD_ACCEPT :
			// アクセプト処理
			ReturnValue = AcceptNetWork() ;
			break ;

		// データ受信
		case FD_READ:
			// 内部データ受信処理を行う
			RecvSocket( NetHandle ) ;
			break ;


		// データ送信準備完了
		case FD_WRITE:
			// 内部送信処理関数を実行する
			SendSocket( NetHandle ) ;
			break;

		// クローズメッセージ
		case FD_CLOSE:
			// 接続フラグと切断確認フラグを倒す
			SockD->ConnectionFlag = FALSE ;
			SockD->ConnectionLostFlag = FALSE ;
			break ;
		}
	}

	// クリティカルセクションの解放
	if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}

// ＷｉｎＳｏｃｋ専用のウインドウのメッセージコールバック関数
extern LRESULT CALLBACK WinSockWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int Ret ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	switch( message )
	{
	case WM_CREATE :
		hWnd = hWnd ;
		break ;

	case WM_CLOSE :
		WinAPIData.Win32Func.DestroyWindowFunc( hWnd ) ;
		break ;

	case WM_DESTROY :
		SockData.DestroyFlag = TRUE ;
		break ;

	case WSA_WINSOCKMESSAGE :
		Ret = WinSockProc( hWnd , message , wParam , lParam ) ;

		// クリティカルセクションの解放
		if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

		return Ret ;
	}

	// クリティカルセクションの解放
	if( HandleManageArray[ DX_HANDLETYPE_NETWORK ].InitializeFlag != FALSE )
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return WinAPIData.Win32Func.DefWindowProcWFunc( hWnd , message , wParam , lParam ) ;
}
	


// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する
extern int NS_GetHostIPbyName( const TCHAR *HostName, IPDATA *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	hostent *HostInfo ;
	int Ret = 0 ;
	unsigned int ip ;
	int IPNum ;
	const char *UseHostName ;
#ifdef UNICODE
	char TempHostName[ 1024 ] ;

	ConvString( ( const char * )HostName, -1, WCHAR_T_CHARCODEFORMAT, TempHostName, sizeof( TempHostName ), DX_CHARCODEFORMAT_ASCII ) ;
	UseHostName = TempHostName ;
#else
	UseHostName = HostName ;
#endif

	if( SockData.InitializeFlag == FALSE )
	{
		if( WinData.CloseMessagePostFlag != TRUE ) InitializeNetWork() ;
		else return -1 ;
	}

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ＩＰを数値文字列にしただけだった時のための処理
	// 成功すれば分岐
	ip = WinAPIData.WinSockFunc.inet_addrFunc( UseHostName ) ;
	if( ip != INADDR_NONE )
	{
		// アドレスデータからホストの情報を得る
		HostInfo = WinAPIData.WinSockFunc.gethostbyaddrFunc( ( const char * )&ip, 4, AF_INET ) ;
	}
	else
	{
		// ホスト名からホストの情報を得る
		HostInfo = WinAPIData.WinSockFunc.gethostbynameFunc( UseHostName ) ;
	}
	if( HostInfo == NULL )
	{
		Ret = -1 ;
		goto ENDLABEL ;
	}

	// IPv4 のアドレスかチェック
	if( HostInfo->h_addrtype != AF_INET || HostInfo->h_length != 4 )
	{
		Ret = -2 ;
		goto ENDLABEL ;
	}

	// ホストの情報からＩＰアドレスを得る
	for( IPNum = 0 ; HostInfo->h_addr_list[ IPNum ] != NULL ; IPNum ++ )
	{
		if( IPNum < IPDataBufLength )
		{
			IPDataBuf[ IPNum ].d1 = ( unsigned char )HostInfo->h_addr_list[ IPNum ][0] ;
			IPDataBuf[ IPNum ].d2 = ( unsigned char )HostInfo->h_addr_list[ IPNum ][1] ;
			IPDataBuf[ IPNum ].d3 = ( unsigned char )HostInfo->h_addr_list[ IPNum ][2] ;
			IPDataBuf[ IPNum ].d4 = ( unsigned char )HostInfo->h_addr_list[ IPNum ][3] ;
		}
	}

	if( IPDataGetNum != NULL )
	{
		*IPDataGetNum = IPNum ;
	}

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Ret ;
}

// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv4版 )
extern int NS_GetHostIPbyNameWithStrLen( const TCHAR *HostName, size_t HostNameLength, IPDATA *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	int Result ;
#ifdef UNICODE
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_ONE_BEGIN( HostName, HostNameLength, return -1 )
	Result = NS_GetHostIPbyName( UseHostNameBuffer, IPDataBuf, IPDataBufLength , IPDataGetNum ) ;
	WCHAR_T_STRING_WITH_STRLEN_TO_WCHAR_T_STRING_END( HostName )
#else
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( HostName, HostNameLength, return -1 )
	Result = NS_GetHostIPbyName( UseHostNameBuffer, IPDataBuf, IPDataBufLength, IPDataGetNum ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( HostName )
#endif
	return Result ;
}

// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv6版 )
extern int NS_GetHostIPbyName_IPv6( const TCHAR *HostName, IPDATA_IPv6 *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	const char *UseHostName ;
	_addrinfo hints ;
	_addrinfo *ainfo = NULL ;
	_addrinfo *tmp_ainfo ;
	_sockaddr_in6 *addrv6 ;
	int IPNum ;
	int Ret = 0 ;
#ifdef UNICODE
	char TempHostName[ 1024 ] ;

	ConvString( ( const char * )HostName, -1, WCHAR_T_CHARCODEFORMAT, TempHostName, sizeof( TempHostName ), DX_CHARCODEFORMAT_ASCII ) ;
	UseHostName = TempHostName ;
#else
	UseHostName = HostName ;
#endif

	if( SockData.InitializeFlag == FALSE )
	{
		if( WinData.CloseMessagePostFlag != TRUE ) InitializeNetWork() ;
		else return -1 ;
	}

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ホスト名からＩＰ情報を取得
    _MEMSET( &hints, 0, sizeof( hints ) ) ;
    hints.ai_family = 23/*AF_INET6 は 23*/ ;
	if( WinAPIData.WinSockFunc.getaddrinfoFunc( UseHostName, NULL, &hints, &ainfo ) != 0 )
	{
		Ret = -1 ;
		goto ENDLABEL ;
	}

	// IPv6 のアドレスかチェック
	if( ainfo->ai_family != 23/*AF_INET6 は 23*/ || ainfo->ai_addr == NULL )
	{
		Ret = -2 ;
		goto ENDLABEL ;
	}

	// IPv6アドレスを保存
	for( IPNum = 0, tmp_ainfo = ainfo ; tmp_ainfo != NULL ; IPNum ++, tmp_ainfo = tmp_ainfo->ai_next )
	{
		if( IPNum < IPDataBufLength )
		{
			addrv6 = ( _sockaddr_in6 * )tmp_ainfo->ai_addr ;
			IPDataBuf[ IPNum ] = addrv6->sin6_addr ;
		}
	}

	if( IPDataGetNum != NULL )
	{
		*IPDataGetNum = IPNum ;
	}

ENDLABEL :

	// メモリの解放
	if( ainfo != NULL )
	{
		WinAPIData.WinSockFunc.freeaddrinfoFunc( ainfo ) ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Ret ;
}

// ＤＮＳサーバーを使ってホスト名からＩＰアドレスを取得する( IPv6版 )
extern int NS_GetHostIPbyName_IPv6WithStrLen( const TCHAR *HostName, size_t HostNameLength, IPDATA_IPv6 *IPDataBuf, int IPDataBufLength , int *IPDataGetNum )
{
	int Result ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_ONE_BEGIN( HostName, HostNameLength, return -1 )
	Result = NS_GetHostIPbyName_IPv6( UseHostNameBuffer, IPDataBuf, IPDataBufLength , IPDataGetNum ) ;
	TCHAR_STRING_WITH_STRLEN_TO_TCHAR_STRING_END( HostName )
	return Result ;
}

// ネットワークハンドルを初期化する
extern int InitializeNetworkHandle( HANDLEINFO * /*HandleInfo*/ )
{
	// 終了
	return 0 ;
}

// ネットワークハンドルの後始末を行う
extern int TerminateNetworkHandle( HANDLEINFO *HandleInfo )
{
	SOCKETDATA *Sock = ( SOCKETDATA * )HandleInfo ;

	SETUP_WIN_API

	// UDPかどうかで処理を分岐
	if( Sock->IsUDP )
	{
		// ソケットが有効だった場合処理
		if( Sock->Socket != NULL && Sock->Socket != INVALID_SOCKET )
		{
			// 非ブロッキング解除
			WinAPIData.WinSockFunc.WSAAsyncSelectFunc( Sock->Socket, SockData.MessageWindow, 0, 0 );

			// ソケットを閉じる
			WinAPIData.WinSockFunc.closesocketFunc( Sock->Socket );
			Sock->Socket = NULL ;
		}
	}
	else
	{
		// TCP

		// 接続待ち用ハンドルかどうかで処理を分岐
		if( SockData.ListenHandle == HandleInfo->Handle )
		{
			// 接続待ち用ハンドル

			if( Sock->Socket && Sock->Socket != INVALID_SOCKET )
			{
				// 通信関係処理
				NS_ProcessNetMessage() ;

				// 非ブロッキング解除
				WinAPIData.WinSockFunc.WSAAsyncSelectFunc( Sock->Socket, SockData.MessageWindow, 0, 0 );

				// データ送受信シャットダウン
				WinAPIData.WinSockFunc.shutdownFunc( Sock->Socket , 2 ) ;

				// ソケットを閉じる
				WinAPIData.WinSockFunc.closesocketFunc( Sock->Socket );
				Sock->Socket = NULL ;
			}
			
			// 使用中フラグを倒す
			Sock->UseFlag = FALSE ;

			// 接続受付用のハンドルを無効にする
			SockData.ListenHandle = 0 ;
		}
		else
		{
			// 有効なソケットデータではなかったら何もせず終了
			if( Sock->UseFlag == FALSE )
				return 0 ;

			// 通信関係処理
			NS_ProcessNetMessage() ;

			// 残りのデータをバッファから消去する
			{
				int time ;
				
				time = NS_GetNowCount() ;
				while( NS_GetNowCount() - time < 1000 && Sock->RecvComDataVol != 0 )
				{
					if( NS_ProcessNetMessage() != 0 ) break ;
					RecvSocket( HandleInfo->Handle ) ;
				}

				time = NS_GetNowCount() ;
				while( NS_GetNowCount() - time < 1000 && Sock->SendBufferR.DataLength != 0 )
				{
					if( NS_ProcessNetMessage() != 0 ) break ;
					SendSocket( HandleInfo->Handle ) ;
				}
			}

			// ソケットをの後始末を行う
			if( Sock->Socket && Sock->Socket != INVALID_SOCKET )
			{
				// 非ブロッキング解除
				WinAPIData.WinSockFunc.WSAAsyncSelectFunc( Sock->Socket, SockData.MessageWindow, 0, 0 );

				// ソケットを閉じる
				WinAPIData.WinSockFunc.closesocketFunc( Sock->Socket );
				Sock->Socket = NULL ;
			}

			// 送受信用バッファの解放
			RingBufTerminate( &Sock->RecvBufferToUserR ) ;
			RingBufTerminate( &Sock->SendBufferR ) ;

			// 使用中フラグを倒す
			Sock->UseFlag = FALSE ;
			
			// その他適当にフラグをセット
			Sock->ConnectionFlag = FALSE ;
			Sock->ConnectionLostFlag = TRUE ;
			Sock->AccepteConfirFlag = FALSE ;
		}
	}

	// 終了
	return 0 ;
}

// CONNECTNETWORK_GPARAM を初期化する
extern void InitConnectNetWorkBaseGParam( CONNECTNETWORK_GPARAM *GParam )
{
	GParam->NonCloseAfterLostFlag = SockData.NonCloseAfterLostFlag ;
	GParam->NonUseDXProtocolFlag = SockData.NonUseDXProtocolFlag ;
}

// ConnectNetWorkBase の実処理関数
static int ConnectNetWorkBase_Static(
	CONNECTNETWORK_GPARAM *GParam,
	int NetHandle,
	IPDATA *IPData_IPv4,
	IPDATA_IPv6 *IPData_IPv6,
	int Port,
	int ASync,
	int ASyncThread
)
{
	_sockaddr_in6 con_v6 ;
	SOCKADDR_IN con;
	SOCKETDATA *Sock ;
	unsigned int pt ;
	int i ;
	int IsIPv6 ;

	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	if( ASyncThread )
	{
		if( NETHCHK_ASYNC( NetHandle, Sock) )
			goto ERR ;
	}
	else
	{
		if( NETHCHK( NetHandle, Sock) )
			goto ERR ;
	}

	// IPv6 かどうかをセット
	IsIPv6 = IPData_IPv6 != NULL ? TRUE : FALSE ;

	// 通信関係処理
	NS_ProcessNetMessage() ;
	
	// ポートの値を決定
	pt = Port == -1 ? WinAPIData.WinSockFunc.htonsFunc( WSA_DEFAULTPORT ) : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )Port ) ;

	// データを初期化
	Sock->IsUDP					= FALSE ;							// ＵＤＰは使用しない
	Sock->IsUDPBroadCast		= FALSE ;							// ブロードキャストは使用しない
	Sock->IsIPv6				= IsIPv6 ;							// ＩＰｖ６かどうかを保存
	Sock->ErrorFlag 			= FALSE ;							// エラーが発生したフラグを倒す
	Sock->UseFlag				= TRUE ;							// データを使用中にする
	Sock->PreConnectionFlag		= TRUE ;							// 接続前フラグをセットする
	Sock->ConnectionFlag		= FALSE ;							// 接続したかフラグを接続していない状態に
	Sock->ConnectionLostFlag 	= FALSE ;							// 切断確認フラグはとりあえず倒しておく
	Sock->AccepteConfirFlag 	= TRUE ;							// 接続確認フラグは自ら接続しているので把握済みということにしておく
	Sock->AcceptFlag			= FALSE ;							// 相手から接続されたのかフラグを倒す
	Sock->DXProtocolFlag 		= !GParam->NonUseDXProtocolFlag ;	// ＤＸライブラリ独自の通信方式を使うかどうかをセット
	Sock->CloseAfterLostFlag	= !GParam->NonCloseAfterLostFlag ;	// 切断直後にハンドルを解放するかどうかのフラグをセット
	if( IsIPv6 )
	{
		Sock->AcceptedIP_IPv6	= *IPData_IPv6 ;					// 接続先のＩＰを保存
	}
	else
	{
		Sock->AcceptedIP 		= *IPData_IPv4 ;					// 接続先のＩＰを保存
	}
	Sock->Port 					= pt ;								// 接続先のポートを保存

	// ＤＸライブラリ独自の方式を取る場合は送受信用バッファを初期化
	if( Sock->DXProtocolFlag == TRUE )
	{
		// ユーザーに渡すときに使うリングバッファの初期化
		RingBufInitialize( &Sock->RecvBufferToUserR ) ;
		Sock->RecvComDataVol = 0 ;
		Sock->RecvComDataOriginalVol = 0 ;
		Sock->RecvBufferToUserOpenSize = 0 ;

		// 送信バッファ関連の初期化
		RingBufInitialize( &Sock->SendBufferR ) ;
		Sock->SendComDataVol = 0 ;
		Sock->SendComDataComVol = 0 ;
	}

	// ソケットの作成
	Sock->Socket = WinAPIData.WinSockFunc.socketFunc( IsIPv6 ? 23/*AF_INET6 は 23*/ : AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
	if( Sock->Socket == INVALID_SOCKET )
	{
		DXERRORNETWORK(( "\x77\x00\x73\x00\x32\x00\x5f\x00\x33\x00\x32\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x4c\x30\xd5\x52\x5c\x4f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x01\x30\xbd\x30\xb1\x30\xc3\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x32\x00\x00"/*@ L"ws2_32.dll が動作していません、ソケットの作成に失敗しました_2" @*/ )) ;
		goto ERR ;
	}

	// WinSock メッセージ受け取り設定
	if( WinAPIData.WinSockFunc.WSAAsyncSelectFunc(
			Sock->Socket,
			SockData.MessageWindow,
			WSA_WINSOCKMESSAGE,
			FD_CONNECT | FD_WRITE | FD_READ | FD_CLOSE ) == SOCKET_ERROR )
	{
		DXERRORNETWORK(( "\x57\x00\x53\x00\x41\x00\x41\x00\x73\x00\x79\x00\x6e\x00\x63\x00\x53\x00\x65\x00\x6c\x00\x65\x00\x63\x00\x74\x00\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"WSAAsyncSelectでエラーが発生しました" @*/ )) ;
		goto ERR ;
	}

	// 接続
	if( IsIPv6 )
	{
		// 接続パラメータセット
		_MEMSET( &con_v6, 0, sizeof( con_v6 ) ) ;
		con_v6.sin6_family = 23/*AF_INET6 は 23*/ ;
		con_v6.sin6_port = ( unsigned short )Sock->Port ;
		for( i = 0 ; i < 8 ; i ++ )
			con_v6.sin6_addr.Word[ i ] = IPData_IPv6->Word[ i ] ;

		// 接続
		if( WinAPIData.WinSockFunc.connectFunc( Sock->Socket , (PSOCKADDR) &con_v6, sizeof( _sockaddr_in6 ) ) == SOCKET_ERROR )
		{
			if( WinAPIData.WinSockFunc.WSAGetLastErrorFunc() != WSAEWOULDBLOCK )
				goto ERR ;
		}
	}
	else
	{

		// 接続パラメータセット
		_MEMSET( &con, 0, sizeof( con ) ) ;
		con.sin_family = AF_INET;
		con.sin_addr = *(( in_addr *)IPData_IPv4 ) ;
		con.sin_port = ( u_short )Sock->Port ;

		// 接続
		if( WinAPIData.WinSockFunc.connectFunc( Sock->Socket , (PSOCKADDR) &con, sizeof( SOCKADDR_IN ) ) == SOCKET_ERROR )
		{
			if( WinAPIData.WinSockFunc.WSAGetLastErrorFunc() != WSAEWOULDBLOCK )
				goto ERR ;
		}
	}

	// 同期接続指定の場合はここで接続を一定時間接続を待つ
	if( ASyncThread == FALSE && ASync == FALSE )
	{
		int StTime ;
	
		StTime = NS_GetNowCount() ;
		if( SockData.TimeOutWait == 0 ) SockData.TimeOutWait = WSA_TIMEOUTWAIT ;
		while( ( ( NS_GetNowCount() - StTime ) < SockData.TimeOutWait ) && ( Sock->ConnectionFlag == FALSE ) )
		{
			if( NS_ProcessNetMessage() != 0 ) break ;
			Thread_Sleep( 1 ) ;
		}
		if( Sock->ConnectionFlag == FALSE )
			goto ERR ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 正常終了
	return 0 ;

ERR :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// エラー終了
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD

// ConnectNetWorkBase の非同期読み込みスレッドから呼ばれる関数
static void ConnectNetWorkBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	CONNECTNETWORK_GPARAM *GParam ;
	int NetHandle ;
	IPDATA *IPData_IPv4 ;
	IPDATA_IPv6 *IPData_IPv6 ;
	int Port ;
	int ASync ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( CONNECTNETWORK_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	NetHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	IPData_IPv4 = ( IPDATA * )GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	IPData_IPv6 = ( IPDATA_IPv6 * )GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	Port = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	ASync = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	if( IPData_IPv4 != NULL )
	{
		IPData_IPv4 = ( IPDATA * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	}
	if( IPData_IPv6 != NULL )
	{
		IPData_IPv6 = ( IPDATA_IPv6 * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	}

	Result = ConnectNetWorkBase_Static( GParam, NetHandle, IPData_IPv4, IPData_IPv6, Port, ASync, TRUE ) ;

	DecASyncLoadCount( NetHandle ) ;
	if( Result < 0 )
	{
		SubHandle( NetHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// ConnectNetWorkBase のグローバルデータにアクセスしない版
extern int ConnectNetWorkBase_UseGParam(
	CONNECTNETWORK_GPARAM *GParam,
	IPDATA *IPData_IPv4,
	IPDATA_IPv6 *IPData_IPv6,
	int Port,
	int ASync,
	int ASyncLoadFlag
)
{
	int NetHandle ;
	int IsIPv6 ;
	HANDLELIST *List ;
	SOCKETDATA *Sock ;
	unsigned int pt ;
	int i ;

	// 通信関係が初期化されていなかったら初期化
	if( SockData.InitializeFlag == FALSE )
	{
		if( WinData.CloseMessagePostFlag != TRUE )	InitializeNetWork() ;
		else										return -1 ;
	}
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// IPv6 かどうかをセット
	IsIPv6 = IPData_IPv6 != NULL ? TRUE : FALSE ;

	// ポートの値を決定
	pt = Port == -1 ? WinAPIData.WinSockFunc.htonsFunc( WSA_DEFAULTPORT ) : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )Port ) ;

	// 現在接続している中に指定のＩＰがあるか確認、あったらそのハンドルを返す
	for( List = HandleManageArray[ DX_HANDLETYPE_NETWORK ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sock = ( SOCKETDATA * )List->Data ;
		if( Sock->UseFlag == FALSE ||
			Sock->IsIPv6 != IsIPv6 ||
			Sock->IsUDP != FALSE ||
			Sock->Port != pt ||
			Sock->ConnectionFlag == FALSE )
			continue ;

		if( IsIPv6 )
		{
			for( i = 0 ; i < 8 ; i ++ )
			{
				if( Sock->AcceptedIP_IPv6.Word[ i ] != IPData_IPv6->Word[ i ] )
					continue ;
			}
		}
		else
		{
			if( Sock->AcceptedIP.d1 != IPData_IPv4->d1 ||
				Sock->AcceptedIP.d2 != IPData_IPv4->d2 ||
				Sock->AcceptedIP.d3 != IPData_IPv4->d3 ||
				Sock->AcceptedIP.d4 != IPData_IPv4->d4 )
				continue ;
		}
		NetHandle = Sock->HandleInfo.Handle ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

		// ハンドルを返す
		return NetHandle ;
	}

	// ハンドルの作成
	NetHandle = AddHandle( DX_HANDLETYPE_NETWORK, FALSE, -1 ) ;
	if( NetHandle == -1 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( CONNECTNETWORK_GPARAM ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, NetHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, IPData_IPv4 ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, IPData_IPv6 ) ;
		AddASyncLoadParamInt( NULL, &Addr, Port ) ;
		AddASyncLoadParamInt( NULL, &Addr, ASync ) ;
		if( IPData_IPv4 != NULL )
		{
			AddASyncLoadParamStruct( NULL, &Addr, IPData_IPv4, sizeof( IPDATA ) ) ;
		}
		if( IPData_IPv6 != NULL )
		{
			AddASyncLoadParamStruct( NULL, &Addr, IPData_IPv6, sizeof( IPDATA_IPv6 ) ) ;
		}

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = ConnectNetWorkBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( CONNECTNETWORK_GPARAM ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, NetHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, IPData_IPv4 ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, IPData_IPv6 ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Port ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, ASync ) ;
		if( IPData_IPv4 != NULL )
		{
			AddASyncLoadParamStruct( AParam->Data, &Addr, IPData_IPv4, sizeof( IPDATA ) ) ;
		}
		if( IPData_IPv6 != NULL )
		{
			AddASyncLoadParamStruct( AParam->Data, &Addr, IPData_IPv6, sizeof( IPDATA_IPv6 ) ) ;
		}

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( NetHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( ConnectNetWorkBase_Static( GParam, NetHandle, IPData_IPv4, IPData_IPv6, Port, ASync, FALSE ) < 0 )
			goto ERR ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return NetHandle ;

ERR :
	SubHandle( NetHandle ) ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return -1 ;
}

// ConnectNetWork の処理を行う関数
static int ConnectNetWorkBase( IPDATA *IPData_IPv4, IPDATA_IPv6 *IPData_IPv6, int Port, int ASync )
{
	CONNECTNETWORK_GPARAM GParam ;
	int Result ;

	InitConnectNetWorkBaseGParam( &GParam ) ;

	Result = ConnectNetWorkBase_UseGParam( &GParam, IPData_IPv4, IPData_IPv6, Port, ASync, GetASyncLoadFlag() ) ;
	if( Result < 0 )
	{
		if( IPData_IPv6 != NULL )
		{
			DXERRORNETWORK(( "\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x3a\x00\x25\x00\x78\x00\x20\x00\x78\x30\x6e\x30\xa5\x63\x9a\x7d\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x02\x30\x0a\x00\x00"/*@ L"%x:%x:%x:%x:%x:%x:%x:%x への接続に失敗しました。\n" @*/,
				IPData_IPv6->Word[ 0 ], IPData_IPv6->Word[ 1 ], IPData_IPv6->Word[ 2 ], IPData_IPv6->Word[ 3 ],
				IPData_IPv6->Word[ 4 ], IPData_IPv6->Word[ 5 ], IPData_IPv6->Word[ 6 ], IPData_IPv6->Word[ 7 ] ) ) ;
		}
		else
		{
			DXERRORNETWORK(( "\x25\x00\x64\x00\x2e\x00\x25\x00\x64\x00\x2e\x00\x25\x00\x64\x00\x2e\x00\x25\x00\x64\x00\x20\x00\x78\x30\x6e\x30\xa5\x63\x9a\x7d\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x02\x30\x0a\x00\x00"/*@ L"%d.%d.%d.%d への接続に失敗しました。\n" @*/,IPData_IPv4->d1 , IPData_IPv4->d2 , IPData_IPv4->d3 , IPData_IPv4->d4) ) ;
		}
	}

	return Result ;
}

// 他マシンに接続する
extern int NS_ConnectNetWork_IPv6( IPDATA_IPv6 IPData, int Port )
{
	return ConnectNetWorkBase( NULL, &IPData, Port, FALSE ) ;
}

// 他マシンに接続する
extern int NS_ConnectNetWork( IPDATA IPData, int Port )
{
	return ConnectNetWorkBase( &IPData, NULL, Port, FALSE ) ;
}

// 他マシンに接続する
extern int NS_ConnectNetWork_IPv6_ASync( IPDATA_IPv6 IPData, int Port )
{
	return ConnectNetWorkBase( NULL, &IPData, Port, TRUE ) ;
}

// 他マシンに接続する
extern int NS_ConnectNetWork_ASync( IPDATA IPData, int Port )
{
	return ConnectNetWorkBase( &IPData, NULL, Port, TRUE ) ;
}

// 接続を受けられる状態にする
static int PreparationListenNetWork_Base( int IsIPv6, int Port )
{
	int ReturnValue = 0 ;
	SOCKETDATA *ListenSock ;

	// 通信関係が初期化されていなかったら初期化
	if( SockData.InitializeFlag == FALSE )
	{
		if( WinData.CloseMessagePostFlag != TRUE ) InitializeNetWork() ;
		else										return -1 ;
	}
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 既に受付中だったら何もせず終了	
//	if( SockData.CSocket[ MAX_SOCKET_NUM ] != NULL )
	if( !NETHCHK( SockData.ListenHandle, ListenSock ) )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 接続受付待ち用のハンドルを追加
	SockData.ListenHandle = AddHandle( DX_HANDLETYPE_NETWORK, FALSE, -1 ) ;
	if( NETHCHK_ASYNC( SockData.ListenHandle, ListenSock ) )
	{
		DXERRORNETWORK(( "\xa5\x63\x9a\x7d\x85\x5f\x61\x30\x28\x75\xcd\x30\xc3\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"接続待ち用ネットハンドルの作成に失敗しました" @*/ )) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 使用するデータだけ初期化
	ListenSock->ErrorFlag 			= FALSE ;			// エラーが発生したフラグを倒す
	ListenSock->UseFlag				= TRUE ;			// データを使用中にする
	ListenSock->IsIPv6				= IsIPv6 ;			// IPv6 かどうかを保存

	// アクセプト用ソケットの作成
	ListenSock->Socket = WinAPIData.WinSockFunc.socketFunc( IsIPv6 ? 23/*AF_INET6 は 23*/ : AF_INET , SOCK_STREAM , IPPROTO_TCP ) ;
	if( ListenSock->Socket == INVALID_SOCKET )
	{
		DXERRORNETWORK(( "\xbd\x30\xb1\x30\xc3\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x02\x30\x77\x00\x73\x00\x32\x00\x5f\x00\x33\x00\x32\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x4c\x30\x5c\x4f\xd5\x52\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"ソケットの作成に失敗しました。ws2_32.dllが作動していません" @*/ )) ;
		goto ERR ;
	}

	// WinSock メッセージ受け取り設定
	if( WinAPIData.WinSockFunc.WSAAsyncSelectFunc( ListenSock->Socket, SockData.MessageWindow, WSA_WINSOCKMESSAGE , FD_READ | FD_WRITE | FD_CLOSE | FD_ACCEPT ) == SOCKET_ERROR )
	{
		DXERRORNETWORK(( "\x57\x00\x53\x00\x41\x00\x73\x00\x79\x00\x6e\x00\x63\x00\x53\x00\x65\x00\x6c\x00\x65\x00\x63\x00\x74\x00\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"WSAsyncSelectでエラーが発生しました" @*/ )) ;
		goto ERR ;
	}

	// 作成したソケットとポート番号の関連付けを行う
	if( IsIPv6 )
	{
		_sockaddr_in6 local_v6;
	
		_MEMSET( &local_v6, 0, sizeof( local_v6 ) ) ;
		local_v6.sin6_family = 23/*AF_INET6 は 23*/ ;
		local_v6.sin6_port = Port == -1 ? WinAPIData.WinSockFunc.htonsFunc( WSA_DEFAULTPORT ) : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )Port ) ;

		if( WinAPIData.WinSockFunc.bindFunc( ListenSock->Socket , (struct sockaddr FAR *) &local_v6, sizeof(local_v6) ) == SOCKET_ERROR )
		{
			if( WinAPIData.WinSockFunc.WSAGetLastErrorFunc() != WSAEWOULDBLOCK )
			{
				DXERRORNETWORK(( "\x62\x00\x69\x00\x6e\x00\x64\x00\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"bind出来ません" @*/ )) ;
				goto ERR ;
			}
		}
	}
	else
	{
		SOCKADDR_IN	local;
	
		_MEMSET( &local, 0, sizeof( local ) ) ;
		local.sin_family = AF_INET;
		local.sin_port = Port == -1 ? WinAPIData.WinSockFunc.htonsFunc( WSA_DEFAULTPORT ) : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )Port ) ;
		local.sin_addr.s_addr = INADDR_ANY;

		if( WinAPIData.WinSockFunc.bindFunc( ListenSock->Socket , (struct sockaddr FAR *) &local, sizeof(local) ) == SOCKET_ERROR )
		{
			if( WinAPIData.WinSockFunc.WSAGetLastErrorFunc() != WSAEWOULDBLOCK )
			{
				DXERRORNETWORK(( "\x62\x00\x69\x00\x6e\x00\x64\x00\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"bind出来ません" @*/ )) ;
				goto ERR ;
			}
		}
	}

	// 接続待ち用ソケットを待機状態にする
	if( WinAPIData.WinSockFunc.listenFunc( ListenSock->Socket, SOMAXCONN ) == SOCKET_ERROR )
	{
		DXERRORNETWORK(( "\x6c\x00\x69\x00\x73\x00\x74\x00\x65\x00\x6e\x00\x92\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"listenを失敗しました" @*/ )) ;
		goto ERR ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
	
	// エラー処理
ERR :
	// ハンドルの削除
	SubHandle( SockData.ListenHandle ) ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return -1 ;
}

// 接続を受けられる状態にする
extern int NS_PreparationListenNetWork_IPv6( int Port )
{
	return PreparationListenNetWork_Base( TRUE, Port ) ;
}

// 接続を受けられる状態にする
extern int NS_PreparationListenNetWork( int Port )
{
	return PreparationListenNetWork_Base( FALSE, Port ) ;
}

// アクセプトする
extern int AcceptNetWork( void )
{
	int ReturnValue = TRUE ;
//	SOCKETDATA * ListenSock = SockData.CSocket[ MAX_SOCKET_NUM ] ;
	SOCKETDATA *ListenSock = NULL ;
	SOCKETDATA *Sock = NULL ;
	int NewNetHandle ;

	// 通信関係が初期化されていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係の処理を行う
	NS_ProcessNetMessage() ;

	// 接続待ちをしていない場合は何もせず終了
//	if( ListenSock == NULL )
	if( NETHCHK( SockData.ListenHandle, ListenSock ) )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 新しいハンドルを追加
	NewNetHandle = AddHandle( DX_HANDLETYPE_NETWORK, FALSE, -1 ) ;
	if( NETHCHK_ASYNC( NewNetHandle, Sock ) )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x61\x00\x63\x00\x63\x00\x65\x00\x70\x00\x74\x00\x20\x00\x28\x75\x6e\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x0a\x00\x00"/*@ L"accept 用のハンドル作成に失敗しました \n" @*/ ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// データ初期化
	Sock->IsUDP					= FALSE ;							// 接続された場合は必ずＴＣＰ
	Sock->IsUDPBroadCast		= FALSE ;							// ブロードキャストは使用しない
	Sock->IsIPv6				= ListenSock->IsIPv6 ;				// ＩＰｖ６かどうかを保存
	Sock->ErrorFlag 			= FALSE ;							// エラーが発生したフラグを倒す
	Sock->UseFlag				= TRUE ;							// データを使用中にする
	Sock->PreConnectionFlag		= FALSE ;							// 接続前フラグを倒す
	Sock->ConnectionFlag		= TRUE ;							// 接続したかフラグを接続している状態に
	Sock->ConnectionLostFlag 	= FALSE ;							// 切断確認フラグはとりあえず倒しておく
	Sock->AccepteConfirFlag 	= FALSE ;							// 接続確認フラグは相手から勝手に接続されたので確認していない状態にしておく
	Sock->AcceptFlag			= TRUE ;							// 相手から接続されたのかフラグを立てる
	Sock->DXProtocolFlag 		= !SockData.NonUseDXProtocolFlag ;	// ＤＸライブラリ独自の通信方式を使うかどうかをセット
	Sock->CloseAfterLostFlag	= !SockData.NonCloseAfterLostFlag ;	// 切断直後にハンドルを解放するかどうかのフラグをセット

	// ＤＸライブラリ独自の方式を取る場合は送受信用バッファを初期化
	if( Sock->DXProtocolFlag == TRUE )
	{
		// ユーザーに渡すときに使うリングバッファの初期化
		RingBufInitialize( &Sock->RecvBufferToUserR ) ;
		Sock->RecvComDataVol = 0 ;
		Sock->RecvComDataOriginalVol = 0 ;
		Sock->RecvBufferToUserOpenSize = 0 ;

		// 送信バッファ関連の初期化
		RingBufInitialize( &Sock->SendBufferR ) ;
		Sock->SendComDataVol = 0 ;
		Sock->SendComDataComVol = 0 ;
	}
	
	// 接続待ちをしている接続先の情報を取得する
	{
		int Len ;
		if( Sock->IsIPv6 )
		{
			int i ;
			_sockaddr_in6 AcIP_v6 ;

			// 接続確立
			Len = sizeof( _sockaddr_in6 ) ;
			if( ( Sock->Socket = WinAPIData.WinSockFunc.acceptFunc( ListenSock->Socket , ( SOCKADDR * )&AcIP_v6 , &Len ) ) == INVALID_SOCKET )
			{
				DXERRORNETWORK(( "\x61\x00\x63\x00\x63\x00\x65\x00\x70\x00\x74\x00\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"acceptに失敗しました" @*/ )) ;
				goto ERR ;
			}

			// ＩＰの保存
			for( i = 0 ; i < 8 ; i ++ )
				Sock->AcceptedIP_IPv6.Word[ i ] = AcIP_v6.sin6_addr.Word[ i ] ;
			
			// ポートの保存
			Sock->Port = AcIP_v6.sin6_port ;
		}
		else
		{
			SOCKADDR AcIP ;
			SOCKADDR_IN *AcIP_IN ;

			// 接続確立
			Len = sizeof( SOCKADDR_IN ) ;
			if( ( Sock->Socket = WinAPIData.WinSockFunc.acceptFunc( ListenSock->Socket , &AcIP , &Len ) ) == INVALID_SOCKET )
			{
				DXERRORNETWORK(( "\x61\x00\x63\x00\x63\x00\x65\x00\x70\x00\x74\x00\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"acceptに失敗しました" @*/ )) ;
				goto ERR ;
			}

			// ＩＰの保存
			AcIP_IN = ( SOCKADDR_IN * )&AcIP ;
			Sock->AcceptedIP.d1 = AcIP_IN->sin_addr.S_un.S_un_b.s_b1 ;
			Sock->AcceptedIP.d2 = AcIP_IN->sin_addr.S_un.S_un_b.s_b2 ;
			Sock->AcceptedIP.d3 = AcIP_IN->sin_addr.S_un.S_un_b.s_b3 ;
			Sock->AcceptedIP.d4 = AcIP_IN->sin_addr.S_un.S_un_b.s_b4 ;
			
			// ポートの保存
			Sock->Port = AcIP_IN->sin_port ;
		}
	}

	// WinSock メッセージ受け取り設定
	if( WinAPIData.WinSockFunc.WSAAsyncSelectFunc( Sock->Socket , SockData.MessageWindow, WSA_WINSOCKMESSAGE , FD_WRITE | FD_READ | FD_CLOSE ) == SOCKET_ERROR )
	{
		DXERRORNETWORK(( "\x57\x00\x53\x00\x41\x00\x41\x00\x73\x00\x79\x00\x6e\x00\x63\x00\x53\x00\x65\x00\x6c\x00\x65\x00\x63\x00\x74\x00\x6e\x30\xa8\x30\xe9\x30\xfc\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x41\x00\x63\x00\x63\x00\x65\x00\x70\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x00"/*@ L"WSAAsyncSelectのエラー in AcceptNetWork" @*/ )) ;
		goto ERR ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
	
	// エラー処理	
ERR :

	// ハンドルを削除
	SubHandle( NewNetHandle ) ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return -1 ;
}

// 接続を受けつけ状態の解除
extern int NS_StopListenNetWork( void )
{
	if( SockData.InitializeFlag == FALSE )
		return -1 ;

	// 接続受け付け用のハンドルを削除
	return SubHandle( SockData.ListenHandle ) ;
/*
	int ReturnValue = 0 ;
	SOCKETDATA * ListenSock = SockData.CSocket[ MAX_SOCKET_NUM ] ;

	// 通信関係が初期化されていなかったら初期化
	if( SockData.InitializeFlag == FALSE )
	{
		if( WinData.CloseMessagePostFlag != TRUE ) InitializeNetWork() ;
		else										return -1 ;
	}
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 接続受付状態では無かったら何もせず終了
	if( ListenSock == NULL )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 非ブロッキング解除
	WinAPIData.WinSockFunc.WSAAsyncSelectFunc( ListenSock->Socket, SockData.MessageWindow, 0, 0 );

	// データ送受信シャットダウン
	WinAPIData.WinSockFunc.shutdownFunc( ListenSock->Socket , 2 ) ;

	// ソケットを閉じる
	WinAPIData.WinSockFunc.closesocketFunc( ListenSock->Socket );
	ListenSock->Socket = 0 ;
	
	// 使用中フラグを倒す
	ListenSock->UseFlag = FALSE ;

	// リストから外す
	SubHandleList( &ListenSock->List ) ;

	// メモリの解放
	DXFREE( ListenSock ) ;
	SockData.CSocket[ MAX_SOCKET_NUM ] = NULL ;

	// ソケットの総数をデクリメント
	SockData.SocketNum -- ;

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
*/
}


// 接続を終了する
extern int NS_CloseNetWork( int NetHandle )
{
	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE )
		return -1 ;

	// ハンドルを削除
	return SubHandle( NetHandle ) ;
/*
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

		return -1 ;
	}

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 残りのデータをバッファから消去する
	{
		int time ;
		
		time = NS_GetNowCount() ;
		while( NS_GetNowCount() - time < 1000 && Sock->RecvComDataVol != 0 )
		{
			if( NS_ProcessNetMessage() != 0 ) break ;
			RecvSocket( NetHandle ) ;
		}

		time = NS_GetNowCount() ;
		while( NS_GetNowCount() - time < 1000 && Sock->SendBufferR.DataLength != 0 )
		{
			if( NS_ProcessNetMessage() != 0 ) break ;
			SendSocket( NetHandle ) ;
		}
	}

	// ソケットをの後始末を行う
	{
		// 非ブロッキング解除
		WinAPIData.WinSockFunc.WSAAsyncSelectFunc( Sock->Socket, SockData.MessageWindow, 0, 0 );

		// ソケットを閉じる
		WinAPIData.WinSockFunc.closesocketFunc( Sock->Socket );
		Sock->Socket = 0 ;
	}

	// 送受信用バッファの解放
	RingBufTerminate( &Sock->RecvBufferToUserR ) ;
	RingBufTerminate( &Sock->SendBufferR ) ;

	// 使用中フラグを倒す
	Sock->UseFlag = FALSE ;
	
	// その他適当にフラグをセット
	Sock->ConnectionFlag = FALSE ;
	Sock->ConnectionLostFlag = TRUE ;
	Sock->AccepteConfirFlag = FALSE ;

	// リストから外す
	SubHandleList( &Sock->List ) ;

	// メモリの解放
	DXFREE( Sock ) ;
	SockData.CSocket[ NetHandle & DX_HANDLEINDEX_MASK ] = NULL ;

	// ソケットの総数をデクリメント
	SockData.SocketNum -- ;

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
*/
}




// データの受信処理
static int RecvSocket( int NetHandle )
{
	static int ProcessRunFlag = FALSE ;
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;
	int RecvLen ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 既に処理が実行されていたら何もせず終了
	if( ProcessRunFlag == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 「処理を実行中」フラグを立てる
	ProcessRunFlag = TRUE ;


	// 通信関係処理
	NS_ProcessNetMessage() ;
	
	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x52\x00\x65\x00\x63\x00\x76\x00\x53\x00\x6f\x00\x63\x00\x6b\x00\x65\x00\x74\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in RecvSocket\n" @*/ ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}


	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x52\x00\x65\x00\x63\x00\x76\x00\x53\x00\x6f\x00\x63\x00\x6b\x00\x65\x00\x74\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in RecvSocket\n" @*/ ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// ＤＸライブラリ独自の方式かどうかで処理を分岐
	if( Sock->DXProtocolFlag == TRUE )
	{
		do{ 
			// 受信するデータの量をセット
			if( Sock->RecvComDataVol == 0 )
			{
				// 受信するデータの量が取得できるか調べる
				RecvLen = WinAPIData.WinSockFunc.recvFunc( Sock->Socket , ( char * )&Sock->RecvComDataVol , sizeof( int ) , MSG_PEEK ) ;
				if( RecvLen < 0 )
					break ;

				// 受信するデータ量を保存出来るデータ量が送られていなかったら何もせず終了
				if( RecvLen < 4 )
					break ;

				// 受信するデータの量を正式に取得
				RecvLen = WinAPIData.WinSockFunc.recvFunc( Sock->Socket , ( char * )&Sock->RecvComDataVol , sizeof( int ) , 0 ) ;
				if( RecvLen < 4 )
					break ;

				// 受信するデータサイズを保存
				Sock->RecvComDataOriginalVol  = Sock->RecvComDataVol ;

				// 新たに受信するデータが収まるようにバッファをリサイズ
				if( RingBufReSize( &Sock->RecvBufferToUserR, Sock->RecvComDataVol ) < 0 )
				{
					DXST_LOGFILEFMT_ADDUTF16LE(( "\xd7\x53\xe1\x4f\xc7\x30\xfc\x30\xbf\x30\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xea\x30\xf3\x30\xb0\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xea\x30\xb5\x30\xa4\x30\xba\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x28\x00\xc7\x30\xfc\x30\xbf\x30\xb5\x30\xa4\x30\xba\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x29\x00\x00"/*@ L"受信データを保存するリングバッファのリサイズに失敗しました(データサイズ %d byte)" @*/, Sock->RecvComDataVol )) ;

					// 失敗したら重大なエラー
					Sock->ErrorFlag = TRUE ;

					ReturnValue = -1 ;
					goto FUNCTIONEND ;
				}
			}

			// データの受信
			{
				RINGBUF *RingBuf = &Sock->RecvBufferToUserR ;

				// アドレスチェック
				RingBufAddressCheck( RingBuf ) ;

				// ２回に分けて格納しなければならないかどうかで処理を分岐
				if( RingBuf->End + Sock->RecvComDataVol > RingBuf->BufferSize )
				{
					int AllRecvLen ;
				
					// ２回に別けて格納する場合の処理

					// 最初のデータを取得
					AllRecvLen = 0 ;
					RecvLen = WinAPIData.WinSockFunc.recvFunc( Sock->Socket,(char *)RingBuf->DataBuffer + RingBuf->End,
															 RingBuf->BufferSize - RingBuf->End, 0 ) ;
					if( RecvLen < 0 )
						break ;

					// 指定したデータ量がちゃんと取得できたか調べる
					if( RecvLen < RingBuf->BufferSize - RingBuf->End )
					{
						RingBuf->End += RecvLen ;
					}
					else
					{
						// 取得したデータ量の保存
						AllRecvLen = RecvLen ;

						// 次のデータを取得
						RecvLen = WinAPIData.WinSockFunc.recvFunc( Sock->Socket, (char *)RingBuf->DataBuffer, Sock->RecvComDataVol - AllRecvLen, 0 ) ;

						// エラーが起きて何もデータを受信できなかったら最初に取得したデータ分だけ取得したことにする
						if( RecvLen < 0 )
						{
							RecvLen = AllRecvLen ;
							RingBuf->End = 0  ;
						}
						else
						{
							// 最後まできっちりデータを取得できた場合
						
							// アドレスの整理
							RingBuf->End = RecvLen  ;

							// 取得したデータの総量をセット
							RecvLen += AllRecvLen ;
						}
					}
				}
				else
				{
					// １回で格納する場合の処理
					
					// 一度に取得
					RecvLen = WinAPIData.WinSockFunc.recvFunc( Sock->Socket, (char *)RingBuf->DataBuffer + RingBuf->End, Sock->RecvComDataVol, 0 ) ;
					if( RecvLen < 0 )
						break ;

					// アドレスの整理
					RingBuf->End += RecvLen ;
				}

				// 取得したデータの量だけ有効データ量を増やす
				RingBuf->DataLength += RecvLen ;
			}

			// 受信すべきデータ量を受信したデータ量分だけ減らす
			Sock->RecvComDataVol -= RecvLen ;

			// 受信すべきデータ量が０になった場合は利用者に公開するデータ量に今回受信したデータの量を加算する
			if( Sock->RecvComDataVol == 0 )
				Sock->RecvBufferToUserOpenSize += Sock->RecvComDataOriginalVol ;

			if( NS_ProcessNetMessage() != 0 ) break ;
		
		}// エラーが起きていなくてまだ取得すべきデータがある場合はループ
		while( RecvLen > 0 && Sock->RecvComDataVol == 0 ) ;
	}

FUNCTIONEND :

	// 「処理を実行中」フラグを倒す
	ProcessRunFlag = FALSE ; 

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}

// 溜まったデータの送信処理
extern int SendSocket( int NetHandle )
{
	static int ProcessRunFlag = FALSE ;
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;
	int SendVol ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 既に処理が実行されていたら何もせず終了
	if( ProcessRunFlag == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 「処理を実行中」フラグを立てる
	ProcessRunFlag = TRUE ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x53\x00\x6f\x00\x63\x00\x6b\x00\x65\x00\x74\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in SendSocket\n" @*/ ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x53\x00\x6f\x00\x63\x00\x6b\x00\x65\x00\x74\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in SendSocket\n" @*/ ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 送信すべきデータ容量がない場合は終了
	if( Sock->SendBufferR.DataLength == 0 )
	{
		ReturnValue = 0 ;
		goto FUNCTIONEND ;
	}

	// ＤＸライブラリ独自の方式かどうかで処理を分岐
	if( Sock->DXProtocolFlag == TRUE )
	{
		do{
			// まだ次に送信するデータ量を送信していない場合は送信する
			if( Sock->SendComDataComVol != 0 )
			{
				SendVol = WinAPIData.WinSockFunc.sendFunc( Sock->Socket , ( char * )&Sock->SendComDataVol + 4 - Sock->SendComDataComVol , Sock->SendComDataComVol , 0 ) ;
				if( SendVol < 0 )
					break ;

				// 送信できた分を減算
				Sock->SendComDataComVol -= SendVol ;
				
				// 送信しきれなかったらここで終了
				if( Sock->SendComDataComVol != 0 )
					break ;
			}

			// 送信中データがない場合は次なるデータの送信にかかる
			if( Sock->SendComDataVol == 0 )
			{
				// 送信するデータ量を送信する
				Sock->SendComDataVol = Sock->SendBufferR.DataLength ;
				SendVol = WinAPIData.WinSockFunc.sendFunc( Sock->Socket , ( char * )&Sock->SendComDataVol , 4 , 0 ) ;
				
				// 全く送信できなかったら次回にもちこし
				if( SendVol < 0 )
				{
					Sock->SendComDataVol = 0 ;
					break ;
				}

				// 送信しきれなかったら次回続きを送信する
				if( SendVol < 4 )
				{
					Sock->SendComDataComVol = 4 - SendVol ;
					break ;
				}
			}

			// データの送信
			{
				RINGBUF *RingBuf = &Sock->SendBufferR ;
				
				// アドレスチェック
				RingBufAddressCheck( RingBuf ) ;

				// ２回に別けなければいけないかどうかで処理を分岐
				if( RingBuf->Start + Sock->SendComDataVol > RingBuf->BufferSize )
				{
					int AllSendVol ;
				
					// ２回に別ける場合の処理

					// 最初のデータを送信
					AllSendVol = 0 ;
					SendVol = WinAPIData.WinSockFunc.sendFunc( Sock->Socket , (char *)RingBuf->DataBuffer + RingBuf->Start, RingBuf->BufferSize - RingBuf->Start, 0 ) ;
					if( SendVol < 0 )
						break ;

					// 送信できたデータ量が指定よりも少なかったらここで終了
					if( SendVol < RingBuf->BufferSize - RingBuf->Start )
					{
						// アドレスの調整
						RingBuf->Start += SendVol ;
					}
					else
					{
						// まだ送信できる場合は送信

						// 送信できたデータ量を保存
						AllSendVol = SendVol ;

						// 次のデータを送信
						SendVol = WinAPIData.WinSockFunc.sendFunc( Sock->Socket, RingBuf->DataBuffer, Sock->SendComDataVol - AllSendVol, 0 ) ;
						
						// エラーが起きた場合は一つ前に送信できた分だけを送信したことにして調整
						if( SendVol < 0 )
						{
							// アドレスの調整
							RingBuf->Start = 0 ;

							// 取得したデータの量をセット
							SendVol = AllSendVol ;
						}
						else
						if( SendVol == Sock->SendComDataVol - AllSendVol )
						{
							// きっちり全部送信できた時の処理

							// アドレスの調整
							RingBuf->Start = SendVol ;
							
							// 取得したデータ量のセット
							SendVol += AllSendVol ;
						}
					}
				}
				else
				{
					// １回で格納する場合の処理
					
					// 一気に送信
					SendVol = WinAPIData.WinSockFunc.sendFunc( Sock->Socket , (char *)RingBuf->DataBuffer + RingBuf->Start, Sock->SendComDataVol, 0 ) ;
					if( SendVol < 0 )
						break ;

					// アドレスの調整
					RingBuf->Start += SendVol ;
				}

				// 送信出来たデータ量分だけ送信待ちを喰らっているデータ量を減らす
				RingBuf->DataLength -= SendVol ;

				// 送信できたデータ量分だけ今回送信すべきデータ量を減らす
				Sock->SendComDataVol -= SendVol ;
			}

			if( NS_ProcessNetMessage() != 0 ) break ;
		
		}// エラーが起きていなくてまだ送信すべきデータがある場合はループ
		while( SendVol != 0 && Sock->SendComDataVol == 0 && Sock->SendBufferR.DataLength != 0 );
	}

FUNCTIONEND :

	// 「処理を実行中」フラグを倒す
	ProcessRunFlag = FALSE ; 

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}



// 接続状態を取得する
extern int NS_GetNetWorkAcceptState( int NetHandle )
{
	SOCKETDATA * Sock ;
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
		goto ENDLABEL ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x65\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x41\x00\x63\x00\x63\x00\x65\x00\x70\x00\x74\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in GetNetWorkAcceptState\n" @*/ ) ; 
		goto ENDLABEL ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x65\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x41\x00\x63\x00\x63\x00\x65\x00\x70\x00\x74\x00\x53\x00\x74\x00\x61\x00\x74\x00\x65\x00\x68\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in GetNetWorkAcceptStateh\n" @*/ ) ;
		goto ENDLABEL ;
	}
	
	// もし切断されていてそれがまだ確認されていなかったら確認されたフラグを立てる
	if( Sock->PreConnectionFlag  == FALSE &&
		Sock->ConnectionFlag     == FALSE &&
		Sock->ConnectionLostFlag == FALSE )
	{
		Sock->ConnectionLostFlag = TRUE ;
	}

	// 接続されているかどうかを返す
	Result = Sock->ConnectionFlag == TRUE ? 1 : 0 ;

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// 受信データの量を得る
extern int NS_GetNetWorkDataLength( int NetHandle )
{
	SOCKETDATA * Sock ;
	int temp ;
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 内部データ送受信処理を行う
	RecvSocket( NetHandle ) ;
	SendSocket( NetHandle ) ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
		goto ENDLABEL ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x65\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x44\x00\x61\x00\x74\x00\x61\x00\x4c\x00\x65\x00\x6e\x00\x67\x00\x74\x00\x68\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in GetNetWorkDataLength\n" @*/ ) ; 
		goto ENDLABEL ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x65\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x44\x00\x61\x00\x74\x00\x61\x00\x4c\x00\x65\x00\x6e\x00\x67\x00\x74\x00\x68\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in GetNetWorkDataLength\n" @*/ ) ;
		goto ENDLABEL ;
	}

	// ＤＸライブラリ独自の方式をとるかどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		// ＤＸライブラリ独自の方式ではない場合受信量の取得は出来ないので
		// データが来ているかどうかだけ返す
		Result = NS_NetWorkRecvToPeek( NetHandle, &temp, 1 ) ;
		Result = Result < 0 ? Result : ( Result == 1 ? TRUE : FALSE ) ;
	}
	else
	{
//		// 受信バッファに溜まっているデータ量を返す
//		Result = RingBufGetDataLength( &Sock->RecvBufferToUserR ) ;

		// 受信バッファに受信が完了しているデータ量を返す
		Result = Sock->RecvBufferToUserOpenSize ;
	}

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// 未送信のデータの量を得る 
extern int NS_GetNetWorkSendDataLength( int NetHandle )
{
	SOCKETDATA * Sock ;
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
		goto ENDLABEL ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x65\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x44\x00\x61\x00\x74\x00\x61\x00\x4c\x00\x65\x00\x6e\x00\x67\x00\x74\x00\x68\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in GetNetWorkSendDataLength\n" @*/ ) ; 
		goto ENDLABEL ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x47\x00\x65\x00\x74\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x44\x00\x61\x00\x74\x00\x61\x00\x4c\x00\x65\x00\x6e\x00\x67\x00\x74\x00\x68\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in GetNetWorkSendDataLength\n" @*/ ) ;
		goto ENDLABEL ;
	}

	// ＤＸライブラリ独自の方式をとっているかどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		// ＤＸライブラリ独自の方式をとっていない場合未送信分のデータは蓄えられないので常に０
		Result = 0 ;
	}
	else
	{
		// 送信バッファに溜まったデータ量を返す
		Result = RingBufGetDataLength( &Sock->SendBufferR ) ;
	}

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}




// 新たに接続した通信回線を得る
extern int NS_GetNewAcceptNetWork( void )
{
	int Result = 0 ;
	SOCKETDATA * Sock ;
	HANDLELIST *List ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 新たに接続した接続先がある場合はその接続ハンドルを返す
	for( List = HandleManageArray[ DX_HANDLETYPE_NETWORK ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sock = (SOCKETDATA *)List->Data ;

		// 新たに接続されたハンドルか判定
		if( Sock->UseFlag == TRUE &&			// ハンドルが有効であるか判定
			Sock->ConnectionFlag == TRUE &&		// そもそも接続されているか判定
			Sock->AccepteConfirFlag == FALSE )	// 接続されていることがまだ確認されていないか判定
		{
			// 今回で接続されたことは確実に確認されるので
			// 接続されたことが確認されたことにする
			Sock->AccepteConfirFlag = TRUE ;

			// 見つけた未確認接続先のハンドルを返す
			Result = Sock->HandleInfo.Handle ;
			goto FUNCTIONEND ;
		}
	}

	// 此処に来たら新たに接続された接続先はないということなので -1 を返す
	Result = -1 ;

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// 接続を切断された通信回線を得る
extern int NS_GetLostNetWork( void )
{
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;
	HANDLELIST *List ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// まだライブラリユーザーが未確認の切断された接続先を探す
	for( List = HandleManageArray[ DX_HANDLETYPE_NETWORK ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sock = (SOCKETDATA *)List->Data ;

		// 判定
		if( Sock->UseFlag == TRUE &&								// データは有効であるかの判定
			Sock->HandleInfo.Handle  != SockData.ListenHandle &&	// 接続待ちソケットではないか判定
			Sock->PreConnectionFlag  == FALSE &&					// 接続前ではないか判定
			Sock->ConnectionFlag     == FALSE &&					// 接続を切断されているかの判定
			Sock->ConnectionLostFlag == FALSE )						// 切断されたことがまだ確認されていないかの判定
		{
			// 切断を確認したフラグを立てる
			Sock->ConnectionLostFlag = TRUE ;

			// あらたに確認された接続ハンドルを返す
			ReturnValue = Sock->HandleInfo.Handle ;
			goto FUNCTIONEND ;
		}
	}

	// ここにきたら新たに切断された接続はなかったと言うことなので -1 を返す
	ReturnValue = -1 ;

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}

// 接続先のＩＰを得る( 切断されている接続先でも OK )
extern int NS_GetNetWorkIP_IPv6( int NetHandle , IPDATA_IPv6 *IpBuf )
{
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// IPv6 では無かったらエラー
	if( Sock->IsIPv6 == FALSE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// ＩＰを代入する
	*IpBuf = Sock->AcceptedIP_IPv6 ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// 接続先のＩＰを得る( 切断されている接続先でも OK )
extern int NS_GetNetWorkIP( int NetHandle , IPDATA *IpBuf )
{
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 有効なハンドルではなかったらエラー
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// IPv4 では無かったらエラー
	if( Sock->IsIPv6 == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// ＩＰを代入する
	*IpBuf = Sock->AcceptedIP ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// 自分のIPv4を得る
extern int NS_GetMyIPAddress( IPDATA *IpBuf, int IpBufLength, int *IpNum )
{
	// 通信関係の初期化がまだ行われていない場合は初期化する
	if( !SockData.InitializeFlag )
	{
		if( WinData.CloseMessagePostFlag != TRUE ) InitializeNetWork() ;
		else										return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ＩＰを書き込む
	if( SockData.MyIPv4 != NULL && IpBuf != NULL )
	{
		int i ;

		for( i = 0 ; i < IpBufLength && i < SockData.MyIPv4Num ; i ++ )
		{
			IpBuf[ i ] = SockData.MyIPv4[ i ] ;
		}
	}

	// ＩＰアドレスの数をセットする
	if( IpNum != NULL )
	{
		*IpNum = SockData.MyIPv4Num ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// 自分のIPv6を得る
extern int NS_GetMyIPAddress_IPv6( IPDATA_IPv6 *IpBuf, int IpBufLength, int *IpNum )
{
	// 通信関係の初期化がまだ行われていない場合は初期化する
	if( !SockData.InitializeFlag )
	{
		if( WinData.CloseMessagePostFlag != TRUE ) InitializeNetWork() ;
		else										return -1 ;
	}

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ＩＰを書き込む
	if( SockData.MyIPv6 != NULL && IpBuf != NULL )
	{
		int i ;

		for( i = 0 ; i < IpBufLength && i < SockData.MyIPv6Num ; i ++ )
		{
			IpBuf[ i ] = SockData.MyIPv6[ i ] ;
		}
	}

	// ＩＰアドレスの数をセットする
	if( IpNum != NULL )
	{
		*IpNum = SockData.MyIPv6Num ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return 0 ;
}

// 接続のタイムアウトまでの時間を設定する
extern int NS_SetConnectTimeOutWait( int Time )
{
	// クリティカルセクションの取得
	if( SockData.InitializeFlag )
	{
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
	}

	SockData.TimeOutWait = Time ;

	// クリティカルセクションの解放
	if( DxSysData.DxLib_InitializeFlag == TRUE )
	{
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
	}

	// 終了
	return 0 ;
}

// ＤＸライブラリの通信形態を使うかどうかをセットする
extern int NS_SetUseDXNetWorkProtocol( int Flag )
{
	// クリティカルセクションの取得
	if( SockData.InitializeFlag )
	{
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
	}

	SockData.NonUseDXProtocolFlag = !Flag ;

	// クリティカルセクションの解放
	if( SockData.InitializeFlag )
	{
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
	}

	// 終了
	return 0 ;
}

// ＤＸライブラリの通信形態を使うかどうかを取得する
extern int NS_GetUseDXNetWorkProtocol( void )
{
	return SockData.NonUseDXProtocolFlag ;
}

// SetUseDXNetWorkProtocol の別名
extern int NS_SetUseDXProtocol( int Flag )
{
	return NS_SetUseDXNetWorkProtocol( Flag ) ;
}

// GetUseDXNetWorkProtocol の別名
extern int NS_GetUseDXProtocol( void )
{
	return NS_GetUseDXNetWorkProtocol() ;
}


// 接続が切断された直後に接続ハンドルを解放するかどうかのフラグをセットする
extern int NS_SetNetWorkCloseAfterLostFlag( int Flag )
{
	// クリティカルセクションの取得
	if( SockData.InitializeFlag )
	{
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
	}

	// フラグを保存する
	SockData.NonCloseAfterLostFlag = !Flag ;

	// クリティカルセクションの解放
	if( SockData.InitializeFlag )
	{
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
	}

	// 終了
	return 0 ;
}

// 接続が切断された直後に接続ハンドルを解放するかどうかのフラグを取得する
extern int NS_GetNetWorkCloseAfterLostFlag( void )
{
	// フラグを返す
	return SockData.NonCloseAfterLostFlag ;
}

/*
// ＩＥで設定されているプロキシを使用するかどうかのフラグをセットする
extern	int			SetUseIEProxySettingFlag( int Flag )
{
	HttpData.NonUseIEProxy = !Flag ;
	
	return 0 ;
}

// ＩＥで設定されているプロキシを使用するかどうかのフラグを取得する
extern	int			GetUseIEProxySettingFlag( void )
{
	return !HttpData.NonUseIEProxy ;
}

// ＨＴＴＰ通信で使用するプロキシ設定を行う
extern int NS_SetProxySetting( int UseFlag, const char *Address, int Port )
{
	HttpData.UseProxy = UseFlag ;
	_STRCPY( HttpData.ProxyHost, Address ) ;
	HttpData.ProxyPort = Port == -1 ? 80 : Port ;
	
	return 0 ;
}

// ＨＴＴＰ通信で使用するプロキシ設定を取得する
extern int NS_GetProxySetting( int *UseFlagBuffer, char *AddressBuffer, int *PortBuffer )
{
	if( UseFlagBuffer != NULL ) *UseFlagBuffer = HttpData.UseProxy ;
	if( AddressBuffer != NULL ) _STRCPY( AddressBuffer, HttpData.ProxyHost ) ;
	if( PortBuffer != NULL ) *PortBuffer = HttpData.ProxyPort ;

	return 0 ;
}

// ＩＥのプロキシ設定を適応する
extern int NS_SetIEProxySetting( void ) 
{
	char str[256] ;
	DWORD size ;
	DWORD type ;
	char *p ;
	DWORD hr ;
	HKEY key ;
	int OpenFlag ;

	// レジストリからプロキシの情報を得る
	_MEMSET( str, 0, 256 ) ;
	type = REG_SZ ;
	size = 256 ;
	OpenFlag = FALSE ;
	
	// レジストリを開く
	hr = RegOpenKeyEx( HKEY_CURRENT_USER, IEPROXY, 0, KEY_ALL_ACCESS, &key ) ; 
	if( hr != ERROR_SUCCESS ) goto ERR ;
	OpenFlag = TRUE ;

	// レジストリを格納するのに必要なデータのサイズを得る
	hr = RegQueryValueEx( key, "ProxyServer", NULL, &type, NULL, &size ) ;
	if( hr != ERROR_SUCCESS ) goto ERR ;

	// レジストリの内容を得る
	hr = RegQueryValueEx( key, "ProxyServer", NULL, &type, ( unsigned char * )str, &size ) ;
	if( hr != ERROR_SUCCESS ) goto ERR ;

	// レジストリを閉じる
	RegCloseKey( key ) ;
	OpenFlag = FALSE ;
						
	// 使用しているプロキシサーバーの文字列を解析する
	p = _STRCHR( str, ':' ) ;
	_STRCPY( HttpData.ProxyHost, str ) ;
	if( p == NULL )
	{
		HttpData.ProxyPort = 80 ;
	}
	else
	{
		HttpData.ProxyHost[p-str] = '\0' ;
		HttpData.ProxyPort = _ATOI( p + 1 ) ;
		if( HttpData.ProxyPort <= 0 ) HttpData.ProxyPort = -1 ;
	}

	// 終了
	return 0 ;
	
	// エラー処理
ERR :
	if( OpenFlag == TRUE ) 	RegCloseKey( key ) ;

	return -1 ;
}
*/

// NetWorkRecv の実処理関数
static int NetWorkRecv_Static(
	int NetHandle,
	void *Buffer,
	int Length,
	int Peek, 
	int ASyncThread
)
{
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;

	// 取得するデータ長の値が不正な場合は何もせず終了
	if( Length < 0 ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( ASyncThread )
	{
		if( TCPNETHCHK_ASYNC( NetHandle, Sock ) )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
			return -1 ;
		}
	}
	else
	{
		if( TCPNETHCHK( NetHandle, Sock ) )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
			return -1 ;
		}
	}

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x52\x00\x65\x00\x63\x00\x76\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in NetWorkRecv\n" @*/ ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x52\x00\x65\x00\x63\x00\x76\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in NetWorkRecv\n" @*/ ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 内部受信データ取得処理を行う
	if( RecvSocket( NetHandle ) < 0 )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// ＤＸライブラリ独自の方式を使用するかどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		// WinSock から直接読み込む
		int RecvVol ;

		// 受信できる場合は受信、受信したデータ量を返す
		RecvVol = WinAPIData.WinSockFunc.recvFunc( Sock->Socket, (char *)Buffer, Length, Peek ? MSG_PEEK : 0 ) ;

		// サイズを返す
		ReturnValue = RecvVol ;
		goto FUNCTIONEND ;
	}
	else
	{
		// 受信したデータ容量が要求容量よりも少ない場合は失敗
		if( RingBufGetDataLength( &Sock->RecvBufferToUserR ) < Length )
		{
			ReturnValue = -1 ;
			goto FUNCTIONEND ;
		}

		// データの取得
		if( Peek )
		{
			RingBufDataGet( &Sock->RecvBufferToUserR, Buffer, Length, TRUE ) ;
		}
		else
		{
			RingBufDataGet( &Sock->RecvBufferToUserR, Buffer, Length, FALSE ) ;
			Sock->RecvBufferToUserOpenSize -= Length ;
		}

		// 内部受信データ取得処理を行う
		RecvSocket( NetHandle ) ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}

#ifndef DX_NON_ASYNCLOAD
// NetWorkRecv の非同期読み込みスレッドから呼ばれる関数
static void NetWorkRecv_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	SOCKETDATA * Sock ;
	int NetHandle ;
	void *Buffer ;
	int Length ;
	int Peek ;
	int Addr ;

	Addr = 0 ;
	NetHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Buffer = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	Length = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Peek = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	if( !NETHCHK_ASYNC( NetHandle, Sock ) )
	{
		Sock->HandleInfo.ASyncLoadResult = NetWorkRecv_Static( NetHandle, Buffer, Length, Peek, TRUE ) ;
	}

	DecASyncLoadCount( NetHandle ) ;
}
#endif // DX_NON_ASYNCLOAD

// NetWorkRecv のグローバルデータにアクセスしないバージョン
extern int NetWorkRecv_UseGParam(
	int NetHandle,
	void *Buffer,
	int Length,
	int Peek, 
	int ASyncLoadFlag
)
{
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, NetHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, Buffer ) ;
		AddASyncLoadParamInt( NULL, &Addr, Length ) ;
		AddASyncLoadParamInt( NULL, &Addr, Peek ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = NetWorkRecv_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, NetHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, Buffer ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Length ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Peek ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( NetHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = NetWorkRecv_Static( NetHandle, Buffer, Length, Peek, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// 受信したデータを読み込む
extern int NS_NetWorkRecv( int NetHandle, void *Buffer, int Length )
{
	return NetWorkRecv_UseGParam( NetHandle, Buffer, Length, FALSE, GetASyncLoadFlag() ) ;
/*
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// 取得するデータ長の値が不正な場合は何もせず終了
	if( Length < 0 ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( L"有効なネットワークハンドルではありません in NetWorkRecv\n" ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( L"このネットワークハンドルにはエラーが発生しています in NetWorkRecv\n" ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 内部受信データ取得処理を行う
	if( RecvSocket( NetHandle ) < 0 )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// ＤＸライブラリ独自の方式を使用するかどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		// WinSock から直接読み込む
		int RecvVol ;

		// 受信できる場合は受信、受信したデータ量を返す
		RecvVol = WinAPIData.WinSockFunc.recvFunc( Sock->Socket, (char *)Buffer, Length, 0 ) ;

		// サイズを返す
		ReturnValue = RecvVol ;
		goto FUNCTIONEND ;
	}
	else
	{
		// 受信したデータ容量が要求容量よりも少ない場合は失敗
		if( RingBufGetDataLength( &Sock->RecvBufferToUserR ) < Length )
		{
			ReturnValue = -1 ;
			goto FUNCTIONEND ;
		}

		// データの取得
		RingBufDataGet( &Sock->RecvBufferToUserR, Buffer, Length, FALSE ) ;
		Sock->RecvBufferToUserOpenSize -= Length ;

		// 内部受信データ取得処理を行う
		RecvSocket( NetHandle ) ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
*/
}

// 受信したデータを読み込む、読み込んだデータはバッファから削除されない
extern int NS_NetWorkRecvToPeek( int NetHandle , void *Buffer , int Length )
{
	return NetWorkRecv_UseGParam( NetHandle, Buffer, Length, TRUE, GetASyncLoadFlag() ) ;
/*
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( L"有効なネットワークハンドルではありません in NetWorkRecvToPeek\n" ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( L"このネットワークハンドルにはエラーが発生しています in NetWorkRecvToPeek\n" ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 内部受信データ取得処理を行う
	if( RecvSocket( NetHandle ) < 0 )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}
	
	// ＤＸライブラリ独自の方式かどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		int RecvVol ;

		// 受信できる場合は受信、受信したデータ量を返す
		RecvVol = WinAPIData.WinSockFunc.recvFunc( Sock->Socket, (char *)Buffer, Length, MSG_PEEK ) ;

		// サイズを返す
		ReturnValue = RecvVol ;
		goto FUNCTIONEND ;
	}
	else
	{
		// 受信したデータ容量が要求容量よりも少ない場合は失敗
		if( RingBufGetDataLength( &Sock->RecvBufferToUserR ) < Length )
		{
			ReturnValue = -1 ;
			goto FUNCTIONEND ;
		}

		// データの取得
		RingBufDataGet( &Sock->RecvBufferToUserR, Buffer, Length, TRUE ) ;

		// 内部受信データ取得処理を行う
		RecvSocket( NetHandle ) ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
*/
}

// 受信したデータをクリアする
extern int NS_NetWorkRecvBufferClear( int NetHandle )
{
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( TCPNETHCHK( NetHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x52\x00\x65\x00\x63\x00\x76\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x43\x00\x6c\x00\x65\x00\x61\x00\x72\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in NetWorkRecvBufferClear\n" @*/ ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x52\x00\x65\x00\x63\x00\x76\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x43\x00\x6c\x00\x65\x00\x61\x00\x72\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in NetWorkRecvBufferClear\n" @*/ ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 内部受信データ取得処理を行う
	if( RecvSocket( NetHandle ) < 0 )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}
	
	// ＤＸライブラリ独自の方式かどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		void *Buffer;

		// 一時的な受信バッファを確保する
		Buffer = DXALLOC( 256 * 1024 ) ;

		// 受信できたサイズがバッファより小さくなるまで繰り返す
		while( WinAPIData.WinSockFunc.recvFunc( Sock->Socket, (char *)Buffer, 256 * 1024, 0 ) < 256 * 1024 ){}

		// 一時的な受信バッファを開放する
		DXFREE( Buffer ) ;
	}
	else
	{
		// リングバッファを再初期化する
		RingBufTerminate( &Sock->RecvBufferToUserR );
		RingBufInitialize( &Sock->RecvBufferToUserR );
		Sock->RecvComDataOriginalVol = 0 ;
		Sock->RecvBufferToUserOpenSize = 0 ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}

// NetWorkSend の実処理関数
static int NetWorkSend_Static(
	int NetHandle,
	const void *Buffer,
	int Length,
	int ASyncThread
)
{
	int ReturnValue = 0 ;
	SOCKETDATA * Sock ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( ASyncThread )
	{
		if( TCPNETHCHK_ASYNC( NetHandle, Sock ) )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
			return -1 ;
		}
	}
	else
	{
		if( TCPNETHCHK( NetHandle, Sock ) )
		{
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
			return -1 ;
		}
	}

	// 通信関係処理
	NS_ProcessNetMessage() ;

	// 有効なソケットデータではなかったら何もせず終了
	if( Sock->UseFlag == FALSE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x09\x67\xb9\x52\x6a\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x0a\x00\x00"/*@ L"有効なネットワークハンドルではありません in NetWorkSend\n" @*/ ) ; 
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 重大なエラーが発生している場合は何もせず終了
	if( Sock->ErrorFlag == TRUE )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x53\x30\x6e\x30\xcd\x30\xc3\x30\xc8\x30\xef\x30\xfc\x30\xaf\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\x6f\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x66\x30\x44\x30\x7e\x30\x59\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x74\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x0a\x00\x00"/*@ L"このネットワークハンドルにはエラーが発生しています in NetWorkSend\n" @*/ ) ;
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}
	
	// 接続が断たれている場合も何もせず終了
	if( Sock->ConnectionFlag == FALSE )
	{
		ReturnValue = -1 ;
		goto FUNCTIONEND ;
	}

	// 内部送信プロセスを実行する
	SendSocket( NetHandle ) ;

	// ＤＸライブラリ独自の方式かどうかで処理を分岐
	if( Sock->DXProtocolFlag == FALSE )
	{
		int SendVol ;
	
		SendVol = WinAPIData.WinSockFunc.sendFunc( Sock->Socket, (char *)Buffer, Length, 0 ) ;

		// 終了
		ReturnValue = SendVol ;
		goto FUNCTIONEND ;
	}
	else
	{
		// 送信バッファにデータ追加
		if( RingBufDataAdd( &Sock->SendBufferR, Buffer, Length ) < 0 )
		{
			DXST_LOGFILEFMT_ADDUTF16LE(( "\x01\x90\xe1\x4f\xc7\x30\xfc\x30\xbf\x30\x20\x00\x25\x00\x64\x00\x20\x00\x62\x00\x79\x00\x74\x00\x65\x00\x20\x00\x6e\x30\xfd\x8f\xa0\x52\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x4e\x00\x65\x00\x77\x00\x57\x00\x6f\x00\x72\x00\x6b\x00\x53\x00\x65\x00\x6e\x00\x64\x00\x00"/*@ L"送信データ %d byte の追加に失敗しました in NewWorkSend" @*/, Length )) ;
			ReturnValue = -1 ;
			goto FUNCTIONEND ;
		}

		// 内部送信プロセスを実行する
		SendSocket( NetHandle ) ;
	}

FUNCTIONEND :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return ReturnValue ;
}

#ifndef DX_NON_ASYNCLOAD
// NetWorkSend の非同期読み込みスレッドから呼ばれる関数
static void NetWorkSend_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	SOCKETDATA * Sock ;
	int NetHandle ;
	void *Buffer ;
	int Length ;
	int Addr ;

	Addr = 0 ;
	NetHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Buffer = GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	Length = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	if( !NETHCHK_ASYNC( NetHandle, Sock ) )
	{
		Sock->HandleInfo.ASyncLoadResult = NetWorkSend_Static( NetHandle, Buffer, Length, TRUE ) ;
	}

	DecASyncLoadCount( NetHandle ) ;
}
#endif // DX_NON_ASYNCLOAD

// NetWorkSend のグローバルデータにアクセスしないバージョン
extern int NetWorkSend_UseGParam(
	int NetHandle,
	const void *Buffer,
	int Length,
	int ASyncLoadFlag
)
{
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, NetHandle ) ;
		AddASyncLoadParamStruct( NULL, &Addr, Buffer, Length ) ;
		AddASyncLoadParamInt( NULL, &Addr, Length ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = NetWorkSend_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, NetHandle ) ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, Buffer, Length ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Length ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( NetHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = NetWorkSend_Static( NetHandle, Buffer, Length, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// データを送信する
extern int NS_NetWorkSend( int NetHandle, const void *Buffer, int Length )
{
	return NetWorkSend_UseGParam( NetHandle, Buffer, Length, GetASyncLoadFlag() ) ;
}

// MakeUDPSocketBase の実処理関数
static int MakeUDPSocketBase_Static(
	int NetHandle,
	int IsIPv6,
	int RecvPort,
	int ASyncThread
)
{
	int ReturnValue = 0 ;
	SOCKETDATA *Sock ;
	unsigned int pt ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	if( ASyncThread )
	{
		if( NETHCHK_ASYNC( NetHandle, Sock) )
			return -1 ;
	}
	else
	{
		if( NETHCHK( NetHandle, Sock) )
			return -1 ;
	}

	// ポートの値を決定
	pt = ( unsigned int )( RecvPort == -1 ? -1 : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )RecvPort ) ) ;

	// データを初期化
	Sock->IsUDP					= TRUE ;							// ＵＤＰを使用する
	Sock->IsUDPBroadCast		= FALSE ;							// ブロードキャストは使用しない
	Sock->IsIPv6				= IsIPv6 ? TRUE : FALSE ;			// IPv6を使用するかどうかを保存する
	Sock->Port 					= pt ;								// 接続先のポートを保存
	Sock->UDPReadFlag			= FALSE ;							// 受信データが存在するか、フラグを倒す
	Sock->UDPWriteFlag			= TRUE ;							// 送信可能な状態か、フラグを立てる

	// ソケットの作成
	Sock->Socket = WinAPIData.WinSockFunc.socketFunc( IsIPv6 ? 23/*AF_INET6 は 23*/ : AF_INET , SOCK_DGRAM , IPPROTO_UDP ) ;
	if( Sock->Socket == INVALID_SOCKET )
	{
		DXERRORNETWORK(( "\x77\x00\x73\x00\x32\x00\x5f\x00\x33\x00\x32\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x4c\x30\xd5\x52\x5c\x4f\x57\x30\x66\x30\x44\x30\x7e\x30\x5b\x30\x93\x30\x01\x30\xbd\x30\xb1\x30\xc3\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x31\x00\x00"/*@ L"ws2_32.dllが動作していません、ソケットの作成に失敗しました_1" @*/ )) ;
		goto ERR ;
	}

	// RecvPort が -1 以外の場合は作成したソケットとポート番号の関連付けを行う
	if( RecvPort != -1 )
	{
		if( IsIPv6 )
		{
			_sockaddr_in6 local_v6;
		
			_MEMSET( &local_v6, 0, sizeof( local_v6 ) ) ;
			local_v6.sin6_family = 23/*AF_INET6 は 23*/;
			local_v6.sin6_port = ( unsigned short )pt ;

			if( WinAPIData.WinSockFunc.bindFunc( Sock->Socket , (struct sockaddr FAR *) &local_v6, sizeof( local_v6 ) ) == SOCKET_ERROR )
			{
				if( WinAPIData.WinSockFunc.WSAGetLastErrorFunc() != WSAEWOULDBLOCK )
				{
					DXERRORNETWORK(( "\x62\x00\x69\x00\x6e\x00\x64\x00\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"bind出来ません" @*/ )) ;
					goto ERR ;
				}
			}
		}
		else
		{
			SOCKADDR_IN	local;
		
			_MEMSET( &local, 0, sizeof( local ) ) ;
			local.sin_family = AF_INET;
			local.sin_port = ( u_short )pt ;
			local.sin_addr.s_addr = INADDR_ANY;

			if( WinAPIData.WinSockFunc.bindFunc( Sock->Socket , (struct sockaddr FAR *) &local, sizeof( local ) ) == SOCKET_ERROR )
			{
				if( WinAPIData.WinSockFunc.WSAGetLastErrorFunc() != WSAEWOULDBLOCK )
				{
					DXERRORNETWORK(( "\x62\x00\x69\x00\x6e\x00\x64\x00\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"bind出来ません" @*/ )) ;
					goto ERR ;
				}
			}
		}
	}

	// WinSock メッセージ受け取り設定
	if( WinAPIData.WinSockFunc.WSAAsyncSelectFunc(
			Sock->Socket,
			SockData.MessageWindow,
			WSA_WINSOCKMESSAGE,
			FD_WRITE | FD_READ ) == SOCKET_ERROR )
	{
		DXERRORNETWORK(( "\x57\x00\x53\x00\x41\x00\x41\x00\x73\x00\x79\x00\x6e\x00\x63\x00\x53\x00\x65\x00\x6c\x00\x65\x00\x63\x00\x74\x00\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"WSAAsyncSelectでエラーが発生しました" @*/ )) ;
		goto ERR ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return ReturnValue ;

	// エラー処理	
ERR :
	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// MakeUDPSocketBase の非同期読み込みスレッドから呼ばれる関数
static void MakeUDPSocketBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int NetHandle ;
	int IsIPv6 ;
	int RecvPort ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	NetHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	IsIPv6 = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	RecvPort = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = MakeUDPSocketBase_Static( NetHandle, IsIPv6, RecvPort, TRUE ) ;

	DecASyncLoadCount( NetHandle ) ;
	if( Result < 0 )
	{
		SubHandle( NetHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// MakeUDPSocketBase のグローバルデータにアクセスしないバージョン
extern int MakeUDPSocketBase_UseGParam(
	int IsIPv6,
	int RecvPort,
	int ASyncLoadFlag
)
{
	int NetHandle ;

	// 通信関係が初期化されていなかったら初期化
	if( SockData.InitializeFlag == FALSE )
	{
		if( WinData.CloseMessagePostFlag != TRUE )	InitializeNetWork() ;
		else										return -1 ;
	}
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ハンドルの作成
	NetHandle = AddHandle( DX_HANDLETYPE_NETWORK, FALSE, -1 ) ;
	if( NetHandle == -1 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, NetHandle ) ;
		AddASyncLoadParamInt( NULL, &Addr, IsIPv6 ) ;
		AddASyncLoadParamInt( NULL, &Addr, RecvPort ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = MakeUDPSocketBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, NetHandle ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, IsIPv6 ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, RecvPort ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( NetHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( MakeUDPSocketBase_Static( NetHandle, IsIPv6, RecvPort, FALSE ) < 0 )
			goto ERR ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return NetHandle ;

ERR :
	SubHandle( NetHandle ) ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return -1 ;
}

// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )
static int MakeUDPSocketBase( int IsIPv6, int RecvPort )
{
	return MakeUDPSocketBase_UseGParam( IsIPv6, RecvPort, GetASyncLoadFlag() ) ;
}

// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )
extern int NS_MakeUDPSocket( int RecvPort )
{
	return MakeUDPSocketBase( FALSE, RecvPort ) ;
}

// UDPを使用した通信を行うソケットハンドルを作成する( RecvPort を -1 にすると送信専用のソケットハンドルになります )( IPv6版 )
extern int NS_MakeUDPSocket_IPv6( int RecvPort )
{
	return MakeUDPSocketBase( TRUE, RecvPort ) ;
}

// UDPを使用した通信を行うソケットハンドルを削除する
extern int NS_DeleteUDPSocket( int NetUDPHandle )
{
	return SubHandle( NetUDPHandle ) ;
}

// NetWorkSendUDP の実処理関数
static int NetWorkSendUDP_Static(
	int NetUDPHandle,
	IPDATA SendIP_IPv4,
	IPDATA_IPv6 SendIP_IPv6,
	int SendPort,
	const void *Buffer,
	int Length,
	int ASyncThread
)
{
	SOCKETDATA * Sock = NULL ;
	int SendVol ;
	int Result ;
	int i ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( ( ASyncThread          && UDPNETHCHK_ASYNC( NetUDPHandle, Sock ) ) ||
		( ASyncThread == FALSE && UDPNETHCHK_ASYNC( NetUDPHandle, Sock ) ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}
	if( SendPort == -1 && Sock->Port == -1 )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データの送信
	if( Sock->IsIPv6 )
	{
		_sockaddr_in6 addr ;

		_MEMSET( &addr, 0, sizeof( addr ) ) ;
		addr.sin6_family = 23/*AF_INET6 は 23*/ ;
		for( i = 0 ; i < 8 ; i ++ )
			addr.sin6_addr.Word[ i ] = SendIP_IPv6.Word[ i ] ;
		addr.sin6_port = SendPort == -1 ? ( unsigned short )Sock->Port : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )SendPort ) ;
		SendVol = WinAPIData.WinSockFunc.sendtoFunc( Sock->Socket, (char *)Buffer, Length, 0, ( struct sockaddr FAR * )&addr, sizeof( addr ) ) ;
	}
	else
	{
		SOCKADDR_IN addr ;
		BOOL Enable ;

		// ブロードキャストだったら設定を切り替える
		if( SendIP_IPv4.d1 == 255 && SendIP_IPv4.d2 == 255 &&
			SendIP_IPv4.d3 == 255 && SendIP_IPv4.d4 == 255 )
		{
			// 既にブロードキャスト有効化設定を行っていた場合は何もしない
			if( Sock->IsUDPBroadCast == FALSE )
			{
				Sock->IsUDPBroadCast = TRUE ;
				Enable = 1 ;
				WinAPIData.WinSockFunc.setsockoptFunc( Sock->Socket, SOL_SOCKET, SO_BROADCAST, (char *)&Enable, sizeof( Enable ) ) ;
			}
		}
		else
		{
			// 既にブロードキャスト無効化設定を行っていた場合は何もしない
			if( Sock->IsUDPBroadCast == TRUE )
			{
				Sock->IsUDPBroadCast = FALSE ;
				Enable = 0 ;
				WinAPIData.WinSockFunc.setsockoptFunc( Sock->Socket, SOL_SOCKET, SO_BROADCAST, (char *)&Enable, sizeof( Enable ) ) ;
			}
		}

		_MEMSET( &addr, 0, sizeof( addr ) ) ;
		addr.sin_family = AF_INET ;
		addr.sin_addr = *(( in_addr *)&SendIP_IPv4 ) ;
		addr.sin_port = SendPort == -1 ? ( u_short )Sock->Port : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )SendPort ) ;
		SendVol = WinAPIData.WinSockFunc.sendtoFunc( Sock->Socket, (char *)Buffer, Length, 0, ( struct sockaddr FAR * )&addr, sizeof( addr ) ) ;
	}
	if( SendVol < 0 )
	{
		int hr ;
		hr = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
		switch( hr )
		{
		case WSAEMSGSIZE :
			Result = -2 ;
			goto ENDLABEL ;

		case WSAEWOULDBLOCK :
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

			NS_ProcessMessage() ;
			return -3 ;

		default :
			Result = -4 ;
			goto ENDLABEL ;
		}
	}
	else
	{
		Result = SendVol ;
	}

	// 送信可能フラグを倒す
	Sock->UDPWriteFlag = FALSE ;

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

#ifndef DX_NON_ASYNCLOAD
// NetWorkSendUDP の非同期読み込みスレッドから呼ばれる関数
static void NetWorkSendUDP_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	SOCKETDATA *Sock ;
	int NetHandle ;
	IPDATA *SendIP_IPv4 ;
	IPDATA_IPv6 *SendIP_IPv6 ;
	int SendPort ;
	const void *Buffer ;
	int Length ;
	int Addr ;

	Addr = 0 ;
	NetHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SendIP_IPv4 = ( IPDATA * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SendIP_IPv6 = ( IPDATA_IPv6 * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SendPort = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Buffer = GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	Length = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	if( !NETHCHK_ASYNC( NetHandle, Sock ) )
	{
		Sock->HandleInfo.ASyncLoadResult = NetWorkSendUDP_Static( NetHandle, *SendIP_IPv4, *SendIP_IPv6, SendPort, Buffer, Length, TRUE ) ;
	}

	DecASyncLoadCount( NetHandle ) ;
}
#endif // DX_NON_ASYNCLOAD

// NetWorkSendUDP のグローバルデータにアクセスしないバージョン
extern int NetWorkSendUDP_UseGParam(
	int NetUDPHandle,
	IPDATA SendIP_IPv4,
	IPDATA_IPv6 SendIP_IPv6,
	int SendPort,
	const void *Buffer,
	int Length,
	int ASyncLoadFlag
)
{
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, NetUDPHandle ) ;
		AddASyncLoadParamStruct( NULL, &Addr, &SendIP_IPv4, sizeof( IPDATA ) ) ;
		AddASyncLoadParamStruct( NULL, &Addr, &SendIP_IPv6, sizeof( IPDATA_IPv6 ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SendPort ) ;
		AddASyncLoadParamStruct( NULL, &Addr, Buffer, Length ) ;
		AddASyncLoadParamInt( NULL, &Addr, Length ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = NetWorkSendUDP_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, NetUDPHandle ) ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, &SendIP_IPv4, sizeof( IPDATA ) ) ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, &SendIP_IPv6, sizeof( IPDATA_IPv6 ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SendPort ) ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, Buffer, Length ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Length ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( NetUDPHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = NetWorkSendUDP_Static( NetUDPHandle, SendIP_IPv4, SendIP_IPv6, SendPort, Buffer, Length, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// UDPを使用した通信で指定のＩＰにデータを送信する、Length は最大65507、SendPort を -1 にすると MakeUDPSocket に RecvPort で渡したポートが使用されます( 戻り値  0以上;送信できたデータサイズ  -1:エラー   -2:送信データが大きすぎる  -3:送信準備ができていない )
extern int NS_NetWorkSendUDP( int NetUDPHandle, IPDATA SendIP, int SendPort, const void *Buffer, int Length )
{
	SOCKETDATA *Sock ;
	IPDATA_IPv6 SendIP_IPv6 ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
		Sock->IsIPv6 == TRUE )
		return -1 ;

	_MEMSET( &SendIP_IPv6, 0, sizeof( SendIP_IPv6 ) ) ;
	return NetWorkSendUDP_UseGParam( NetUDPHandle, SendIP, SendIP_IPv6, SendPort, Buffer, Length, GetASyncLoadFlag() ) ;
/*
	SOCKETDATA * Sock ;
	SOCKADDR_IN addr ;
	int SendVol ;
	int Result ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
	    ( SendPort == -1 && Sock->Port == -1 ) ||
	    Sock->IsIPv6 == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データの送信
	_MEMSET( &addr, 0, sizeof( addr ) ) ;
	addr.sin_family = AF_INET ;
	addr.sin_addr = *(( in_addr *)&SendIP ) ;
	addr.sin_port = SendPort == -1 ? Sock->Port : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )SendPort ) ;
	SendVol = WinAPIData.WinSockFunc.sendtoFunc( Sock->Socket, (char *)Buffer, Length, 0, ( struct sockaddr FAR * )&addr, sizeof( addr ) ) ;
	if( SendVol < 0 )
	{
		int hr ;
		hr = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
		switch( hr )
		{
		case WSAEMSGSIZE :
			Result = -2 ;
			goto ENDLABEL ;

		case WSAEWOULDBLOCK :
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

			NS_ProcessMessage() ;
			return -3 ;

		default :
			Result = -1 ;
			goto ENDLABEL ;
		}
	}
	else
	{
		Result = SendVol ;
	}

	// 送信可能フラグを倒す
	Sock->UDPWriteFlag = FALSE ;

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
*/
}

// UDPを使用した通信で指定のＩＰにデータを送信する、Length は最大65507、SendPort を -1 にすると MakeUDPSocket に RecvPort で渡したポートが使用されます( 戻り値  0以上;送信できたデータサイズ  -1:エラー   -2:送信データが大きすぎる  -3:送信準備ができていない )( IPv6版 )
extern int NS_NetWorkSendUDP_IPv6( int NetUDPHandle, IPDATA_IPv6 SendIP, int SendPort, const void *Buffer, int Length )
{
	SOCKETDATA *Sock ;
	IPDATA SendIP_IPv4 ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
		Sock->IsIPv6 == FALSE )
		return -1 ;

	_MEMSET( &SendIP_IPv4, 0, sizeof( SendIP_IPv4 ) ) ;
	return NetWorkSendUDP_UseGParam( NetUDPHandle, SendIP_IPv4, SendIP, SendPort, Buffer, Length, GetASyncLoadFlag() ) ;
/*
	SOCKETDATA * Sock ;
	_sockaddr_in6 addr ;
	int SendVol ;
	int Result ;
	int i ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
	    ( SendPort == -1 && Sock->Port == -1 ) ||
	    Sock->IsIPv6 == FALSE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データの送信
	_MEMSET( &addr, 0, sizeof( addr ) ) ;
	// AF_INET6 は 23
	addr.sin6_family = 23 ;
	for( i = 0 ; i < 8 ; i ++ )
		addr.sin6_addr.Word[ i ] = SendIP.Word[ i ] ;
	addr.sin6_port = SendPort == -1 ? Sock->Port : WinAPIData.WinSockFunc.htonsFunc( ( unsigned short )SendPort ) ;
	SendVol = WinAPIData.WinSockFunc.sendtoFunc( Sock->Socket, (char *)Buffer, Length, 0, ( struct sockaddr FAR * )&addr, sizeof( addr ) ) ;
	if( SendVol < 0 )
	{
		int hr ;
		hr = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
		switch( hr )
		{
		case WSAEMSGSIZE :
			Result = -2 ;
			goto ENDLABEL ;

		case WSAEWOULDBLOCK :
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

			NS_ProcessMessage() ;
			return -3 ;

		default :
			Result = -1 ;
			goto ENDLABEL ;
		}
	}
	else
	{
		Result = SendVol ;
	}

	// 送信可能フラグを倒す
	Sock->UDPWriteFlag = FALSE ;

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
*/
}

// NetWorkRecvUDP の実処理関数
static int NetWorkRecvUDP_Static(
	int NetUDPHandle,
	void *RecvIP,
	int *RecvPort,
	void *Buffer,
	int Length,
	int Peek,
	int ASyncThread
)
{
	SOCKETDATA *Sock = NULL ;
	SOCKADDR_IN addr_ipv4 = { 0 } ;
	_sockaddr_in6 addr_ipv6 = { 0 } ;
	int addrsize ;
	int RecvVol ;
	int Result ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( ( ASyncThread          && UDPNETHCHK_ASYNC( NetUDPHandle, Sock ) ) ||
		( ASyncThread == FALSE && UDPNETHCHK_ASYNC( NetUDPHandle, Sock ) ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データの受信
	if( Sock->IsIPv6 )
	{
		_MEMSET( &addr_ipv6, 0, sizeof( addr_ipv6 ) ) ;
		addrsize = sizeof( addr_ipv6 ) ;
		RecvVol = WinAPIData.WinSockFunc.recvfromFunc(
					Sock->Socket,
					(char *)Buffer,
					Length,
					Peek ? MSG_PEEK : 0,
					( struct sockaddr FAR * )&addr_ipv6,
					&addrsize ) ;
	}
	else
	{
		_MEMSET( &addr_ipv4, 0, sizeof( addr_ipv4 ) ) ;
		addrsize = sizeof( addr_ipv4 ) ;
		RecvVol = WinAPIData.WinSockFunc.recvfromFunc(
					Sock->Socket,
					(char *)Buffer,
					Length,
					Peek ? MSG_PEEK : 0,
					( struct sockaddr FAR * )&addr_ipv4,
					&addrsize ) ;
	}

	// 受信データありフラグを倒す
	Sock->UDPReadFlag = FALSE ;

	if( RecvVol < 0 )
	{
		int hr ;

		hr = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
		switch( hr )
		{
		case WSAEMSGSIZE :
			Result = -2 ;
			goto ENDLABEL ;

		case WSAEWOULDBLOCK :
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

			NS_ProcessMessage() ;
			return -3 ;

		default :
			Result = -1 ;
			goto ENDLABEL ;
		}
	}
	else
	{
		Result = RecvVol ;
	}

	if( RecvIP )
	{
		if( Sock->IsIPv6 )
		{
			int i ;
			for( i = 0 ; i < 8 ; i ++ )
				( ( IPDATA_IPv6 * )RecvIP )->Word[ i ] = addr_ipv6.sin6_addr.Word[ i ] ;
		}
		else
		{
			( ( IPDATA * )RecvIP )->d1 = addr_ipv4.sin_addr.S_un.S_un_b.s_b1;
			( ( IPDATA * )RecvIP )->d2 = addr_ipv4.sin_addr.S_un.S_un_b.s_b2;
			( ( IPDATA * )RecvIP )->d3 = addr_ipv4.sin_addr.S_un.S_un_b.s_b3;
			( ( IPDATA * )RecvIP )->d4 = addr_ipv4.sin_addr.S_un.S_un_b.s_b4;
		}
	}

	if( RecvPort )
	{
		if( Sock->IsIPv6 )
		{
			*RecvPort = WinAPIData.WinSockFunc.ntohsFunc( addr_ipv6.sin6_port ) ;
		}
		else
		{
			*RecvPort = WinAPIData.WinSockFunc.ntohsFunc( addr_ipv4.sin_port ) ;
		}
	}

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return Result ;
}

#ifndef DX_NON_ASYNCLOAD
// NetWorkRecvUDP の非同期読み込みスレッドから呼ばれる関数
static void NetWorkRecvUDP_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	SOCKETDATA *Sock ;
	int NetHandle ;
	void *RecvIP ;
	int *RecvPort ;
	void *Buffer ;
	int Length ;
	int Peek ;
	int Addr ;

	Addr = 0 ;
	NetHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	RecvIP = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	RecvPort = ( int * )GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	Buffer = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	Length = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Peek = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	if( !NETHCHK_ASYNC( NetHandle, Sock ) )
	{
		Sock->HandleInfo.ASyncLoadResult = NetWorkRecvUDP_Static( NetHandle, RecvIP, RecvPort, Buffer, Length, Peek, TRUE ) ;
	}

	DecASyncLoadCount( NetHandle ) ;
}
#endif // DX_NON_ASYNCLOAD

// NetWorkRecvUDP のグローバルデータにアクセスしないバージョン
extern int NetWorkRecvUDP_UseGParam(
	int NetUDPHandle,
	void *RecvIP,
	int *RecvPort,
	void *Buffer,
	int Length,
	int Peek,
	int ASyncLoadFlag
)
{
	int Result = -1 ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, NetUDPHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, RecvIP ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, RecvPort ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, Buffer ) ;
		AddASyncLoadParamInt( NULL, &Addr, Length ) ;
		AddASyncLoadParamInt( NULL, &Addr, Peek ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = NetWorkRecvUDP_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, NetUDPHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, RecvIP ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, RecvPort ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, Buffer ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Length ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Peek ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( NetUDPHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = NetWorkRecvUDP_Static( NetUDPHandle, RecvIP, RecvPort, Buffer, Length, Peek, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// 終了
	return Result ;
}

// UDPを使用した通信でデータを受信する、Peek に TRUE を渡すと受信に成功してもデータを受信キューから削除しません( 戻り値  0以上:受信したデータのサイズ  -1:エラー  -2:バッファのサイズが足りない  -3:受信データがない )
extern int NS_NetWorkRecvUDP( int NetUDPHandle, IPDATA *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek )
{
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
		Sock->IsIPv6 == TRUE )
		return -1 ;

	return NetWorkRecvUDP_UseGParam( NetUDPHandle, RecvIP, RecvPort, Buffer, Length, Peek, GetASyncLoadFlag() ) ;
/*
	SOCKETDATA * Sock ;
	SOCKADDR_IN addr ;
	int addrsize ;
	int RecvVol ;
	int Result ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
	    Sock->IsIPv6 == TRUE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データの受信
	_MEMSET( &addr, 0, sizeof( addr ) ) ;
	addrsize = sizeof( addr ) ;
	RecvVol = WinAPIData.WinSockFunc.recvfromFunc(
				Sock->Socket,
				(char *)Buffer,
				Length,
				Peek ? MSG_PEEK : 0,
				( struct sockaddr FAR * )&addr,
				&addrsize ) ;

	// 受信データありフラグを倒す
	Sock->UDPReadFlag = FALSE ;

	if( RecvVol < 0 )
	{
		int hr ;

		hr = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
		switch( hr )
		{
		case WSAEMSGSIZE :
			Result = -2 ;
			goto ENDLABEL ;

		case WSAEWOULDBLOCK :
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

			NS_ProcessMessage() ;
			return -3 ;

		default :
			Result = -1 ;
			goto ENDLABEL ;
		}
	}
	else
	{
		Result = RecvVol ;
	}

	if( RecvIP )
	{
		RecvIP->d1 = addr.sin_addr.S_un.S_un_b.s_b1;
		RecvIP->d2 = addr.sin_addr.S_un.S_un_b.s_b2;
		RecvIP->d3 = addr.sin_addr.S_un.S_un_b.s_b3;
		RecvIP->d4 = addr.sin_addr.S_un.S_un_b.s_b4;
	}

	if( RecvPort )
	{
		*RecvPort = WinAPIData.WinSockFunc.ntohsFunc( addr.sin_port ) ;
	}

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return Result ;
*/
}

// UDPを使用した通信でデータを受信する、Peek に TRUE を渡すと受信に成功してもデータを受信キューから削除しません( 戻り値  0以上:受信したデータのサイズ  -1:エラー  -2:バッファのサイズが足りない  -3:受信データがない )( IPv6版 )
extern int NS_NetWorkRecvUDP_IPv6( int NetUDPHandle, IPDATA_IPv6 *RecvIP, int *RecvPort, void *Buffer, int Length, int Peek )
{
	SOCKETDATA * Sock ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
		Sock->IsIPv6 == FALSE )
		return -1 ;

	return NetWorkRecvUDP_UseGParam( NetUDPHandle, RecvIP, RecvPort, Buffer, Length, Peek, GetASyncLoadFlag() ) ;
/*
	SOCKETDATA * Sock ;
	_sockaddr_in6 addr ;
	int addrsize ;
	int RecvVol ;
	int Result ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) ||
	    Sock->IsIPv6 == FALSE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データの受信
	_MEMSET( &addr, 0, sizeof( addr ) ) ;
	addrsize = sizeof( addr ) ;
	RecvVol = WinAPIData.WinSockFunc.recvfromFunc(
				Sock->Socket,
				(char *)Buffer,
				Length,
				Peek ? MSG_PEEK : 0,
				( struct sockaddr FAR * )&addr,
				&addrsize ) ;

	// 受信データありフラグを倒す
	Sock->UDPReadFlag = FALSE ;

	if( RecvVol < 0 )
	{
		int hr ;

		hr = WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
		switch( hr )
		{
		case WSAEMSGSIZE :
			Result = -2 ;
			goto ENDLABEL ;

		case WSAEWOULDBLOCK :
			// クリティカルセクションの解放
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

			NS_ProcessMessage() ;
			return -3 ;

		default :
			Result = -1 ;
			goto ENDLABEL ;
		}
	}
	else
	{
		Result = RecvVol ;
	}

	if( RecvIP )
	{
		int i ;
		for( i = 0 ; i < 8 ; i ++ )
			RecvIP->Word[ i ] = addr.sin6_addr.Word[ i ] ;
	}

	if( RecvPort )
	{
		*RecvPort = WinAPIData.WinSockFunc.ntohsFunc( addr.sin6_port ) ;
	}

ENDLABEL :

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	return Result ;
*/
}

// UDPを使用した通信でデータが送信できる状態かどうかを調べる( 戻り値  -1:エラー  TRUE:送信可能  FALSE:送信不可能 )
extern int NS_CheckNetWorkSendUDP( int NetUDPHandle )
{
	SOCKETDATA * Sock ;
	int Result ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// データ送信可能フラグをセット
	Result = Sock->UDPWriteFlag ;

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// データ送信可能フラグを返す
	return Result ;
}

// UDPを使用した通信で新たな受信データが存在するかどうかを調べる( 戻り値  -1:エラー  TRUE:受信データあり  FALSE:受信データなし )
extern int NS_CheckNetWorkRecvUDP( int NetUDPHandle )
{
	SOCKETDATA * Sock ;
	int addrsize ;
	int RecvVol ;
	BYTE TempBuffer[ 16 ] ;

	// 通信関係の初期化がされていなかったら何もせず終了
	if( SockData.InitializeFlag == FALSE ) return -1 ;

	SETUP_WIN_API

	// クリティカルセクションの取得
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	// ネットワークハンドルのチェック
	if( UDPNETHCHK( NetUDPHandle, Sock ) )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return -1 ;
	}

	// 受信データがある場合は TRUE を返す
	if( Sock->UDPReadFlag )
	{
		Sock->UDPReadFlag = FALSE ;

		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return TRUE ;
	}

	// データの受信チェック
	if( Sock->IsIPv6 )
	{
		_sockaddr_in6 addr_v6 ;
		_MEMSET( &addr_v6, 0, sizeof( addr_v6 ) ) ;
		addrsize = sizeof( addr_v6 ) ;
		RecvVol = WinAPIData.WinSockFunc.recvfromFunc(
					Sock->Socket,
					(char *)TempBuffer,
					sizeof( TempBuffer ),
					MSG_PEEK,
					( struct sockaddr FAR * )&addr_v6,
					&addrsize ) ;
	}
	else
	{
		SOCKADDR_IN addr ;
		_MEMSET( &addr, 0, sizeof( addr ) ) ;
		addrsize = sizeof( addr ) ;
		RecvVol = WinAPIData.WinSockFunc.recvfromFunc(
					Sock->Socket,
					(char *)TempBuffer,
					sizeof( TempBuffer ),
					MSG_PEEK,
					( struct sockaddr FAR * )&addr,
					&addrsize ) ;
	}

	if( RecvVol >= 0 || WinAPIData.WinSockFunc.WSAGetLastErrorFunc() == WSAEMSGSIZE )
	{
		// クリティカルセクションの解放
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;
		return TRUE ;
	}

	// クリティカルセクションの解放
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_NETWORK ].CriticalSection ) ;

	NS_ProcessMessage() ;

	// 終了
	return FALSE ;
}











// WinSock で最後に発生したエラーのコードを取得する
extern int NS_GetWinSockLastError( void )
{
	SETUP_WIN_API

	return WinAPIData.WinSockFunc.WSAGetLastErrorFunc() ;
}













// ProcessNetMessage をひたすら呼び続けるスレッド
DWORD WINAPI ProcessNetMessageThreadFunction( LPVOID )
{
	for(;;)
	{
		if( SockData.ProcessNetMessageThreadEndRequest != FALSE ) break ;
		if( SockData.InitializeFlag == FALSE ) break ;

		// ネットメッセージプロセス処理
		NS_ProcessNetMessage() ;

		// 待ち
		Thread_Sleep( 8 ) ;
	}

	SETUP_WIN_API

	// スレッド終了
	SockData.ProcessNetMessageThreadExitFlag = TRUE ;
	WinAPIData.Win32Func.ExitThreadFunc( 0 ) ;

	return 0 ;
}









#if 0

// HTTP 関係の処理の初期化を行う
static int HTTP_Initialize( void )
{
	// 零初期化
	_MEMSET( &HttpData, 0, sizeof( HttpData ) ) ;
	
	// 終了
	return 0 ;
}

// HTTP 関係の処理の後始末を行う
static int HTTP_Terminate( void )
{
	// 全てのハンドルを解放
	NS_HTTP_CloseAll() ;
	
	// 終了
	return 0 ;
}

// HTTP メッセージの結果を得る
static int HTTP_GetResult( const char *ResStr )
{
	const char *p ;

	p = _STRCHR( ResStr, ' ' ) ;
	if( p == NULL ) return -1 ;

	return _ATOI( p ) ;
}

// HTTP を使用してネットワーク上のファイルをダウンロードする
extern int NS_HTTP_FileDownload( const char *FileURL, const char *SavePath, void **SaveBufferP,
										int *FileSize, char **ParamList )
{
	int DHandle = -1, BackUp = -1, Screen = -1, Font = -1, DrawScreen, BlendMode, BlendPal ;
	int width, rate, ScW, ScH, CX, CY ;
	int StartTime ;
	char FileName[64] ;
	const int CommentY = -32, MeterX = -15, MeterY = 32, MeterH = 8, MeterW = 110, SpeedX = 30, SpeedY = 8 ;
	const int DownSizeX = 10, DownSizeY = 8, SecY = -10, FontSize = 10, BlackW = 320, BlackH = 120 ;
	const char *Message1 = "\x90\xda\x91\xb1\x92\x86\x82\xc5\x82\xb7"/*@ "接続中です" @*/ ;
	const char *Message2 = "%s \x82\xf0\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x92\x86\x82\xc5\x82\xb7"/*@ "%s をダウンロード中です" @*/ ;
	const char *Message3 = "\x82\xa0\x82\xc6 %s"/*@ "あと %s" @*/ ;
	const char *Message4 = "%s %cbyte/sec" ;
	const char *Message5 = "%s %cbyte/%s %cbyte" ;

	SETUP_WIN_API

	// パスの指定がない場合はカレントフォルダに格納する	
	if( SaveBufferP == NULL && SavePath == NULL ) SavePath = "" ;
	
	// ファイル名を得る
	NS_URLAnalys( FileURL, NULL, NULL, FileName, NULL ) ;
//	ErrorLogFmtAdd( "FileName:%s\n", FileName ) ;

	// 描画先を表画面にする
	DrawScreen = GetActiveGraph() ;
	SetDrawScreen( DX_SCREEN_FRONT ) ;

	// 描画ブレンドモードを保存
	GetDrawBlendMode( &BlendMode, &BlendPal ) ;

	// メッセージを描画する為のフォントを作成
	{
		int FontTexFlag = GetUseSystemMemGraphCreateFlag() ;
		int CacheNum = GetFontCacheCharNum() ;
		SetFontCacheToTextureFlag( FALSE ) ;
		SetFontCacheCharNum( 60 ) ;
		Font = CreateFontToHandle( NULL, FontSize, -1, DX_FONTTYPE_ANTIALIASING ) ;
		if( Font < 0 ) goto ERR ;
		SetFontCacheCharNum( CacheNum ) ;
		SetFontCacheToTextureFlag( FontTexFlag ) ;
	}

	// 描画用画面の作成と画面のコピーを取る
	{
		int UseSystemMem ;

		GetDrawScreenSize( &ScW, &ScH ) ;
		UseSystemMem = GetUseSystemMemGraphCreateFlag() ;
		SetUseSystemMemGraphCreateFlag( TRUE ) ;
		BackUp = MakeGraph( BlackW, BlackH, TRUE ) ;
		if( BackUp < 0 ) goto ERR ;
		Screen = MakeGraph( BlackW, BlackH, TRUE ) ;
		if( Screen < 0 ) goto ERR ;
		GetDrawScreenGraph( ScW / 2 - BlackW / 2, ScH / 2 - BlackH / 2,
							ScW / 2 + BlackW / 2, ScH / 2 + BlackH / 2, BackUp ) ;
		SetUseSystemMemGraphCreateFlag( UseSystemMem ) ;

		CX = BlackW / 2 ;
		CY = BlackH / 2 ;
	}

	// 接続待ち表示
	if( GetActiveFlag() == TRUE )
	{
		SetDrawScreen( Screen ) ;
		DrawBox( 0, 0, BlackW, BlackH, GetColor( 0,0,0 ), TRUE ) ;

		width = GetDrawStringWidthToHandle( Message1, WinAPIData.Win32Func.lstrlenAFunc( Message1 ), Font ) ;
		DrawStringToHandle( CX - width / 2, CY + CommentY, Message1, GetColor( 255,255,255 ), Font ) ;

		SetDrawScreen( DX_SCREEN_FRONT ) ;
		DrawGraph( ScW / 2 - BlackW / 2, ScH / 2 - BlackH / 2, Screen, FALSE ) ;
	}

	// ダウンロード処理開始
	DHandle = HTTP_StartFileDownload( FileURL, SavePath, SaveBufferP, ParamList ) ;
	if( DHandle == -1 )
	{
		ErrorLogFmtAdd( "\x83\x74\x83\x40\x83\x43\x83\x8b %s \x82\xcc\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "ファイル %s のダウンロードに失敗しました" @*/, FileName ) ;
		goto ERR ;
	}

	// 速度計算の準備
	StartTime = GetNowCount() ;

	// ダウンロード完了まで待つ
	while( ProcessMessage() == 0 && HTTP_GetState( DHandle ) == HTTP_RES_NOW )
	{
		// 垂直同期待ち
		WaitVSync(1) ;

		// ダウンロード完了率の表示
		if( ProcessMessage() == 0 && GetActiveFlag() == TRUE && HTTP_GetDownloadedFileSize( DHandle ) != -1 )
		{
			SetDrawScreen( Screen ) ;
			DrawBox( 0, 0, BlackW, BlackH, GetColor( 0,0,0 ), TRUE ) ;

			// 説明の表示
			width = GetDrawFormatStringWidthToHandle( Font, Message2, FileName ) ;
			DrawFormatStringToHandle( CX - width / 2, CY + CommentY, GetColor( 255,255,255 ), Font, Message2, FileName ) ;

			// ダウンロード情報を表示
			if( GetNowCount() - StartTime > 1000 )
			{
				int RecvSize, FileSize, LapsTime ;
				char tempstr[128], timestr[32], numstr1[32], numstr2[32], unit1, unit2 ;
			
				RecvSize = HTTP_GetDownloadedFileSize( DHandle ) ;
				FileSize = HTTP_GetDownloadFileSize( DHandle ) ;
				LapsTime = GetNowCount() - StartTime ;

				// ダウンロードメーターの描画
				{
					rate = _DTOL( (double)HTTP_GetDownloadedFileSize( DHandle ) * MeterW / HTTP_GetDownloadFileSize( DHandle ) ) ;
					DrawBox( CX - MeterW / 2 + MeterX, CY + MeterY, CX - MeterW / 2 + MeterX + rate, CY + MeterY + MeterH, GetColor( 255,0,0 ), TRUE ) ;
					DrawBox( CX - MeterW / 2 + MeterX + rate, CY + MeterY, CX + MeterX - MeterW / 2 + MeterW, CY + MeterY + MeterH, GetColor( 80,80,80 ), TRUE ) ;
					_SNPRINTF( tempstr, sizeof( tempstr ), "%.1f%%", ( double )HTTP_GetDownloadedFileSize( DHandle ) * 100 / HTTP_GetDownloadFileSize( DHandle ) ) ;
					if( _STRCMP( tempstr, "-0.0" ) == 0 ) _STRCPY( tempstr, "0.0" ) ;
					DrawStringToHandle( CX - MeterW / 2 + MeterW, CY + MeterY + MeterH - FontSize, tempstr, GetColor( 255,255,255 ), Font ) ;
				}

				// 残り時間の描画
				if( RecvSize != 0 )
				{
					GetTimeLengthString( _DTOL( ( double )( FileSize - RecvSize ) / RecvSize * LapsTime ), timestr ) ;
					_SNPRINTF( tempstr, sizeof( tempstr ), Message3, timestr ) ;
					width = GetDrawStringWidthToHandle( tempstr, WinAPIData.Win32Func.lstrlenAFunc( tempstr ), Font )  ;
					DrawStringToHandle( CX - width / 2, CY + SecY, tempstr, GetColor( 255,255,255 ), Font ) ;
				}
				
				// ダウンロード速度の描画
				{
					unit1 = GetDataSizeString( _DTOL( ( double )RecvSize / ( ( double )LapsTime / 1000.0 ) ), numstr1 ) ;
					_SNPRINTF( tempstr, sizeof( tempstr ), Message4, numstr1, unit1 ) ;
					width = GetDrawStringWidthToHandle( tempstr, WinAPIData.Win32Func.lstrlenAFunc( tempstr ), Font ) ;
					DrawStringToHandle( CX + SpeedX, CY + SpeedY, tempstr, GetColor( 255,255,255 ), Font ) ;
				}
				
				// ダウンロード完了サイズの描画
				{
					unit1 = GetDataSizeString( RecvSize, numstr1 ) ;
					unit2 = GetDataSizeString( FileSize, numstr2 ) ;
					_SNPRINTF( tempstr, sizeof( tempstr ), Message5, numstr1, unit1, numstr2, unit2 ) ;
					width = GetDrawStringWidthToHandle( tempstr, WinAPIData.Win32Func.lstrlenAFunc( tempstr ), Font ) ;
					DrawStringToHandle( CX - width + DownSizeX, CY + DownSizeY, tempstr, GetColor( 255,255,255 ), Font ) ;
				}
			}
			
			// 表画面に反映
			SetDrawScreen( DX_SCREEN_FRONT ) ;
			DrawGraph( ScW / 2 - BlackW / 2, ScH / 2 - BlackH / 2, Screen, FALSE ) ;
		}
	}
	if( HTTP_GetState( DHandle ) != HTTP_RES_COMPLETE )
	{
		switch( HTTP_GetState( DHandle ) )
		{
		case HTTP_RES_STOP : ErrorLogFmtAdd( "\x83\x74\x83\x40\x83\x43\x83\x8b %s \x82\xcc\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x82\xcd\x92\x86\x8e\x7e\x82\xb3\x82\xea\x82\xdc\x82\xb5\x82\xbd"/*@ "ファイル %s のダウンロードは中止されました" @*/, FileName ) ; break ;
		case HTTP_RES_ERROR : ErrorLogFmtAdd( "\x83\x74\x83\x40\x83\x43\x83\x8b %s \x82\xcc\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x82\xc5\x83\x47\x83\x89\x81\x5b\x82\xaa\x94\xad\x90\xb6\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "ファイル %s のダウンロードでエラーが発生しました" @*/, FileName ) ; break ;
		}
		goto ERR ;
	}
	
	// ファイルサイズを得る
	if( FileSize != NULL ) *FileSize = HTTP_GetDownloadFileSize( DHandle ) ;

	// HTTP ハンドルを閉じる
	HTTP_Close( DHandle ) ;

	// フォントハンドルの削除
	DeleteFontToHandle( Font ) ;

	// 色々元に戻す
	if( ProcessMessage() == 0 && GetActiveFlag() == TRUE )
	{
		// バックアップしていた画像を復元
		SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 ) ;
		DrawGraph( ScW / 2 - BlackW / 2, ScH / 2 - BlackH / 2, BackUp, FALSE ) ;
		DeleteGraph( BackUp ) ;
		
		// 描画先に使用していた画面を削除
		DeleteGraph( Screen ) ;

		SetDrawScreen( DrawScreen ) ;				// 描画先を元に戻す
		SetDrawBlendMode( BlendMode, BlendPal ) ;	// 描画ブレンドモードを元に戻す
	}
	
	
	// 終了
	return 0 ;

	// エラー処理
ERR :
	if( BackUp != -1 )
	{
		SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 ) ;
		if( ProcessMessage() == 0 && GetActiveFlag() == TRUE ) DrawGraph( ScW / 2 - BlackW / 2, ScH / 2 - BlackH / 2, BackUp, FALSE ) ;
		DeleteGraph( BackUp ) ;
	}
	if( Screen != -1 ) DeleteGraph( Screen ) ;
	SetDrawScreen( DrawScreen ) ;
	SetDrawBlendMode( BlendMode, BlendPal ) ;

	if( DHandle != -1 ) HTTP_Close( DHandle ) ;
	if( Font != -1 ) DeleteFontToHandle( Font ) ;

	return -1 ;
}

// HTTP を使用してネットワーク上のファイルのサイズを得る
extern int NS_HTTP_GetFileSize( const char *FileURL )
{
	int DHandle, FileSize ;

	// ファイルサイズ取得開始
	DHandle = HTTP_StartGetFileSize( FileURL ) ;
	if( DHandle == -1 ) return -1 ;
	
	// ダウンロード完了まで待つ
	while( ProcessMessage() == 0 && HTTP_GetState( DHandle ) == HTTP_RES_NOW ){}
	if( HTTP_GetState( DHandle ) != HTTP_RES_COMPLETE )
	{
		// 失敗したら此処で終了
		HTTP_Close( DHandle ) ;
		return -1 ;
	}
	
	// ファイルサイズを得る
	FileSize = HTTP_GetDownloadFileSize( DHandle ) ;

	// HTTP ハンドルを閉じる
	HTTP_Close( DHandle ) ;
	
	// ファイルサイズを返す
	return FileSize ;
}

// 指定のＵＲＬからホストパス、ホストのアクセスパス、アクセスポートを取得する
static int HTTP_GetConnectInfo( const char *URL, int UseProxy, char *HostBuf,
								char *PathBuf, char *FileNameBuf, int *PortBuf )
{
	SETUP_WIN_API

	// ＩＥのプロキシを使わない場合と使う場合で処理を分岐
	HostBuf[0] = '\0' ;
	if( UseProxy == TRUE )
	{
		char temppath[128] ;
		int tempport ;

		// ＵＲＬを解析
		if( NS_URLAnalys( URL, HostBuf, temppath, FileNameBuf, &tempport ) == -1 ) return -1 ;

		// 使えない記号を使える記号に変換
		NS_URLConvert( temppath, FALSE ) ;

		// パスを構築
		_STRCPY( PathBuf, "http://" ) ;
		_STRCAT( PathBuf, HostBuf ) ;
		if( tempport != -1 ) _SNPRINTF( PathBuf + WinAPIData.Win32Func.lstrlenAFunc( PathBuf ), sizeof( PathBuf ) - WinAPIData.Win32Func.lstrlenAFunc( PathBuf ), ":%d", tempport ) ;
		_STRCAT( PathBuf, temppath ) ;
		
		// プロキシの情報を得る
		_STRCPY( HostBuf, HttpData.ProxyHost ) ;
		*PortBuf = HttpData.ProxyPort ;
	}

	// プロキシが有効ではなかった場合
	if( HostBuf[0] == '\0' )
	{
		// ＵＲＬを解析
		if( NS_URLAnalys( URL, HostBuf, PathBuf, FileNameBuf, PortBuf ) == -1 ) return -1 ;
		if( *PortBuf == -1 ) *PortBuf = 80 ;

		// 使えない記号を使える記号に変換
		NS_URLConvert( PathBuf, FALSE ) ;
	}
	
	// 終了
	return 0 ;
}

// 指定の HTTP ホストに接続する( 戻り値: 0以上 = ネットワークハンドル  -1 = エラー )
static int HTTP_ConnectHost( const char *URL, int UseProxy, IPDATA *HostIPBuf,
							 char *HostBuf, char *PathBuf, char *FileNameBuf, int *PortBuf )
{
	char Host[128], Path[128], FileName[128] ;
	int Port, NetHandle ;
	IPDATA HostIP ;

	// 接続先ホスト、ホストへのアクセスパス、アクセスポートを得る
	if( HTTP_GetConnectInfo( URL, UseProxy, Host, Path, FileName, &Port ) < 0 )
	{
		DXST_LOGFILEFMT_ADDA(( "\x82\x74\x82\x71\x82\x6b %s \x82\xcc\x89\xf0\x90\xcd\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd\n"/*@ "ＵＲＬ %s の解析に失敗しました\n" @*/, URL )) ;
		return -1 ;
	}

	// 指定のホストが存在するか得る
	if( NS_GetHostIPbyName( Host, &HostIP ) == -1 )
	{
		DXST_LOGFILEFMT_ADDA(( "\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x90\xe6\x83\x7a\x83\x58\x83\x67 %s \x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd"/*@ "ダウンロード先ホスト %s が見つかりませんでした" @*/, Host )) ;
		return -1 ;
	}

	// ホストに接続する
	{
		int DXProtocol, CloseAfterLost ;

		// フラグを退避		
		DXProtocol = NS_GetUseDXNetWorkProtocol() ;
		CloseAfterLost = NS_GetNetWorkCloseAfterLostFlag() ;

		// フラグをセット
		NS_SetUseDXNetWorkProtocol( FALSE ) ;
		NS_SetNetWorkCloseAfterLostFlag( FALSE )  ;

		// 接続
		NetHandle = NS_ConnectNetWork( HostIP, Port ) ;

		// フラグを復帰
		NS_SetUseDXNetWorkProtocol( DXProtocol ) ;
		NS_SetNetWorkCloseAfterLostFlag( CloseAfterLost )  ;
		
		// 失敗の判定
		if( NetHandle == -1 )
		{
			DXST_LOGFILEFMT_ADDA(( "\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x90\xe6\x83\x7a\x83\x58\x83\x67 %s \x82\xd6\x82\xcc\x90\xda\x91\xb1\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd"/*@ "ダウンロード先ホスト %s への接続に失敗しました" @*/, Host )) ;
			return -1 ;
		}
	}
	
	// バッファのポインタが渡されている場合情報をコピーする
	if( HostBuf != NULL ) _STRCPY( HostBuf, Host ) ;
	if( PathBuf != NULL ) _STRCPY( PathBuf, Path ) ;
	if( FileNameBuf != NULL ) _STRCPY( FileNameBuf, FileName ) ;
	if( PortBuf != NULL ) *PortBuf = Port ;
	if( HostIPBuf != NULL ) *HostIPBuf = HostIP ;

	// ネットハンドルを返す
	return NetHandle ;
}

// 新しくHTTPハンドルを追加する( 戻り値: -1 = エラー  0以上 = HTTPハンドル )
static	int			HTTP_AddHandle( void )
{
	int NewHandle ;
	HTTPDATA *http ;

	// 既にHTTPハンドルが一杯の場合はエラー
	if( HttpData.Num == MAX_HTTPHANDLE_NUM )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x0c\x54\x42\x66\x6b\x30\xc0\x30\xa6\x30\xf3\x30\xed\x30\xfc\x30\xc9\x30\x57\x30\x66\x30\x44\x30\x8b\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4c\x30\x50\x96\x4c\x75\x6b\x30\x54\x90\x57\x30\x66\x30\x44\x30\x5f\x30\x5f\x30\x81\x30\xc0\x30\xa6\x30\xf3\x30\xed\x30\xfc\x30\xc9\x30\x92\x30\x8b\x95\xcb\x59\x67\x30\x4d\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"同時にダウンロードしているファイルが限界に達していたためダウンロードを開始できませんでした\n" @*/ ) ;
		return -1 ;
	}
	
	// 使われていないHTTPハンドルを探す
	for( NewHandle = 0 ; HttpData.Data[NewHandle] != NULL ; NewHandle ++ ){}

	// データを格納するメモリ領域を確保
	HttpData.Data[NewHandle] = http = ( HTTPDATA * )DXALLOC( sizeof( HTTPDATA ) ) ;
	if( http == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xc0\x30\xa6\x30\xf3\x30\xed\x30\xfc\x30\xc9\x30\xe6\x51\x06\x74\x28\x75\xc7\x30\xfc\x30\xbf\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ダウンロード処理用データ領域の確保に失敗しました\n" @*/ ) ;
		return -1 ;
	}
	
	// 零初期化
	_MEMSET( http, 0, sizeof( HTTPDATA ) ) ;

	// ハンドルの数を増やす
	HttpData.Num ++ ;

	// ハンドルを返す
	return NewHandle ;
}

// HTTPハンドルの後始末を行う
static	int			HTTP_DelHandle( int HttpHandle )
{
	HTTPDATA *http = HttpData.Data[HttpHandle] ;
	
	// 既に解放されていたら何もせず終了
	if( http == NULL ) return 0 ;
	
	// メモリの解放
	DXFREE( http ) ;
	HttpData.Data[HttpHandle] = NULL ;
	
	// ハンドルの数を減らす
	HttpData.Num -- ;
	
	// 終了
	return 0 ;
}

// HTTP を使用したネットワーク上のファイルをダウンロードする処理を開始する
extern int NS_HTTP_StartFileDownload( const char *FileURL, const char *SavePath, void **SaveBufferP, char **ParamList )
{
	HTTPDATA *http = NULL ;
	int NewHandle = -1 ;

	SETUP_WIN_API

	// 保存先の指定が不正な場合はエラー
	if( SavePath == NULL && SaveBufferP == NULL )
	{
		DXST_LOGFILE_ADDUTF16LE( "\xdd\x4f\x58\x5b\x48\x51\x6e\x30\x07\x63\x9a\x5b\x4c\x30\x0d\x4e\x63\x6b\x6a\x30\x5f\x30\x81\x30\xc0\x30\xa6\x30\xf3\x30\xed\x30\xfc\x30\xc9\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"保存先の指定が不正なためダウンロード出来ません\n" @*/ ) ;
		return -1 ;
	}

	// HTTP 処理用ハンドルを追加する
	NewHandle = HTTP_AddHandle() ;
	if( NewHandle < 0 ) goto ERR ;
	http = HttpData.Data[NewHandle] ;

	// ホストに接続する
	http->NetHandle = HTTP_ConnectHost( FileURL, HttpData.UseProxy,
								&http->HostIP, http->Host, http->Path, http->FileName, &http->Port ) ;
	if( http->NetHandle < 0 )
	{
		DXST_LOGFILE_ADDUTF16LE( "\x48\x00\x54\x00\x54\x00\x50\x00\x20\x00\xdb\x30\xb9\x30\xc8\x30\x78\x30\x6e\x30\xa5\x63\x9a\x7d\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"HTTP ホストへの接続に失敗しました\n" @*/ ) ;
		goto ERR ;
	}

	// パラメータを解析する
	if( ParamList != NULL )
	{
		http->ParamLength = NS_URLParamAnalysis( ParamList, &http->Param ) ;
		if( http->ParamLength < 0 )
		{
			DXST_LOGFILE_ADDUTF16LE( "\x48\x00\x54\x00\x54\x00\x50\x00\x20\x00\x6e\x30\xd1\x30\xe9\x30\xe1\x30\xfc\x30\xbf\x30\xe3\x89\x90\x67\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"HTTP のパラメータ解析に失敗しました\n" @*/ ) ;
			goto ERR ;
		}
	}
	else
	{
		http->Param = NULL ;
		http->ParamLength = 0 ;
	}

	// データの初期化
	http->Process 		= http->Param != NULL ? HTTP_PR_POST : HTTP_PR_GET ;
	http->State 		= HTTP_ST_CONNECT ;
	http->Step			= 0 ;
	http->Counter		= 0 ;
	http->RecvFileSize 	= 0 ;
	http->FileSize 		= -1 ;
	http->Result		= HTTP_RES_NOW ;
	http->Error			= HTTP_ERR_NONE ;

	// 保存先情報をセットアップ
	if( SaveBufferP != NULL )
	{
		// メモリに格納
		http->SavePlace = HTTP_SV_MEM ;
		http->BufferPoint = SaveBufferP ;
		*http->BufferPoint = NULL ;
	}
	else
	{
		// ファイルに保存
#ifdef UNICODE
		WinAPIData.Win32Func.DeleteFileWFunc( http->FileName ) ;
		http->FilePoint = WinAPIData.Win32Func.CreateFileWFunc( http->FileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;
#else
		WinAPIData.Win32Func.DeleteFileAFunc( http->FileName ) ;
		http->FilePoint = WinAPIData.Win32Func.CreateFileAFunc( http->FileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;
#endif
		if( http->FilePoint == NULL )
		{
			DXST_LOGFILEFMT_ADDA(( "\x83\x5f\x83\x45\x83\x93\x83\x8d\x81\x5b\x83\x68\x82\xb5\x82\xbd\x83\x74\x83\x40\x83\x43\x83\x8b\x82\xf0\x95\xdb\x91\xb6\x82\xb7\x82\xe9\x83\x74\x83\x40\x83\x43\x83\x8b %s \x82\xf0\x8a\x4a\x82\xaf\x82\xdc\x82\xb9\x82\xf1\x82\xc5\x82\xb5\x82\xbd\n"/*@ "ダウンロードしたファイルを保存するファイル %s を開けませんでした\n" @*/, http->FileName )) ;
			goto ERR ;
		}
	}

	// HTTP 処理を一回分行う
	if( HTTP_Process( NewHandle ) == -1 )
	{
		NS_HTTP_Close( NewHandle ) ;
		return -1 ;
	}

	// 終了
	return NewHandle ;

	// エラー処理	
ERR :
	if( NewHandle != -1 )
	{
		if( http->Param != NULL ) DXFREE( http->Param ) ;
		if( http->NetHandle != -1 )
		{
			NS_CloseNetWork( http->NetHandle ) ;
		}
		if( http->FilePoint != NULL ) WinAPIData.Win32Func.CloseHandleFunc( http->FilePoint ) ;
		HTTP_DelHandle( NewHandle ) ;
	}

	return -1 ; 
}

// HTTP を使用したネットワーク上のファイルのサイズを得る処理を開始する
extern int NS_HTTP_StartGetFileSize( const char *FileURL )
{
	HTTPDATA *http = NULL ;
	int NewHandle = -1 ;

	// HTTP 処理用ハンドルを追加する
	NewHandle = HTTP_AddHandle() ;
	if( NewHandle < 0 ) goto ERR ;
	http = HttpData.Data[NewHandle] ;

	// ホストに接続する
	http->NetHandle = HTTP_ConnectHost( FileURL, HttpData.UseProxy,
								&http->HostIP, http->Host, http->Path, http->FileName, &http->Port ) ;
	if( http->NetHandle < 0 ) goto ERR ;
	
	// データの初期化
	http->Process 		= HTTP_PR_HEAD ;
	http->State 		= HTTP_ST_CONNECT ;
	http->Step			= 0 ;
	http->Counter		= 0 ;
	http->FileSize 		= -1 ;
	http->Result		= HTTP_RES_NOW ;
	http->Param			= NULL ;
	http->Error			= HTTP_ERR_NONE ;

	// HTTP 処理を一回分行う
	if( HTTP_Process( NewHandle ) == -1 )
	{
		NS_HTTP_Close( NewHandle ) ;
		return -1 ;
	}

	// 終了
	return NewHandle ;

	// エラー処理	
ERR :
	if( NewHandle != -1 )
	{
		if( http->Param != NULL ) DXFREE( http->Param ) ;
		if( http->NetHandle != -1 )
		{
			NS_CloseNetWork( http->NetHandle ) ;
		}
		HTTP_DelHandle( NewHandle ) ;
	}

	return -1 ; 
}

// HTTP の処理に使用しているリソースを解放する
static int HTTP_ReleaseResource( int HttpHandle, int Result, int Error )
{
	HTTPDATA *http = HttpData.Data[HttpHandle] ;

	SETUP_WIN_API

	if( http == NULL ) return -1 ;

	// ネットハンドルを閉じる
	if( http->NetHandle != -1 )
	{
		NS_CloseNetWork( http->NetHandle ) ;
	}
	http->NetHandle = -1 ;
	
	// POST メソッド用の長い文字列のために文字列を確保していた場合は解放する
	if( http->Param != NULL ) DXFREE( http->Param ) ;
	http->Param = NULL ;

	// ファイル書き出しだった場合、ファイルハンドルを解放する
	if( http->FilePoint != NULL ) WinAPIData.Win32Func.CloseHandleFunc( http->FilePoint ) ;
	http->FilePoint = NULL ;

	// 処理結果をセットする
	if( http->Result == -1 ) http->Result = Result ;
	if( http->Error == -1 ) http->Error = Error ;

	// メモリ読み込みだった場合で読み込みが完了していない場合、確保したメモリを解放する
	if( http->BufferPoint != NULL && http->Result != HTTP_RES_COMPLETE )
	{
		DXFREE( *http->BufferPoint ) ;
		*http->BufferPoint = NULL ;
	}

	// 終了
	return 0 ;
}

// HTTP の処理を終了し、ハンドルを解放する
extern int NS_HTTP_Close( int HttpHandle )
{
	HTTPDATA *http ;
	
	if( HttpHandle >= MAX_HTTPHANDLE_NUM ) return -1 ;
	http = HttpData.Data[HttpHandle] ;
	if( http == NULL ) return -1 ;

	// HTTP の処理に使用したリソースを解放する
	HTTP_ReleaseResource( HttpHandle, HTTP_RES_COMPLETE ) ;

	// HTTP ハンドルの解放
	HTTP_DelHandle( HttpHandle ) ;

	// 終了
	return 0 ;	
}

// 全てのハンドルに対して HTTP_Close を行う
extern int NS_HTTP_CloseAll( void )
{
	HTTPDATA **http ;
	int Num, i, j ;

	// HTTP ハンドルが一つも無かったら何もしない
	if( HttpData.Num == 0 ) return 0 ;

	// 全ての HTTP ハンドルを解放する
	http = HttpData.Data ;
	Num = HttpData.Num ;
	for( i = 0, j = 0 ; i < Num ; j ++ )
	{
		if( http[j] == NULL ) continue ;
		NS_HTTP_Close( j ) ;
		i ++ ;
	}

	// 終了
	return 0 ;
}

// HTTP の処理を行う
static int HTTP_Process( int HttpHandle )
{
	HTTPDATA *http = HttpData.Data[HttpHandle] ;
	int res1, res2 ;
	
	if( http == NULL ) return -1 ;
	
	// HTTP の処理が何らかの形で終了していたら何もせず終了
	if( http->Result != HTTP_RES_NOW ) return 0;

	// 切断されていたら中止
	res1 = NS_GetNetWorkAcceptState( http->NetHandle ) ;
	res2 = NS_GetNetWorkDataLength( http->NetHandle ) ;
	if( res1 == 0 && res2 == 0 )
	{
		HTTP_ReleaseResource( HttpHandle, HTTP_RES_ERROR, HTTP_ERR_LOST ) ;
		return 0 ;
	}
	
	// 行う処理によって処理を分岐
	switch( http->Process )
	{
	case HTTP_PR_GET :	// GET メソッドの処理
		return HTTP_ProcessForGET( HttpHandle ) ;
		
	case HTTP_PR_POST :	// POST メソッドの処理
		return HTTP_ProcessForPOST( HttpHandle ) ;
		
	case HTTP_PR_HEAD :	// HEAD メソッドの処理
		return HTTP_ProcessForHEAD( HttpHandle ) ;
	}

	// 終了
	return 0 ;
}

// HTTP_Proess を全てのハンドルに対して行う
static int HTTP_ProcessAll( void )
{
	HTTPDATA **http ;
	int i, j, Num ;

	// HTTP の処理が何も行われていなかったら何もしない
	if( HttpData.Num == 0 ) return 0 ;

	// 全ての HTTP ハンドルを処理する
	http = HttpData.Data ;
	Num = HttpData.Num ;
	for( i = 0, j = 0 ; i < Num ; j ++ )
	{
		if( http[j] == NULL ) continue ;
		HTTP_Process( j ) ;
		i ++ ;
	}

	// 終了
	return 0 ;
}

// HTTP の GET メソッドの処理をする
static	int			HTTP_ProcessForGET( int HttpHandle )
{
	HTTPDATA *http = HttpData.Data[HttpHandle] ;
	char str[512]/*, buf[128]*/ ;
	int i ;

	SETUP_WIN_API

	if( http == NULL ) return -1 ;
	
	// 状態によって処理を分岐
	switch( http->Step ) 
	{
	case 0 :		// GET メッセージをホストに送る
		_SNPRINTF( str, sizeof( str ), "GET %s HTTP/1.0\r\nHost: %s:%d\r\nConnection: close\r\n\r\n", http->Path, http->Host, http->Port ) ;
		NS_NetWorkSend( http->NetHandle, str, WinAPIData.Win32Func.lstrlenAFunc( str ) ) ;
		
		http->Step ++ ;

		
	case 1 :		// 接続に成功しなかったら終了
		if( NS_fgetsForNetHandle( http->NetHandle, str ) == -1 ) break ;
//		sscanf( str, "%s%d", buf, &i ) ;
		i = HTTP_GetResult( str ) ;
		if( i != 200 )
		{
			int err ;
		
			switch( i )
			{
			case 404 : err = HTTP_ERR_NOTFOUND ; break ;
			case 500 : err = HTTP_ERR_SERVER ; break ;
			default : err = HTTP_ERR_SERVER ; break ;
			}
			
			DXST_LOGFILEFMT_ADDA(( "\x90\xda\x91\xb1\x82\xc9\x8e\xb8\x94\x73\x82\xb5\x82\xdc\x82\xb5\x82\xbd (\x96\xdf\x82\xe8\x92\x6c:%d)"/*@ "接続に失敗しました (戻り値:%d)" @*/, i )) ;
			
			HTTP_ReleaseResource( HttpHandle, HTTP_RES_ERROR, err ) ;
			break ;
		}

		http->Step ++ ;
		
	case 2 :		// ヘッダを解析する
		if( NS_fgetsForNetHandle( http->NetHandle, str ) == -1 ) break ;

		// ヘッダ終了だったら次へ
		if( _STRCHR( str, ':' ) == NULL )
		{
			http->Step ++ ;
		}
		else
		{
			// ファイルサイズの情報だったらサイズを取得する
//			sscanf( str, "%s%d", buf, &i ) ;
			i = HTTP_GetResult( str ) ;
//			if( lstrcmp( buf, "Content-Length:" ) != 0 ) break ;
			if( _STRNCMP( str, "Content-Length:", WinAPIData.Win32Func.lstrlenAFunc( "Content-Length:" ) ) != 0 ) break ;
			http->FileSize = i ;

			// メモリに読み込む場合はメモリの確保を行う
			if( http->SavePlace == HTTP_SV_MEM )
			{
				*http->BufferPoint = DXALLOC( http->FileSize ) ;
				if( *http->BufferPoint == NULL )
				{
					HTTP_ReleaseResource( HttpHandle, HTTP_RES_ERROR, HTTP_ERR_MEMORY ) ;
					break ;
				}
			}
			break ;
		}

	case 3 :		// データを取得する
		{
			int GetSize ;
			int res1, res2 ;
			
	GETLOOP :

			// 保存先によって処理を分岐
			switch( http->SavePlace )
			{
			case HTTP_SV_FILE :		// ファイルの場合
				{
					static char buf[4096] ;
					DWORD WriteSize ;
					
					// データを取得
					GetSize = NS_NetWorkRecv( http->NetHandle, buf, 4096 ) ;
					if( GetSize < 0 ) break ;
					
					// データの書き出し
					WinAPIData.Win32Func.WriteFileFunc( http->FilePoint, buf, GetSize, &WriteSize, NULL ) ;
				}
				break ;
				
			case HTTP_SV_MEM :		// メモリの場合
				{
					// データを取得
					GetSize = NS_NetWorkRecv( http->NetHandle, ( char * )*http->BufferPoint + http->RecvFileSize, http->FileSize - http->RecvFileSize ) ;
					if( GetSize < 0 ) break ;
				}
				break ;
			}
			if( GetSize < 0 ) break ;
			
			// 取得分を加算
			http->RecvFileSize += GetSize ;

			// ダウンロード終了検知
			res1 = NS_GetNetWorkAcceptState( http->NetHandle ) ;
			res2 = NS_GetNetWorkDataLength( http->NetHandle ) ;
			if( ( http->FileSize == -1 && ( res1 != 0 || res2 != 0 ) ) || 
				( http->FileSize != -1 && http->RecvFileSize < http->FileSize ) )
			{
				if( GetSize != 0 ) goto GETLOOP ;
				break ;
			}
		}
		http->Step ++ ;

	case 4 :	// 終了
		HTTP_ReleaseResource( HttpHandle, HTTP_RES_COMPLETE ) ;
		break ;
	}
	
	// 終了
	return 0 ;
}

// HTTP の POST メソッドの処理をする
static	int			HTTP_ProcessForPOST( int HttpHandle )
{
	HTTPDATA *http = HttpData.Data[HttpHandle] ;
	char str[256]/*, buf[128]*/ ;
	int i ;
	
	SETUP_WIN_API

	if( http == NULL ) return -1 ;
	
	// 状態によって処理を分岐
	switch( http->Step ) 
	{
	case 0 :		// POST メッセージをホストに送る

		_SNPRINTF( str, sizeof( str ), "POST %s HTTP/1.0\nHost: %s:%d\nContent-Length: %d\n\n", http->Path, http->Host, http->Port, http->ParamLength ) ;
		NS_NetWorkSend( http->NetHandle, str, WinAPIData.Win32Func.lstrlenAFunc( str ) ) ;
		NS_NetWorkSend( http->NetHandle, http->Param, http->ParamLength ) ;

//		NS_NetWorkSend( http->NetHandle, "\n", 1 ) ;
/*
		_SNPRINTF( str, sizeof( str ), "GET %s?%s HTTP/1.0\nHost: %s:%d\n\n", http->Path, http->Param, http->Host, http->Port ) ;
		DXST_LOGFILE_ADDA( str ) ;
		NS_NetWorkSend( http->NetHandle, str, WinAPIData.Win32Func.lstrlenAFunc( str ) ) ;
*/		
		http->Step ++ ;

		
	case 1 :		// 接続に成功しなかったら終了
		if( NS_fgetsForNetHandle( http->NetHandle, str ) == -1 ) break ;
//		sscanf( str, "%s%d", buf, &i ) ;
		i = HTTP_GetResult( str ) ;
		if( i != 200 )
		{
			int err ;
		
			switch( i )
			{
			case 404 : err = HTTP_ERR_NOTFOUND ; break ;
			case 500 : err = HTTP_ERR_SERVER ; break ;
			default : err = HTTP_ERR_SERVER ; break ;
			}
			HTTP_ReleaseResource( HttpHandle, HTTP_RES_ERROR, err ) ;
			break ;
		}

		http->Step ++ ;
		
	case 2 :		// ヘッダを解析する
		if( NS_fgetsForNetHandle( http->NetHandle, str ) == -1 ) break ;

		// ヘッダ終了だったら次へ
		if( _STRCHR( str, ':' ) == NULL )
		{
			http->Step ++ ;
		}
		else
		{
			// ファイルサイズの情報だったらサイズを取得する
//			sscanf( str, "%s%d", buf, &i ) ;
			i = HTTP_GetResult( str ) ;
//			if( lstrcmp( buf, "Content-Length:" ) != 0 ) break ;
			if( _STRNCMP( str, "Content-Length:", WinAPIData.Win32Func.lstrlenAFunc( "Content-Length:" ) ) != 0 ) break ;
			http->FileSize = i ;

			// メモリに読み込む場合はメモリの確保を行う
			if( http->SavePlace == HTTP_SV_MEM )
			{
				*http->BufferPoint = DXALLOC( http->FileSize ) ;
				if( *http->BufferPoint == NULL )
				{
					HTTP_ReleaseResource( HttpHandle, HTTP_RES_ERROR, HTTP_ERR_MEMORY ) ;
					break ;
				}
			}
			break ;
		}

	case 3 :		// データを取得する
		{
			int GetSize ;
			int res1, res2 ;
			
	GETLOOP :

			// 保存先によって処理を分岐
			switch( http->SavePlace )
			{
			case HTTP_SV_FILE :		// ファイルの場合
				{
					static char buf[4096] ;
					DWORD WriteSize ;
					
					// データを取得
					GetSize = NS_NetWorkRecv( http->NetHandle, buf, 4096 ) ;
					if( GetSize < 0 ) break ;
					
					// データの書き出し
					WinAPIData.Win32Func.WriteFileFunc( http->FilePoint, buf, GetSize, &WriteSize, NULL ) ;
				}
				break ;
				
			case HTTP_SV_MEM :		// メモリの場合
				{
					// データを取得
					GetSize = NS_NetWorkRecv( http->NetHandle, ( char * )*http->BufferPoint + http->RecvFileSize, http->FileSize - http->RecvFileSize ) ;
					if( GetSize < 0 ) break ;
				}
				break ;
			}
			if( GetSize < 0 ) break ;
			
			// 取得分を加算
			http->RecvFileSize += GetSize ;

			// ダウンロード終了検知
			res1 = NS_GetNetWorkAcceptState( http->NetHandle ) ;
			res2 = NS_GetNetWorkDataLength( http->NetHandle ) ;
			if( ( http->FileSize == -1 && ( res1 != 0 || res2 != 0 ) ) || 
				( http->FileSize != -1 && http->RecvFileSize < http->FileSize ) )
			{
				if( http->FileSize != -1 && http->RecvFileSize < http->FileSize && GetSize != 0 ) goto GETLOOP ;
				break ;
			}
		}
		http->Step ++ ;

	case 4 :	// 終了
		HTTP_ReleaseResource( HttpHandle, HTTP_RES_COMPLETE ) ;
		break ;
	}
	
	// 終了
	return 0 ;
}

// HTTP の HEAD メソッドの処理をする
static	int			HTTP_ProcessForHEAD( int HttpHandle )
{
	HTTPDATA *http = HttpData.Data[HttpHandle] ;
	char str[256]/*, buf[128]*/ ;
	int i ;
	
	SETUP_WIN_API

	if( http == NULL ) return -1 ;

	// 状態によって処理を分岐
	switch( http->Step ) 
	{
	case 0 :		// HEAD メッセージをホストに送る
		_SNPRINTF( str, sizeof( str ), "HEAD %s HTTP/1.0\n\n", http->Path ) ;
		NS_NetWorkSend( http->NetHandle, str, WinAPIData.Win32Func.lstrlenAFunc( str ) ) ;
		
		http->Step ++ ;
		
	case 1 :		// 接続に成功しなかったら終了
		if( NS_fgetsForNetHandle( http->NetHandle, str ) == -1 ) break ;
		i = HTTP_GetResult( str ) ;
//		sscanf( str, "%s%d", buf, &i ) ;
		if( i != 200 )
		{
			int err ;
		
			switch( i )
			{
			case 404 : err = HTTP_ERR_NOTFOUND ; break ;
			case 500 : err = HTTP_ERR_SERVER ; break ;
			default : err = HTTP_ERR_SERVER ; break ;
			}
			HTTP_ReleaseResource( HttpHandle, HTTP_RES_ERROR, err ) ;
			break ;
		}

		http->Step ++ ;
		
	case 2 :		// ヘッダを解析する
		// 文字列が来ていなかったら何もせず終了
		if( NS_fgetsForNetHandle( http->NetHandle, str ) == -1 ) break ;

		// ファイルサイズの情報だったらサイズを取得する
//		sscanf( str, "%s%d", buf, &i ) ;
//		if( lstrcmp( buf, "Content-Length:" ) != 0 ) break ;
		i = HTTP_GetResult( str ) ;
		if( _STRNCMP( str, "Content-Length:", WinAPIData.Win32Func.lstrlenAFunc( "Content-Length:" ) ) != 0 ) break ;
		http->FileSize = i ;

		// ファイルサイズの取得が出来たらプロセス完了
		http->Step ++ ;

	case 3 :	// 終了
		HTTP_ReleaseResource( HttpHandle, HTTP_RES_COMPLETE ) ;
		break ;
	}
	
	// 終了
	return 0 ;
}

// HTTP 処理の現在の状態を得る( HTTP_RES_COMPLETE 等 )
extern int NS_HTTP_GetState( int HttpHandle )
{
	HTTPDATA *http ;
	
	if( HttpHandle >= MAX_HTTPHANDLE_NUM ) return -1 ;
	http = HttpData.Data[HttpHandle] ;
	if( http == NULL ) return -1 ;

	// 状態を返す
	return http->Result ;
}

// HTTP 処理でエラーが発生した場合、エラーの内容を得る( HTTP_ERR_NONE 等 )
extern int NS_HTTP_GetError( int HttpHandle )
{
	HTTPDATA *http ;
	
	if( HttpHandle >= MAX_HTTPHANDLE_NUM ) return -1 ;
	http = HttpData.Data[HttpHandle] ;
	if( http == NULL ) return -1 ;

	// エラー状態を返す
	return http->Error ;
}


// HTTP 処理で対象となっているファイルのサイズを得る( 戻り値: -1 = エラー・若しくはまだファイルのサイズを取得していない  0以上 = ファイルのサイズ )
extern int NS_HTTP_GetDownloadFileSize( int HttpHandle )
{
	HTTPDATA *http ;
	
	if( HttpHandle >= MAX_HTTPHANDLE_NUM ) return -1 ;
	http = HttpData.Data[HttpHandle] ;
	if( http == NULL ) return -1 ;

	return http->FileSize ;
}

// HTTP 処理で既にダウンロードしたファイルのサイズを取得する
extern int NS_HTTP_GetDownloadedFileSize( int HttpHandle ) 
{
	HTTPDATA *http ;
	
	if( HttpHandle >= MAX_HTTPHANDLE_NUM ) return -1 ;
	http = HttpData.Data[HttpHandle] ;
	if( http == NULL ) return -1 ;

	return http->RecvFileSize ;
}

// fgets のネットワークハンドル版( -1:取得できず 0:取得できた )
extern int NS_fgetsForNetHandle( int NetHandle, char *strbuffer )
{
	char str[256], *p ;
	int length ;

	// 現在取得しているデータを取得する
	_MEMSET( str, 0, 255 ) ;
	length = NS_NetWorkRecvToPeek( NetHandle, str, 255 ) ;
	if( length <= 0 ) return -1 ;
	str[length] = '\0' ;
	
	// 改行文字が含まれていなかったら何もせず終了
	p = _STRCHR( str, '\n' ) ;
	if( p == NULL ) return -1 ;
	
	// 含まれていたらそこまでの文字列を返す
	*p = '\0' ;
	_STRCPY( strbuffer, str ) ;
	
	// 取得した分だけバッファからデータを消す
	NS_NetWorkRecv( NetHandle, str, ( p - str ) + 1 ) ;

	// 終了
	return 0 ;
}

// ＵＲＬを解析する
extern int NS_URLAnalys( const char *URL, char *HostBuf, char *PathBuf, char *FileNameBuf, int *PortBuf )
{
	char tempHost[128], tempPath[128], tempFileName[128] ;
	int tempPort ;
	char *hp, *p, *pt ;
	
	if( HostBuf == NULL ) HostBuf = tempHost ;
	if( PathBuf == NULL ) PathBuf = tempPath ;
	if( FileNameBuf == NULL ) FileNameBuf = tempFileName ;
	if( PortBuf == NULL ) PortBuf = &tempPort ;
	
	// プロトコル名以降の部分を取得する
	// なかったらエラー
	hp = _STRSTR( ( char * )URL, "//" ) ;
	if( hp == NULL ) return -1 ;
	hp += 2 ;
	
	// パスの先頭を取得する
	p = _STRCHR( hp, '/' ) ;
	if( p == NULL )
	{
		// パスがない場合の処理
		_STRCPY( HostBuf, hp ) ;
		_STRCPY( PathBuf, "/" ) ;
	}
	else
	{
		// パスがある場合の処理
		_MEMCPY( HostBuf, hp, p - hp ) ;
		HostBuf[p - hp] = '\0' ;
		_STRCPY( PathBuf, p ) ;
	}
	
	// ポートの取得
	pt = _STRCHR( HostBuf, ':' ) ;
	if( pt == NULL )
	{
		*PortBuf = -1 ;
	}
	else
	{
		*PortBuf = _ATOI( pt + 1 ) ;
		if( *PortBuf <= 0 ) *PortBuf = -1 ;
		*pt = '\0' ;
	}

	// ファイル名の取得
	{
		char *p ;

		p = _STRRCHR( PathBuf, '/' ) ;
		if( p[1] == '\0' )
		{
			_STRCPY( FileNameBuf, "index.htm" ) ;
		}
		else
		{
			_STRCPY( FileNameBuf, p + 1 ) ;
		}
	}

	// 正常終了
	return 0 ;
}


// HTTP に渡せない記号が使われた文字列を渡せるような文字列に変換する
extern int NS_URLConvert( char *URL, int ParamConvert, int NonConvert )
{
	int len, count ;
	char *Number = "0123456789ABCDEF" ;
	unsigned char code ;

	SETUP_WIN_API

	count = 0 ;
	len = WinAPIData.Win32Func.lstrlenAFunc( URL ) + 1 ;
	while( *URL != '\0' )
	{
		code = (unsigned char)*URL ;
		if( code == ' ' )
		{
			if( NonConvert == FALSE ) *URL = '+' ;
			URL ++ ;
			count ++ ;
		}
		else
		if( ( ( code < 'a' || code > 'z' ) && ( code < 'A' || code > 'Z' ) && ( code < '0' || code > '9' ) &&
			  code != '.' && code != '_' && code != '-' && code != '*' && code != '/' && code != ':' && code != '%' ) || 
			( ParamConvert == TRUE && ( code == '/' || code == ':' || code == '%' ) ) )
		{
			if( NonConvert == FALSE )
			{
				if( ParamConvert == FALSE && /*_mbclen( (unsigned char *)URL ) == 2*/ CheckMultiByteChar( *URL, _GET_CHARSET() ) == TRUE )
				{
					_MEMMOVE( URL + 6, URL + 2, len - 2 ) ;
					code = URL[0] ;
					URL[0] = '%' ;
					URL[1] = Number[code >> 4] ;
					URL[2] = Number[code & 0xf] ;
					
					URL += 3 ;
					
					code = URL[0] ;
					URL[0] = '%' ;
					URL[1] = Number[code >> 4] ;
					URL[2] = Number[code & 0xf] ;
					
					URL += 3 ;
				}
				else
				{
					_MEMMOVE( URL + 3, URL + 1, len - 1 ) ;
					code = URL[0] ;
					URL[0] = '%' ;
					URL[1] = Number[code >> 4] ;
					URL[2] = Number[code & 0xf] ;
					
					URL += 3 ;
				}
			}
			else
			{
				URL ++ ;
			}

			count += 3 ;
		}
		else
		{
			URL ++ ;
			count ++ ;
		}
		len -- ;
	}
	
	// 終了
	return count ;
}

// HTTP 用パラメータリストから一つのパラメータ文字列を作成する( 戻り値:  -1 = エラー  0以上 = パラメータの文字列の長さ )
extern int NS_URLParamAnalysis( char **ParamList, char **ParamStringP )
{
	int i, len ;
	char *ParamString, *p ;

	if( ParamList == NULL ) return -1 ;

	// 全てのパラメータを変換した後の文字列長を得る
	len = 0 ;
	for( i = 0 ; ParamList[i] != NULL ; i ++ )
	{
		p = _STRCHR( ParamList[i], '=' ) ;
		len += p - ParamList[i] + 1 ;
		if( p == NULL ) return -1 ;
		len += NS_URLConvert( p + 1, TRUE, TRUE ) + 1 ;
	}
	len -- ;

	// パラメータが一つもなかった場合の処理
	if( len == 0 )
	{
		if( ParamStringP != NULL ) *ParamStringP = NULL ;
		return 0 ;
	}

	// メモリの確保
	ParamString = ( char * )DXALLOC( len + 1 ) ;
	if( ParamString == NULL ) return -1 ;
	_MEMSET( ParamString, 0, len + 1 ) ;

	// 全てのパラメータを変換した文字列を得る
	p = ParamString ;
	for( i = 0 ; ParamList[i] != NULL ; i ++ )
	{
		_STRCPY( p, ParamList[i] ) ;
		p = _STRCHR( p, '=' ) + 1 ;
		p += NS_URLConvert( p, TRUE, FALSE ) ;
		*p = '&' ;
		p ++ ;
	}
	p -- ;
	*p = '\0' ;

	// ポインタを保存
	if( ParamStringP != NULL ) *ParamStringP = ParamString ;

	// 終了
	return len ;
}


// サイズに見合った単位で表した文字列とその単位を表す一文字を返す
static	char		GetDataSizeString( int ByteSize, char *SizeString )
{
	const int KByte = 1024, MByte = KByte * 1024, GByte = MByte * 1024 ;

	if( ( double )ByteSize > GByte * 0.8 )
	{
		_SPRINTF( SizeString, "%.1f", ( double )ByteSize / GByte ) ;
		return 'G' ;
	}
	else
	if( ( double )ByteSize > MByte * 0.8 )
	{
		_SPRINTF( SizeString, "%.1f", ( double )ByteSize / MByte ) ;
		return 'M' ;
	}
	else
	{
		_SPRINTF( SizeString, "%.1f", ( double )ByteSize / KByte ) ;
		if( _STRCMP( SizeString, "-0.0" ) == 0 ) _STRCPY( SizeString, "0.0" ) ;
		return 'k' ;
	}

	// 此処に来ることは無い	
	return 0 ;
}

// ミリ秒の時間長から時間長に合った単位の文字列に変換する
static	int			GetTimeLengthString( int MillSec, char *LengthString )
{
	int Sec = 1000, Min = Sec * 60, Hour = Min * 60, Day = Hour * 24 ;
	int DayFlag = FALSE, HourFlag = FALSE, MinFlag = FALSE ;
	
	SETUP_WIN_API

	LengthString[0] = '\0' ;
	if( MillSec >= Day )
	{
		_SPRINTF( LengthString, "%d\x93\xfa "/*@ "%d日 " @*/, MillSec / Day ) ;
		MillSec %= Day ;
		LengthString += WinAPIData.Win32Func.lstrlenAFunc( LengthString ) ;
		
		DayFlag = TRUE ;
	}

	if( MillSec >= Hour )
	{
		_SPRINTF( LengthString, "%d\x8e\x9e\x8a\xd4 "/*@ "%d時間 " @*/, MillSec / Hour ) ;
		MillSec %= Hour ;
		LengthString += WinAPIData.Win32Func.lstrlenAFunc( LengthString ) ;
		
		HourFlag = TRUE ;
	}

	if( MillSec >= Min && DayFlag == FALSE )
	{
		_SPRINTF( LengthString, "%d\x95\xaa "/*@ "%d分 " @*/, MillSec / Min ) ;
		MillSec %= Hour ;
		LengthString += WinAPIData.Win32Func.lstrlenAFunc( LengthString ) ;
		
		MinFlag = TRUE ;
	}

	if( DayFlag == FALSE && HourFlag == FALSE )
	{
		_SPRINTF( LengthString, "%d\x95\x62"/*@ "%d秒" @*/, MillSec / Sec ) ;
		MillSec %= Sec ;
		LengthString += WinAPIData.Win32Func.lstrlenAFunc( LengthString ) ;
	}
	
	// 終了
	return 0 ;
}

#endif

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_NON_NETWORK
