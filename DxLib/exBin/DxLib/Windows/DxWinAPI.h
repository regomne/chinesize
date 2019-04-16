// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		WinAPIプログラムヘッダファイル
// 
// 				Ver 3.20c
// 
// -------------------------------------------------------------------------------

#ifndef __DXWINAPI_H__
#define __DXWINAPI_H__

// インクルード ------------------------------------------------------------------
#include "../DxCompileConfig.h"

#if !defined(CINTERFACE) && defined(__c2__) &&  __clang_major__ == 3 && __clang_minor__ == 8
//To avoid compile error
//C:\Program Files (x86)\Windows Kits\8.1\Include\um\combaseapi.h(229,21): error : unknown type name 'IUnknown'
//          static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
#define CINTERFACE
#endif
#include <windows.h>

#if _MSC_VER == 1200
#include <multimon.h>
#endif

#include <winsock.h>
#include <wtypes.h>
#include "../DxNetwork.h"
#include "DxDirectX.h"

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define D_HTOUCHINPUT		void *

// API が初期化されていなかったら初期化するマクロ
#define SETUP_WIN_API		if( WinAPIData.InitializeFlag == FALSE ){ LoadWinAPI() ; }

// コールバック関数定義 ----------------------------------------------------------

typedef VOID ( CALLBACK* D_TIMERPROC )( HWND, UINT, UINT_PTR, DWORD ) ;

#if _MSC_VER > 1200 || defined( DX_GCC_COMPILE_4_9_2 )
typedef void ( CALLBACK D_TIMECALLBACK )( UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2 ) ;
#else
typedef void ( CALLBACK D_TIMECALLBACK )( UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 ) ;
#endif
typedef D_TIMECALLBACK FAR *LPD_TIMECALLBACK ;

// 構造体定義 --------------------------------------------------------------------

typedef struct D_tagWCRANGE
{
	WCHAR					wcLow ;
	USHORT					cGlyphs ;
} D_WCRANGE, *D_PWCRANGE,FAR *D_LPWCRANGE ;

typedef struct D_tagGLYPHSET
{
	DWORD					cbThis ;
	DWORD					flAccel ;
	DWORD					cGlyphsSupported ;
	DWORD					cRanges ;
	D_WCRANGE				ranges[ 1 ] ;
} D_GLYPHSET, *D_PGLYPHSET, FAR *D_LPGLYPHSET;

typedef struct D__TOUCHINPUT
{
	LONG					x ;
	LONG					y ;
	HANDLE					hSource ;
	DWORD					dwID ;
	DWORD					dwFlags ;
	DWORD					dwMask ;
	DWORD					dwTime ;
#ifdef _WIN64
	ULONGLONG				dwExtraInfo ;
#else
	DWORD					dwExtraInfo;
#endif
	DWORD					cxContact ;
	DWORD					cyContact ;
} D_TOUCHINPUT, *D_PTOUCHINPUT ;
typedef D_TOUCHINPUT const * D_PCTOUCHINPUT;

#ifndef DX_NON_NETWORK

// WinSock の DLL のポインタや中のAPIのポインタなど
struct WINSOCKFUNCTION
{
	HMODULE					WinSockDLL ;						// WinSockDLL
	int						( WINAPI *WSAGetLastErrorFunc )( void ) ;
	int						( WINAPI *WSAStartupFunc )( WORD wVersionRequested, LPWSADATA lpWSAData ) ;
	int						( WINAPI *WSACleanupFunc )( void ) ;
	int						( WINAPI *WSAAsyncSelectFunc )( SOCKET s, HWND hWnd, unsigned int wMsg, long lEvent ) ;
	int						( WINAPI *getaddrinfoFunc )( const char *nodename, const char *servname, const _addrinfo *hints, _addrinfo **res ) ;
	void					( WINAPI *freeaddrinfoFunc )( _addrinfo *pAddrInfo ) ;
	struct hostent*			( WINAPI *gethostbyaddrFunc )( const char *addr, int len, int type ) ;
	struct hostent*			( WINAPI *gethostbynameFunc )( const char *name ) ;
	int						( WINAPI *gethostnameFunc )( char *name, int namelen ) ;
	u_short					( WINAPI *ntohsFunc )( u_short netshort ) ;
	u_short					( WINAPI *htonsFunc )( u_short hostshort ) ;
	int						( WINAPI *connectFunc )( SOCKET s, const struct sockaddr *name, int namelen ) ;
	SOCKET					( WINAPI *socketFunc )( int af, int type, int protocol ) ;
	int						( WINAPI *sendFunc )( SOCKET s, const char *buf, int len, int flags ) ;
	int						( WINAPI *sendtoFunc )( SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen ) ;
	int						( WINAPI *recvfromFunc )( SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen ) ;
	SOCKET					( WINAPI *acceptFunc )( SOCKET s, struct sockaddr *addr, int *addrlen ) ;
	int						( WINAPI *closesocketFunc )( SOCKET s ) ;
	int						( WINAPI *shutdownFunc )( SOCKET s, int how ) ;
	int						( WINAPI *listenFunc )( SOCKET s, int backlog ) ;
	int						( WINAPI *bindFunc )( SOCKET s, const struct sockaddr *name, int namelen ) ;
	unsigned long			( WINAPI *inet_addrFunc )( const char *cp ) ;
	int						( WINAPI *recvFunc )( SOCKET s, char *buf, int len, int flags ) ;
	int						( WINAPI *setsockoptFunc )( SOCKET s, int level, int optname, const char *optval, int optlen ) ;
} ;

