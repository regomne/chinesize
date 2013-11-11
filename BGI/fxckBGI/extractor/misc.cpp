#include <Windows.h>
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include "extractor.h"
#include "resource.h"
#include <tchar.h>
#include <string>

#include <png.h>

#pragma comment(lib,"libpng16.lib")
#pragma comment(lib,"zlib.lib")

int FillBmpHdr(BYTE* lp,int width,int height,int bit)
{
	memset(lp,0,0x36);
	BITMAPFILEHEADER* fhdr=(BITMAPFILEHEADER*)lp;
	auto ihdr=(BITMAPINFOHEADER*)(fhdr+1);

	if(bit==8)
		fhdr->bfOffBits=0x436;
	else
		fhdr->bfOffBits=0x36;
	fhdr->bfSize=((width*(bit/8)+3)&~3)*height+fhdr->bfOffBits;
	fhdr->bfType='MB';
	
	ihdr->biBitCount=bit;
	ihdr->biHeight=-height;
	ihdr->biWidth=width;
	ihdr->biPlanes=1;
	ihdr->biSize=0x28;
	ihdr->biSizeImage=fhdr->bfSize-fhdr->bfOffBits;

	if(bit==8)
	{
		auto p=(lp+0x36);
		for(int i=0;i<0x100;i++)
		{
			p[0]=p[1]=p[2]=i;
			p[3]=0;
			p+=4;
		}
	}

	return fhdr->bfSize;
}

void Copy24To24(BYTE* dibDest,BYTE* dibSrc,int width,int height)
{
	if(height<0)
		height=-height;

	int stride=(width*3+3)&~3;
	BYTE* lbegin=dibDest;
	BYTE* src=dibSrc;
	for(int i=0;i<height;i++)
	{
		memcpy(lbegin,src,width*3);
		lbegin+=stride;
		src+=width*3;
	}
}

int Log(const wchar_t* str)
{
	static HWND logWindow=GetDlgItem(g_hwndMain,IDC_LOGLIST);
	int index=SendMessage(logWindow,LB_ADDSTRING,0,(LPARAM)str);
	SendMessage(logWindow,LB_SETTOPINDEX,index,0);
	return 0;
}
int Log(const std::wstring& s)
{
	return Log(s.c_str());
}
HANDLE MakeFile(LPCTSTR path,BOOL isDir)
{
	int fullLen=_tcslen(path);
	const TCHAR* p=path+fullLen;
	for(;p>path;p--)
		if(*p==_T('\\') || *p==_T('/'))
			break;

	if(p!=path)
	{
		TCHAR* parentDir=new TCHAR[p-path+1];
		if(!parentDir)
			return INVALID_HANDLE_VALUE;
		_tcsncpy(parentDir,path,p-path);
		parentDir[p-path]=L'\0';

		DWORD parentAttr=GetFileAttributes(parentDir);
		if(parentAttr==-1)
		{
			HANDLE par=MakeFile(parentDir,TRUE);
			delete[] parentDir;
			if(par==INVALID_HANDLE_VALUE)
				return par;
		}
		else if(!(parentAttr&FILE_ATTRIBUTE_DIRECTORY))
		{
			delete[] parentDir;
			return INVALID_HANDLE_VALUE;
		}
	}

	if(isDir)
	{
		CreateDirectory(path,0);
		return NULL;
	}
	else
	{
		return CreateFile(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	}
	
}

wchar_t* ToWCS(char* s,int cp)
{
	wchar_t* wcs;

	int len=MultiByteToWideChar(cp,0,s,-1,0,0);

	wcs=new wchar_t[len+1];

	int ret=MultiByteToWideChar(cp,0,s,-1,wcs,len+1);
	if(ret==0 && len!=0)
	{
		delete[] wcs;
		return NULL;
	}
	return wcs;
}

HRESULT BasicFileOpen(void* filePath, BOOL isDir=FALSE, BOOL allowMulti=FALSE)
{
	// CoCreate the File Open Dialog object.
	*(DWORD*)filePath=NULL;
	IFileOpenDialog *pfd = NULL;
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
			dwFlags|=isDir?(FOS_FORCEFILESYSTEM|FOS_PICKFOLDERS):FOS_FORCEFILESYSTEM;
			dwFlags|=allowMulti?(FOS_ALLOWMULTISELECT):0;
			hr = pfd->SetOptions(dwFlags);
			if (SUCCEEDED(hr))
			{
				static const COMDLG_FILTERSPEC c_rgSaveTypes[] =
				{
					{L"Arc files(*.arc)",       L"*.arc"},
					{L"All files", L"*"},
				};
				if(!isDir)
				{
					pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
					pfd->SetFileTypeIndex(1);
					//pfd->SetDefaultExtension(L"exe");
				}
				hr = pfd->Show(g_hwndMain);
				if (SUCCEEDED(hr))
				{
					// Obtain the result once the user clicks 
					// the 'Open' button.
					// The result is an IShellItem object.
					if(!allowMulti)
					{
						IShellItem *psiResult;
						hr = pfd->GetResult(&psiResult);
						if (SUCCEEDED(hr))
						{
							PWSTR pszFilePath = NULL;
							hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, 
								&pszFilePath);
							if (SUCCEEDED(hr))
							{
								*(PWSTR*)filePath=pszFilePath;

							}
							psiResult->Release();
						}
					}
					else
					{
						IShellItemArray* flist;
						hr=pfd->GetResults(&flist);
						if(SUCCEEDED(hr))
						{
							*(IShellItemArray**)filePath=flist;
							//IShellItem* fitem;
							//DWORD itemCount;
							//flist->GetCount(&itemCount);
							//for(DWORD i=0;i<itemCount;i++)
							//{
							//	hr=flist->GetItemAt(i,&fitem);
							//	if(SUCCEEDED(hr))
							//	{

							//		CoTaskMemFree(fitem);
							//	}
							//}
							//CoTaskMemFree(flist);
						}
					}
				}
			}
		}
		pfd->Release();
	}
	else if(isDir || allowMulti)
	{
		return E_NOTIMPL;
	}
	else
	{
		hr=E_OUTOFMEMORY;
		OPENFILENAME ofn;
		LPWSTR pszFile=(LPWSTR)CoTaskMemAlloc(260*2);
		if(pszFile!=NULL)
		{
			memset(&ofn,0,sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hwndMain;
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
				*(PWSTR*)filePath=pszFile;
				hr=0;
			}
		}

	}
	return hr;
}

