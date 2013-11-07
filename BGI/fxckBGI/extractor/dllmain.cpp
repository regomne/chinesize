#include <Windows.h>
#include <Shellapi.h>
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include "..\shell\fxckBGI.h"
#include "extractor.h"
#include "resource.h"
#include "misc.h"
#include <vector>
#include <string>

//#pragma comment(linker,"/entry:MyDllMain")

using namespace std;

DecoprFunc g_DecompressFile;
HANDLE g_hProcessHeap;
HWND g_hwndMain;

HANDLE g_workingThread;
HANDLE g_paused;
HANDLE g_needStop;
wstring g_extPath;

BYTE g_stolenBytes[8];
BOOL g_ready=FALSE;
DWORD g_setWindowTextAddr;

Options g_options;

DWORD WINAPI waitReady(LPVOID param)
{
	for(int i=0;i<30;i++)
	{
		if(g_ready)
		{
            if(g_DecompressFile)
			    SetWindowText(g_hwndMain,L"fxckBGI - ready");
            else
            {
                MessageBox(g_hwndMain,L"Can't find the function address! Please restart fxckBGI and specify it manually.",
                    L"fxckBGI",MB_ICONASTERISK);
                EndDialog(g_hwndMain,0);
            }
			break;
		}
		Sleep(100);
	}
	return 0;
}

