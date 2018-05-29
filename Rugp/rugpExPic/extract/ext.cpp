#include <Windows.h>
#include <string>

#include "Header.h"
#include "misc.h"

using namespace std;

typedef int (__cdecl *ExPic7Routine)(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,DWORD,DWORD,BOOL);
typedef int (__cdecl *ExPicR8Routine)(BYTE* src,int srcLen,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,DWORD,DWORD,BOOL);
typedef int (__cdecl *ExPic6Routine)(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom);

ExPic6Routine oldextR6ti;
ExPic7Routine oldExtPicType7;
ExPicR8Routine oldExtPicTypeRip008;
DWORD crcTable[256];

wstring r6path=L"r6ti\\";
wstring r7path=L"r7\\";
wstring rip008path=L"r08\\";

DWORD crc32(DWORD crc,BYTE *buffer, DWORD size)  
{  
    unsigned int i;  
    for (i = 0; i < size; i++) {  
        crc = crcTable[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);  
    }  
    return crc ;  
}  

void CopyDib32(BYTE* src,BYTE* dest,int width,int height,int srcStride,int destStride)
{

    for(int i=0;i<height;i++)
    {
        auto ps=(DWORD*)src;
        auto pd=(DWORD*)dest;
        for(int j=0;j<width;j++)
        {
            *pd++=*ps++;
        }
        src-=srcStride;
        dest-=destStride;
    }
}

void FillDib32(BYTE* dest,int width,int height,int stride,DWORD color)
{
    for(int i=0;i<height;i++)
    {
        auto pd=(DWORD*)dest;
        for(int j=0;j<width;j++)
            *pd++=color;
        dest-=stride;
    }
}

void CalculatePic(BYTE* pic1,BYTE* pic2,int width,int height)
{
    auto p1=pic1;
    auto p2=pic2;
    auto pd=pic2;
    for(int i=0;i<width*height;i++)
    {
        pd[3]=255+p1[0]-p2[0];
        if(pd[3]==0)
        {
            pd[0]=pd[1]=pd[2]=0;
        }
        else
        {
            pd[0]=255*p1[0]/pd[3];
            pd[1]=255*p1[1]/pd[3];
            pd[2]=255*p1[2]/pd[3];
        }
        p1+=4;
        p2+=4;
        pd+=4;
    }
}

