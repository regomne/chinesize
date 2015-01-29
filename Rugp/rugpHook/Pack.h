#pragma once

#include <windows.h>

typedef unsigned __int64 QWORD;
typedef __int64 int64;

#pragma pack(1)
struct PackHeader
{
    DWORD magic; //"\x07\x75\x85\x21" 0x21857507
    QWORD NameTreeOffset;
    DWORD NameTreeSize;
    QWORD NameVerifyTreeOffset;
    QWORD EntryOffset;
    DWORD EntrySize;
    BYTE LeafSize;
    BYTE LeafVTreeSize;
    BYTE resv[2];
};

struct PackFileEntry
{
    QWORD offset;
    QWORD size;
};

#pragma pack()

struct PackInfo2
{
    HANDLE handle;
    short* tree;
    short* vtree;
    PackFileEntry* entry;
};

template<class T>
int QueryFlatACTree(T* tree,T* vtree,char* s)
{
    int node=0;
    int p=0;
    int len=strlen(s)+1;
    for(int i=0;i<len;i++)
    {
        int t=node;
        p+=tree[node];
        node=p+((BYTE*)s)[i];
        if(vtree[node]!=t)
            return -1;
    }
    return tree[node];
}

void Decrypt(void* buff,DWORD len,BYTE* key);
int InitializePack(wchar_t* fileName,PackHeader* hdr,PackInfo2* info);