BOOL CheckReady(HWND hwndDlg)
{
    if(!g_ready)
    {
        if(g_DecompressFile==0)
        {
            MessageBox(hwndDlg,L"Please wait for the engine getting ready!",L"fxckBGI",
                MB_ICONASTERISK);
            return FALSE;
        }
        int rslt=MessageBox(hwndDlg,L"It seems not ready for the engine, force to start?",L"fxckBGI",
            MB_YESNO|MB_ICONASTERISK);
        if(rslt==IDNO)
            return FALSE;
    }
    else if(!g_DecompressFile)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK MainWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	vector<wstring>* arcList;
	wchar_t* path;
	IShellItemArray* items;
	IShellItem* item;
	DWORD itemCount;
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_PAUSE:
			if(WaitForSingleObject(g_paused,0)==0)
			{
				ResetEvent(g_paused);
				SetWindowText((HWND)lParam,L"Continue");
			}
			else
			{
				SetEvent(g_paused);
				SetWindowText((HWND)lParam,L"Pause");
			}
			break;
		case IDC_STOP:
			SetEvent(g_paused);
			SetEvent(g_needStop);
			Log(L"操作被终止");
			break;
		case IDC_SELECTARC:
            if(!CheckReady(hwndDlg))
                break;
			if(BasicFileOpen(&items,FALSE,TRUE)==E_NOTIMPL)
			{
				MessageBox(hwndDlg,L"Please drag arc file(s) to this window!",L"Not supported",MB_ICONINFORMATION);
				break;
			}
			if(items)
			{
				items->GetCount(&itemCount);
				arcList=new vector<wstring>();
				for(int i=0;i<itemCount;i++)
				{
					HRESULT hr=items->GetItemAt(i,&item);
					if(SUCCEEDED(hr))
					{
						hr=item->GetDisplayName(SIGDN_FILESYSPATH,&path);
						if(SUCCEEDED(hr))
						{
							arcList->push_back(path);
							CoTaskMemFree(path);
						}
						CoTaskMemFree(item);
					}
				}
				CoTaskMemFree(items);

				g_workingThread=CreateThread(0,0,ExtractThread,arcList,0,0);
				if(g_workingThread==0)
				{
					delete arcList;
					MessageBox(hwndDlg,L"Can't start working thread",0,0);
				}
			}
			break;
		case IDC_BROWSE:

			if(BasicFileOpen(&path,TRUE)==E_NOTIMPL)
			{
				MessageBox(hwndDlg,L"Please type or paste the path by yourself!",L"Not supported",MB_ICONINFORMATION);
				break;
			}
			if(path!=NULL)
			{
				SetDlgItemText(g_hwndMain,IDC_PATHTOSAVE,path);
				CoTaskMemFree(path);
			}
			break;
		case IDC_ENCODE:
			g_options.isEncode=IsDlgButtonChecked(hwndDlg,IDC_ENCODE);
			EnableWindow(GetDlgItem(hwndDlg,IDC_PICFORMAT),
				g_options.isEncode?TRUE:FALSE);
			EnableWindow(GetDlgItem(hwndDlg,IDC_ALPHABLEND),
				g_options.isEncode?FALSE:TRUE);
			break;
		case IDC_EXPORTLOG:
			BasicFileSave(&path);
			if(path!=NULL)
			{
				HANDLE logFile=CreateFile(path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				CoTaskMemFree(path);
				if(logFile==INVALID_HANDLE_VALUE)
				{
					MessageBox(hwndDlg,L"无法创建文件！",0,0);
					break;
				}

				DWORD temp;

				WriteFile(logFile,"\xff\xfe",2,&temp,0);

				int curLen=100;
				wchar_t* str=new wchar_t[curLen];
				HWND listBox=GetDlgItem(hwndDlg,IDC_LOGLIST);
				int itemCount=SendMessage(listBox,LB_GETCOUNT,0,0);
				
				for(int i=0;i<itemCount;i++)
				{
					int len=SendMessage(listBox,LB_GETTEXTLEN,i,0);
					if(len!=LB_ERR)
					{
						if(curLen<len+3)
						{
							curLen=len+3;
							delete[] str;
							str=new wchar_t[curLen];
						}
						len=SendMessage(listBox,LB_GETTEXT,i,(LPARAM)str);
						wcscat(str,L"\r\n");

						if(len!=LB_ERR)
						{
							WriteFile(logFile,str,(len+2)*2,&temp,0);
						}
					}
				}
				CloseHandle(logFile);
				delete[] str;
			}
			break;
		case IDCANCEL:
			EndDialog(hwndDlg,0);

		}
		break;
	case WM_DROPFILES:
		{
            if(!CheckReady(hwndDlg))
                break;
			HDROP drop=(HDROP)wParam;
			int fileCount=DragQueryFile(drop,-1,0,0);
			arcList=new vector<wstring>();
			for(int i=0;i<fileCount;i++)
			{
				wchar_t* fname;
				int namelen=DragQueryFile(drop,i,0,0);
				if(namelen!=0)
				{
					fname=new wchar_t[namelen+1];
					if(DragQueryFile(drop,i,fname,namelen+1))
					{
						arcList->push_back(fname);
						//Log(fname);
					}
					delete[] fname;
				}
			}
			if(arcList->size()==0)
			{
				delete arcList;
				MessageBox(hwndDlg,L"no files input",0,0);
			}
			else
			{
				g_workingThread=CreateThread(0,0,ExtractThread,arcList,0,0);
				if(g_workingThread==0)
				{
					delete arcList;
					MessageBox(hwndDlg,L"Can't start working thread",0,0);
				}
			}
		}
		break;
	case WM_INITDIALOG:
		g_hwndMain=hwndDlg;
		g_needStop=CreateEvent(0,FALSE,FALSE,0);
		g_paused=CreateEvent(0,TRUE,TRUE,0);
		CreateThread(0,0,waitReady,0,0,0);

		SendDlgItemMessage(hwndDlg,IDC_PICFORMAT,CB_ADDSTRING,0,(LPARAM)L"PNG");
// 		SendDlgItemMessage(hwndDlg,IDC_PICFORMAT,CB_ADDSTRING,0,(LPARAM)L"JPEG");
		SendDlgItemMessage(hwndDlg,IDC_PICFORMAT,CB_SETCURSEL,0,0);
		CheckDlgButton(hwndDlg,IDC_ENCODE,BST_CHECKED);
		CheckDlgButton(hwndDlg,IDC_ALPHABLEND,BST_CHECKED);
		EnableWindow(GetDlgItem(hwndDlg,IDC_ALPHABLEND),FALSE);

		return TRUE;
	}
	return FALSE;
}
DWORD WINAPI MainWnd(LPVOID param)
{
	wchar_t pipeName[30];
	wsprintf(pipeName,PIPE_NAME,GetCurrentProcessId());
	CoInitialize(0);

	HANDLE pipe=CreateFile(pipeName,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
	if(pipe==INVALID_HANDLE_VALUE)
	{
		MessageBox(0,pipeName,0,0);
		return 0;
	}
	DWORD dwMode = PIPE_READMODE_BYTE; 

	BOOL fSuccess = SetNamedPipeHandleState(pipe,&dwMode,NULL,NULL);
	if ( ! fSuccess) 
	{
		CloseHandle(pipe);
		MessageBox(0,L"Can't set pipe state",0,0);
		return 0;
	}

	//MyDataStruct* mds;
	DWORD readBytes;
	ReadFile(pipe,(DWORD*)&g_DecompressFile,4,&readBytes,0);
	if(readBytes!=4)
	{
		CloseHandle(pipe);
		MessageBox(0,L"Receive error",0,0);
		return 0;
	}

	//g_DecompressFile=mds->funcAddr;

	DWORD message='OVER';
	WriteFile(pipe,&message,4,&readBytes,0);
	CloseHandle(pipe);

	HMODULE thisMod=GetModuleHandle(L"extractor.dll");
	DialogBoxParam(thisMod,(LPCWSTR)IDD_MAIN,0,MainWndProc,0);
	CoUninitialize();
	FreeLibraryAndExitThread(thisMod,0);
	return 0;
}

__declspec(naked) int SetWindowOnce()
{
	__asm
	{
        cmp g_DecompressFile,0
        jnz _lbl1
        pushad
        call SearchDecompressFunc
        mov g_DecompressFile,eax
        popad
_lbl1:
		mov g_ready,1
		lea ecx,g_stolenBytes
		mov eax,[ecx]
		mov edx,g_setWindowTextAddr
		mov [edx],eax
		mov al,[ecx+4]
		mov [edx+4],al
		jmp g_setWindowTextAddr
	}
}

int WINAPI DllMain(_In_  HINSTANCE hinstDLL,
  _In_  DWORD fdwReason,
  _In_  LPVOID lpvReserved
  )
{
	if(fdwReason==DLL_PROCESS_ATTACH)
	{
		g_hProcessHeap=GetProcessHeap();
		//g_extPath=new wstring(L"");

		HMODULE hmod=LoadLibrary(L"user32.dll");
		BYTE* stAddr=(BYTE*)GetProcAddress(hmod,"SetWindowTextA");
		g_setWindowTextAddr=(DWORD)stAddr;
		DWORD oldProtect;
		if(!VirtualProtect(stAddr,5,PAGE_EXECUTE_READWRITE,&oldProtect))
		{
			MessageBox(0,L"hook failed",0,0);
			return FALSE;
		}
		*(DWORD*)g_stolenBytes=*(DWORD*)stAddr;
		g_stolenBytes[4]=stAddr[4];
		*stAddr=0xe9;
		*(DWORD*)(stAddr+1)=(DWORD)SetWindowOnce-(DWORD)stAddr-5;

		CreateThread(0,0,MainWnd,0,0,0);
	}
	return TRUE;
}