#endif // DX_NON_NETWORK

// Input Method Manager DLL のポインタや中のAPIのポインタなど
struct IMMFUNCTION
{
	HMODULE					Imm32DLL ;
	HIMC					( WINAPI *ImmGetContextFunc )( HWND hWnd );
	BOOL					( WINAPI *ImmReleaseContextFunc )( HWND hWnd, HIMC hIMC ) ;
	BOOL					( WINAPI *ImmGetOpenStatusFunc )( HIMC hIMC ) ;
	BOOL					( WINAPI *ImmGetConversionStatusFunc )( HIMC hIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence ) ;
	BOOL					( WINAPI *ImmNotifyIMEFunc )( HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue ) ;
	BOOL					( WINAPI *ImmSetOpenStatusFunc )( HIMC hIMC, BOOL fOpen ) ;

	DWORD					( WINAPI *ImmGetCandidateListFunc )( HIMC hIMC, DWORD deIndex, LPCANDIDATELIST lpCandList, DWORD dwBufLen ) ;
	DWORD					( WINAPI *ImmGetCandidateListCountFunc )( HIMC hIMC, LPDWORD lpdwListCount ) ;
	LONG					( WINAPI *ImmGetCompositionStringFunc )( HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen ) ;
	BOOL					( WINAPI *ImmSetCompositionStringFunc )( HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen ) ;
} ;

// Win32 API DLL のポインタや API のポインタなど
struct WIN32APIFUNCTION
{
	HMODULE					WinMMDLL ;
	MMRESULT				( WINAPI *timeSetEventFunc )( UINT uDelay, UINT uResolution, LPD_TIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent ) ;
	MMRESULT				( WINAPI *timeKillEventFunc )( UINT uTimerID ) ;
	MMRESULT				( WINAPI *timeBeginPeriodFunc )( UINT uPeriod ) ;
	MMRESULT				( WINAPI *timeEndPeriodFunc )( UINT uPeriod ) ;
	DWORD					( WINAPI *timeGetTimeFunc )( VOID ) ;
	MMRESULT				( WINAPI *timeGetDevCapsFunc )( LPTIMECAPS ptc, UINT cbtc ) ;
	MMRESULT				( WINAPI *joyGetPosExFunc )( UINT uJoyID, LPJOYINFOEX pji ) ;

	MMRESULT				( WINAPI *joyGetDevCapsFunc )( UINT uJoyID, LPJOYCAPSW pjc, UINT cbjc ) ;
	MCIERROR				( WINAPI *mciSendCommandFunc )( MCIDEVICEID IDDevice, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam ) ;