HRESULT BasicFileSave(void* filePath)
{
	// CoCreate the File Open Dialog object.
	*(DWORD*)filePath=NULL;
	IFileDialog *pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, 
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
			dwFlags|=FOS_FORCEFILESYSTEM;
			hr = pfd->SetOptions(dwFlags);
			if (SUCCEEDED(hr))
			{
				static const COMDLG_FILTERSPEC c_rgSaveTypes[] =
				{
					{L"Text Files(*.txt)",       L"*.txt"},
				};
				pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
				pfd->SetFileTypeIndex(1);
				pfd->SetDefaultExtension(L"txt");
				hr = pfd->Show(g_hwndMain);
				if (SUCCEEDED(hr))
				{
					// Obtain the result once the user clicks 
					// the 'Open' button.
					// The result is an IShellItem object.
					IShellItem *psiResult;
					hr = pfd->GetResult(&psiResult);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath = NULL;
						hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, 
							&pszFilePath);
						if (SUCCEEDED(hr))
						{
							*(PWSTR*)filePath=pszFilePath;

						}
						psiResult->Release();
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
			memset(&ofn,0,sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = g_hwndMain;
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

			if (GetSaveFileName(&ofn)==TRUE)
			{
				*(PWSTR*)filePath=pszFile;
				hr=0;
			}
		}

	}
	return hr;
}

void PngWrite(png_struct* png,png_byte* buff,png_size_t len)
{
	auto stream=(MemStream*)png_get_io_ptr(png);
	if(stream->cur-stream->start+len > stream->len)
	{
		png_error(png,"err");
	}

	memcpy(stream->cur,buff,len);
	stream->cur+=len;
}

void PngFlush(png_struct* png)
{
	
}

