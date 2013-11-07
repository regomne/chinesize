//Regomne
#include <Windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include "resource.h"
#include "fxckBGI.h"
#include "shell.h"

#pragma comment(linker,"/entry:main")
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hInstance;
HWND g_hwnd;
HANDLE g_hProcessHeap;
GameInfo g_gameList[]={
    {L"Auto Detect",0},
	{L"大図書館の羊飼い",0x44f040},
	{L"向日葵の教会と長い夏休み",0x44f550},
    {L"大図書館の羊飼い～放課後しっぽデイズ～",0x451380},
};
const int g_gameCount=sizeof(g_gameList)/sizeof(GameInfo);

void * operator new(size_t size)
{
	void *p = HeapAlloc(g_hProcessHeap,0,size);
	return (p);
}

void operator delete(void *p)
{
	HeapFree(g_hProcessHeap,0,p);
}

void memset1(void* src, unsigned char val, unsigned int count)
{
	unsigned char* p=(unsigned char*)src;
	for(unsigned int i=0;i<count;i++)
		p[i]=val;
}

HRESULT BasicFileOpen(PWSTR* filePath)
{
	// CoCreate the File Open Dialog object.
	IFileDialog *pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, 
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents *pfde = NULL;
		//DWORD dwCookie;
		// Set the options on the dialog.
		DWORD dwFlags;

		// Before setting, always get the options first in order 
		// not to override existing options.
		hr = pfd->GetOptions(&dwFlags);
		if (SUCCEEDED(hr))
		{
			// In this case, get shell items only for file system items.
			hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
			if (SUCCEEDED(hr))
			{
				static const COMDLG_FILTERSPEC c_rgSaveTypes[] =
				{
					{L"Executable Files(*.exe)",       L"*.exe"},
				};
				// Set the file types to display only. 
				// Notice that this is a 1-based array.
				hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
				if (SUCCEEDED(hr))
				{
					// Set the selected file type index to Word Docs for this example.
					hr = pfd->SetFileTypeIndex(1);
					if (SUCCEEDED(hr))
					{
						// Set the default extension to be ".doc" file.
						hr = pfd->SetDefaultExtension(L"exe");
						if (SUCCEEDED(hr))
						{
							// Show the dialog
							hr = pfd->Show(g_hwnd);
							if (SUCCEEDED(hr))
							{
								// Obtain the result once the user clicks 
								// the 'Open' button.
								// The result is an IShellItem object.
								IShellItem *psiResult;
								hr = pfd->GetResult(&psiResult);
								if (SUCCEEDED(hr))
								{
									// We are just going to print out the 
									// name of the file for sample sake.
									PWSTR pszFilePath = NULL;
									hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, 
										&pszFilePath);
									if (SUCCEEDED(hr))
									{
										*filePath=pszFilePath;
										
									}
									psiResult->Release();
								}
							}
						}
					}
				}
			}
		}
		pfd->Release();
	}
	else
	{
		hr=E_OUTOFMEMORY;
		OPENFILENAME ofn;
		LPWSTR pszFile=(LPWSTR)CoTaskMemAlloc(260*2);
		if(pszFile!=NULL)
		{
			memset1(&ofn,0,sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hwnd;
			ofn.lpstrFile = pszFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = L'\0';
			ofn.nMaxFile = 260;
			ofn.lpstrFilter = L"Executable Files(*.exe)\0*.exe\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn)==TRUE)
			{
				*filePath=pszFile;
				hr=0;
			}
		}

	}
	return hr;
}