	HMODULE					Kernel32DLL ;
	HMODULE					( WINAPI *GetModuleHandleWFunc )( LPCWSTR lpModuleName ) ;
	BOOL					( WINAPI *VerifyVersionInfoWFunc )( LPOSVERSIONINFOEXW lpVersionInformation, DWORD dwTypeMask, DWORDLONG dwlConditionMask ) ;
	ULONGLONG				( WINAPI *VerSetConditionMaskFunc )( ULONGLONG ConditionMask, DWORD TypeMask, BYTE Condition ) ;
	HANDLE					( WINAPI *CreateEventAFunc )( LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName ) ;
	BOOL					( WINAPI *SetEventFunc )( HANDLE hEvent ) ;
	BOOL					( WINAPI *ResetEventFunc )( HANDLE hEvent ) ;
	HANDLE					( WINAPI *CreateThreadFunc )( LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId ) ;
	BOOL					( WINAPI *SetThreadPriorityFunc )( HANDLE hThread, int nPriority );
	int						( WINAPI *GetThreadPriorityFunc )( HANDLE hThread ) ;
	VOID					( WINAPI *ExitThreadFunc )( DWORD dwExitCode ) ;
	BOOL					( WINAPI *GetExitCodeThreadFunc )( HANDLE hThread, LPDWORD lpExitCode ) ;
	HANDLE					( WINAPI *GetCurrentThreadFunc )( VOID ) ;
	DWORD					( WINAPI *GetCurrentThreadIdFunc )( VOID ) ;
	DWORD					( WINAPI *SuspendThreadFunc )( HANDLE hThread ) ;
	DWORD					( WINAPI *ResumeThreadFunc )( HANDLE hThread ) ;
	DWORD					( WINAPI *WaitForSingleObjectFunc )( HANDLE hHandle, DWORD dwMilliseconds ) ;
	DWORD					( WINAPI *WaitForMultipleObjectsFunc )( DWORD nCount, CONST HANDLE * lpHandles, BOOL bWaitAll, DWORD dwMilliseconds ) ;
	HANDLE					( WINAPI *GetProcessHeapFunc )( VOID ) ;
	LPVOID					( WINAPI *HeapAllocFunc )( HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes ) ;
	BOOL					( WINAPI *HeapFreeFunc )( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem ) ;
	LPVOID					( WINAPI *VirtualAllocFunc )( LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect ) ;
	BOOL					( WINAPI *VirtualFreeFunc )( LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType ) ;
	SIZE_T					( WINAPI *VirtualQueryFunc )( LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength ) ;
	HGLOBAL					( WINAPI *GlobalAllocFunc )( UINT uFlags, SIZE_T dwBytes ) ;
	HGLOBAL					( WINAPI *GlobalFreeFunc )( HGLOBAL hMem ) ;
	LPVOID					( WINAPI *GlobalLockFunc )( HGLOBAL hMem ) ;
	BOOL					( WINAPI *GlobalUnlockFunc )( HGLOBAL hMem ) ;
	SIZE_T					( WINAPI *GlobalSizeFunc )( HGLOBAL hMem ) ;
	HANDLE					( WINAPI *CreateSemaphoreAFunc )( LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCSTR lpName ) ;
	BOOL					( WINAPI *ReleaseSemaphoreFunc )( HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount ) ;
	BOOL					( WINAPI *GetVersionExAFunc )( LPOSVERSIONINFOA lpVersionInformation ) ;
	BOOL					( WINAPI *GetVersionExWFunc )( LPOSVERSIONINFOW lpVersionInformation ) ;
	VOID					( WINAPI *GetSystemInfoFunc )( LPSYSTEM_INFO lpSystemInfo ) ;
	int						( WINAPI *lstrlenAFunc )( LPCSTR lpString ) ;
	int						( WINAPI *lstrlenWFunc )( LPCWSTR lpString ) ;
	int						( WINAPI *lstrcmpAFunc )( LPCSTR lpString1, LPCSTR lpString2 ) ;
	int						( WINAPI *lstrcmpWFunc )( LPCWSTR lpString1, LPCWSTR lpString2 ) ;
	LPSTR					( WINAPI *lstrcpyAFunc )( LPSTR lpString1, LPCSTR lpString2 ) ;
	LPWSTR					( WINAPI *lstrcpyWFunc )( LPWSTR lpString1, LPCWSTR lpString2 ) ;
	LPSTR					( WINAPI *lstrcpynAFunc )( LPSTR lpString1, LPCSTR lpString2, int iMaxLength ) ;
	LPWSTR					( WINAPI *lstrcpynWFunc )( LPWSTR lpString1, LPCWSTR lpString2, int iMaxLength ) ;
	DWORD					( WINAPI *GetTickCountFunc )( VOID ) ;
	BOOL					( WINAPI *QueryPerformanceCounterFunc )( LARGE_INTEGER * lpPerformanceCount ) ;
	BOOL					( WINAPI *QueryPerformanceFrequencyFunc )( LARGE_INTEGER * lpFrequency ) ;
	BOOL					( WINAPI *CloseHandleFunc )( HANDLE hObject ) ;
	DWORD					( WINAPI *GetTempPathAFunc )( DWORD nBufferLength, LPSTR lpBuffer ) ;
	DWORD					( WINAPI *GetTempPathWFunc )( DWORD nBufferLength, LPWSTR lpBuffer ) ;
	UINT					( WINAPI *GetTempFileNameWFunc )( LPCWSTR lpPathName, LPCWSTR lpPrefixString, UINT uUnique, LPWSTR lpTempFileName ) ;
	HANDLE					( WINAPI *CreateFileAFunc )( LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) ;
	HANDLE					( WINAPI *CreateFileWFunc )( LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) ;
	BOOL					( WINAPI *DeleteFileAFunc )( LPCSTR lpFileName ) ;
	BOOL					( WINAPI *DeleteFileWFunc )( LPCWSTR lpFileName ) ;
	DWORD					( WINAPI *GetFileSizeFunc )( HANDLE hFile, LPDWORD lpFileSizeHigh ) ;
	BOOL					( WINAPI *WriteFileFunc )( HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped ) ;
	BOOL					( WINAPI *ReadFileFunc )( HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped ) ;
	DWORD					( WINAPI *SetFilePointerFunc )( HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod ) ;
	HANDLE					( WINAPI *FindFirstFileWFunc )( LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData ) ;
	BOOL					( WINAPI *FindNextFileWFunc )( HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData ) ;
	BOOL					( WINAPI *FindCloseFunc )( HANDLE hFindFile ) ;
	BOOL					( WINAPI *SetCurrentDirectoryWFunc )( LPCWSTR lpPathName ) ;
	DWORD					( WINAPI *GetCurrentDirectoryWFunc )( DWORD nBufferLength, LPWSTR lpBuffer ) ;
	VOID					( WINAPI *SleepFunc )( DWORD dwMilliseconds ) ;
	VOID					( WINAPI *InitializeCriticalSectionFunc )( LPCRITICAL_SECTION lpCriticalSection ) ;
	VOID					( WINAPI *DeleteCriticalSectionFunc )( LPCRITICAL_SECTION lpCriticalSection ) ;
	VOID					( WINAPI *EnterCriticalSectionFunc )( LPCRITICAL_SECTION lpCriticalSection ) ;
	VOID					( WINAPI *LeaveCriticalSectionFunc )( LPCRITICAL_SECTION lpCriticalSection ) ;
	VOID					( WINAPI *ExitProcessFunc )( UINT uExitCode ) ;
	HGLOBAL					( WINAPI *LoadResourceFunc )( HMODULE hModule, HRSRC hResInfo ) ;
	HRSRC					( WINAPI *FindResourceWFunc )( HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType ) ;
	LPVOID					( WINAPI *LockResourceFunc )( HGLOBAL hResData ) ;
	DWORD					( WINAPI *SizeofResourceFunc )( HMODULE hModule, HRSRC hResInfo ) ;
	UINT					( WINAPI *GetOEMCPFunc )( void ) ;
	VOID					( WINAPI *OutputDebugStringWFunc )( LPCWSTR lpOutputString ) ;
	VOID					( WINAPI *GlobalMemoryStatusFunc )( LPMEMORYSTATUS lpBuffer ) ;
	int						( WINAPI *MulDivFunc )( int nNumber, int nNumerator, int nDenominator ) ;
	VOID					( WINAPI *GetLocalTimeFunc )( LPSYSTEMTIME lpSystemTime ) ;
	BOOL					( WINAPI *GetSystemPowerStatusFunc )( LPSYSTEM_POWER_STATUS lpSystemPowerStatus ) ;
	BOOL					( WINAPI *FileTimeToLocalFileTimeFunc )( FILETIME * lpFileTime, LPFILETIME lpLocalFileTime ) ;
	BOOL					( WINAPI *FileTimeToSystemTimeFunc )( FILETIME * lpFileTime, LPSYSTEMTIME lpSystemTime ) ;
	DWORD					( WINAPI *GetLastErrorFunc )( VOID ) ;


