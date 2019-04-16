// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WinAPIプログラム
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxWinAPI.h"
#include <tchar.h>
#include "../DxASyncLoad.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

WINAPIDATA WinAPIData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// プログラム --------------------------------------------------------------------

// WindowsOS の DLL を読み込む
extern int LoadWinAPI( void )
{
	// 初期化済みの場合は何もしない
	if( WinAPIData.InitializeFlag )
	{
		return 0 ;
	}

#ifndef DX_NON_NETWORK
	// ws2_32.dll をロードする
	if( WinAPIData.WinSockFunc.WinSockDLL == NULL )
	{
		WinAPIData.WinSockFunc.WinSockDLL = LoadLibraryW( L"ws2_32.dll" ) ;
		WinAPIData.WinSockFunc.WSAGetLastErrorFunc = ( int ( WINAPI * )( void ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "WSAGetLastError" ) ;
		WinAPIData.WinSockFunc.WSAStartupFunc = ( int ( WINAPI * )( WORD , LPWSADATA  ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "WSAStartup" ) ;
		WinAPIData.WinSockFunc.WSACleanupFunc = ( int ( WINAPI * )( void ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "WSACleanup" ) ;
		WinAPIData.WinSockFunc.WSAAsyncSelectFunc = ( int ( WINAPI * )( SOCKET, HWND, unsigned int, long ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "WSAAsyncSelect" ) ;
		WinAPIData.WinSockFunc.getaddrinfoFunc = ( int ( WINAPI * )( const char *, const char *, const _addrinfo *, _addrinfo ** ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "getaddrinfo" ) ;
		WinAPIData.WinSockFunc.freeaddrinfoFunc = ( void ( WINAPI * )( _addrinfo * ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "freeaddrinfo" ) ;
		WinAPIData.WinSockFunc.gethostbyaddrFunc = ( struct hostent* ( WINAPI * )( const char *, int, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "gethostbyaddr" ) ;
		WinAPIData.WinSockFunc.gethostbynameFunc = ( struct hostent* ( WINAPI * )( const char * ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "gethostbyname" ) ;
		WinAPIData.WinSockFunc.gethostnameFunc = ( int ( WINAPI * )( char *name, int namelen ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "gethostname" ) ;
		WinAPIData.WinSockFunc.ntohsFunc = ( u_short ( WINAPI * )( u_short ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "ntohs" ) ;
		WinAPIData.WinSockFunc.htonsFunc = ( u_short ( WINAPI * )( u_short ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "htons" ) ;
		WinAPIData.WinSockFunc.connectFunc = ( int ( WINAPI * )( SOCKET s, const struct sockaddr *, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "connect" ) ;
		WinAPIData.WinSockFunc.socketFunc = ( SOCKET ( WINAPI * )( int, int, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "socket" ) ;
		WinAPIData.WinSockFunc.sendFunc = ( int ( WINAPI * )( SOCKET s, const char *, int, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "send" ) ;
		WinAPIData.WinSockFunc.sendtoFunc = ( int ( WINAPI * )( SOCKET s, const char *, int, int, const struct sockaddr *, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "sendto" ) ;
		WinAPIData.WinSockFunc.recvfromFunc = ( int ( WINAPI * )( SOCKET s, char *, int , int , struct sockaddr *, int * ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "recvfrom" ) ;
		WinAPIData.WinSockFunc.acceptFunc = ( SOCKET ( WINAPI * )( SOCKET s, struct sockaddr *, int * ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "accept" ) ;
		WinAPIData.WinSockFunc.closesocketFunc = ( int ( WINAPI * )( SOCKET ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "closesocket" ) ;
		WinAPIData.WinSockFunc.shutdownFunc = ( int ( WINAPI * )( SOCKET , int  ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "shutdown" ) ;
		WinAPIData.WinSockFunc.listenFunc = ( int ( WINAPI * )( SOCKET , int  ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "listen" ) ;
		WinAPIData.WinSockFunc.bindFunc = ( int ( WINAPI * )( SOCKET , const struct sockaddr *, int  ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "bind" ) ;
		WinAPIData.WinSockFunc.inet_addrFunc = ( unsigned long ( WINAPI * )( const char * ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "inet_addr" ) ;
		WinAPIData.WinSockFunc.recvFunc = ( int ( WINAPI * )( SOCKET, char *, int, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "recv" ) ;
		WinAPIData.WinSockFunc.setsockoptFunc = ( int ( WINAPI * )( SOCKET, int, int, const char *, int ) )GetProcAddress( WinAPIData.WinSockFunc.WinSockDLL, "setsockopt" ) ;
	}
#endif

#ifndef DX_NON_KEYEX
	// Imm32DLL をロードする
	if( WinAPIData.ImmFunc.Imm32DLL == NULL )
	{
		WinAPIData.ImmFunc.Imm32DLL = LoadLibraryW( L"Imm32.dll" ) ;
		WinAPIData.ImmFunc.ImmGetContextFunc = ( HIMC ( WINAPI * )( HWND  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetContext" ) ;
		WinAPIData.ImmFunc.ImmReleaseContextFunc = ( BOOL ( WINAPI * )( HWND , HIMC  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmReleaseContext" ) ;
		WinAPIData.ImmFunc.ImmGetOpenStatusFunc = ( BOOL ( WINAPI * )( HIMC  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetOpenStatus" ) ;
		WinAPIData.ImmFunc.ImmGetConversionStatusFunc = ( BOOL ( WINAPI * )( HIMC , LPDWORD , LPDWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetConversionStatus" ) ;
		WinAPIData.ImmFunc.ImmNotifyIMEFunc = ( BOOL ( WINAPI * )( HIMC , DWORD , DWORD , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmNotifyIME" ) ;
		WinAPIData.ImmFunc.ImmSetOpenStatusFunc = ( BOOL ( WINAPI * )( HIMC , BOOL  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmSetOpenStatus" ) ;

//#ifdef UNICODE
		WinAPIData.ImmFunc.ImmGetCandidateListFunc = ( DWORD ( WINAPI * )( HIMC , DWORD , LPCANDIDATELIST , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetCandidateListW" ) ;
		WinAPIData.ImmFunc.ImmGetCandidateListCountFunc = ( DWORD ( WINAPI * )( HIMC , LPDWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetCandidateListCountW" ) ;
		WinAPIData.ImmFunc.ImmGetCompositionStringFunc = ( LONG ( WINAPI * )( HIMC , DWORD , LPVOID , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetCompositionStringW" ) ;
		WinAPIData.ImmFunc.ImmSetCompositionStringFunc = ( BOOL ( WINAPI * )( HIMC , DWORD , LPCVOID , DWORD , LPCVOID , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmSetCompositionStringW" ) ;
//#else
//		WinAPIData.ImmFunc.ImmGetCandidateListFunc = ( DWORD ( WINAPI * )( HIMC , DWORD , LPCANDIDATELIST , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetCandidateListA" ) ;
//		WinAPIData.ImmFunc.ImmGetCandidateListCountFunc = ( DWORD ( WINAPI * )( HIMC , LPDWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetCandidateListCountA" ) ;
//		WinAPIData.ImmFunc.ImmGetCompositionStringFunc = ( LONG ( WINAPI * )( HIMC , DWORD , LPVOID , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmGetCompositionStringA" ) ;
//		WinAPIData.ImmFunc.ImmSetCompositionStringFunc = ( BOOL ( WINAPI * )( HIMC , DWORD , LPCVOID , DWORD , LPCVOID , DWORD  ) )GetProcAddress( WinAPIData.ImmFunc.Imm32DLL, "ImmSetCompositionStringA" ) ;
//#endif
	}
#endif

	// WinMM DLL をロードする
	if( WinAPIData.Win32Func.WinMMDLL == NULL )
	{
		WinAPIData.Win32Func.WinMMDLL = LoadLibraryW( L"winmm.dll" ) ;
		WinAPIData.Win32Func.timeSetEventFunc = ( MMRESULT	( WINAPI * )( UINT , UINT , LPD_TIMECALLBACK , DWORD_PTR , UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "timeSetEvent" ) ;
		WinAPIData.Win32Func.timeKillEventFunc = ( MMRESULT ( WINAPI * )( UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "timeKillEvent" ) ;
		WinAPIData.Win32Func.timeBeginPeriodFunc = ( MMRESULT ( WINAPI * )( UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "timeBeginPeriod" ) ;
		WinAPIData.Win32Func.timeEndPeriodFunc = ( MMRESULT ( WINAPI * )( UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "timeEndPeriod" ) ;
		WinAPIData.Win32Func.timeGetTimeFunc = ( DWORD ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "timeGetTime" ) ;
		WinAPIData.Win32Func.timeGetDevCapsFunc = ( MMRESULT ( WINAPI * )( LPTIMECAPS , UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "timeGetDevCaps" ) ;
		WinAPIData.Win32Func.joyGetPosExFunc = ( MMRESULT ( WINAPI * )( UINT , LPJOYINFOEX  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "joyGetPosEx" ) ;

//#ifdef UNICODE
		WinAPIData.Win32Func.joyGetDevCapsFunc = ( MMRESULT ( WINAPI * )( UINT , LPJOYCAPSW , UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "joyGetDevCapsW" ) ;
		WinAPIData.Win32Func.mciSendCommandFunc = ( MCIERROR ( WINAPI * )( MCIDEVICEID , UINT , DWORD_PTR , DWORD_PTR  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "mciSendCommandW" ) ;
//#else
//		WinAPIData.Win32Func.joyGetDevCapsFunc = ( MMRESULT ( WINAPI * )( UINT , LPJOYCAPSA , UINT  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "joyGetDevCapsA" ) ;
//		WinAPIData.Win32Func.mciSendCommandFunc = ( MCIERROR ( WINAPI * )( MCIDEVICEID , UINT , DWORD_PTR , DWORD_PTR  ) )GetProcAddress( WinAPIData.Win32Func.WinMMDLL, "mciSendCommandA" ) ;
//#endif
	}

	// Kernel32 DLL をロードする
	if( WinAPIData.Win32Func.Kernel32DLL == NULL )
	{
		WinAPIData.Win32Func.Kernel32DLL = LoadLibraryW( L"kernel32.dll" ) ;
		WinAPIData.Win32Func.GetModuleHandleWFunc = ( HMODULE ( WINAPI * )( LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetModuleHandleW" ) ;
		WinAPIData.Win32Func.VerifyVersionInfoWFunc = ( BOOL ( WINAPI * )( LPOSVERSIONINFOEXW, DWORD, DWORDLONG ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "VerifyVersionInfoW" ) ;
		WinAPIData.Win32Func.VerSetConditionMaskFunc = ( ULONGLONG ( WINAPI * )( ULONGLONG, DWORD, BYTE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "VerSetConditionMask" ) ;
		WinAPIData.Win32Func.CreateEventAFunc = ( HANDLE ( WINAPI * )( LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "CreateEventA" ) ;
		WinAPIData.Win32Func.SetEventFunc = ( BOOL ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "SetEvent" ) ;
		WinAPIData.Win32Func.ResetEventFunc = ( BOOL ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "ResetEvent" ) ;
		WinAPIData.Win32Func.CreateThreadFunc = ( HANDLE ( WINAPI * )( LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "CreateThread" ) ;
		WinAPIData.Win32Func.SetThreadPriorityFunc = ( BOOL ( WINAPI * )( HANDLE, int ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "SetThreadPriority" ) ;
		WinAPIData.Win32Func.GetThreadPriorityFunc = ( int ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetThreadPriority" ) ;
		WinAPIData.Win32Func.ExitThreadFunc = ( VOID ( WINAPI * )( DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "ExitThread" ) ;
		WinAPIData.Win32Func.GetExitCodeThreadFunc = ( BOOL ( WINAPI * )( HANDLE, LPDWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetExitCodeThread" ) ;
		WinAPIData.Win32Func.GetCurrentThreadFunc = ( HANDLE ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetCurrentThread" ) ;
		WinAPIData.Win32Func.GetCurrentThreadIdFunc = ( DWORD ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetCurrentThreadId" ) ;
		WinAPIData.Win32Func.SuspendThreadFunc = ( DWORD ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "SuspendThread" ) ;
		WinAPIData.Win32Func.ResumeThreadFunc = ( DWORD ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "ResumeThread" ) ;
		WinAPIData.Win32Func.WaitForSingleObjectFunc = ( DWORD ( WINAPI * )( HANDLE, DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "WaitForSingleObject" ) ;
		WinAPIData.Win32Func.WaitForMultipleObjectsFunc = ( DWORD ( WINAPI * )( DWORD, CONST HANDLE *, BOOL, DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "WaitForMultipleObjects" ) ;
		WinAPIData.Win32Func.GetProcessHeapFunc = ( HANDLE ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetProcessHeap" ) ;
		WinAPIData.Win32Func.HeapAllocFunc = ( LPVOID ( WINAPI * )( HANDLE, DWORD, SIZE_T ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "HeapAlloc" ) ;
		WinAPIData.Win32Func.HeapFreeFunc = ( BOOL ( WINAPI * )( HANDLE, DWORD, LPVOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "HeapFree" ) ;
		WinAPIData.Win32Func.VirtualAllocFunc = ( LPVOID ( WINAPI * )( LPVOID, SIZE_T, DWORD, DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "VirtualAlloc" ) ;
		WinAPIData.Win32Func.VirtualFreeFunc = ( BOOL ( WINAPI * )( LPVOID, SIZE_T, DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "VirtualFree" ) ;
		WinAPIData.Win32Func.VirtualQueryFunc = ( SIZE_T ( WINAPI * )( LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "VirtualQuery" ) ;
		WinAPIData.Win32Func.GlobalAllocFunc = ( HGLOBAL ( WINAPI * )( UINT, SIZE_T ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GlobalAlloc" ) ;
		WinAPIData.Win32Func.GlobalFreeFunc = ( HGLOBAL ( WINAPI * )( HGLOBAL ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GlobalFree" ) ;
		WinAPIData.Win32Func.GlobalLockFunc = ( HGLOBAL ( WINAPI * )( HGLOBAL ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GlobalLock" ) ;
		WinAPIData.Win32Func.GlobalUnlockFunc = ( BOOL ( WINAPI * )( HGLOBAL ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GlobalUnlock" ) ;
		WinAPIData.Win32Func.GlobalSizeFunc = ( SIZE_T ( WINAPI * )( HGLOBAL ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GlobalSize" ) ;
		WinAPIData.Win32Func.CreateSemaphoreAFunc = ( HANDLE ( WINAPI * )( LPSECURITY_ATTRIBUTES, LONG, LONG, LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "CreateSemaphoreA" ) ;
		WinAPIData.Win32Func.ReleaseSemaphoreFunc = ( BOOL ( WINAPI * )( HANDLE, LONG, LPLONG ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "ReleaseSemaphore" ) ;
		WinAPIData.Win32Func.GetVersionExAFunc = ( BOOL ( WINAPI * )( LPOSVERSIONINFOA ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetVersionExA" ) ;
		WinAPIData.Win32Func.GetVersionExWFunc = ( BOOL ( WINAPI * )( LPOSVERSIONINFOW ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetVersionExW" ) ;
		WinAPIData.Win32Func.GetSystemInfoFunc = ( VOID ( WINAPI * )( LPSYSTEM_INFO ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetSystemInfo" ) ;
		WinAPIData.Win32Func.lstrlenAFunc = ( int ( WINAPI * )( LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrlenA" ) ;
		WinAPIData.Win32Func.lstrlenWFunc = ( int ( WINAPI * )( LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrlenW" ) ;
		WinAPIData.Win32Func.lstrcmpAFunc = ( int ( WINAPI * )( LPCSTR, LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrcmpA" ) ;
		WinAPIData.Win32Func.lstrcmpWFunc = ( int ( WINAPI * )( LPCWSTR, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrcmpW" ) ;
		WinAPIData.Win32Func.lstrcpyAFunc = ( LPSTR ( WINAPI * )( LPSTR, LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrcpyA" ) ;
		WinAPIData.Win32Func.lstrcpyWFunc = ( LPWSTR ( WINAPI * )( LPWSTR, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrcpyW" ) ;
		WinAPIData.Win32Func.lstrcpynAFunc = ( LPSTR ( WINAPI * )( LPSTR, LPCSTR, int ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrcpynA" ) ;
		WinAPIData.Win32Func.lstrcpynWFunc = ( LPWSTR ( WINAPI * )( LPWSTR, LPCWSTR, int ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "lstrcpynW" ) ;
		WinAPIData.Win32Func.GetTickCountFunc = ( DWORD ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetTickCount" ) ;
		WinAPIData.Win32Func.QueryPerformanceCounterFunc = ( BOOL ( WINAPI * )( LARGE_INTEGER * ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "QueryPerformanceCounter" ) ;
		WinAPIData.Win32Func.QueryPerformanceFrequencyFunc = ( BOOL ( WINAPI * )( LARGE_INTEGER * ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "QueryPerformanceFrequency" ) ;
		WinAPIData.Win32Func.CloseHandleFunc = ( BOOL ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "CloseHandle" ) ;
		WinAPIData.Win32Func.GetTempPathAFunc = ( DWORD ( WINAPI * )( DWORD, LPSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetTempPathA" ) ;
		WinAPIData.Win32Func.GetTempPathWFunc = ( DWORD ( WINAPI * )( DWORD, LPWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetTempPathW" ) ;
		WinAPIData.Win32Func.GetTempFileNameWFunc = ( UINT ( WINAPI * )( LPCWSTR, LPCWSTR, UINT, LPWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetTempFileNameW" ) ;
		WinAPIData.Win32Func.CreateFileAFunc = ( HANDLE ( WINAPI * )( LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "CreateFileA" ) ;
		WinAPIData.Win32Func.CreateFileWFunc = ( HANDLE ( WINAPI * )( LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "CreateFileW" ) ;
		WinAPIData.Win32Func.DeleteFileAFunc = ( BOOL ( WINAPI * )( LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "DeleteFileA" ) ;
		WinAPIData.Win32Func.DeleteFileWFunc = ( BOOL ( WINAPI * )( LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "DeleteFileW" ) ;
		WinAPIData.Win32Func.GetFileSizeFunc = ( DWORD ( WINAPI * )( HANDLE, LPDWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetFileSize" ) ;
		WinAPIData.Win32Func.WriteFileFunc = ( BOOL ( WINAPI * )( HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "WriteFile" ) ;
		WinAPIData.Win32Func.ReadFileFunc = ( BOOL ( WINAPI * )( HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "ReadFile" ) ;
		WinAPIData.Win32Func.SetFilePointerFunc = ( DWORD ( WINAPI * )( HANDLE, LONG, PLONG, DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "SetFilePointer" ) ;
		WinAPIData.Win32Func.FindFirstFileWFunc = ( HANDLE ( WINAPI * )( LPCWSTR, LPWIN32_FIND_DATAW ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "FindFirstFileW" ) ;
		WinAPIData.Win32Func.FindNextFileWFunc = ( BOOL ( WINAPI * )( HANDLE, LPWIN32_FIND_DATAW ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "FindNextFileW" ) ;
		WinAPIData.Win32Func.FindCloseFunc = ( BOOL ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "FindClose" ) ;
		WinAPIData.Win32Func.SetCurrentDirectoryWFunc = ( BOOL ( WINAPI * )( LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "SetCurrentDirectoryW" ) ;
		WinAPIData.Win32Func.GetCurrentDirectoryWFunc = ( DWORD ( WINAPI * )( DWORD, LPWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetCurrentDirectoryW" ) ;
		WinAPIData.Win32Func.SleepFunc = ( VOID ( WINAPI * )( DWORD ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "Sleep" ) ;
		WinAPIData.Win32Func.InitializeCriticalSectionFunc = ( VOID ( WINAPI * )( LPCRITICAL_SECTION ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "InitializeCriticalSection" ) ;
		WinAPIData.Win32Func.DeleteCriticalSectionFunc = ( VOID ( WINAPI * )( LPCRITICAL_SECTION ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "DeleteCriticalSection" ) ;
		WinAPIData.Win32Func.EnterCriticalSectionFunc = ( VOID ( WINAPI * )( LPCRITICAL_SECTION ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "EnterCriticalSection" ) ;
		WinAPIData.Win32Func.LeaveCriticalSectionFunc = ( VOID ( WINAPI * )( LPCRITICAL_SECTION ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "LeaveCriticalSection" ) ;
		WinAPIData.Win32Func.ExitProcessFunc = ( VOID ( WINAPI * )( UINT ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "ExitProcess" ) ;
		WinAPIData.Win32Func.LoadResourceFunc = ( HGLOBAL ( WINAPI * )( HMODULE, HRSRC ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "LoadResource" ) ;
		WinAPIData.Win32Func.FindResourceWFunc = ( HRSRC ( WINAPI * )( HMODULE, LPCWSTR, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "FindResourceW" ) ;
		WinAPIData.Win32Func.LockResourceFunc = ( LPVOID ( WINAPI * )( HGLOBAL ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "LockResource" ) ;
		WinAPIData.Win32Func.SizeofResourceFunc = ( DWORD ( WINAPI * )( HMODULE, HRSRC ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "SizeofResource" ) ;
		WinAPIData.Win32Func.GetOEMCPFunc = ( UINT ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetOEMCP" ) ;
		WinAPIData.Win32Func.OutputDebugStringWFunc = ( VOID ( WINAPI * )( LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "OutputDebugStringW" ) ;
		WinAPIData.Win32Func.GlobalMemoryStatusFunc = ( VOID ( WINAPI * )( LPMEMORYSTATUS ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GlobalMemoryStatus" ) ;
		WinAPIData.Win32Func.MulDivFunc = ( int ( WINAPI * )( int, int, int ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "MulDiv" ) ;
		WinAPIData.Win32Func.GetLocalTimeFunc = ( VOID ( WINAPI * )( LPSYSTEMTIME ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetLocalTime" ) ;
		WinAPIData.Win32Func.GetSystemPowerStatusFunc = ( BOOL ( WINAPI * )( LPSYSTEM_POWER_STATUS ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetSystemPowerStatus" ) ;
		WinAPIData.Win32Func.FileTimeToLocalFileTimeFunc = ( BOOL ( WINAPI * )( FILETIME *, LPFILETIME ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "FileTimeToLocalFileTime" ) ;
		WinAPIData.Win32Func.FileTimeToSystemTimeFunc = ( BOOL ( WINAPI * )( FILETIME *, LPSYSTEMTIME ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "FileTimeToSystemTime" ) ;
		WinAPIData.Win32Func.GetLastErrorFunc = ( DWORD ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Kernel32DLL, "GetLastError" ) ;
	}

	// Old32 DLL をロードする
	if( WinAPIData.Win32Func.Old32DLL == NULL )
	{
		WinAPIData.Win32Func.Old32DLL = LoadLibraryW( L"ole32.dll" ) ;
		WinAPIData.Win32Func.CoCreateInstanceFunc = ( HRESULT ( WINAPI * )( REFCLSID , D_IUnknown * , DWORD , REFIID , LPVOID * ) )GetProcAddress( WinAPIData.Win32Func.Old32DLL, "CoCreateInstance" ) ;
		WinAPIData.Win32Func.CoTaskMemAllocFunc = ( LPVOID ( WINAPI * )( SIZE_T  ) )GetProcAddress( WinAPIData.Win32Func.Old32DLL, "CoTaskMemAlloc" ) ;
		WinAPIData.Win32Func.CoTaskMemFreeFunc = ( void ( WINAPI * )( LPVOID  ) )GetProcAddress( WinAPIData.Win32Func.Old32DLL, "CoTaskMemFree" ) ;
		WinAPIData.Win32Func.CoInitializeExFunc = ( HRESULT ( WINAPI * )( LPVOID, DWORD ) )GetProcAddress( WinAPIData.Win32Func.Old32DLL, "CoInitializeEx" ) ;
		WinAPIData.Win32Func.CoFreeUnusedLibrariesFunc = ( void ( WINAPI * )( void ) )GetProcAddress( WinAPIData.Win32Func.Old32DLL, "CoFreeUnusedLibraries" ) ;
		WinAPIData.Win32Func.CoUninitializeFunc = ( void ( WINAPI * )( void ) )GetProcAddress( WinAPIData.Win32Func.Old32DLL, "CoUninitialize" ) ;
	}

	// OleAut32 DLL をロードする
	if( WinAPIData.Win32Func.OleAut32DLL == NULL )
	{
		WinAPIData.Win32Func.OleAut32DLL = LoadLibraryW( L"oleaut32.dll" ) ;
		WinAPIData.Win32Func.SysFreeStringFunc = ( void ( WINAPI * )( BSTR ) )GetProcAddress( WinAPIData.Win32Func.OleAut32DLL, "SysFreeString" ) ;
	}

	// Comctl32 DLL をロードする
	if( WinAPIData.Win32Func.Comctl32DLL == NULL )
	{
		WinAPIData.Win32Func.Comctl32DLL = LoadLibraryW( L"comctl32.dll" ) ;
		if( WinAPIData.Win32Func.Comctl32DLL != NULL )
		{
			WinAPIData.Win32Func.InitCommonControlsFunc = ( void ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.Comctl32DLL, "InitCommonControls" ) ;
		}
	}

	// User32DLL をロードする
	if( WinAPIData.Win32Func.User32DLL == NULL )
	{
		WinAPIData.Win32Func.User32DLL = LoadLibraryW( L"User32.dll" ) ;
		WinAPIData.Win32Func.WINNLSEnableIME_Func = ( BOOL ( WINAPI * )( HWND, BOOL ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "WINNLSEnableIME" ) ;
		WinAPIData.Win32Func.UpdateLayeredWindow = ( BOOL ( WINAPI * )( HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "UpdateLayeredWindow" ) ;
		WinAPIData.Win32Func.CreateWindowExAFunc = ( HWND ( WINAPI * )( DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CreateWindowExA" ) ;
		WinAPIData.Win32Func.CreateWindowExWFunc = ( HWND ( WINAPI * )( DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CreateWindowExW" ) ;
		WinAPIData.Win32Func.DestroyWindowFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DestroyWindow" ) ;
		WinAPIData.Win32Func.ShowWindowFunc = ( BOOL ( WINAPI * )( HWND, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ShowWindow" ) ;
		WinAPIData.Win32Func.UpdateWindowFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "UpdateWindow" ) ;
		WinAPIData.Win32Func.BringWindowToTopFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "BringWindowToTop" ) ;
		WinAPIData.Win32Func.SetForegroundWindowFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetForegroundWindow" ) ;
		WinAPIData.Win32Func.GetWindowThreadProcessIdFunc = ( DWORD ( WINAPI * )( HWND, LPDWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetWindowThreadProcessId" ) ;
		WinAPIData.Win32Func.SetActiveWindowFunc = ( HWND ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetActiveWindow" ) ;
		WinAPIData.Win32Func.GetForegroundWindowFunc = ( HWND ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetForegroundWindow" ) ;
		WinAPIData.Win32Func.GetDesktopWindowFunc = ( HWND ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetDesktopWindow" ) ;
		WinAPIData.Win32Func.FindWindowWFunc = ( HWND ( WINAPI * )( LPCWSTR, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "FindWindowW" ) ;
		WinAPIData.Win32Func.MoveWindowFunc = ( BOOL ( WINAPI * )( HWND, int, int, int, int, BOOL ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "MoveWindow" ) ;
		WinAPIData.Win32Func.SetWindowPosFunc = ( BOOL ( WINAPI * )( HWND, HWND, int, int, int, int, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowPos" ) ;
		WinAPIData.Win32Func.GetClientRectFunc = ( BOOL ( WINAPI * )( HWND, LPRECT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetClientRect" ) ;
		WinAPIData.Win32Func.AdjustWindowRectExFunc = ( BOOL ( WINAPI * )( LPRECT, DWORD, BOOL, DWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "AdjustWindowRectEx" ) ;
		WinAPIData.Win32Func.GetWindowRectFunc = ( BOOL ( WINAPI * )( HWND, LPRECT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetWindowRect" ) ;
		WinAPIData.Win32Func.SetWindowLongAFunc = ( LONG ( WINAPI * )( HWND, int, LONG ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowLongA" ) ;
		WinAPIData.Win32Func.SetWindowLongWFunc = ( LONG ( WINAPI * )( HWND, int, LONG ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowLongW" ) ;
		WinAPIData.Win32Func.SetWindowLongPtrAFunc = ( LONG_PTR ( WINAPI * )( HWND, int, LONG_PTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowLongPtrA" ) ;
		WinAPIData.Win32Func.SetWindowLongPtrWFunc = ( LONG_PTR ( WINAPI * )( HWND, int, LONG_PTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowLongPtrW" ) ;
		WinAPIData.Win32Func.GetWindowLongAFunc = ( LONG ( WINAPI * )( HWND, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetWindowLongA" ) ;
		WinAPIData.Win32Func.GetWindowLongWFunc = ( LONG ( WINAPI * )( HWND, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetWindowLongW" ) ;
		WinAPIData.Win32Func.GetWindowLongPtrAFunc = ( LONG_PTR ( WINAPI * )( HWND, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetWindowLongPtrA" ) ;
		WinAPIData.Win32Func.GetWindowLongPtrWFunc = ( LONG_PTR ( WINAPI * )( HWND, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetWindowLongPtrW" ) ;
		WinAPIData.Win32Func.SetWindowTextWFunc = ( BOOL ( WINAPI * )( HWND, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowTextW" ) ;
		WinAPIData.Win32Func.ClientToScreenFunc = ( BOOL ( WINAPI * )( HWND, LPPOINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ClientToScreen" ) ;
		WinAPIData.Win32Func.EnumDisplayDevicesWFunc = ( BOOL ( WINAPI * )( LPCWSTR, DWORD, PDISPLAY_DEVICEW, DWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "EnumDisplayDevicesW" ) ;
		WinAPIData.Win32Func.ShutdownBlockReasonCreateFunc = ( BOOL ( WINAPI * )( HWND, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ShutdownBlockReasonCreate" ) ;
		WinAPIData.Win32Func.ShutdownBlockReasonDestroyFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ShutdownBlockReasonDestroy" ) ;
		WinAPIData.Win32Func.SetTimerFunc = ( UINT_PTR ( WINAPI * )( HWND, UINT_PTR, UINT, D_TIMERPROC ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetTimer" ) ;
		WinAPIData.Win32Func.PostThreadMessageAFunc = ( BOOL ( WINAPI * )( DWORD, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PostThreadMessageA" ) ;
		WinAPIData.Win32Func.PostThreadMessageWFunc = ( BOOL ( WINAPI * )( DWORD, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PostThreadMessageW" ) ;
		WinAPIData.Win32Func.PeekMessageAFunc = ( BOOL ( WINAPI * )( LPMSG, HWND, UINT, UINT, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PeekMessageA" ) ;
		WinAPIData.Win32Func.PeekMessageWFunc = ( BOOL ( WINAPI * )( LPMSG, HWND, UINT, UINT, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PeekMessageW" ) ;
		WinAPIData.Win32Func.GetMessageAFunc = ( BOOL ( WINAPI * )( LPMSG, HWND, UINT, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetMessageA" ) ;
		WinAPIData.Win32Func.GetMessageWFunc = ( BOOL ( WINAPI * )( LPMSG, HWND, UINT, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetMessageW" ) ;
		WinAPIData.Win32Func.SendMessageAFunc = ( LRESULT ( WINAPI * )( HWND, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SendMessageA" ) ;
		WinAPIData.Win32Func.SendMessageWFunc = ( LRESULT ( WINAPI * )( HWND, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SendMessageW" ) ;
		WinAPIData.Win32Func.PostMessageAFunc = ( BOOL ( WINAPI * )( HWND, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PostMessageA" ) ;
		WinAPIData.Win32Func.PostMessageWFunc = ( BOOL ( WINAPI * )( HWND, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PostMessageW" ) ;
		WinAPIData.Win32Func.PostQuitMessageFunc = ( VOID ( WINAPI * )( int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "PostQuitMessage" ) ;
		WinAPIData.Win32Func.DefWindowProcAFunc = ( LRESULT ( WINAPI * )( HWND, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DefWindowProcA" ) ;
		WinAPIData.Win32Func.DefWindowProcWFunc = ( LRESULT ( WINAPI * )( HWND, UINT, WPARAM, LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DefWindowProcW" ) ;
		WinAPIData.Win32Func.TranslateAcceleratorWFunc = ( int ( WINAPI * )( HWND, HACCEL, LPMSG ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "TranslateAcceleratorW" ) ;
		WinAPIData.Win32Func.TranslateMessageFunc = ( BOOL ( WINAPI * )( MSG * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "TranslateMessage" ) ;
		WinAPIData.Win32Func.DispatchMessageAFunc = ( LRESULT ( WINAPI * )( MSG * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DispatchMessageA" ) ;
		WinAPIData.Win32Func.DispatchMessageWFunc = ( LRESULT ( WINAPI * )( MSG * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DispatchMessageW" ) ;
		WinAPIData.Win32Func.MsgWaitForMultipleObjectsFunc = ( DWORD ( WINAPI * )( DWORD, CONST HANDLE *, BOOL, DWORD, DWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "MsgWaitForMultipleObjects" ) ;
		WinAPIData.Win32Func.GetQueueStatusFunc = ( DWORD ( WINAPI * )( UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetQueueStatus" ) ;
		WinAPIData.Win32Func.RegisterWindowMessageAFunc = ( UINT ( WINAPI * )( LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterWindowMessageA" ) ;
		WinAPIData.Win32Func.RegisterWindowMessageWFunc = ( UINT ( WINAPI * )( LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterWindowMessageW" ) ;
		WinAPIData.Win32Func.RegisterClassAFunc = ( ATOM ( WINAPI * )( CONST WNDCLASSA * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterClassA" ) ;
		WinAPIData.Win32Func.RegisterClassWFunc = ( ATOM ( WINAPI * )( CONST WNDCLASSW * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterClassW" ) ;
		WinAPIData.Win32Func.RegisterClassExAFunc = ( ATOM ( WINAPI * )( CONST WNDCLASSEXA * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterClassExA" ) ;
		WinAPIData.Win32Func.RegisterClassExWFunc = ( ATOM ( WINAPI * )( CONST WNDCLASSEXW * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterClassExW" ) ;
		WinAPIData.Win32Func.UnregisterClassAFunc = ( BOOL ( WINAPI * )( LPCSTR, HINSTANCE ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "UnregisterClassA" ) ;
		WinAPIData.Win32Func.UnregisterClassWFunc = ( BOOL ( WINAPI * )( LPCWSTR, HINSTANCE ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "UnregisterClassW" ) ;
		WinAPIData.Win32Func.SetClassLongWFunc = ( DWORD ( WINAPI * )( HWND, int, LONG ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetClassLongW" ) ;
		WinAPIData.Win32Func.SetClassLongPtrWFunc = ( unsigned LONG_PTR ( WINAPI * )( HWND, int, LONG_PTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetClassLongPtrW" ) ;
		WinAPIData.Win32Func.GetDCFunc = ( HDC ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetDC" ) ;
		WinAPIData.Win32Func.ReleaseDCFunc = ( int ( WINAPI * )( HWND, HDC ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ReleaseDC" ) ;
		WinAPIData.Win32Func.BeginPaintFunc = ( HDC ( WINAPI * )( HWND, LPPAINTSTRUCT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "BeginPaint" ) ;
		WinAPIData.Win32Func.EndPaintFunc = ( BOOL ( WINAPI * )( HWND, PAINTSTRUCT * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "EndPaint" ) ;
		WinAPIData.Win32Func.MessageBoxAFunc = ( int ( WINAPI * )( HWND, LPCSTR, LPCSTR, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "MessageBoxA" ) ;
		WinAPIData.Win32Func.MessageBoxWFunc = ( int ( WINAPI * )( HWND, LPCWSTR, LPCWSTR, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "MessageBoxW" ) ;
		WinAPIData.Win32Func.GetSystemMetricsFunc = ( int ( WINAPI * )( int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetSystemMetrics" ) ;
		WinAPIData.Win32Func.ChangeDisplaySettingsAFunc = ( LONG ( WINAPI * )( DEVMODEA*, DWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ChangeDisplaySettingsA" ) ;
		WinAPIData.Win32Func.ChangeDisplaySettingsWFunc = ( LONG ( WINAPI * )( DEVMODEW*, DWORD ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ChangeDisplaySettingsW" ) ;
		WinAPIData.Win32Func.ShowCursorFunc = ( int ( WINAPI * )( BOOL ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ShowCursor" ) ;
		WinAPIData.Win32Func.SetCursorPosFunc = ( BOOL ( WINAPI * )( int, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetCursorPos" ) ;
		WinAPIData.Win32Func.GetCursorPosFunc = ( BOOL ( WINAPI * )( LPPOINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetCursorPos" ) ;
		WinAPIData.Win32Func.SetCursorFunc = ( HCURSOR ( WINAPI * )( HCURSOR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetCursor" ) ;
		WinAPIData.Win32Func.LoadCursorWFunc = ( HCURSOR ( WINAPI * )( HINSTANCE, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "LoadCursorW" ) ;
		WinAPIData.Win32Func.LoadIconWFunc = ( HICON ( WINAPI * )( HINSTANCE, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "LoadIconW" ) ;
		WinAPIData.Win32Func.ClipCursorFunc = ( BOOL ( WINAPI * )( CONST RECT * ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "ClipCursor" ) ;
		WinAPIData.Win32Func.LoadMenuWFunc = ( HMENU ( WINAPI * )( HINSTANCE, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "LoadMenuW" ) ;
		WinAPIData.Win32Func.CreateMenuFunc = ( HMENU ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CreateMenu" ) ;
		WinAPIData.Win32Func.DeleteMenuFunc = ( BOOL ( WINAPI * )( HMENU, UINT, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DeleteMenu" ) ;
		WinAPIData.Win32Func.DestroyMenuFunc = ( BOOL ( WINAPI * )( HMENU ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DestroyMenu" ) ;
		WinAPIData.Win32Func.EnableMenuItemFunc = ( BOOL ( WINAPI * )( HMENU, UINT, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "EnableMenuItem" ) ;
		WinAPIData.Win32Func.GetSubMenuFunc = ( HMENU ( WINAPI * )( HMENU, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetSubMenu" ) ;
		WinAPIData.Win32Func.SetMenuItemInfoWFunc = ( BOOL ( WINAPI * )( HMENU, UINT, BOOL, LPCMENUITEMINFOW ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetMenuItemInfoW" ) ;
		WinAPIData.Win32Func.GetMenuItemInfoWFunc = ( BOOL ( WINAPI * )( HMENU, UINT, BOOL, LPMENUITEMINFOW ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetMenuItemInfoW" ) ;
		WinAPIData.Win32Func.SetMenuFunc = ( BOOL ( WINAPI * )( HWND, HMENU ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetMenu" ) ;
		WinAPIData.Win32Func.DrawMenuBarFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DrawMenuBar" ) ;
		WinAPIData.Win32Func.GetMenuItemCountFunc = ( int ( WINAPI * )( HMENU ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetMenuItemCount" ) ;
		WinAPIData.Win32Func.InsertMenuItemWFunc = ( BOOL ( WINAPI * )( HMENU, UINT, BOOL, LPCMENUITEMINFOW ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "InsertMenuItemW" ) ;
		WinAPIData.Win32Func.SetWindowRgnFunc = ( int ( WINAPI * )( HWND, HRGN, BOOL ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetWindowRgn" ) ;
		WinAPIData.Win32Func.FillRectFunc = ( int ( WINAPI * )( HDC, CONST RECT *, HBRUSH ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "FillRect" ) ;
		WinAPIData.Win32Func.SystemParametersInfoWFunc = ( BOOL ( WINAPI * )( UINT, UINT, PVOID, UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SystemParametersInfoW" ) ;
		WinAPIData.Win32Func.AttachThreadInputFunc = ( BOOL ( WINAPI * )( DWORD, DWORD, BOOL ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "AttachThreadInput" ) ;
		WinAPIData.Win32Func.UnhookWindowsHookExFunc = ( BOOL ( WINAPI * )( HHOOK ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "UnhookWindowsHookEx" ) ;
		WinAPIData.Win32Func.GetKeyboardStateFunc = ( BOOL ( WINAPI * )( PBYTE ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetKeyboardState" ) ;
		WinAPIData.Win32Func.KillTimerFunc = ( BOOL ( WINAPI * )( HWND, UINT_PTR ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "KillTimer" ) ;
		WinAPIData.Win32Func.EnumDisplaySettingsWFunc = ( BOOL ( WINAPI * )( LPCWSTR, DWORD, DEVMODEW* ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "EnumDisplaySettingsW" ) ;
		WinAPIData.Win32Func.EnumDisplayMonitorsFunc = ( BOOL ( WINAPI * )( HDC, LPCRECT, MONITORENUMPROC , LPARAM ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "EnumDisplayMonitors" ) ;
		WinAPIData.Win32Func.IsDialogMessageWFunc = ( BOOL ( WINAPI * )( HWND, LPMSG ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "IsDialogMessageW" ) ;
		WinAPIData.Win32Func.OpenClipboardFunc = ( BOOL ( WINAPI * )( HWND ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "OpenClipboard" ) ;
		WinAPIData.Win32Func.CloseClipboardFunc = ( BOOL ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CloseClipboard" ) ;
		WinAPIData.Win32Func.EmptyClipboardFunc = ( BOOL ( WINAPI * )( VOID ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "EmptyClipboard" ) ;
		WinAPIData.Win32Func.SetClipboardDataFunc = ( HANDLE ( WINAPI * )( UINT, HANDLE ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "SetClipboardData" ) ;
		WinAPIData.Win32Func.GetClipboardDataFunc = ( HANDLE ( WINAPI * )( UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetClipboardData" ) ;
		WinAPIData.Win32Func.IsClipboardFormatAvailableFunc = ( BOOL ( WINAPI * )( UINT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "IsClipboardFormatAvailable" ) ;
		WinAPIData.Win32Func.CreateAcceleratorTableWFunc = ( HACCEL ( WINAPI * )( LPACCEL, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CreateAcceleratorTableW" ) ;
		WinAPIData.Win32Func.DestroyAcceleratorTableFunc = ( BOOL ( WINAPI * )( HACCEL ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "DestroyAcceleratorTable" ) ;
		WinAPIData.Win32Func.CopyAcceleratorTableWFunc = ( int ( WINAPI * )( HACCEL, LPACCEL, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CopyAcceleratorTableW" ) ;
		WinAPIData.Win32Func.GetMonitorInfoWFunc = ( BOOL ( WINAPI * )( HMONITOR, LPMONITORINFO ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetMonitorInfoW" ) ;

		WinAPIData.Win32Func.CloseTouchInputHandleFunc = ( BOOL ( WINAPI * )( D_HTOUCHINPUT ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "CloseTouchInputHandle" ) ;
		WinAPIData.Win32Func.GetTouchInputInfoFunc     = ( BOOL ( WINAPI * )( D_HTOUCHINPUT, UINT, D_PTOUCHINPUT, int ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "GetTouchInputInfo" ) ;
		WinAPIData.Win32Func.IsTouchWindowFunc         = ( BOOL ( WINAPI * )( HWND, ULONG *) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "IsTouchWindow" ) ;
		WinAPIData.Win32Func.RegisterTouchWindowFunc   = ( BOOL ( WINAPI * )( HWND, ULONG ) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "RegisterTouchWindow" ) ;
		WinAPIData.Win32Func.UnregisterTouchWindowFunc = ( BOOL ( WINAPI * )( HWND) )GetProcAddress( WinAPIData.Win32Func.User32DLL, "UnregisterTouchWindow" ) ;
	}

	// Gdi32DLL をロードする
	if( WinAPIData.Win32Func.GDI32DLL == NULL )
	{
		WinAPIData.Win32Func.GDI32DLL = LoadLibraryW( L"gdi32.dll" ) ;
		if( WinAPIData.Win32Func.GDI32DLL != NULL )
		{
			WinAPIData.Win32Func.AddFontMemResourceExFunc = ( HANDLE ( WINAPI * )( LPVOID, DWORD, void /* DESIGNVECTOR */ *, DWORD * ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "AddFontMemResourceEx" ) ;
			WinAPIData.Win32Func.RemoveFontMemResourceExFunc = ( int ( WINAPI * )( HANDLE ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "RemoveFontMemResourceEx" ) ;
			WinAPIData.Win32Func.GetFontUnicodeRangesFunc = ( DWORD  ( WINAPI * )( HDC, D_LPGLYPHSET ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetFontUnicodeRanges" ) ;
			WinAPIData.Win32Func.GetDeviceCapsFunc = ( int ( WINAPI * )( HDC, int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetDeviceCaps" ) ;
			WinAPIData.Win32Func.EnumFontFamiliesExAFunc = ( int ( WINAPI * )( HDC, LPLOGFONTA, FONTENUMPROCA, LPARAM, DWORD ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "EnumFontFamiliesExA" ) ;
			WinAPIData.Win32Func.EnumFontFamiliesExWFunc = ( int ( WINAPI * )( HDC, LPLOGFONTW, FONTENUMPROCW, LPARAM, DWORD ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "EnumFontFamiliesExW" ) ;
			WinAPIData.Win32Func.CreateFontAFunc = ( HFONT ( WINAPI * )( int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateFontA" ) ;
			WinAPIData.Win32Func.CreateFontWFunc = ( HFONT ( WINAPI * )( int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCWSTR ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateFontW" ) ;
			WinAPIData.Win32Func.GetTextMetricsWFunc = ( BOOL ( WINAPI * )( HDC, LPTEXTMETRICW ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetTextMetricsW" ) ;
			WinAPIData.Win32Func.GetTextExtentPoint32WFunc = ( BOOL ( WINAPI * )( HDC, LPCWSTR, int, LPSIZE ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetTextExtentPoint32W" ) ;
			WinAPIData.Win32Func.GetCharacterPlacementWFunc = ( DWORD ( WINAPI * )( HDC, LPCWSTR, int, int, LPGCP_RESULTSW, DWORD ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetCharacterPlacementW" ) ;
			WinAPIData.Win32Func.GetGlyphOutlineWFunc = ( DWORD ( WINAPI * )( HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2 * ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetGlyphOutlineW" ) ;
			WinAPIData.Win32Func.TextOutWFunc = ( BOOL ( WINAPI * )( HDC, int, int, LPCWSTR, int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "TextOutW" ) ;
			WinAPIData.Win32Func.SetTextColorFunc = ( COLORREF ( WINAPI * )( HDC, COLORREF ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "SetTextColor" ) ;
			WinAPIData.Win32Func.SetBkColorFunc = ( COLORREF ( WINAPI * )( HDC, COLORREF ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "SetBkColor" ) ;
			WinAPIData.Win32Func.SetBkModeFunc = ( int ( WINAPI * )( HDC, int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "SetBkMode" ) ;
			WinAPIData.Win32Func.CreateCompatibleDCFunc = ( HDC ( WINAPI * )( HDC ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateCompatibleDC" ) ;
			WinAPIData.Win32Func.CreateDCWFunc = ( HDC ( WINAPI * )( LPCWSTR, LPCWSTR, LPCWSTR, CONST DEVMODEW * ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateDCW" ) ;
			WinAPIData.Win32Func.GetObjectAFunc = ( int ( WINAPI * )( HANDLE, int, LPVOID ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetObjectA" ) ;
			WinAPIData.Win32Func.GetObjectWFunc = ( int ( WINAPI * )( HANDLE, int, LPVOID ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetObjectW" ) ;
			WinAPIData.Win32Func.GetStockObjectFunc = ( HGDIOBJ ( WINAPI * )( int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "GetStockObject" ) ;
			WinAPIData.Win32Func.SelectObjectFunc = ( HGDIOBJ ( WINAPI * )( HDC, HGDIOBJ ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "SelectObject" ) ;
			WinAPIData.Win32Func.DeleteObjectFunc = ( BOOL ( WINAPI * )( HGDIOBJ ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "DeleteObject" ) ;
			WinAPIData.Win32Func.DeleteDCFunc = ( BOOL ( WINAPI * )( HDC ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "DeleteDC" ) ;
			WinAPIData.Win32Func.CreateSolidBrushFunc = ( HBRUSH ( WINAPI * )( COLORREF ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateSolidBrush" ) ;
			WinAPIData.Win32Func.RectangleFunc = ( BOOL ( WINAPI * )( HDC, int, int, int, int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "Rectangle" ) ;
			WinAPIData.Win32Func.BitBltFunc = ( BOOL ( WINAPI * )( HDC, int, int, int, int, HDC, int, int, DWORD ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "BitBlt" ) ;
			WinAPIData.Win32Func.CreateDIBSectionFunc = ( HBITMAP ( WINAPI * )( HDC, CONST BITMAPINFO *, UINT, VOID **, HANDLE, DWORD ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateDIBSection" ) ;
			WinAPIData.Win32Func.StretchDIBitsFunc = ( int ( WINAPI * )( HDC, int, int, int, int, int, int, int, int, CONST VOID *, CONST BITMAPINFO *, UINT, DWORD ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "StretchDIBits" ) ;
			WinAPIData.Win32Func.SetDIBitsToDeviceFunc = ( int ( WINAPI * )( HDC, int, int, DWORD, DWORD, int, int, UINT, UINT, CONST VOID *, CONST BITMAPINFO *, UINT ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "SetDIBitsToDevice" ) ;
			WinAPIData.Win32Func.CreateRectRgnFunc = ( HRGN ( WINAPI * )( int, int, int, int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CreateRectRgn" ) ;
			WinAPIData.Win32Func.CombineRgnFunc = ( int ( WINAPI * )( HRGN, HRGN, HRGN, int ) )GetProcAddress( WinAPIData.Win32Func.GDI32DLL, "CombineRgn" ) ;
		}
	}

	// Shell32DLL をロードする
	if( WinAPIData.Win32Func.SHELL32DLL == NULL )
	{
		WinAPIData.Win32Func.SHELL32DLL = LoadLibraryW( L"shell32.dll" ) ;
		if( WinAPIData.Win32Func.SHELL32DLL != NULL )
		{
			WinAPIData.Win32Func.DragQueryFileWFunc = ( UINT ( WINAPI * )( HDROP, UINT, LPWSTR, UINT ) )GetProcAddress( WinAPIData.Win32Func.SHELL32DLL, "DragQueryFileW" ) ;
			WinAPIData.Win32Func.DragFinishFunc = ( void ( WINAPI * )( HDROP ) )GetProcAddress( WinAPIData.Win32Func.SHELL32DLL, "DragFinish" ) ;
			WinAPIData.Win32Func.DragAcceptFilesFunc = ( void ( WINAPI * )( HWND, BOOL ) )GetProcAddress( WinAPIData.Win32Func.SHELL32DLL, "DragAcceptFiles" ) ;
		}
	}

	// NTDLL をロードする
	if( WinAPIData.Win32Func.NTDLL == NULL )
	{
		WinAPIData.Win32Func.NTDLL = LoadLibraryW( L"ntdll.dll" ) ;
		if( WinAPIData.Win32Func.NTDLL != NULL )
		{
			WinAPIData.Win32Func.RtlGetVersionFunc = ( void ( WINAPI * )( LPOSVERSIONINFOEXW ) )GetProcAddress( WinAPIData.Win32Func.NTDLL, "RtlGetVersion" ) ;
		}
	}

	// MfplatDLL をロードする
	if( WinAPIData.Win32Func.MFPLATDLL == NULL )
	{
		WinAPIData.Win32Func.MFPLATDLL = LoadLibraryW( L"Mfplat.dll" ) ;
		if( WinAPIData.Win32Func.MFPLATDLL != NULL )
		{
			WinAPIData.Win32Func.MFStartupFunc								= ( HRESULT ( WINAPI * )( ULONG, DWORD ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFStartup" ) ;
			WinAPIData.Win32Func.MFShutdownFunc								= ( HRESULT ( WINAPI * )( void ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFShutdown" ) ;
			WinAPIData.Win32Func.MFCreateMediaTypeFunc						= ( HRESULT ( WINAPI * )( D_IMFMediaType ** ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFCreateMediaType" ) ;
			WinAPIData.Win32Func.MFCreateWaveFormatExFromMFMediaTypeFunc	= ( HRESULT ( WINAPI * )( D_IMFMediaType *, WAVEFORMATEX **, UINT32 *, UINT32 ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFCreateWaveFormatExFromMFMediaType" ) ;
			WinAPIData.Win32Func.MFCreateAttributesFunc						= ( HRESULT	( WINAPI * )( D_IMFAttributes**, UINT32 ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFCreateAttributes" ) ;
			WinAPIData.Win32Func.MFCreateAsyncResultFunc					= ( HRESULT ( WINAPI * )( D_IUnknown *, D_IMFAsyncCallback *, D_IUnknown *, D_IMFAsyncResult ** ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFCreateAsyncResult" ) ;
			WinAPIData.Win32Func.MFInvokeCallbackFunc						= ( HRESULT ( WINAPI * )( D_IMFAsyncResult * ) )GetProcAddress( WinAPIData.Win32Func.MFPLATDLL, "MFInvokeCallback" ) ;
		}
	}

	// MfreadwriteDLL をロードする
	if( WinAPIData.Win32Func.MFREADWRITEDLL == NULL )
	{
		WinAPIData.Win32Func.MFREADWRITEDLL = LoadLibraryW( L"Mfreadwrite.dll" ) ;
		if( WinAPIData.Win32Func.MFREADWRITEDLL != NULL )
		{
			WinAPIData.Win32Func.MFCreateSourceReaderFromURLFunc			= ( HRESULT ( WINAPI * )( LPCWSTR, D_IMFAttributes *, D_IMFSourceReader ** ) )GetProcAddress( WinAPIData.Win32Func.MFREADWRITEDLL, "MFCreateSourceReaderFromURL" ) ;
			WinAPIData.Win32Func.MFCreateSourceReaderFromByteStreamFunc		= ( HRESULT	( WINAPI * )( D_IMFByteStream *, D_IMFAttributes *, D_IMFSourceReader ** ) )GetProcAddress( WinAPIData.Win32Func.MFREADWRITEDLL, "MFCreateSourceReaderFromByteStream" ) ;
		}
	}

	// PropsysDLL をロードする
	if( WinAPIData.Win32Func.PROPSYSDLL == NULL )
	{
		WinAPIData.Win32Func.PROPSYSDLL = LoadLibraryW( L"Propsys.dll" ) ;
		if( WinAPIData.Win32Func.PROPSYSDLL != NULL )
		{
			WinAPIData.Win32Func.PropVariantToInt64Func = ( HRESULT	( WINAPI * )( const D_PROPVARIANT &, LONGLONG * ) )GetProcAddress( WinAPIData.Win32Func.PROPSYSDLL, "PropVariantToInt64" ) ;
		}
	}

	// DwmApiDLL をロードする
	if( WinAPIData.DwmApiDLL == NULL )
	{
		WinAPIData.DwmApiDLL = LoadLibraryW( L"dwmapi.dll" ) ;
		if( WinAPIData.DwmApiDLL != NULL )
		{
			WinAPIData.DF_DwmEnableComposition = ( HRESULT	( WINAPI * )( UINT ) )GetProcAddress( WinAPIData.DwmApiDLL, "DwmEnableComposition" ) ;
			WinAPIData.DwmGetWindowAttributeFunc = ( HRESULT ( WINAPI * )( HWND, DWORD, PVOID, DWORD ) )GetProcAddress( WinAPIData.DwmApiDLL, "DwmGetWindowAttribute" ) ;
		}
	}

	// 初期化完了フラグを立てる
	WinAPIData.InitializeFlag = TRUE ;

	// 終了
	return 0 ;
}

// WindowsOS の DLL を解放する
extern int ReleaseWinAPI( void )
{
	// 初期化されていなかったら何もしない
	if( WinAPIData.InitializeFlag == FALSE )
	{
		return 0 ;
	}

	// DWMAPI.DLL の解放
	if( WinAPIData.DwmApiDLL )
	{
		FreeLibrary( WinAPIData.DwmApiDLL ) ;
		WinAPIData.DwmApiDLL = NULL ;
		WinAPIData.DF_DwmEnableComposition = NULL ;
	}

	// User32.DLL の解放
	if( WinAPIData.Win32Func.User32DLL )
	{
		FreeLibrary( WinAPIData.Win32Func.User32DLL ) ;
		WinAPIData.Win32Func.User32DLL = NULL ;
		WinAPIData.Win32Func.WINNLSEnableIME_Func = NULL ;
		WinAPIData.Win32Func.UpdateLayeredWindow = NULL ;
		WinAPIData.Win32Func.CreateWindowExWFunc = NULL ;
		WinAPIData.Win32Func.EnumDisplayDevicesWFunc = NULL ;
		WinAPIData.Win32Func.CloseTouchInputHandleFunc = NULL ;
		WinAPIData.Win32Func.GetTouchInputInfoFunc = NULL ;
		WinAPIData.Win32Func.IsTouchWindowFunc = NULL ;
		WinAPIData.Win32Func.RegisterTouchWindowFunc = NULL ;
		WinAPIData.Win32Func.UnregisterTouchWindowFunc = NULL ;
		WinAPIData.Win32Func.ShutdownBlockReasonCreateFunc = NULL ;
		WinAPIData.Win32Func.ShutdownBlockReasonDestroyFunc = NULL ;
		WinAPIData.Win32Func.SetTimerFunc = NULL ;
	}

#ifndef DX_NON_NETWORK
	// ws2_32.dll の解放
	if( WinAPIData.WinSockFunc.WinSockDLL )
	{
		FreeLibrary( WinAPIData.WinSockFunc.WinSockDLL ) ;
		WinAPIData.WinSockFunc.WinSockDLL = NULL ;
		WinAPIData.WinSockFunc.WSAGetLastErrorFunc = NULL ;
		WinAPIData.WinSockFunc.WSAStartupFunc = NULL ;
		WinAPIData.WinSockFunc.WSACleanupFunc = NULL ;
		WinAPIData.WinSockFunc.WSAAsyncSelectFunc = NULL ;
		WinAPIData.WinSockFunc.getaddrinfoFunc = NULL ;
		WinAPIData.WinSockFunc.gethostbyaddrFunc = NULL ;
		WinAPIData.WinSockFunc.gethostbynameFunc = NULL ;
		WinAPIData.WinSockFunc.gethostnameFunc = NULL ;
		WinAPIData.WinSockFunc.ntohsFunc = NULL ;
		WinAPIData.WinSockFunc.htonsFunc = NULL ;
		WinAPIData.WinSockFunc.connectFunc = NULL ;
		WinAPIData.WinSockFunc.socketFunc = NULL ;
		WinAPIData.WinSockFunc.sendFunc = NULL ;
		WinAPIData.WinSockFunc.sendtoFunc = NULL ;
		WinAPIData.WinSockFunc.recvfromFunc = NULL ;
		WinAPIData.WinSockFunc.acceptFunc = NULL ;
		WinAPIData.WinSockFunc.closesocketFunc = NULL ;
		WinAPIData.WinSockFunc.shutdownFunc = NULL ;
		WinAPIData.WinSockFunc.listenFunc = NULL ;
		WinAPIData.WinSockFunc.bindFunc = NULL ;
		WinAPIData.WinSockFunc.inet_addrFunc = NULL ;
		WinAPIData.WinSockFunc.recvFunc = NULL ;
		WinAPIData.WinSockFunc.setsockoptFunc = NULL ;
	}
#endif

#ifndef DX_NON_KEYEX
	// Imm32DLL を解放する
	if( WinAPIData.ImmFunc.Imm32DLL )
	{
		FreeLibrary( WinAPIData.ImmFunc.Imm32DLL ) ;
		WinAPIData.ImmFunc.Imm32DLL = NULL ;
		WinAPIData.ImmFunc.ImmGetContextFunc = NULL ;
		WinAPIData.ImmFunc.ImmReleaseContextFunc = NULL ;
		WinAPIData.ImmFunc.ImmGetOpenStatusFunc = NULL ;
		WinAPIData.ImmFunc.ImmGetConversionStatusFunc = NULL ;
		WinAPIData.ImmFunc.ImmNotifyIMEFunc = NULL ;
		WinAPIData.ImmFunc.ImmSetOpenStatusFunc = NULL ;

		WinAPIData.ImmFunc.ImmGetCandidateListFunc = NULL ;
		WinAPIData.ImmFunc.ImmGetCompositionStringFunc = NULL ;
		WinAPIData.ImmFunc.ImmSetCompositionStringFunc = NULL ;
	}
#endif

	// WinMM DLL を解放する
	if( WinAPIData.Win32Func.WinMMDLL )
	{
		FreeLibrary( WinAPIData.Win32Func.WinMMDLL ) ;
		WinAPIData.Win32Func.WinMMDLL = NULL ;
		WinAPIData.Win32Func.timeSetEventFunc = NULL ;
		WinAPIData.Win32Func.timeKillEventFunc = NULL ;
		WinAPIData.Win32Func.timeBeginPeriodFunc = NULL ;
		WinAPIData.Win32Func.timeEndPeriodFunc = NULL ;
		WinAPIData.Win32Func.timeGetTimeFunc = NULL ;
		WinAPIData.Win32Func.timeGetDevCapsFunc = NULL ;
		WinAPIData.Win32Func.joyGetPosExFunc = NULL ;

		WinAPIData.Win32Func.joyGetDevCapsFunc = NULL ;
		WinAPIData.Win32Func.mciSendCommandFunc = NULL ;
	}

	// Kernel32 DLL を解放する
	if( WinAPIData.Win32Func.Kernel32DLL )
	{
		FreeLibrary( WinAPIData.Win32Func.Kernel32DLL ) ;
		WinAPIData.Win32Func.Kernel32DLL = NULL ;
		WinAPIData.Win32Func.GetModuleHandleWFunc = NULL ;
	}

	// Old32 DLL を解放する
	if( WinAPIData.Win32Func.Old32DLL )
	{
		FreeLibrary( WinAPIData.Win32Func.Old32DLL ) ;
		WinAPIData.Win32Func.Old32DLL = NULL ;
		WinAPIData.Win32Func.CoCreateInstanceFunc = NULL ;
		WinAPIData.Win32Func.CoTaskMemAllocFunc = NULL ;
		WinAPIData.Win32Func.CoTaskMemFreeFunc = NULL ;
		WinAPIData.Win32Func.CoInitializeExFunc = NULL ;
		WinAPIData.Win32Func.CoFreeUnusedLibrariesFunc = NULL ;
		WinAPIData.Win32Func.CoUninitializeFunc = NULL ;
	}

	// OleAut32 DLL を解放する
	if( WinAPIData.Win32Func.OleAut32DLL )
	{
		FreeLibrary( WinAPIData.Win32Func.OleAut32DLL ) ;
		WinAPIData.Win32Func.SysFreeStringFunc = NULL ;
	}

	// Comctl32 DLL を解放する
	if( WinAPIData.Win32Func.Comctl32DLL )
	{
		FreeLibrary( WinAPIData.Win32Func.Comctl32DLL ) ;
		WinAPIData.Win32Func.Comctl32DLL = NULL ;
		WinAPIData.Win32Func.InitCommonControlsFunc = NULL ;
	}

	// GDI32 DLL を解放する
	if( WinAPIData.Win32Func.GDI32DLL )
	{
		FreeLibrary( WinAPIData.Win32Func.GDI32DLL ) ;
		WinAPIData.Win32Func.GDI32DLL = NULL ;
		WinAPIData.Win32Func.AddFontMemResourceExFunc = NULL ;
		WinAPIData.Win32Func.RemoveFontMemResourceExFunc = NULL ;
		WinAPIData.Win32Func.GetFontUnicodeRangesFunc = NULL ;
	}

	// NT DLL を解放する
	if( WinAPIData.Win32Func.NTDLL )
	{
		FreeLibrary( WinAPIData.Win32Func.NTDLL ) ;
		WinAPIData.Win32Func.NTDLL = NULL ;
		WinAPIData.Win32Func.RtlGetVersionFunc = NULL ;
	}

	// Mfplat DLL を解放する
	if( WinAPIData.Win32Func.MFPLATDLL )
	{
		FreeLibrary( WinAPIData.Win32Func.MFPLATDLL ) ;
		WinAPIData.Win32Func.MFPLATDLL = NULL ;
		WinAPIData.Win32Func.MFStartupFunc = NULL ;
		WinAPIData.Win32Func.MFShutdownFunc = NULL ;
		WinAPIData.Win32Func.MFCreateMediaTypeFunc = NULL ;
		WinAPIData.Win32Func.MFCreateWaveFormatExFromMFMediaTypeFunc = NULL ;
		WinAPIData.Win32Func.MFCreateAttributesFunc = NULL ;
		WinAPIData.Win32Func.MFCreateAsyncResultFunc = NULL ;
		WinAPIData.Win32Func.MFInvokeCallbackFunc = NULL ;
	}

	// Mfreadwrite DLL を解放する
	if( WinAPIData.Win32Func.MFREADWRITEDLL )
	{
		FreeLibrary( WinAPIData.Win32Func.MFREADWRITEDLL ) ;
		WinAPIData.Win32Func.MFREADWRITEDLL = NULL ;
		WinAPIData.Win32Func.MFCreateSourceReaderFromURLFunc = NULL ;
		WinAPIData.Win32Func.MFCreateSourceReaderFromByteStreamFunc = NULL ;
	}

	// Propsys DLL を解放する
	if( WinAPIData.Win32Func.PROPSYSDLL )
	{
		FreeLibrary( WinAPIData.Win32Func.PROPSYSDLL ) ;
		WinAPIData.Win32Func.PROPSYSDLL = NULL ;
		WinAPIData.Win32Func.PropVariantToInt64Func = NULL ;
	}

	// 初期化フラグを倒す
	WinAPIData.InitializeFlag = FALSE ;

	// 終了
	return 0 ;
}













#ifndef DX_NON_ASYNCLOAD
extern int WinAPI_CoCreateInstance_ASyncCallback( ASYNCLOAD_MAINTHREAD_REQUESTINFO *Info )
{
	GUID rclsid ;
	GUID riid ;

	rclsid.Data1      = ( unsigned long  )    Info->Data[ 0 ] ;
	rclsid.Data2      = ( unsigned short )(   Info->Data[ 1 ] & 0xffff ) ;
	rclsid.Data3      = ( unsigned short )( ( Info->Data[ 1 ] >> 16 ) & 0xffff ) ;
	rclsid.Data4[ 0 ] = ( unsigned char  )(   Info->Data[ 2 ]         & 0xff ) ;
	rclsid.Data4[ 1 ] = ( unsigned char  )( ( Info->Data[ 2 ] >>  8 ) & 0xff ) ;
	rclsid.Data4[ 2 ] = ( unsigned char  )( ( Info->Data[ 2 ] >> 16 ) & 0xff ) ;
	rclsid.Data4[ 3 ] = ( unsigned char  )( ( Info->Data[ 2 ] >> 24 ) & 0xff ) ;
	rclsid.Data4[ 4 ] = ( unsigned char  )(   Info->Data[ 3 ]         & 0xff ) ;
	rclsid.Data4[ 5 ] = ( unsigned char  )( ( Info->Data[ 3 ] >>  8 ) & 0xff ) ;
	rclsid.Data4[ 6 ] = ( unsigned char  )( ( Info->Data[ 3 ] >> 16 ) & 0xff ) ;
	rclsid.Data4[ 7 ] = ( unsigned char  )( ( Info->Data[ 3 ] >> 24 ) & 0xff ) ;

	riid.Data1        = ( unsigned long  )    Info->Data[ 6 ] ;
	riid.Data2        = ( unsigned short )(   Info->Data[ 7 ] & 0xffff ) ;
	riid.Data3        = ( unsigned short )( ( Info->Data[ 7 ] >> 16 ) & 0xffff ) ;
	riid.Data4[ 0 ]   = ( unsigned char  )(   Info->Data[ 8 ]         & 0xff ) ;
	riid.Data4[ 1 ]   = ( unsigned char  )( ( Info->Data[ 8 ] >>  8 ) & 0xff ) ;
	riid.Data4[ 2 ]   = ( unsigned char  )( ( Info->Data[ 8 ] >> 16 ) & 0xff ) ;
	riid.Data4[ 3 ]   = ( unsigned char  )( ( Info->Data[ 8 ] >> 24 ) & 0xff ) ;
	riid.Data4[ 4 ]   = ( unsigned char  )(   Info->Data[ 9 ]         & 0xff ) ;
	riid.Data4[ 5 ]   = ( unsigned char  )( ( Info->Data[ 9 ] >>  8 ) & 0xff ) ;
	riid.Data4[ 6 ]   = ( unsigned char  )( ( Info->Data[ 9 ] >> 16 ) & 0xff ) ;
	riid.Data4[ 7 ]   = ( unsigned char  )( ( Info->Data[ 9 ] >> 24 ) & 0xff ) ;

	return WinAPI_CoCreateInstance_ASync(
		rclsid,
		( D_IUnknown * )Info->Data[ 4 ],
		( DWORD )Info->Data[ 5 ],
		riid,
		( LPVOID * )Info->Data[ 10 ],
		FALSE ) ;
}
#endif // DX_NON_ASYNCLOAD

extern HRESULT WinAPI_CoCreateInstance_ASync( REFCLSID rclsid, D_IUnknown * pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv, int ASyncThread )
{
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		ASYNCLOAD_MAINTHREAD_REQUESTINFO AInfo ;

		AInfo.Function = WinAPI_CoCreateInstance_ASyncCallback ;
		AInfo.Data[  0 ] = ( DWORD_PTR )rclsid.Data1 ;
		AInfo.Data[  1 ] = ( DWORD_PTR )( rclsid.Data2 | ( ( ( DWORD )rclsid.Data3 ) ) << 16 );
		AInfo.Data[  2 ] = ( DWORD_PTR )( rclsid.Data4[ 0 ] | ( ( ( DWORD )rclsid.Data4[ 1 ] ) << 8 ) | ( ( ( DWORD )rclsid.Data4[ 2 ] ) << 16 ) | ( ( ( DWORD )rclsid.Data4[ 3 ] ) << 24 ) ) ;
		AInfo.Data[  3 ] = ( DWORD_PTR )( rclsid.Data4[ 4 ] | ( ( ( DWORD )rclsid.Data4[ 5 ] ) << 8 ) | ( ( ( DWORD )rclsid.Data4[ 6 ] ) << 16 ) | ( ( ( DWORD )rclsid.Data4[ 7 ] ) << 24 ) ) ;
		AInfo.Data[  4 ] = ( DWORD_PTR )pUnkOuter ;
		AInfo.Data[  5 ] = ( DWORD_PTR )dwClsContext ;
		AInfo.Data[  6 ] = ( DWORD_PTR )riid.Data1 ;
		AInfo.Data[  7 ] = ( DWORD_PTR )( riid.Data2 | ( ( ( DWORD )riid.Data3 ) ) << 16 );
		AInfo.Data[  8 ] = ( DWORD_PTR )( riid.Data4[ 0 ] | ( ( ( DWORD )riid.Data4[ 1 ] ) << 8 ) | ( ( ( DWORD )riid.Data4[ 2 ] ) << 16 ) | ( ( ( DWORD )riid.Data4[ 3 ] ) << 24 ) ) ;
		AInfo.Data[  9 ] = ( DWORD_PTR )( riid.Data4[ 4 ] | ( ( ( DWORD )riid.Data4[ 5 ] ) << 8 ) | ( ( ( DWORD )riid.Data4[ 6 ] ) << 16 ) | ( ( ( DWORD )riid.Data4[ 7 ] ) << 24 ) ) ;
		AInfo.Data[ 10 ] = ( DWORD_PTR )ppv ;
		return AddASyncLoadRequestMainThreadInfo( &AInfo ) ;
	}
#endif // DX_NON_ASYNCLOAD

	return WinAPIData.Win32Func.CoCreateInstanceFunc( rclsid, pUnkOuter, dwClsContext, riid, ppv );
}


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