int EncodeBmpToPng(int width,int height,int bit,void* pallete,void* dib,MemStream* stream)
{
	int colorType;
	if(bit==8)
		colorType=PNG_COLOR_TYPE_PALETTE;
	else if(bit==24)
		colorType=PNG_COLOR_TYPE_RGB;
	else if(bit==32)
		colorType=PNG_COLOR_TYPE_RGBA;
	else
	{
		return -1;
	}

	auto png=png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if(!png)
		return -1;
	auto pngInfo=png_create_info_struct(png);
	if(!pngInfo)
	{
		png_destroy_write_struct(&png,0);
		return -1;
	}

	if(setjmp(png_jmpbuf(png)))
	{
		png_destroy_write_struct(&png,&pngInfo);
		return -1;
	}

	png_set_write_fn(png,stream,PngWrite,PngFlush);

	int realHeight=height>0 ? height : -height;

	png_set_IHDR(png,pngInfo,width,realHeight,8,colorType,
		PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_color* newPlte;
	if(bit==8)
	{
		newPlte=new png_color[256];
		auto p1=newPlte;
		auto p2=(BYTE*)pallete;
		for(int i=0;i<256;i++)
		{
			p1->red=p2[2];
			p1->green=p2[1];
			p1->blue=p2[0];
			p1++;
			p2+=4;
		}

		png_set_PLTE(png,pngInfo,newPlte,256);
	}

	png_write_info(png,pngInfo);

 	if(bit!=8)
 		png_set_bgr(png);

	auto rowPointers=new png_byte*[realHeight];
	int bytesPerRow=(bit/8*width+3)&~3;

	if(height<0)
	{
		auto p=(png_byte*)dib;
		for(int i=0;i<realHeight;i++)
		{
			rowPointers[i]=p;
			p+=bytesPerRow;
		}
	}
	else
	{
		auto p=(png_byte*)dib+(realHeight-1)*bytesPerRow;
		for(int i=0;i<realHeight;i++)
		{
			rowPointers[i]=p;
			p-=bytesPerRow;
		}
	}

	png_write_image(png,rowPointers);
	png_write_end(png,pngInfo);

	if(bit==8)
		delete[] newPlte;

	delete[] rowPointers;

	png_destroy_write_struct(&png,&pngInfo);

	return 0;
}

int AlphaBlend1(BYTE* dib,int width,int height)
{
	auto p=dib;
	for(int i=0;i<width*height;i++)
	{
		p[0] = p[0] * p[3] / 255 + 255 - p[3];
		p[1] = p[1] * p[3] / 255 + 255 - p[3];
		p[2] = p[2] * p[3] / 255 + 255 - p[3];
		p += 4;
	}
	return 0;
}

int MemcmpWithMask(const void* m1,const void* m2,const void* mask,unsigned length)
{
    
    for(int i=0;i<length;i++)
    {
        if(((BYTE*)mask)[i])
        {
            if(((BYTE*)m1)[i]!=((BYTE*)m2)[i])
                return ((BYTE*)m1)[i]-((BYTE*)m2)[i];
        }
    }
    return 0;
}

BYTE* SearchCode(BYTE* start,int length)
{
    static const BYTE chCode[]={0x8B, 0x43, 0x14, 0x8D, 0x73, 0x14, 0x3D, 0x00, 0x00, 0x00, 0x04};
    static const BYTE chMask[]={1,0,1,1,0,1,1,1,1,1,1};

    BYTE* p=start;
    for(;p<start+length-sizeof(chCode);p++)
    {
        if(!MemcmpWithMask(p,chCode,chMask,sizeof(chCode)))
        {
            for(int i=3;i<(200>p-start?p-start:200);i++)
            {
                if(!memcmp(p-i,"\x90\x6a\xff",3))
                    return p-i+1;
                if(!memcmp(p-i,"\x55\x8b\xec",3))
                    return p-i;
            }
        }
    }
    return 0;
}

BYTE* WINAPI SearchDecompressFunc()
{
    auto pDosHdr=(IMAGE_DOS_HEADER*)GetModuleHandle(0);
    auto pNtHdr=(IMAGE_NT_HEADERS*)((BYTE*)pDosHdr+pDosHdr->e_lfanew);
    auto secPtr=IMAGE_FIRST_SECTION(pNtHdr);
    for(int i=0;i<pNtHdr->FileHeader.NumberOfSections;i++)
    {
        if(secPtr->Characteristics & IMAGE_SCN_MEM_EXECUTE)
        {
            BYTE* rslt=SearchCode((BYTE*)pDosHdr+secPtr->VirtualAddress,secPtr->Misc.VirtualSize);
            if(rslt)
                return rslt;
        }
        secPtr++;
    }
    return 0;
}