int __cdecl extR6ti(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom)
{
    int right=rightbottom&0xffff;
    int bottom=rightbottom>>16;
    int left=lefttop&0xffff;
    int top=lefttop>>16;
    int width=right-left;
    int height=bottom-top;

    DWORD crc=crc32(-1,src,*(DWORD*)(info+0x14));
    wchar_t fname[60];
    wsprintf(fname,L"%x_%x.png",crc,(height<<16)|width);
    HANDLE hf=CreateFile((r6path+fname).c_str(),GENERIC_ALL,FILE_SHARE_READ,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    if(hf==INVALID_HANDLE_VALUE)
    {
        return oldextR6ti(src,info,newDib,stride,lefttop,rightbottom);
    }


    auto oldDib=new BYTE[width*height*4];
    auto destStart=newDib-top*stride+left*4;
    CopyDib32(destStart,oldDib+(height-1)*(width*4),width,height,stride,width*4);

    FillDib32(destStart,width,height,stride,0);
    oldextR6ti(src,info,newDib,stride,lefttop,rightbottom);
    auto newDib1=new BYTE[width*height*4];
    CopyDib32(destStart,newDib1+(height-1)*(width*4),width,height,stride,width*4);

    FillDib32(destStart,width,height,stride,-1);
    oldextR6ti(src,info,newDib,stride,lefttop,rightbottom);
    auto newDib2=new BYTE[width*height*4];
    CopyDib32(destStart,newDib2+(height-1)*(width*4),width,height,stride,width*4);

    CalculatePic(newDib1,newDib2,width,height);

    MemStream stm;
    stm.len=width*height*4+0x200;
    stm.start=stm.cur=new BYTE[stm.len];
    EncodeBmpToPng(width,height,32,0,newDib2,&stm);
    DWORD temp;
    WriteFile(hf,stm.start,stm.cur-stm.start,&temp,0);
    CloseHandle(hf);

    delete[] stm.start;
    delete[] newDib2;
    delete[] newDib1;
    CopyDib32(oldDib+(height-1)*(width*4),destStart,width,height,width*4,stride);
    delete[] oldDib;

    return oldextR6ti(src,info,newDib,stride,lefttop,rightbottom);
}

int __cdecl extPicType7(BYTE* src,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,
                        DWORD lefttop2,DWORD rightbottom2,BOOL unk)
{
    int right=rightbottom&0xffff;
    int bottom=rightbottom>>16;
    int left=lefttop&0xffff;
    int top=lefttop>>16;
    int width=right-left;
    int height=bottom-top;

    if(lefttop!=lefttop2 || rightbottom2!=rightbottom)
        return oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);

    DWORD crc=crc32(-1,src,*(DWORD*)(info+0x14));
    wchar_t fname[60];
    wsprintf(fname,L"%x_%x.png",crc,(height<<16)|width);
    HANDLE hf=CreateFile((r7path+fname).c_str(),GENERIC_ALL,FILE_SHARE_READ,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    if(hf==INVALID_HANDLE_VALUE)
    {
        return oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    }

    
    auto oldDib=new BYTE[width*height*4];
    auto destStart=newDib-top*stride+left*4;
    CopyDib32(destStart,oldDib+(height-1)*(width*4),width,height,stride,width*4);

    FillDib32(destStart,width,height,stride,0);
    oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    auto newDib1=new BYTE[width*height*4];
    CopyDib32(destStart,newDib1+(height-1)*(width*4),width,height,stride,width*4);

    FillDib32(destStart,width,height,stride,-1);
    oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    auto newDib2=new BYTE[width*height*4];
    CopyDib32(destStart,newDib2+(height-1)*(width*4),width,height,stride,width*4);

    CalculatePic(newDib1,newDib2,width,height);

    MemStream stm;
    stm.len=width*height*4+0x200;
    stm.start=stm.cur=new BYTE[stm.len];
    EncodeBmpToPng(width,height,32,0,newDib2,&stm);
    DWORD temp;
    WriteFile(hf,stm.start,stm.cur-stm.start,&temp,0);
    CloseHandle(hf);

    delete[] stm.start;
    delete[] newDib2;
    delete[] newDib1;
    CopyDib32(oldDib+(height-1)*(width*4),destStart,width,height,width*4,stride);
    delete[] oldDib;

    return oldExtPicType7(src,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
}
int __cdecl extPicTypeRip008(BYTE* src,int srcLen,BYTE* info,BYTE* newDib,DWORD stride,DWORD lefttop,DWORD rightbottom,
                        DWORD lefttop2,DWORD rightbottom2,BOOL unk)
{
    int right=rightbottom&0xffff;
    int bottom=rightbottom>>16;
    int left=lefttop&0xffff;
    int top=lefttop>>16;
    int width=right-left;
    int height=bottom-top;

    if(lefttop!=lefttop2 || rightbottom2!=rightbottom)
        return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);

    auto oldDib=new BYTE[width*height*4];
    auto destStart=newDib-top*stride+left*4;
    CopyDib32(destStart,oldDib+(height-1)*(width*4),width,height,stride,width*4);

    FillDib32(destStart,width,height,stride,0);
    oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    auto newDib1=new BYTE[width*height*4];
    CopyDib32(destStart,newDib1+(height-1)*(width*4),width,height,stride,width*4);

    DWORD crc=crc32(-1,newDib1,width*height*4);
    wchar_t fname[60];
    wsprintf(fname,L"%x_%x.png",crc,(height<<16)|width);
    HANDLE hf=CreateFile((rip008path+fname).c_str(),GENERIC_ALL,FILE_SHARE_READ,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    if(hf==INVALID_HANDLE_VALUE)
    {
        delete[] newDib1;
        delete[] oldDib;
        return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    }

    FillDib32(destStart,width,height,stride,-1);
    oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);
    auto newDib2=new BYTE[width*height*4];
    CopyDib32(destStart,newDib2+(height-1)*(width*4),width,height,stride,width*4);

    CalculatePic(newDib1,newDib2,width,height);

    MemStream stm;
    stm.len=width*height*4+0x200;
    stm.start=stm.cur=new BYTE[stm.len];
    EncodeBmpToPng(width,height,32,0,newDib2,&stm);
    DWORD temp;
    WriteFile(hf,stm.start,stm.cur-stm.start,&temp,0);
    CloseHandle(hf);

    delete[] stm.start;
    delete[] newDib2;
    delete[] newDib1;
    CopyDib32(oldDib+(height-1)*(width*4),destStart,width,height,width*4,stride);
    delete[] oldDib;

    return oldExtPicTypeRip008(src,srcLen,info,newDib,stride,lefttop,rightbottom,lefttop2,rightbottom2,unk);

}

void InitCrcTable()
{
	unsigned int c;  
	unsigned int i, j;  

	for (i = 0; i < 256; i++) {  
		c = (unsigned int)i;  
		for (j = 0; j < 8; j++) {  
			if (c & 1)  
				c = 0xedb88320L ^ (c >> 1);  
			else  
				c = c >> 1;  
		}  
		crcTable[i] = c;  
	}  
}

int WINAPI DllMain(_In_  HINSTANCE hinstDLL,
  _In_  DWORD fdwReason,
  _In_  LPVOID lpvReserved
  )
{
	if(fdwReason==DLL_PROCESS_ATTACH)
	{

		HMODULE rvmm=LoadLibrary(L"rvmm.dll");
        DWORD oldProt;
        VirtualProtect((BYTE*)rvmm+0x1000,0x8b000,PAGE_EXECUTE_READWRITE,&oldProt);

        BYTE* p;
        oldextR6ti=(ExPic6Routine)((BYTE*)rvmm+0x69760);
        p=(BYTE*)rvmm+0x6b305;
        *(DWORD*)(p+1)=(BYTE*)extR6ti-(p+5);

        oldExtPicType7=(ExPic7Routine)((BYTE*)rvmm+0x69e70);
        p=(BYTE*)rvmm+0x6b137;
        *(DWORD*)(p+1)=(BYTE*)extPicType7-(p+5);

        oldExtPicTypeRip008=(ExPicR8Routine)((BYTE*)rvmm+0x66870);
        p=(BYTE*)rvmm+0x68cbb;
        *(DWORD*)(p+1)=(BYTE*)extPicTypeRip008-(p+5);

		InitCrcTable();

        CreateDirectory(L"r6ti",0);
        CreateDirectory(L"r7",0);
        CreateDirectory(L"r08",0);

		//CreateThread(0,0,MainWnd,0,0,0);
	}
	return TRUE;
}