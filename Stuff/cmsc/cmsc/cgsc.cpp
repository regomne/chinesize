// cmsc.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "resource.h"
#include<assert.h>
#include<string>

using namespace std;

#define MAX_LOADSTRING 100

typedef struct _GSC_HEADER
{
	int nFileSize;
	int nHeaderSize;
	int nControlStreamSize;
	int nTextIndexSize;
	int nTextSize;
	int nDataIndexSize;
	int nDataSize;
	int nUnkSize3;
	int nUnkSize4;
}GSC_HEADER,*PGSC_HEADER;

static int dtParamSize[256]={
						-1,	-1,	-1,	4,	4,	4,	-1,	-1,	0,	4,	0,	0,	8,	4,	58,	0x30,
						4,	0,	8,	4,	8,	4,	16,	16,	8,	8,	0,	0,	12,	8,	24,	-1,
						24,	20,	20,	8,	8,	8,	16,	12,	8,	8,	8,	8,	4,	8,	4,	8,
						12,	8,	0,	0,	0,	4,	-1,	0,	20,	0,	0,	16,	12,	8,	4,	12,
						4,	4,	16,	4,	0,	0,	16,	16,	4,	12,	4,	20,	-1,	16,	-1,	-1,
						4,	28,	24,	4,	8,	-1,	-1,	-1,	-1,	-1,	12,	20,	8,	8,	4,	8,
						8,	8,	8,	12,	12,	8,	4,	8,	16,	-1,	20,	-1,	-1,	-1,	12,	12,
						4,	8,	8,	8,	8,	8,	-1,	-1,	8,	8,	-1,	-1,	-1,	-1,	-1,	-1,
						-1,	-1,	16,	20,	8,	-1,	12,	20,	12,	-1,	-1,	-1,	8,	8,	8,	-1,
						-1,	-1,	-1,	-1,	-1,	-1,	8,	8,	8,	8,	8,	8,	12,	20,	8,	8,
						-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
						-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
						-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	44,	20,	12,	-1,	-1,	-1,	-1,	-1,
						-1,	-1,	8,	16,	4,	12,	-1,	-1,	-1,	-1,	-1,	-1,	12,	8,	0,	8,
						-1,	20,	-1,	-1,	-1,	-1,	4,	4,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
						-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20
};

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
DWORD Gsc2Txt(char*,int,char*,int);

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
			SetCurrentDirectory(_T("E:\\temp\\gsctest"));
			hFind=FindFirstFile(_T("*.gsc"),&stFindData);
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
					nSizeT=::Gsc2Txt ((char*)lpMemM,nSizeM,(char*)lpMemT,nSizeT);
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

void ProcControlStream(char* lpCS,int nSizeC,int* lpJumpTbl,int nSizeJT)
{
	char* pC=lpCS;
	int* pJ=lpJumpTbl;
	RtlZeroMemory(lpJumpTbl,nSizeJT);

	while(pC<lpCS+nSizeC)
	{
		WORD inst=*(WORD*)pC;
		pC+=2;
		if(inst&0xf000)
			if((inst&0xf000)==0xf000)
				pC+=4;
			else
				pC+=6;
		else if(inst>=3 && inst<=5)
		{
			*pJ++=*(DWORD*)pC;
			pC+=4;
			assert((char*)pJ<(char*)lpJumpTbl+nSizeJT);
		}
		else if (inst==0xc8)
		{
			*pJ++=*(DWORD*)pC;
			pC+=44;
			assert((char*)pJ<(char*)lpJumpTbl+nSizeJT);
		}
		else
		{
			assert(inst<=0xff);
			assert(dtParamSize[inst]!=-1);
			pC+=dtParamSize[inst];
		}
	}

	//排序
	int n=pJ-lpJumpTbl;
	pJ=lpJumpTbl;
	int i,j,k;
	for(i=0;i<n-1;i++)
		for(j=n-1;j>i;j--)
			if(pJ[j]<pJ[j-1])
			{
				k=pJ[j-1];
				pJ[j-1]=pJ[j];
				pJ[j]=k;
			}
}

void MyLtoa(int* lpInt,char* rslt)
{
	int a=*lpInt;

	if(!HIWORD(a))
		ltoa(a,rslt,10);
	else if(HIWORD(a)==1)
		wsprintf(rslt,TEXT("Btbl[%d]"),LOWORD(a));
	else
		wsprintf(rslt,TEXT("Btbl[%d]@%d"),LOWORD(a),HIWORD(a));
}