	HMODULE					Old32DLL ;
	HRESULT					( WINAPI *CoCreateInstanceFunc )( REFCLSID rclsid, D_IUnknown * pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv ) ;
	LPVOID					( WINAPI *CoTaskMemAllocFunc )( SIZE_T cb ) ;
	void					( WINAPI *CoTaskMemFreeFunc )( LPVOID pv ) ;
	HRESULT					( WINAPI *CoInitializeExFunc )( LPVOID pvReserved, DWORD dwCoInit ) ;
	void					( WINAPI *CoFreeUnusedLibrariesFunc )( void ) ;
	void					( WINAPI *CoUninitializeFunc )( void ) ;


	HMODULE					OleAut32DLL ;
	void					( WINAPI *SysFreeStringFunc )( BSTR bstrString ) ;


	HMODULE					Comctl32DLL ;
	void					( WINAPI *InitCommonControlsFunc )( VOID ) ;


	HMODULE					User32DLL ;
	BOOL					( WINAPI *WINNLSEnableIME_Func )( HWND hwnd, BOOL bFlag ) ;	// WINNLSEnableIME APIのアドレス
	BOOL					( WINAPI *UpdateLayeredWindow )( HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD ) ;		// UpdateLayeredWindow のＡＰＩポインタ
	HWND					( WINAPI *CreateWindowExAFunc )( DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam ) ;
	HWND					( WINAPI *CreateWindowExWFunc )( DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam ) ;
	BOOL					( WINAPI *DestroyWindowFunc )( HWND hWnd ) ;
	BOOL					( WINAPI *ShowWindowFunc )( HWND hWnd, int nCmdShow ) ;
	BOOL					( WINAPI *UpdateWindowFunc )( HWND hWnd ) ;
	BOOL					( WINAPI *BringWindowToTopFunc )( HWND hWnd ) ;
	BOOL					( WINAPI *SetForegroundWindowFunc )( HWND hWnd ) ;
	DWORD					( WINAPI *GetWindowThreadProcessIdFunc )( HWND hWnd, LPDWORD lpdwProcessId ) ;
	HWND					( WINAPI *SetActiveWindowFunc )( HWND hWnd ) ;
	HWND					( WINAPI *GetForegroundWindowFunc )( VOID ) ;
	HWND					( WINAPI *GetDesktopWindowFunc )( VOID ) ;
	HWND					( WINAPI *FindWindowWFunc )( LPCWSTR lpClassName, LPCWSTR lpWindowName ) ;
	BOOL					( WINAPI *MoveWindowFunc )( HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint ) ;
	BOOL					( WINAPI *SetWindowPosFunc )( HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags ) ;
	BOOL					( WINAPI *GetClientRectFunc )( HWND hWnd, LPRECT lpRect ) ;
	BOOL					( WINAPI *AdjustWindowRectExFunc )( LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle ) ;
	BOOL					( WINAPI *GetWindowRectFunc )( HWND hWnd, LPRECT lpRect ) ;
	LONG					( WINAPI *SetWindowLongAFunc )( HWND hWnd, int nIndex, LONG dwNewLong ) ;
	LONG					( WINAPI *SetWindowLongWFunc )( HWND hWnd, int nIndex, LONG dwNewLong ) ;
	LONG_PTR				( WINAPI *SetWindowLongPtrAFunc )( HWND hWnd, int nIndex, LONG_PTR dwNewLong ) ;
	LONG_PTR				( WINAPI *SetWindowLongPtrWFunc )( HWND hWnd, int nIndex, LONG_PTR dwNewLong ) ;
	LONG					( WINAPI *GetWindowLongAFunc )( HWND hWnd, int nIndex ) ;
	LONG					( WINAPI *GetWindowLongWFunc )( HWND hWnd, int nIndex ) ;
	LONG_PTR				( WINAPI *GetWindowLongPtrAFunc )( HWND hWnd, int nIndex ) ;
	LONG_PTR				( WINAPI *GetWindowLongPtrWFunc )( HWND hWnd, int nIndex ) ;
	BOOL					( WINAPI *SetWindowTextWFunc )( HWND hWnd, LPCWSTR lpString ) ;
	BOOL					( WINAPI *ClientToScreenFunc )( HWND hWnd, LPPOINT lpPoint ) ;
	BOOL					( WINAPI *EnumDisplayDevicesWFunc )( LPCWSTR lpDevice, DWORD iDevNum, PDISPLAY_DEVICEW lpDisplayDevice, DWORD dwFlags ) ;
	BOOL					( WINAPI *CloseTouchInputHandleFunc )( D_HTOUCHINPUT hTouchInput ) ;
	BOOL					( WINAPI *GetTouchInputInfoFunc )( D_HTOUCHINPUT hTouchInput, UINT cInputs, D_PTOUCHINPUT pInputs, int cbSize ) ;
	BOOL					( WINAPI *IsTouchWindowFunc )( HWND hWnd, ULONG *pulFlags ) ;
	BOOL					( WINAPI *RegisterTouchWindowFunc )( HWND hWnd, ULONG ulFlags ) ;
	BOOL					( WINAPI *UnregisterTouchWindowFunc )( HWND hWnd ) ;
	BOOL					( WINAPI *ShutdownBlockReasonCreateFunc )( HWND hWnd, LPCWSTR pwszReason ) ;
	BOOL					( WINAPI *ShutdownBlockReasonDestroyFunc )( HWND hWnd ) ;
	UINT_PTR				( WINAPI *SetTimerFunc )( HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, D_TIMERPROC lpTimerFunc ) ;
	BOOL					( WINAPI *PostThreadMessageAFunc )( DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	BOOL					( WINAPI *PostThreadMessageWFunc )( DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	BOOL					( WINAPI *PeekMessageAFunc )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg ) ;
	BOOL					( WINAPI *PeekMessageWFunc )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg ) ;
	BOOL					( WINAPI *GetMessageAFunc )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax ) ;
	BOOL					( WINAPI *GetMessageWFunc )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax ) ;
	LRESULT					( WINAPI *SendMessageAFunc )( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	LRESULT					( WINAPI *SendMessageWFunc )( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	BOOL					( WINAPI *PostMessageAFunc )( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	BOOL					( WINAPI *PostMessageWFunc )( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	VOID					( WINAPI *PostQuitMessageFunc )( int nExitCode ) ;
	LRESULT					( WINAPI *DefWindowProcAFunc )( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	LRESULT					( WINAPI *DefWindowProcWFunc )( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) ;
	int						( WINAPI *TranslateAcceleratorWFunc )( HWND hWnd, HACCEL hAccTable, LPMSG lpMsg ) ;
	BOOL					( WINAPI *TranslateMessageFunc )( MSG *lpMsg ) ;
	LRESULT					( WINAPI *DispatchMessageAFunc )( MSG *lpMsg ) ;
	LRESULT					( WINAPI *DispatchMessageWFunc )( MSG *lpMsg ) ;
	DWORD					( WINAPI *MsgWaitForMultipleObjectsFunc )( DWORD nCount, CONST HANDLE *pHandles, BOOL fWaitAll, DWORD dwMilliseconds, DWORD dwWakeMask ) ;
	DWORD					( WINAPI *GetQueueStatusFunc )( UINT flags ) ;
	UINT					( WINAPI *RegisterWindowMessageAFunc )( LPCSTR lpString ) ;
	UINT					( WINAPI *RegisterWindowMessageWFunc )( LPCWSTR lpString ) ;
	ATOM					( WINAPI *RegisterClassAFunc )( CONST WNDCLASSA *lpWndClass ) ;
	ATOM					( WINAPI *RegisterClassWFunc )( CONST WNDCLASSW *lpWndClass ) ;
	ATOM					( WINAPI *RegisterClassExAFunc )( CONST WNDCLASSEXA * ) ;
	ATOM					( WINAPI *RegisterClassExWFunc )( CONST WNDCLASSEXW * ) ;
	BOOL					( WINAPI *UnregisterClassAFunc )( LPCSTR lpClassName, HINSTANCE hInstance ) ;
	BOOL					( WINAPI *UnregisterClassWFunc )( LPCWSTR lpClassName, HINSTANCE hInstance ) ;
	DWORD					( WINAPI *SetClassLongWFunc )( HWND hWnd, int nIndex, LONG dwNewLong ) ;
	unsigned LONG_PTR		( WINAPI *SetClassLongPtrWFunc )( HWND hWnd, int nIndex, LONG_PTR dwNewLong ) ;
	HDC						( WINAPI *GetDCFunc )( HWND hWnd ) ;
	int						( WINAPI *ReleaseDCFunc )( HWND hWnd, HDC hDC ) ;
	HDC						( WINAPI *BeginPaintFunc )( HWND hWnd, LPPAINTSTRUCT lpPaint ) ;
	BOOL					( WINAPI *EndPaintFunc )( HWND hWnd, PAINTSTRUCT *lpPaint ) ;
	int						( WINAPI *MessageBoxAFunc )( HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType ) ;
	int						( WINAPI *MessageBoxWFunc )( HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType ) ;
	int						( WINAPI *GetSystemMetricsFunc )( int nIndex ) ;
	LONG					( WINAPI *ChangeDisplaySettingsAFunc )( DEVMODEA* lpDevMode, DWORD dwFlags ) ;
	LONG					( WINAPI *ChangeDisplaySettingsWFunc )( DEVMODEW* lpDevMode, DWORD dwFlags ) ;
	int						( WINAPI *ShowCursorFunc )( BOOL bShow ) ;
	BOOL					( WINAPI *SetCursorPosFunc )( int X, int Y ) ;
	BOOL					( WINAPI *GetCursorPosFunc )( LPPOINT lpPoint ) ;
	HCURSOR					( WINAPI *SetCursorFunc )( HCURSOR hCursor ) ;
	HCURSOR					( WINAPI *LoadCursorWFunc )( HINSTANCE hInstance, LPCWSTR lpCursorName ) ;
	HICON					( WINAPI *LoadIconWFunc )( HINSTANCE hInstance, LPCWSTR lpIconName ) ;
	BOOL					( WINAPI *ClipCursorFunc )( CONST RECT *lpRect ) ;
	HMENU					( WINAPI *LoadMenuWFunc )( HINSTANCE hInstance, LPCWSTR lpMenuName ) ;
	HMENU					( WINAPI *CreateMenuFunc )( VOID ) ;
	BOOL					( WINAPI *DeleteMenuFunc )( HMENU hMenu, UINT uPosition, UINT uFlags ) ;
	BOOL					( WINAPI *DestroyMenuFunc )( HMENU hMenu ) ;
	BOOL					( WINAPI *EnableMenuItemFunc )( HMENU hMenu, UINT uIDEnableItem, UINT uEnable ) ;
	HMENU					( WINAPI *GetSubMenuFunc )( HMENU hMenu, int nPos ) ;
	BOOL					( WINAPI *SetMenuItemInfoWFunc )( HMENU hmenu, UINT item, BOOL fByPositon, LPCMENUITEMINFOW lpmii ) ;
	BOOL					( WINAPI *GetMenuItemInfoWFunc )( HMENU hmenu, UINT item, BOOL fByPosition, LPMENUITEMINFOW lpmii ) ;
	BOOL					( WINAPI *SetMenuFunc )( HWND hWnd, HMENU hMenu ) ;
	BOOL					( WINAPI *DrawMenuBarFunc )( HWND hWnd ) ;
	int						( WINAPI *GetMenuItemCountFunc )( HMENU hMenu ) ;
	BOOL					( WINAPI *InsertMenuItemWFunc )( HMENU hmenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi ) ;
	int						( WINAPI *SetWindowRgnFunc )( HWND hWnd, HRGN hRgn, BOOL bRedraw ) ;
	int						( WINAPI *FillRectFunc )( HDC hDC, CONST RECT *lprc, HBRUSH hbr ) ;
	BOOL					( WINAPI *SystemParametersInfoWFunc )( UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni ) ;
	BOOL					( WINAPI *AttachThreadInputFunc )( DWORD idAttach, DWORD idAttachTo, BOOL fAttach ) ;
	BOOL					( WINAPI *UnhookWindowsHookExFunc )( HHOOK hhk ) ;
	BOOL					( WINAPI *GetKeyboardStateFunc )( PBYTE lpKeyState ) ;
	BOOL					( WINAPI *KillTimerFunc )( HWND hWnd, UINT_PTR uIDEvent ) ;
	BOOL					( WINAPI *EnumDisplaySettingsWFunc )( LPCWSTR lpszDeviceName, DWORD iModeNum, DEVMODEW* lpDevMode ) ;
	BOOL					( WINAPI *EnumDisplayMonitorsFunc )( HDC hdc, LPCRECT lprcClip, MONITORENUMPROC  lpfnEnum, LPARAM dwData ) ;
	BOOL					( WINAPI *IsDialogMessageWFunc )( HWND hDlg, LPMSG lpMsg ) ;
	BOOL					( WINAPI *OpenClipboardFunc )( HWND hWndNewOwner ) ;
	BOOL					( WINAPI *CloseClipboardFunc )( VOID ) ;
	BOOL					( WINAPI *EmptyClipboardFunc )( VOID ) ;
	HANDLE					( WINAPI *SetClipboardDataFunc )( UINT uFormat, HANDLE hMem ) ;
	HANDLE					( WINAPI *GetClipboardDataFunc )( UINT uFormat ) ;
	BOOL					( WINAPI *IsClipboardFormatAvailableFunc )( UINT format ) ;
	HACCEL					( WINAPI *CreateAcceleratorTableWFunc )( LPACCEL paccel, int cAccel ) ;
	BOOL					( WINAPI *DestroyAcceleratorTableFunc )( HACCEL hAccel ) ;
	int						( WINAPI *CopyAcceleratorTableWFunc )( HACCEL hAccelSrc, LPACCEL lpAccelDst, int cAccelEntries ) ;
	BOOL					( WINAPI *GetMonitorInfoWFunc )( HMONITOR hMonitor, LPMONITORINFO lpmi ) ;


	HMODULE					GDI32DLL ;
	HANDLE					( WINAPI *AddFontMemResourceExFunc )( LPVOID pbFont, DWORD cbFont, void /* DESIGNVECTOR */ *pdv, DWORD *pcFonts ) ;
	int						( WINAPI *RemoveFontMemResourceExFunc )( HANDLE fh ) ;
	DWORD					( WINAPI *GetFontUnicodeRangesFunc )( HDC hdc, D_LPGLYPHSET lpgs ) ;
	int						( WINAPI *GetDeviceCapsFunc )( HDC hdc, int index ) ;
	int						( WINAPI *EnumFontFamiliesExAFunc )( HDC hdc, LPLOGFONTA lpLogfont, FONTENUMPROCA lpProc, LPARAM lParam, DWORD dwFlags ) ;
	int						( WINAPI *EnumFontFamiliesExWFunc )( HDC hdc, LPLOGFONTW lpLogfont, FONTENUMPROCW lpProc, LPARAM lParam, DWORD dwFlags ) ;
	HFONT					( WINAPI *CreateFontAFunc )( int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName ) ;
	HFONT					( WINAPI *CreateFontWFunc )( int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName ) ;
	BOOL					( WINAPI *GetTextMetricsWFunc )( HDC hdc, LPTEXTMETRICW lptm ) ;
	BOOL					( WINAPI *GetTextExtentPoint32WFunc )( HDC hdc, LPCWSTR lpString, int c, LPSIZE psizl ) ;
	DWORD					( WINAPI *GetCharacterPlacementWFunc )( HDC hdc, LPCWSTR lpString, int nCount, int nMexExtent, LPGCP_RESULTSW lpResults, DWORD dwFlags ) ;
	DWORD					( WINAPI *GetGlyphOutlineWFunc )( HDC hdc, UINT uChar, UINT fuFormat, LPGLYPHMETRICS lpgm, DWORD cjBuffer, LPVOID pvBuffer, CONST MAT2 *lpmat2 ) ;
	BOOL					( WINAPI *TextOutWFunc )( HDC hdc, int x, int y, LPCWSTR lpString, int c ) ;
	COLORREF				( WINAPI *SetTextColorFunc )( HDC hdc, COLORREF color ) ;
	COLORREF				( WINAPI *SetBkColorFunc )( HDC hdc, COLORREF color ) ;
	int						( WINAPI *SetBkModeFunc )( HDC hdc, int mode ) ;
	HDC						( WINAPI *CreateCompatibleDCFunc )( HDC hdc ) ;
	HDC						( WINAPI *CreateDCWFunc )( LPCWSTR pwszDriver, LPCWSTR pwszDevice, LPCWSTR pszPort, CONST DEVMODEW * pdm ) ;
	int						( WINAPI *GetObjectAFunc )( HANDLE h, int c, LPVOID pv ) ;
	int						( WINAPI *GetObjectWFunc )( HANDLE h, int c, LPVOID pv ) ;
	HGDIOBJ					( WINAPI *GetStockObjectFunc )( int i ) ;
	HGDIOBJ					( WINAPI *SelectObjectFunc )( HDC hdc, HGDIOBJ h ) ;
	BOOL					( WINAPI *DeleteObjectFunc )( HGDIOBJ ho ) ;
	BOOL					( WINAPI *DeleteDCFunc )( HDC hdc ) ;
	HBRUSH					( WINAPI *CreateSolidBrushFunc )( COLORREF color ) ;
	BOOL					( WINAPI *RectangleFunc )( HDC hdc, int left, int top, int right, int bottom ) ;
	BOOL					( WINAPI *BitBltFunc )( HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop ) ;
	HBITMAP					( WINAPI *CreateDIBSectionFunc )( HDC hdc, CONST BITMAPINFO *pbmi, UINT usage, VOID **ppvBits, HANDLE hSection, DWORD offset ) ;
	int						( WINAPI *StretchDIBitsFunc )( HDC hdc, int xDest, int yDest, int DestWidth, int DestHeight, int xSrc, int ySrc, int SrcWidth, int SrcHeight, CONST VOID * lpBits, CONST BITMAPINFO * lpbmi, UINT iUsage, DWORD rop ) ;
	int						( WINAPI *SetDIBitsToDeviceFunc )( HDC hdc, int xDest, int yDest, DWORD w, DWORD h, int xSrc, int ySrc, UINT StartScan, UINT cLines, CONST VOID * lpvBits, CONST BITMAPINFO * lpbmi, UINT ColorUse ) ;
	HRGN					( WINAPI *CreateRectRgnFunc )( int x1, int y1, int x2, int y2 ) ;
	int						( WINAPI *CombineRgnFunc )( HRGN hrgnDst, HRGN hrgnSrc1, HRGN hrgnSrc2, int iMode ) ;
  

	HMODULE					SHELL32DLL ;
	UINT					( WINAPI *DragQueryFileWFunc )( HDROP hDrop, UINT iFile, LPWSTR lpszFile, UINT cch ) ;
	void					( WINAPI *DragFinishFunc )( HDROP hDrop ) ;
	void					( WINAPI *DragAcceptFilesFunc )( HWND hWnd, BOOL fAccept ) ;


	HMODULE					NTDLL ;
	void					( WINAPI *RtlGetVersionFunc )( LPOSVERSIONINFOEXW lpVersionInfomation ) ;


	HMODULE					MFPLATDLL ;
	HRESULT					( WINAPI *MFStartupFunc )( ULONG Version, DWORD dwFlags ) ;
	HRESULT					( WINAPI *MFShutdownFunc )( void ) ;
	HRESULT					( WINAPI *MFCreateMediaTypeFunc )( D_IMFMediaType **ppMFType ) ;
	HRESULT					( WINAPI *MFCreateWaveFormatExFromMFMediaTypeFunc )( D_IMFMediaType *pMFType, WAVEFORMATEX **ppWF, UINT32 *pcbSize, UINT32 Flags ) ;
	HRESULT					( WINAPI *MFCreateAttributesFunc )( D_IMFAttributes** ppMFAttributes, UINT32 cInitialSize ) ;
	HRESULT					( WINAPI *MFCreateAsyncResultFunc )( D_IUnknown * punkObject, D_IMFAsyncCallback * pCallback, D_IUnknown * punkState, D_IMFAsyncResult ** ppAsyncResult ) ;
	HRESULT					( WINAPI *MFInvokeCallbackFunc )( D_IMFAsyncResult * pAsyncResult ) ;


	HMODULE					MFREADWRITEDLL ;
	HRESULT					( WINAPI *MFCreateSourceReaderFromURLFunc )( LPCWSTR pwszURL, D_IMFAttributes *pAttributes, D_IMFSourceReader **ppSourceReader ) ;
	HRESULT					( WINAPI *MFCreateSourceReaderFromByteStreamFunc )( D_IMFByteStream *pByteStream, D_IMFAttributes *pAttributes, D_IMFSourceReader **ppSourceReader ) ;


	HMODULE					PROPSYSDLL ;
	HRESULT					( WINAPI *PropVariantToInt64Func )( const D_PROPVARIANT &propvarIn, LONGLONG *pllRet ) ;
} ;

// WinAPI 情報構造体
struct WINAPIDATA
{
	volatile int			InitializeFlag ;

#ifndef DX_NON_NETWORK
	WINSOCKFUNCTION			WinSockFunc ;						// WinSock API 関係のデータ
#endif

#ifndef DX_NON_KEYEX
	IMMFUNCTION				ImmFunc ;							// IMM API 関係のデータ
#endif

	WIN32APIFUNCTION		Win32Func ;							// Win32 API 関係のデータ

	HMODULE					DwmApiDLL ;							// Desktop Window Manager API DLL
	HRESULT					( WINAPI *DF_DwmEnableComposition )( UINT uCompositionAction ) ;	// DwmEnableComposition API のアドレス
	HRESULT					( WINAPI *DwmGetWindowAttributeFunc )( HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute ) ;
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern WINAPIDATA WinAPIData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

extern int LoadWinAPI( void ) ;				// WindowsOS の DLL を読み込む
extern int ReleaseWinAPI( void ) ;			// WindowsOS の DLL を解放する

extern HRESULT WinAPI_CoCreateInstance_ASync( REFCLSID rclsid, D_IUnknown * pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv, int ASyncThread = FALSE ) ;

#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // __DXWINAPI_H__