INT_PTR CALLBACK ShellProc(
  _In_  HWND hwndDlg,
  _In_  UINT uMsg,
  _In_  WPARAM wParam,
  _In_  LPARAM lParam
  )
{
	int idx;
	PWSTR exePath;
	wchar_t addrString[16];
	DWORD address;
	switch (uMsg) 
	{ 
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
			{
				int ccnt=GetDlgItemText(hwndDlg,IDC_ADDRESS,addrString,15);
                if(ccnt==0)
                    address=0;
				else if(!StrToIntEx(addrString,STIF_SUPPORT_HEX,(int*)&address))
				{
					MessageBox(hwndDlg,L"Function address must be a hex number.",0,0);
					break;
				}
				
				int len=GetWindowTextLength(GetDlgItem(hwndDlg,IDC_EXEPATH));

				if(len==0)
				{
					MessageBox(hwndDlg,L"Please select the exe file",0,0);
					break;
				}

				exePath=new wchar_t[len+1];
				if(exePath==0)
				{
					delete[] exePath;
					MessageBox(hwndDlg,L"Not enough memory",0,0);
					break;
				}

				GetDlgItemText(hwndDlg,IDC_EXEPATH,exePath,len+1);
				
				STARTUPINFO si;
				memset1(&si,0,sizeof(si));
				si.cb=sizeof(si);
				PROCESS_INFORMATION pi;
				if(!CreateProcess(0,exePath,0,0,FALSE,CREATE_SUSPENDED,0,0,&si,&pi))
				{
					delete[] exePath;
					MessageBox(hwndDlg,L"Can't start exe!",0,0);
					break;
				}
				delete[] exePath;

				int pathLen=256;
				wchar_t* dllPath=new wchar_t[pathLen];
				int retlen=GetModuleFileName(0,dllPath,pathLen);
				while(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
				{
					delete[] dllPath;
					pathLen*=2;
					dllPath=new wchar_t[pathLen];
					retlen=GetModuleFileName(0,dllPath,pathLen);
				};
				wchar_t* p=dllPath+retlen;
				for(;p>dllPath;p--)
					if(*p==L'\\')
						break;
				*(p+1)=L'\0';
				lstrcat(dllPath,L"extractor.dll");

				int rslt=InjectToProcess(pi.hProcess,pi.hThread,dllPath,(DecoprFunc)address);
				delete[] dllPath;
				if(rslt<0)
				{
					MessageBox(hwndDlg,L"Failed to inject process",0,0);
					break;
				}

				wchar_t pipeName[30];
				wsprintf(pipeName,PIPE_NAME,pi.dwProcessId);

				HANDLE pipe=CreateNamedPipe(pipeName,PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
					PIPE_UNLIMITED_INSTANCES,256,256,0,0);

				if(pipe==INVALID_HANDLE_VALUE)
				{
					MessageBox(hwndDlg,L"Faild to create pipe",0,0);
					TerminateProcess(pi.hProcess,0);
					break;
				}
				
				ResumeThread(pi.hThread);

				rslt=PipeComm(pipe,address);
				CloseHandle(pipe);
				if(rslt<0)
				{
					MessageBox(hwndDlg,L"Failed to communicated with sub process",0,0);
					break;
				}

			}
			// Fall through. 

		case IDCANCEL: 
			EndDialog(hwndDlg, wParam); 
			return TRUE; 

		case IDC_ADDRESS:
			if(HIWORD(wParam)==EN_CHANGE)
			{
				idx=SendDlgItemMessage(hwndDlg,IDC_GAMELIST,CB_GETCURSEL,0,0);
				if(idx<g_gameCount)
				{
					int cnt=GetDlgItemText(hwndDlg,IDC_ADDRESS,addrString,15);
                    if(cnt==0)
                        SendDlgItemMessage(hwndDlg,IDC_GAMELIST,CB_SETCURSEL,0,0);
					else if(idx!=CB_ERR && !(StrToIntEx(addrString,STIF_SUPPORT_HEX,(int*)&address) && address==g_gameList[idx].funcAddress))
						SendDlgItemMessage(hwndDlg,IDC_GAMELIST,CB_SETCURSEL,-1,0);
				}
			}
			break;

		case IDC_GAMELIST:
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				idx=SendDlgItemMessage(hwndDlg,IDC_GAMELIST,CB_GETCURSEL,0,0);
                if(idx==0)
                {
                    SetDlgItemText(hwndDlg,IDC_ADDRESS,L"");
                }
				else if(idx!=CB_ERR && idx<g_gameCount)
				{
					wsprintf(addrString,L"0x%X",g_gameList[idx].funcAddress);
					SetDlgItemText(hwndDlg,IDC_ADDRESS,addrString);
				}
				break;
			}
			break;

		case IDC_BROWSE:
			if(SUCCEEDED(BasicFileOpen(&exePath)))
			{
				SetDlgItemText(hwndDlg,IDC_EXEPATH,exePath);
				CoTaskMemFree(exePath);
			}
			break;
		case IDC_ABOUT:
			MessageBox(hwndDlg,L"fxckBGI - an extractor for BGI engine.\n\tv" PRODUCT_VERSION L" by AmaF",L"About",MB_ICONINFORMATION);
			break;
		} 
		break;

	case WM_INITDIALOG:
		g_hwnd=hwndDlg;
		HICON icon=LoadIcon(g_hInstance,(LPCWSTR)IDI_ICON1);
		SendMessage(hwndDlg,WM_SETICON,ICON_SMALL,(LPARAM)icon);

        for(int i=0;i<g_gameCount;i++)
        {
            SendDlgItemMessage(hwndDlg,IDC_GAMELIST,CB_ADDSTRING,0,(LPARAM)(g_gameList[i].gameName));
        }
        SendDlgItemMessage(hwndDlg,IDC_GAMELIST,CB_SETCURSEL,0,0);
		return TRUE;
	} 
	return FALSE; 
}

int main1()
{
	if(GetFileAttributes(L"extractor.dll")==-1)
	{
		MessageBox(0,L"Can't find extractor.dll!",0,MB_ICONERROR);
		return 0;
	}

	DialogBoxParam(g_hInstance,(LPCWSTR)IDD_SHELL,0,ShellProc,0);
	return 0;
}

int main()
{
	g_hInstance=GetModuleHandle(0);
	g_hProcessHeap=GetProcessHeap();
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	main1();
	CoUninitialize();
	ExitProcess(0);
}