DWORD Gsc2Txt(char* lpGsc,int nSizeG,char* lpTxt,int nSizeT)
{
	char* pG=lpGsc;
	char* pT=lpTxt;
	PGSC_HEADER pHdr;

	pHdr=(PGSC_HEADER)pG;
	if(pHdr->nHeaderSize!=0x24)
		return -1;

	pG+=sizeof(GSC_HEADER);
	char* pStart=pG;
	int* lpJumpTable=(int*)VirtualAlloc(0,pHdr->nControlStreamSize*2,MEM_COMMIT,PAGE_READWRITE);
	assert(lpJumpTable);
	ProcControlStream(pStart,pHdr->nControlStreamSize,lpJumpTable,pHdr->nControlStreamSize*2);

	pT+=wsprintf(pT,TEXT("!ControlStream\n"));
	while(pG<pStart+pHdr->nControlStreamSize)
	{
		for(int i=0,j;j=lpJumpTable[i];i++)
		{
			if(j==pG-pStart)
			{
				pT+=wsprintf(pT,TEXT("Label_%d:\r\n"),i);
				break;
			}
		}
		WORD inst=*(WORD*)pG;
		pG+=2;
		if(inst&0xf000)
		{
			char src1[20],src2[20],dest[20];
			unsigned int a=*(WORD*)pG;
			pG+=2;
			wsprintf(dest,TEXT("Atbl[%d]"),a);
			if((inst&0xf000)!=0xf000)
			{
				int b=*(WORD*)pG;
				pG+=2;
				if(((HIBYTE(inst)>>2)&3)==0)
					wsprintf(src1,TEXT("%d"),b);
				else if(((HIBYTE(inst)>>2)&3)==1)
					wsprintf(src1,TEXT("Atbl[%d]"),b);
				else if(((HIBYTE(inst)>>2)&3)==2)
				{
					if((inst>>4)&0xf)
						wsprintf(src1,TEXT("Btbl[%d]@%d"),b,(inst>>4)&0xf);
					else
						wsprintf(src1,TEXT("Btbl[%d]"),b);
				}
			}
			int c=*(WORD*)pG;
			pG+=2;
			if((HIBYTE(inst)&3)==0)
				wsprintf(src2,TEXT("%d"),c);
			else if((HIBYTE(inst)&3)==1)
				wsprintf(src2,TEXT("Atbl[%d]"),c);
			else if((HIBYTE(inst)&3)==2)
			{
				if(inst&0xf)
					wsprintf(src2,TEXT("Btbl[%d]@%d"),c,inst&0xf);
				else
					wsprintf(src2,TEXT("Btbl[%d]"),c);
			}
			switch(inst&0xf000)
			{
			case 0x1000:
				pT+=wsprintf(pT,TEXT("\t%s = %s = %s\r\n"),dest,src1,src2);
				break;
			case 0x2000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s||%s)\r\n"),dest,src1,src2);
				break;
			case 0x3000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s&&%s)\r\n"),dest,src1,src2);
				break;
			case 0x4000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s==%s)\r\n"),dest,src1,src2);
				break;
			case 0x5000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s>=%s)\r\n"),dest,src1,src2);
				break;
			case 0x6000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s>%s)\r\n"),dest,src1,src2);
				break;
			case 0x7000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s<=%s)\r\n"),dest,src1,src2);
				break;
			case 0x8000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s<%s)\r\n"),dest,src1,src2);
				break;
			case 0x9000:
				pT+=wsprintf(pT,TEXT("\t%s = (%s!=%s)\r\n"),dest,src1,src2);
				break;
			case 0xA000:
				pT+=wsprintf(pT,TEXT("\t%s = %s + %s\r\n"),dest,src1,src2);
				break;
			case 0xB000:
				pT+=wsprintf(pT,TEXT("\t%s = %s - %s\r\n"),dest,src1,src2);
				break;
			case 0xC000:
				pT+=wsprintf(pT,TEXT("\t%s = %s * %s\r\n"),dest,src1,src2);
				break;
			case 0xD000:
				pT+=wsprintf(pT,TEXT("\t%s = %s \\ %s\r\n"),dest,src1,src2);
				break;
			case 0xE000:
				pT+=wsprintf(pT,TEXT("\t%s = %s %% %s)\r\n"),dest,src1,src2);
				break;
			case 0xF000:
				pT+=wsprintf(pT,TEXT("\t%s = %s\r\n"),dest,src2);
				break;
			}
		}
		else
		{
			unsigned int a,b,c,d,e,f;
			char szstr[12][20];
			switch(inst)
			{
			case 3:
			case 4:
			case 5:
				a=*(unsigned int*)pG;
				b=(int)pT;
				pG+=4;
				for(int i=0,j;j=lpJumpTable[i];i++)
				{
					if(j==a)
					{
						if(inst==3)
							pT+=wsprintf(pT,TEXT("\tJZ Label_%d\r\n"),i);
						else if (inst==4)
							pT+=wsprintf(pT,TEXT("\tJNZ Label_%d\r\n"),i);
						else
							pT+=wsprintf(pT,TEXT("\tJMP Label_%d\r\n"),i);
						break;
					}
				}
				if(b==(int)pT)
					pT+=wsprintf(pT,TEXT("\tJxx Label Missing!\r\n"));
				break;
			case 0x10:
				pT+=wsprintf(pT,TEXT("\tReturn(%d)\r\n"),*(int*)pG);
				pT+=wsprintf(pT,TEXT("\r\n"));
				pG+=4;
				break;
			case 0x12:
				pT+=wsprintf(pT,TEXT("\tReadFromData(Btbl[%d],Data[%d])\r\n"),*(int*)pG,*((int*)pG+1));
				pG+=8;
				break;
			case 0x1c:
				pT+=wsprintf(pT,TEXT("\tShowDC(%d,%d,%d)\r\n"),*(int*)pG,*((int*)pG+1),*((int*)pG+2));
				pG+=12;
				break;
			case 0x1e:
				for(int i=0;i<6;i++)
				{
					MyLtoa((int*)pG,szstr[i]);
					pG+=4;
				}
				pT+=wsprintf(pT,TEXT("\tDisplayPic(%s,%s,%s,%s,%s,%s)\r\n"),szstr[0],szstr[1],szstr[2],szstr[3],szstr[4],szstr[5],szstr[6]);
				break;
			case 0x4b:
				for(int i=0;i<5;i++)
				{
					MyLtoa((int*)pG,szstr[i]);
					pG+=4;
				}
				pT+=wsprintf(pT,TEXT("\tDisplayPic2(%s,%s,%s,%s,%s)\r\n"),szstr[0],szstr[1],szstr[2],szstr[3],szstr[4],szstr[5]);
				break;
			case 0xC8:
				a=*(int*)pG;
				pG+=4;
				b=-1;
				for(int i=0,j;j=lpJumpTable[i];i++)
				{
					if(j==a)
					{
						b=i;
						break;
					}
				}
				if(b==-1)
					lstrcpy(szstr[0],"Label_Unk");
				else
					wsprintf(szstr[0],TEXT("Label_%d"),b);
				pT+=wsprintf(pT,TEXT("\tCallFunc(%s,%d,%d,%d,%d,%d,"),szstr[0],*(int*)pG,*((int*)pG+1),*((int*)pG+2),*((int*)pG+3),*((int*)pG+4));
				pT+=wsprintf(pT,TEXT("%d,%d,%d,%d,%d)\r\n"),*((int*)pG+5),*((int*)pG+6),*((int*)pG+7),*((int*)pG+8),*((int*)pG+9));
				pG+=40;
				break;
			default:
				assert(inst<=0xff);
				if(dtParamSize[inst]%4!=0)
					__asm int 3
				assert(dtParamSize[inst]%4==0);
				pT+=wsprintf(pT,TEXT("\tinst%02X("),inst);
				for(int i=0;i<dtParamSize[inst]/4;i++)
				{
					pT+=wsprintf(pT,TEXT("%d,"),*(int*)pG);
					pG+=4;
				}
				pT+=wsprintf(pT,TEXT(")\r\n"));
			}
		}
		assert((pT-lpTxt)<nSizeT);
	}

	pT+=wsprintf(pT,TEXT("\r\n!Data\r\n"));

	int *pDI=(int*)(lpGsc+pHdr->nHeaderSize+pHdr->nControlStreamSize+pHdr->nTextIndexSize+pHdr->nTextSize);
	short* pD=(short*)((char*)pDI+pHdr->nDataIndexSize);
	pStart=(char*)pD;
	int nDataCount=pHdr->nDataIndexSize/4;
	for(int i=0;i<nDataCount;i++)
	{
		pT+=wsprintf(pT,TEXT("Data%d:\r\n"),i);

		short* p=pD+pDI[i];
		unsigned int n=(unsigned short)*p++;
		pT+=wsprintf(pT,TEXT("\tSize=%d\r\n\t"),n);
		assert((pT-lpTxt)<nSizeT);
		for(int j=0;j<n;j++)
		{
			pT+=wsprintf(pT,TEXT("%d,"),p[j]);
			assert((pT-lpTxt)<nSizeT);
		}
		if(n)
			pT--;
		pT+=wsprintf(pT,TEXT("\r\n\r\n"));
	}
	return (pT-lpTxt);
}