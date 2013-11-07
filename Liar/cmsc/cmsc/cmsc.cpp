// cmsc.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "cmsc.h"
#include<assert.h>

#define MAX_LOADSTRING 100

#define u8(x) *(pM+(x))
#define u32(x) *(int*)(pM+(x))
#define val(x) *(int*)(pM+(x)+1),*(pM+(x))

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CMSC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CMSC));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CMSC));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CMSC);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HANDLE hFileM,hFileT;
	int nSizeM,nSizeT;
	LPVOID lpMemM,lpMemT;
	DWORD nBytesRead;

	WIN32_FIND_DATA stFindData;
	HANDLE hFind;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_CVT0101:
			SetCurrentDirectory(_T("E:\\Software\\crass-0.4.14.1\\Scenario"));
			hFind=FindFirstFile(_T("*.msc"),&stFindData);
			if(hFind!=INVALID_HANDLE_VALUE)
				do
				{
					hFileM=CreateFile(stFindData.cFileName ,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
					assert(hFileM!=INVALID_HANDLE_VALUE);
					nSizeM=GetFileSize(hFileM,0);
					lpMemM=VirtualAlloc(0,nSizeM,MEM_COMMIT,PAGE_READWRITE);
					assert(lpMemM);
					ReadFile(hFileM,lpMemM,nSizeM,&nBytesRead,0);
					nSizeT=(nSizeM*20>100000)?nSizeM*20:100000;
					lpMemT=VirtualAlloc(0,nSizeT,MEM_COMMIT,PAGE_READWRITE);
					assert(lpMemT);
					nSizeT=::Msc2Txt ((char*)lpMemM,nSizeM,(char*)lpMemT,nSizeT);
					strcat(stFindData.cFileName ,".txt");
					hFileT=CreateFile(stFindData.cFileName ,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
					assert(hFileT!=INVALID_HANDLE_VALUE);
					WriteFile(hFileT,lpMemT,nSizeT,&nBytesRead,0);
					VirtualFree(lpMemM,0,MEM_RELEASE);
					VirtualFree(lpMemT,0,MEM_RELEASE);
					CloseHandle(hFileT);
					CloseHandle(hFileM);
				}while(FindNextFile(hFind,&stFindData)!=0);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

DWORD Msc2Txt(char* lpMsc,int nSizeM,char* lpTxt,int nSizeT)
{
	char* pM=lpMsc;
	char* pT=lpTxt;

	while(pM<lpMsc+nSizeM)
	{
		(*pM++)^=0x88;
	}
	pM=lpMsc;

	strcpy(pT,"Table1:\r\n");
	pT+=strlen("Table1:\r\n");
	int hdrsize=*(int *)(lpMsc+2);
	int t1size=*(int *)(lpMsc+6);
	int t2size=hdrsize-t1size-10;
	pM+=11;
	while((pM-lpMsc-11)<t1size)
	{
		int a=wsprintf(LPSTR(pT),_T("Index:%d\tOffset:%X\r\n"),*(int *)pM,*(int *)(pM+4));
		pT+=a;
		pM+=9;
	}
	strcpy(pT,"Table2:\r\n");
	pT+=strlen("Table2:\r\n");
	pM+=4;
	while(pM<lpMsc+hdrsize)
	{
		int a=wsprintf(LPSTR(pT),_T("Index:%d\tOffset:%X\r\n"),*(int *)pM,*(int *)(pM+4));
		pT+=a;
		pM+=9;
	}

	pM=lpMsc+hdrsize;
	char str[512];
	while(pM<lpMsc+nSizeM)
	{
		char ctl1=*pM++;
		char ctl2=*pM++;
		switch(ctl1)
		{
		case 0:
			strcpy(pT,"P- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				pT+=wsprintf((LPSTR)pT,_T("i0(%d,%d,%d,%dm(%d))\r\n"),u8(0),u32(1),u8(5),val(6));
				pM+=11;
				break;
			case 1:
				pT+=wsprintf((LPSTR)pT,_T("i1(%dm(%d),%d,%dm(%d),%d,%dm(%d))\r\n"),val(0),u8(5),val(6),u8(11),val(12));
				pM+=17;
				break;
			case 2:
				pT+=wsprintf((LPSTR)pT,_T("i2(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 3:
				pT+=wsprintf((LPSTR)pT,_T("CallOffsetTable(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 4:
				strcpy(pT,"i4()\r\n");
				pT+=strlen("i4()\r\n");
				break;
			case 5:
				pT+=wsprintf((LPSTR)pT,_T("i5(%dm(%d),%d)\r\n"),val(0),u8(5));
				pM+=6;
				break;
			case 6:
				strcpy(pT,"return()\r\n");
				pT+=strlen("return()\r\n");
				break;
			case 7:
				pT+=wsprintf((LPSTR)pT,_T("LoadScriptNo(%d)\r\n"),u8(0));
				pM++;
				break;
			case 8:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("ProcessScript(\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 9:
				pT+=wsprintf((LPSTR)pT,_T("YesNoDialog()\r\n"));
				break;
			case 0xa:
				pT+=wsprintf((LPSTR)pT,_T("YesNoDialog2(%d)\r\n"),u8(0));
				pM++;
				break;
			case 0xc:
				pT+=wsprintf((LPSTR)pT,_T("HideMsgWnd()\r\n"));
				break;
			case 0xd:
				pT+=wsprintf((LPSTR)pT,_T("EnterAutoMode()\r\n"));
				break;
			case 0xe:
				pT+=wsprintf((LPSTR)pT,_T("CtrlSkip()\r\n"));
				break;
			case 0x10:
				pT+=wsprintf((LPSTR)pT,_T("SaveDialog(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x11:
				pT+=wsprintf((LPSTR)pT,_T("LoadPressed(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x16:
				pT+=wsprintf((LPSTR)pT,_T("i22(%d)\r\n"),u8(0));
				pM++;
				break;
			case 0x17:
				pT+=wsprintf((LPSTR)pT,_T("i23(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x1d:
				pT+=wsprintf((LPSTR)pT,_T("DestroyWnd(%d,%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6));
				pM+=11;
				break;
			case 0x20:
				pT+=wsprintf((LPSTR)pT,_T("i32(%dm(%d),%dm(%d),%d,%d)\r\n"),val(0),val(5),u8(10),u8(11));
				pM+=12;
				break;
			case 0x30:
				pT+=wsprintf((LPSTR)pT,_T("JumpPressed()\r\n"));
				break;
			case 0x33:
				pT+=wsprintf((LPSTR)pT,_T("i51(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x34:
				pT+=wsprintf((LPSTR)pT,_T("i54(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x36:
				pT+=wsprintf((LPSTR)pT,_T("i54(%d)\r\n"),u8(0));
				pM++;
				break;
			case 0x37:
				pT+=wsprintf((LPSTR)pT,_T("i55(%d,%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6));
				pM+=11;
				break;
			default:
				pT+=wsprintf((LPSTR)pT,_T("i%d()\r\n"),ctl2);
		}
			break;
		case 1:
			strcpy(pT,"S- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("SetWindowCaption(\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 1:
				strncpy(str,pM+5,u32(1));
				str[u32(1)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("JumpSetScriptNo(%d,\"%s\")\r\n"),u8(0),str);
				pM+=u32(1)+5;
				break;
			case 2:
				pT+=wsprintf((LPSTR)pT,_T("i2(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 3:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("AddNameInfo(\"%s\","),str);
				pM+=u32(0)+4;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\","),str);
				pM+=u32(0)+4;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 4:
				strncpy(str,pM+5,u32(1));
				str[u32(1)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("LoadOGV(%d,\"%s\")\r\n"),u8(0),str);
				pM+=u32(1)+5;
				break;
			case 5:
				pT+=wsprintf((LPSTR)pT,_T("i5(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 6:
				pT+=wsprintf((LPSTR)pT,_T("i6(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 7:
				pT+=wsprintf((LPSTR)pT,_T("i7(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 8:
				pT+=wsprintf((LPSTR)pT,_T("i8(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15),val(20),val(25),val(30));
				pM+=35;
				break;
			case 9:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i9(\"%s\","),str);
				pM+=u32(0)+4;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\","),str);
				pM+=u32(0)+4;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\","),str);
				pM+=u32(0)+4;
				pT+=wsprintf((LPSTR)pT,_T("%d)\r\n"),u8(0));
				pM++;
				break;
			case 0xa:
				pT+=wsprintf((LPSTR)pT,_T("i10(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 0xb:
				strncpy(str,pM+14,u32(10));
				str[u32(10)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i11(%dm(%d),%dm(%d),\"%s\")\r\n"),val(0),val(5),str);
				pM+=u32(10)+14;
				break;
			case 0xC:
				pT+=wsprintf((LPSTR)pT,_T("i12(%d,%dm(%d))\r\n"),u8(0),val(1));
				pM+=6;
				break;
			case 0xd:
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i13(%dm(%d),\"%s\""),val(0),str);
				pM+=u32(5)+9;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 0xe:
				pT+=wsprintf((LPSTR)pT,_T("LoadInMenu(%d,%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6),val(11),val(16));
				pM+=21;
				break;
			case 0xf:
				pT+=wsprintf((LPSTR)pT,_T("i15(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x10:
				pT+=wsprintf((LPSTR)pT,_T("i16(%d,i15(%dm(%d),%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6),val(11));
				pM+=16;
				break;
			case 0x11:
				pT+=wsprintf((LPSTR)pT,_T("i17(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x12:
				pT+=wsprintf((LPSTR)pT,_T("EnterHistroy(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15),val(20),val(25),val(30));
				pM+=35;
				break;
			case 0x13:
				pT+=wsprintf((LPSTR)pT,_T("i19(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15),val(20));
				pM+=25;
				break;
			case 0x14:
				pT+=wsprintf((LPSTR)pT,_T("HistoryPrev(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0x15:
				strncpy(str,pM+5,u32(1));
				str[u32(1)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("SysDirSetup(%d,\"%s\")\r\n"),u8(0),str);
				pM+=u32(1)+5;
				break;
			case 0x17:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("AddPackIfDirNotExists(\"%s\","),str);
				pM+=u32(0)+4;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 0x18:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i24(\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 0x1a:
				pT+=wsprintf((LPSTR)pT,_T("i26(%d,%d)\r\n"),u8(0),u8(1));
				pM+=2;
				break;
			case 0x1b:
				pT+=wsprintf((LPSTR)pT,_T("i27(%d,%dm(%d))\r\n"),u8(0),val(1));
				pM+=6;
				break;
			case 0x1c:
				pT+=wsprintf((LPSTR)pT,_T("OKPressed(%d,%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6));
				pM+=11;
				break;
			case 0x1d:
				pT+=wsprintf((LPSTR)pT,_T("i29(%d,%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6));
				pM+=11;
				break;
			case 0x1e:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i30(\"%s\","),str);
				pM+=u32(0)+4;
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			default:
				pT+=wsprintf((LPSTR)pT,_T("i%d()\r\n"),ctl2);
		}
			break;
		case 2:
			strcpy(pT,"M- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("SetFileNo(%dm(%d),\"%s\")\r\n"),val(0),str);
				pM+=u32(5)+9;
				break;
			case 1:
				pT+=wsprintf((LPSTR)pT,_T("SetXY(%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10));
				pM+=15;
				break;
			case 5:
			case 0xa:
			case 0x10:
			case 0x11:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%dm(%d),%dm(%d))\r\n"),ctl2,val(0),val(5),val(10));
				pM+=15;
				break;
			case 2:
			case 0x17:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%d)\r\n"),ctl2,val(0),u8(5));
				pM+=6;
				break;
			case 3:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),ctl2,val(0),val(5),val(10),val(15),val(20));
				pM+=25;
				break;
			case 4:
			case 6:
			case 7:
			case 0xd:
			case 0xE:
			case 0xf:
			case 0x13:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%dm(%d))\r\n"),ctl2,val(0),val(5));
				pM+=10;
				break;
			case 8:
			case 0xb:
			case 0x15:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),ctl2,val(0),val(5),val(10),val(15));
				pM+=20;
				break;
			case 9:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),ctl2,val(0),val(5),val(10),val(15),val(20),val(25));
				pM+=30;
				break;
			case 0xc:
				pT+=wsprintf((LPSTR)pT,_T("i%d(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%d)\r\n"),ctl2,val(0),val(5),val(10),val(15),u8(20));
				pM+=21;
				break;
			case 0x12:
				strncpy(str,pM+29,u32(25));
				str[u32(25)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i18(...,\"%s\")\r\n"),str);
				pM+=u32(25)+29;
				break;
			case 0x14:
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i20(%dm(%d),\"%s\","),val(0),str);
				pM+=u32(5)+9;
				pT+=wsprintf((LPSTR)pT,_T("%dm(%d),%dm(%d),%dm(%d),%d)\r\n"),val(0),val(5),val(10),u8(15));
				pM+=16;
				break;
			case 0x16:
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i22(%dm(%d),\"%s\","),val(0),str);
				pM+=u32(5)+9;
				pT+=wsprintf((LPSTR)pT,_T("%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15));
				pM+=20;
				break;
			default:
				__asm int 3
			}
			break;
		case 3:
			strcpy(pT,"D- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				if(u32(5)>=512)
					__asm int 3
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("ReadFileNo(%d,\"%s\","),val(0),str);
				pM+=u32(5)+9;
				pT+=wsprintf((LPSTR)pT,_T("%dm(%d),%d)\r\n"),val(0),u8(5));
				pM+=6;
				break;
			case 1:
				pT+=wsprintf((LPSTR)pT,_T("i1(%dm(%d),%dm(%d),%d)\r\n"),val(0),val(5),u8(10));
				pM+=11;
				break;
			case 2:
				pT+=wsprintf((LPSTR)pT,_T("i2(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 3:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("PlayVideo(\"%s\","),str);
				pM+=u32(0)+4;
				pT+=wsprintf((LPSTR)pT,_T("%d)\r\n"),u8(0));
				pM++;
				break;
			default:
				__asm int 3
			}
			break;
		case 4:
			strcpy(pT,"4- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				pT+=wsprintf((LPSTR)pT,_T("i0(%dm(%d),%d,%dm(%d))\r\n"),val(0),u8(5),val(6));
				pM+=11;
				break;
			case 1:
				pT+=wsprintf((LPSTR)pT,_T("i1(%dm(%d),%d)\r\n"),val(0),u8(5));
				pM+=6;
				break;
			case 2:
				pT+=wsprintf((LPSTR)pT,_T("i2(%dm(%d),%d)\r\n"),val(0),u8(5));
				pM+=6;
				break;
			case 3:
				pT+=wsprintf((LPSTR)pT,_T("i3(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 4:
				pT+=wsprintf((LPSTR)pT,_T("i4(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			default:
				__asm int 3
			}
			break;
		case 5:
			strcpy(pT,"T- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("ShowText(%dm(%d),\"%s\")\r\n"),val(0),str);
				pM+=u32(5)+9;
				break;
			case 1:
				pT+=wsprintf((LPSTR)pT,_T("i1(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15),val(20),val(25),val(30),val(35));
				pM+=40;
				break;
			case 2:
				pT+=wsprintf((LPSTR)pT,_T("i2(%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15),val(20),val(25),val(30));
				pM+=35;
				break;
			case 3:
				pT+=wsprintf((LPSTR)pT,_T("i3(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 4:
				pT+=wsprintf((LPSTR)pT,_T("i4(%dm(%d),%dm(%d))\r\n"),val(0),val(5));
				pM+=10;
				break;
			case 5:
				pT+=wsprintf((LPSTR)pT,_T("i5(%d)\r\n"),u8(0));
				pM++;
				break;
			case 6:
				pT+=wsprintf((LPSTR)pT,_T("i6()\r\n"));
				break;
			case 7:
				pT+=wsprintf((LPSTR)pT,_T("i7(%d,%d)\r\n"),u8(0),u8(1));
				pM+=2;
				break;
			default:
				__asm int 3
			}
			break;
		case 6:
			strcpy(pT,"V- ");
			pT+=3;
			switch(ctl2)
			{
			case 0:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i0(\"%s\","),str);
				pM+=u32(0)+4;
				pT+=wsprintf((LPSTR)pT,_T("%d,%dm(%d))\r\n"),u8(0),val(1));
				pM+=6;
				break;
			case 1:
				pT+=wsprintf((LPSTR)pT,_T("i1(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 2:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("PlayVoice(\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 3:
				pT+=wsprintf((LPSTR)pT,_T("i3()\r\n"));
				break;
			case 4:
				strncpy(str,pM+9,u32(5));
				str[u32(5)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("PlaySE(%dm(%d),\"%s\")\r\n"),val(0),str);
				pM+=u32(5)+9;
				break;
			case 5:
				pT+=wsprintf((LPSTR)pT,_T("i5(%dm(%d),%d)\r\n"),val(0),u8(5));
				pM+=6;
				break;
			case 6:
				pT+=wsprintf((LPSTR)pT,_T("i6(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 7:
				pT+=wsprintf((LPSTR)pT,_T("ReplayVoice()\r\n"));
				break;
			case 8:
				pT+=wsprintf((LPSTR)pT,_T("ReplayHistoryVoice(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 9:
				pT+=wsprintf((LPSTR)pT,_T("i9(%dm(%d))\r\n"),val(0));
				pM+=5;
				break;
			case 0xa:
				strncpy(str,pM+4,u32(0));
				str[u32(0)]='\0';
				pT+=wsprintf((LPSTR)pT,_T("i10(\"%s\")\r\n"),str);
				pM+=u32(0)+4;
				break;
			case 0xb:
				pT+=wsprintf((LPSTR)pT,_T("i11(%d,%dm(%d),%dm(%d))\r\n"),u8(0),val(1),val(6));
				pM+=11;
				break;
			case 0xc:
				pT+=wsprintf((LPSTR)pT,_T("i12(%dm(%d),%dm(%d),%dm(%d),%dm(%d))\r\n"),val(0),val(5),val(10),val(15));
				pM+=20;
				break;
			case 0xD:
				pT+=wsprintf((LPSTR)pT,_T("i13()\r\n"));
				break;
			default:
				__asm int 3;
			}
			break;
		default:
			__asm int 3
		}
	}
	
	return (pT-lpTxt);
}