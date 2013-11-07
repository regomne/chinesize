#include"resource.h"

#include"mashiroext.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

HWND g_hWinMain;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInst,LPSTR lpCmpLine,int nCmdShow)
{
	DialogBoxParam(GetModuleHandle(NULL),(LPCSTR)IDD_MAINDLG,NULL,(DLGPROC)DlgWndProc,0);
	ExitProcess(0);
}

LRESULT CALLBACK DlgWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	OPENFILENAME opfn;
	BROWSEINFO bi;
	TCHAR fn[512];
	TCHAR fn2[512];
	switch(uMsg)
	{
	case WM_COMMAND:
		switch((wParam<<16)>>16)
		{
		case IDC_BROWSEORI:
			memset(fn,0,512);
			memset(&opfn,0,sizeof(opfn));
			opfn.lStructSize =sizeof(opfn);
			opfn.hwndOwner =hwnd;
			opfn.lpstrFilter =TEXT("DAT文件(*.paz)\0*.paz\0\0");
			opfn.lpstrFile =fn;
			opfn.nMaxFile =512;
			opfn.Flags=OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if(::GetOpenFileName(&opfn))
			{
				::SetDlgItemText(hwnd,IDC_ORINAME,fn);
			}
			break;
		case IDC_RUN:
			if(::IsDlgButtonChecked(hwnd,IDC_RADIO_EXT)==BST_CHECKED)
			{
				::GetDlgItemText(hwnd,IDC_ORINAME,fn,512);
				if(Extract(fn)==-1)
					MessageBox(hwnd,TEXT("解包失败！"),TEXT("摔14"),MB_OK|MB_ICONERROR);
				else
					MessageBox(hwnd,TEXT("解包完成！"),TEXT("摔14"),MB_OK);
			}
			else
			{
				::GetDlgItemText(hwnd,IDC_NEWFOLDER,fn,512);
				::GetDlgItemText(hwnd,IDC_ORINAME,fn2,512);
				if(Compact(fn,fn2)==-1)
					MessageBox(hwnd,TEXT("封装失败！"),TEXT("摔14"),MB_OK|MB_ICONERROR);
				else
					MessageBox(hwnd,TEXT("封装完成！"),TEXT("摔14"),MB_OK);
			}
			break;
		case IDC_BROWSENEW:
			memset(&bi,0,sizeof (bi));
			bi.hwndOwner=hwnd;
			bi.pszDisplayName=fn;
			bi.lpszTitle=TEXT("选择文件夹");
			bi.ulFlags=BIF_USENEWUI;
			LPITEMIDLIST idl;
			idl=SHBrowseForFolder(&bi);
			if(idl==NULL)
				break;
			if(!SHGetPathFromIDList(idl,fn))
				break;
			::SetDlgItemText(hwnd,IDC_NEWFOLDER,fn);
			break;
		case IDC_RADIO_EXT:
			if(wParam>>16==BN_CLICKED)
			{
				ext_main:
				EnableWindow(GetDlgItem(hwnd,IDC_BROWSENEW),FALSE);
				EnableWindow(GetDlgItem(hwnd,IDC_NEWFOLDER),FALSE);
				SetDlgItemText(hwnd,IDC_STATIC1,TEXT("原始脚本(如mov.dat)"));
//				SetDlgItemText(hwnd,IDC_STATIC2,TEXT("待封包目录(如scr文件夹)"));
			}
			break;
		case IDC_RADIO_COM:
			if(wParam>>16==BN_CLICKED)
			{
				EnableWindow(GetDlgItem(hwnd,IDC_BROWSENEW),TRUE);
				EnableWindow(GetDlgItem(hwnd,IDC_NEWFOLDER),TRUE);
				SetDlgItemText(hwnd,IDC_STATIC1,TEXT("新脚本(如mov1.dat)"));
//				SetDlgItemText(hwnd,IDC_STATIC2,TEXT("待封包目录(如scr文件夹)"));
			}
			break;
		}
		break;
	case WM_INITDIALOG:
		g_hWinMain=hwnd;
		CheckDlgButton(hwnd,IDC_RADIO_EXT,BST_CHECKED);
		goto ext_main;
		break;
	case WM_CLOSE:
		EndDialog(hwnd,0);
	}
	return 0;
}
