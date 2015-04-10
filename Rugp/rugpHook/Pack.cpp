#include <windows.h>
#include "Pack.h"
#include "Header.h"

void Decrypt(void* buff,DWORD len,BYTE* key)
{
    BYTE* p=(BYTE*)buff;
    for(int i=len-1;i>=1;i--)
    {
        p[i]^=p[i-1]^key[i&3];
    }
}

int InitializePack(wchar_t* fileName,PackHeader* hdr,PackInfo2* info)
{
    auto hf=CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(hf==INVALID_HANDLE_VALUE)
        return -1;

    info->handle=hf;
    DWORD temp;
    ReadFile(hf,hdr,sizeof(PackHeader),&temp,0);
    if(temp!=sizeof(PackHeader))
    {
        CloseHandle(hf);
        return -1;
    }

    if(hdr->magic!=0x21857507)
    {
        CloseHandle(hf);
        return -1;
    }

    info->tree=(short*)new BYTE[hdr->NameTreeSize];
    if(!info->tree)
    {
        CloseHandle(hf);
        return -1;
    }
    info->vtree=(short*)new BYTE[hdr->NameTreeSize];
    if(!info->vtree)
    {
        delete[] info->tree;
        CloseHandle(hf);
        return -1;
    }
    info->entry=(PackFileEntry*)new BYTE[hdr->EntrySize];
    if(!info->entry)
    {
        delete[] info->vtree;
        delete[] info->tree;
        CloseHandle(hf);
        return -1;
    }

    LARGE_INTEGER li,li2;
    li.QuadPart=hdr->NameTreeOffset;

    SetFilePointerEx(hf,li,&li2,FILE_BEGIN);
    ReadFile(hf,info->tree,hdr->NameTreeSize,&temp,0);

    li.QuadPart=hdr->NameVerifyTreeOffset;
    SetFilePointerEx(hf,li,&li2,FILE_BEGIN);
    ReadFile(hf,info->vtree,hdr->NameTreeSize,&temp,0);

    li.QuadPart=hdr->EntryOffset;
    SetFilePointerEx(hf,li,&li2,FILE_BEGIN);
    ReadFile(hf,info->entry,hdr->EntrySize,&temp,0);

    return 0;
}

