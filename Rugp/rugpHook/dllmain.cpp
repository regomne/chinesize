
#include <Windows.h>
#include <Shlwapi.h>
#include "Header.h"
#include "picHook.h"
#include "pack.h"

//#pragma comment(linker, "/ENTRY:DllMain")

#define FIXED_ASTRING(s) s, strlen(s)

DWORD RsaHash;
int FileNum=-1;

HMODULE modUnivUV;
HMODULE modVm60;
HMODULE modMfc100;
HMODULE modRvmm;

ReadMsgStringFunc OldReadMsgString;
DWORD OldSetNamesTbl;
DWORD OldReadString;
DWORD OldReadString2;
DWORD ATLSetString;
HANDLE hGlobalHeap;

HashFilePair hfPairs[FILE_COUNT];
HashStrEntry* hsEntries;
DWORD hsCount;
WORD MapTbl[65536];
WORD RevMapTbl[65536];
char* Names[NAMES_COUNT];

PackInfo2 MyPack;

PatchStruct PatchAreaVm60[]={
	{0xebbe,"\xFA\x81\x90\x90\x90\x90\x90\x90\x72",9},
	{0xebd2,"\x41\x80\xFA\x81\x90\x90\x90\x90\x90\x90\x72",0xb},
	{0xebed,"\x41\x80\xFA\x81\x90\x90\x90\x90\x90\x90\x72",0xb},
	{0x116f5,"\x42\x3C\x81\x90\x90\x90\x90\x72",8},
	{0x17279,"\xF9\x81\x90\x90\x90\x90\x90\x90\x72",9},
	{0x1736d,"\xF9\x81\x90\x90\x90\x90\x90\x90\x72",9},
	{0x1c503,"\xFA\x81\x90\x90\x90\x90\x90\x90\x72",9},
	{0x1c549,"\xFA\x81\x90\x90\x90\x90\x90\x90\x72",9},
	{0x1c5c8,"\x3C\x81\x90\x90\x90\x90\x72",7},
	{0x1c696,"\xFA\x81\x90\x90\x90\x90\x90\x90\x72",9},
	{0x1c6d6,"\x80\xF1\x20\x80\xC1\x5F\x47\x46\x80\xF9\x3B\x77",0xc},
	{0x1d22c,"\xFA\x81\x90\x90\x90\x90\x90\x90\x0F\x82",0xa},
};

#ifdef _DEBUG
HANDLE hLogFile;
#endif

DWORD __stdcall CalcHash(void* buf,DWORD len)
{
	DWORD hash=0;
	for(DWORD i=0;i<len;i++)
	{
		hash+=(hash<<5)+((BYTE*)buf)[i];
	}
	return hash;
}

void MakeRevertTable(WORD* tblNew,WORD* tblOld)
{
    for(int i=0;i<=65535;i++)
    {
        if(tblOld[i])
        {
            auto idx=(tblOld[i]>>8)+((tblOld[i]&0xff)<<8);
            tblNew[idx]=(i>>8)+((i&0xff)<<8);
        }
    }
}

void ReplaceYuan(wchar_t* str,int len)
{
    for(int i=0;i<len;i++)
    {
        if(str[i]==L'・')
            str[i]=L'·';
    }
}

__declspec(naked) int NewReadTextImgString()
{
    char* str1;
    wchar_t* uStr;
    char* newStr;
    int strLen,uLen,newLen;
    __asm
    {
        pushad
        push ebp
        mov ebp,esp
        sub esp,0x20

        push [ebp+28h]
        call dword ptr [OldReadString2]

        mov eax,[ebp+28h]
        mov ecx,[eax]
        mov str1,ecx
        mov eax,[ecx-0xc]
        mov strLen,eax
    }

    uLen=MultiByteToWideChar(932,0,str1,strLen,0,0);
    uStr=new wchar_t[uLen+1];
    if(uStr)
    {
        uLen=MultiByteToWideChar(932,0,str1,strLen,uStr,uLen);
        uStr[uLen]=0;
        
        ReplaceYuan(uStr,uLen);

        newLen=WideCharToMultiByte(936,0,uStr,uLen,0,0,0,0);
        if(newLen==strLen)
        {
            newLen=WideCharToMultiByte(936,0,uStr,uLen,str1,strLen,0,0);
        }
        delete[] uStr;
    }

    __asm
    {
        add esp,0x20
        pop ebp
        popad
        retn 4
    }
}

__declspec(naked) DWORD NewReadRsa()
{
	__asm{
		pushad
        //分别为rsa块的buff和size
		mov eax,dword ptr [edi+14h]
		mov ecx,dword ptr [edi+10h]
		push eax
		push ecx
		call CalcHash
		mov RsaHash,eax
		popad
		mov ecx,modVm60
		add ecx,0x2c6dd
		jmp ecx
	}
}

__declspec(naked) DWORD NewReadStringInCall()
{
    __asm
    {
        push dword ptr [esp+4]
        call dword ptr [OldReadString]

        pushad
        push ebp
        mov ebp,esp
        sub esp,0x14

    }

    char* pstr;
    DWORD hash,len;
    HashStrEntry* dest;
    __asm
    {
        mov eax,dword ptr [esp+3ch]
        mov pstr, eax
    }

    if(pstr)
    {
        pstr=(char*)(*(DWORD*)pstr);
        len=lstrlenA(pstr);
        hash=CalcHash(pstr,len);
        dest=(HashStrEntry*)bsearch(&hash,hsEntries,hsCount,sizeof(HashStrEntry),compare1);
        if(dest)
        {
            __asm
            {
//                mov eax,dest
                mov ecx,dword ptr [esp+3ch]
                mov eax,[eax+4]
                push eax
                call dword ptr [ATLSetString]
            }
        }
    }
    __asm
    {
        add esp,0x14
        pop ebp
        popad
        retn 4
    }
}

__declspec(naked) DWORD NewReadRsa2()
{
	__asm{
		pushad
		push ebp
		mov ebp,esp
		sub esp,8
	}
	HashFilePair* p;
	p=(HashFilePair*)bsearch(
		&RsaHash,hfPairs,sizeof(hfPairs)/sizeof(HashFilePair),sizeof(HashFilePair),compare1);
	if(p!=0)
		FileNum=p-hfPairs;
	__asm{
		add esp,8
		pop ebp
		popad
		push ebp
		mov ebp,esp
		push -1
		mov eax,modVm60
		add eax,0x29485
		jmp eax
	}
}

BYTE* FindStr(DWORD offset,DWORD hash,DWORD hash2=-1)
{
	int i=FileNum;
	if(i==-1)
		i=0;
	int iend=i+FILE_COUNT;
	for(;i<iend;i++)
	{
		int idx= i>=FILE_COUNT?i-FILE_COUNT:i;
		StringStrt* table=hfPairs[idx].sTbl;
		if(!table)
			continue;
		StringStrt* ret=(StringStrt*)bsearch(&offset,table,hfPairs[idx].count,sizeof(StringStrt),compare1);
		if(ret)
		{
			if(hash!=0 && ret->hash!=hash)
				continue;
			if(hash2!=-1 && ret->hash2!=hash2)
				continue;
			FileNum=idx;
			return ret->str;
		}

	}
	return NULL;
}

DWORD Encode3To4(DWORD val)
{
	BYTE flag=0xf0;
	if(!(val&0xff))
	{
		val|=0xff;
		flag|=1;
	}
	if(!(val&0xff00))
	{
		val|=0xff00;
		flag|=2;
	}
	if(!(val&0xff0000))
	{
		val|=0xff0000;
		flag|=4;
	}
	val|=(flag<<24);
	return val;
}

DWORD Decode4To3(DWORD val)
{
	BYTE flag=val>>24;
	val&=0xffffff;
	if(flag&1)
		val&=0xffff00;
	if(flag&2)
		val&=0xff00ff;
	if(flag&4)
		val&=0x00ffff;
	return val;

}

__declspec(naked) DWORD NewProcessVmMsg()
{
	__asm{
		pushad
		push ebp
		mov ebp,esp
		sub esp,0x14
	}

	DWORD* pStr;
	DWORD off,hash2,len;
	__asm{
		mov eax,[ebp+28h]
		mov pStr,eax
	}
	off=Decode4To3(*pStr);
	len=lstrlenA((char*)pStr);
	if(len>4)
		hash2=CalcHash((BYTE*)pStr+4,len-4);
	else
		hash2=0;
	if(off>=0x20)
	{
		pStr=(DWORD*)FindStr(off,0,hash2);
		if(pStr)
		{
			__asm mov [ebp+28h],eax
		}
	}

	__asm{
		add esp,0x14
		pop ebp
		popad
		push ebp
		mov ebp,esp
		push -1
		mov eax,modVm60
		add eax,0x1e915
		jmp eax
    }
}

DWORD __cdecl NewReadMsgString(BYTE* vmObj,char* pStr)
{
    //if(FileNum==-1)
    //{
    //	return OldReadMsgString(vmObj,pStr);
    //}
    DWORD offset=*(DWORD*)(pStr-0x10);
    DWORD oriLen=OldReadMsgString(vmObj,pStr);
    if(oriLen!=1)
    {
        DWORD oriHash=CalcHash((BYTE*)pStr,oriLen-1);
        if(FindStr(offset,oriHash))
        {
            if(oriLen>4)
                *(DWORD*)pStr=Encode3To4(offset);
            else
                *(DWORD*)pStr=offset;
        }
    }
    return oriLen;
}

__declspec(naked) DWORD NewSetNamesTbl()
{
    __asm 
    {
        push dword ptr [esp+4]
        call dword ptr [OldSetNamesTbl]
        push eax
        push ebp
        mov ebp,esp
        sub esp,0x10
    }

    char* p;
    p=(char*)modVm60+0xf0ab8;
    p=(char*)(*(DWORD*)p);

    p+=0x70;

    for(int i=0;i<NAMES_COUNT;i++)
    {
        if(Names[i])
            lstrcpyA(p,Names[i]);

        p+=0x38;
    }
#ifdef MUV_EXTRA
    if(*(DWORD*)p==0xd58bfc94)
    {
        *(DWORD*)p=0xd9c7c0c3;
    }
#endif

    __asm
    {
        add esp,0x10
        pop ebp
        pop eax
        retn 4
        //mov ecx,modVm60
        //add ecx,0x33d20
        //jmp ecx
    }
}

void* CharGlyphTable=0;
void* __cdecl NewNew(int size)
{
    if(CharGlyphTable==0)
        CharGlyphTable=(BYTE*)VirtualAlloc(0,0x10000,MEM_COMMIT,PAGE_READWRITE);

    memset(CharGlyphTable,0,0x10000);
    return CharGlyphTable;
}

void __cdecl NewDelete(void* ptr1)
{
    if(ptr1==CharGlyphTable)
    {
        *(DWORD*)ptr1=0;
        return;
    }
    DWORD addr=0x50aec+(DWORD)modUnivUV;
    addr=*(DWORD*)addr;
    __asm push ptr1
    __asm call dword ptr [addr];
    __asm add esp,4
    return;
}

#ifdef TEST

BYTE* ReadFileToMem(const WCHAR* fName,DWORD* pSize)
{
    HANDLE hFile=CreateFile(fName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(hFile==INVALID_HANDLE_VALUE)
    {
        Log(L"打开失败");
        return NULL;
    }
    DWORD size=GetFileSize(hFile,0);
    if(pSize)
        *pSize=size;
    BYTE* p=(BYTE*)HeapAlloc(hGlobalHeap,0,size+10);
    *(WORD*)(p+size)=0;
    if(p==0)
    {
        Log(L"Mem error");
        CloseHandle(hFile);
        return NULL;
    }
    DWORD nRead;
    ReadFile(hFile,p,size,&nRead,0);
    CloseHandle(hFile);
    if(nRead!=size)
    {
        Log(L"Read Failed");
        HeapFree(hGlobalHeap,0,p);
        return NULL;
    }
    return p;
}

DWORD ReadFileToMem(WCHAR* fName,void* buff,DWORD nLen)
{
    HANDLE hFile=CreateFile(fName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(hFile==INVALID_HANDLE_VALUE)
    {
        Log(L"打开失败");
        return -1;
    }
    DWORD nRead;
    ReadFile(hFile,buff,nLen,&nRead,0);
    CloseHandle(hFile);
    return nRead;
}
#else
BYTE* ReadFileToMem(const WCHAR* fName,DWORD* pSize)
{
    int nameLen=WideCharToMultiByte(CP_UTF8,0,fName,-1,0,0,0,0);
    char* name=new char[nameLen];
    if(!name)
        return 0;

    nameLen=WideCharToMultiByte(CP_UTF8,0,fName,-1,name,nameLen,0,0);
    QWORD hash=CalcHash(name,nameLen);
    int id=QueryFlatACTree<short>(MyPack.tree,MyPack.vtree,name);
    delete[] name;
    if(id==-1)
        return 0;
    QWORD off=MyPack.entry[id].offset^hash;
    QWORD len=MyPack.entry[id].size^(hash<<2);
    
    //BYTE* buff=new BYTE[len+2];
	auto buff=(BYTE*)HeapAlloc(hGlobalHeap,0,len+2);
    if(!buff)
        return 0;

    LARGE_INTEGER li,li2;
    li.QuadPart=off;
    SetFilePointerEx(MyPack.handle,li,&li2,FILE_BEGIN);
    DWORD temp;
    ReadFile(MyPack.handle,buff,len,&temp,0);
    if(temp!=len)
    {
        delete[] buff;
        return 0;
    }
	*(WORD*)(buff+len)=0;

    Decrypt(buff,(DWORD)len,(BYTE*)&hash);
    if(pSize)
        *pSize=(DWORD)len;

    return buff;
}

DWORD ReadFileToMem(WCHAR* fName,void* buff,DWORD nLen)
{
    int nameLen=WideCharToMultiByte(CP_UTF8,0,fName,-1,0,0,0,0);
    char* name=new char[nameLen];
    if (!name)
    {
        Log(L"ReadFileToMem: read fname cvt failed.");
        return 0;
    }

    nameLen=WideCharToMultiByte(CP_UTF8,0,fName,-1,name,nameLen,0,0);
    QWORD hash=CalcHash(name,nameLen);
    int id=QueryFlatACTree<short>(MyPack.tree,MyPack.vtree,name);
    delete[] name;
    if (id == -1)
    {
        Log(L"ReadFileToMem: ACTree query failed.");
        return 0;
    }
        
    QWORD off=MyPack.entry[id].offset^hash;
    QWORD len=MyPack.entry[id].size^(hash<<2);

    if(nLen>len)
        nLen=(DWORD)len;

    LARGE_INTEGER li,li2;
    li.QuadPart=off;
    SetFilePointerEx(MyPack.handle,li,&li2,FILE_BEGIN);
    DWORD temp;
    ReadFile(MyPack.handle,buff,nLen,&temp,0);

    Decrypt(buff,nLen,(BYTE*)&hash);
    return temp;
}
#endif
BOOL PreCvtTxt(BYTE** pOri, DWORD* nSize)
{
    //auto p=*pOri;
    //auto pend=p+*nSize;
    //while (p<pend)
    //{
    //    if(*p<0x80)
    //    {
    //        p++;
    //    }
    //    else if(pend-p>=2)
    //    {
    //        WORD ch=*(WORD*)p;
    //        if(RevMapTbl[ch])
    //            *(WORD*)p=RevMapTbl[ch];
    //        p+=2;
    //    }
    //    else
    //    {
    //        p++;
    //    }
    //}
	return TRUE;
}

BOOL SplitTxt(StringStrt* sTbl,DWORD nCnt,BYTE* pTxt,DWORD txtSize)
{
	BYTE* p=pTxt;
	BYTE* pEnd=pTxt+txtSize;
	for(DWORD i=0;i<nCnt;i++)
	{
		if(p>=pEnd)
		{
			return FALSE;
		}
		int j=0;
		while(TRUE)
		{
			if((p+j)>=pEnd)
			{
				return FALSE;
			}
			if(p[j]=='\r')
			{
				if((p+j+1)>=pEnd)
				{
					return FALSE;
				}
				if(p[j+1]=='\n')
				{
					p[j]='\0';
					p[j+1]='\0';
					BYTE* s=(BYTE*)HeapAlloc(hGlobalHeap,0,j+1);
					if(s==0)
					{
						Log(L"mem error in splitting");
						return FALSE;
					}
					sTbl[i].str=s;
					memcpy(s,p,j+1);
					p=&p[j+2];
					break;
				}
			}
			j++;
		}
	}
	return TRUE;
}

int EscapeInt(char* s,int* ret)
{
	int sum=0;
	if(s[0]=='n')
	{
		*ret=10;
		return 1;
	}
	if(s[0]>='0' && s[0]<='9')
		sum=s[0]-'0';
	else
		return 0;
	if(s[1]>='0' && s[1]<='9')
	{
		sum=sum*10+s[1]-'0';
		*ret=sum;
		return 2;
	}
	else
	{
		*ret=sum;
		return 1;
	}
}

BOOL ProcessLines(StringStrt* sTbl, DWORD cnt)
{
	for(DWORD i=0;i<cnt;i++)
	{
		char* os=(char*)sTbl[i].str;
		if(!os)
			continue;
		int len=lstrlenA(os);
		char* ns=(char*)HeapAlloc(hGlobalHeap,0,(len+2)*2);
		if(!ns)
			return FALSE;

		int k=0;
		for(int j=0;j<len;j++)
		{
			if(os[j]=='\\')
			{
				if(j+1>=len)
				{
					goto _Error;
				}
				int ret='\\';
				int n=EscapeInt(&os[j+1],&ret);
				ns[k++]=ret;
				j+=n;
			}
			else
				ns[k++]=os[j];
		}
		ns[k]='\0';
		HeapFree(hGlobalHeap,0,os);
		sTbl[i].str=(BYTE*)ns;
		continue;
_Error:
		HeapFree(hGlobalHeap,0,ns);
	}
	return TRUE;
}

BOOL ReadHashTbl()
{

    DWORD nRead=ReadFileToMem(L"txt\\hash.tbl",hfPairs,sizeof(hfPairs));
    if(nRead!=sizeof(hfPairs))
        return FALSE;

    Log(L"hash table read.");
	WCHAR fName[20];
	for(int i=0;i<FILE_COUNT;i++)
	{
		DWORD tblSize,txtSize;
		wsprintf(fName,L"txt\\%02d.tbl",hfPairs[i].fileNum);
		Log(fName);
		BYTE* p=ReadFileToMem(fName,&tblSize);
		if(p==0)
			continue;
		if(*(DWORD*)(p+4)!=hfPairs[i].hash)
		{
			Log(L"文件被损坏！");
			HeapFree(hGlobalHeap,0,p);
			continue;
		}

		wsprintf(fName,L"txt\\%02d.txt",hfPairs[i].fileNum);

		StringStrt* table=(StringStrt*)(p+8);
		hfPairs[i].sTbl=table;

		Log(fName);
		BYTE* pt=ReadFileToMem(fName,&txtSize);
		if(pt==0)
		{
			HeapFree(hGlobalHeap,0,p);
			continue;
		}
		if(!PreCvtTxt(&pt,&txtSize))
		{
			Log(L"预处理失败！");
			HeapFree(hGlobalHeap,0,p);
			HeapFree(hGlobalHeap,0,pt);
			continue;
		}

		int count=(tblSize-8)/sizeof(StringStrt);
		hfPairs[i].count=count;

		if(SplitTxt(table,count,pt,txtSize)==FALSE)
		{
			Log(L"切分txt失败");
			continue;
		}
		HeapFree(hGlobalHeap,0,pt);

		ProcessLines(table,count);
	}
	return TRUE;
}

BOOL ReadSelHashAndTxt()
{
    DWORD size;
    auto entry=(HashStrEntry*)ReadFileToMem(L"txt\\selhash.tbl",&size);
    if(!entry)
    {
        Log(L"can't open selhash.tbl");
        return FALSE;
    }
    hsEntries=entry;
    DWORD count=size/sizeof(HashStrEntry);
    hsCount=count;

    auto txt=ReadFileToMem(L"txt\\sels.txt",&size);
    if(!txt)
    {
        Log(L"can't open sels.txt");
        return FALSE;
    }
    BYTE** strs=(BYTE**)HeapAlloc(hGlobalHeap,HEAP_ZERO_MEMORY,count*4);
    if(!strs)
    {
        Log(L"no mem in selhash");
        return FALSE;
    }



    auto p=txt;
    auto pend=p+size;
    auto prev=p;
    DWORD i=0;
    for(;i<count;i++)
    {
        if(p>=pend)
            break;
        while(*p++!='\r')
            if(p>=pend)
                break;
        *(p-1)=0;
        strs[i]=prev;
        p++;
        prev=p;
    }
    if(i<count)
        strs[i]=prev;

    for(i=0;i<count;i++)
    {
        entry->str=strs[entry->idx];
        entry++;
    }
    return TRUE;
}

BOOL ReadCharMapTbl()
{
    DWORD size=ReadFileToMem(L"txt\\map.tbl",MapTbl,sizeof(MapTbl));
	if(size==-1)
	{
		for(int i=0;i<65536;i++)
        {
            MapTbl[i]=i;
        }
        return TRUE;
	}

	if(size!=sizeof(MapTbl))
	{
		return FALSE;
	}
	return TRUE;

}

void ReadNames()
{
    DWORD size=0;
    auto names=(char*)ReadFileToMem(L"txt\\names.txt",&size);
    if(!names)
        return;
    auto p=names;
    auto prev=p;
    for(int i=0;i<NAMES_COUNT;i++)
    {
        if((DWORD)(p-names)>=size)
            return;
        while(*p++!='\r')
            if((DWORD)(p-names)>=size)
                return;
        *(p-1)=0;
        Names[i]=prev;
        prev=p+1;
        p++;
    }
}


BOOL WINAPI DllMain(HINSTANCE hInstance,DWORD dwReason,LPVOID lpReserved)
{
	if(dwReason==DLL_PROCESS_ATTACH)
	{
#ifdef _DEBUG
		DWORD nRead;
		hLogFile=CreateFile(L"log1.txt",GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if(hLogFile==INVALID_HANDLE_VALUE)
		{
			MessageBox(0,L"无法打开日志文件",L"log",0);
		}
		WriteFile(hLogFile,"\xff\xfe",2,&nRead,0);
#endif
#ifndef TEST
        PackHeader hdr;
        if(InitializePack(L"cnpack",&hdr,&MyPack)==-1)
        {
            MessageBox(0,L"无法找到封包！",0,0);
            return FALSE;
        }
#endif // !TEST
		modVm60=LoadLibrary(L"Vm60.dll");
		modUnivUV=LoadLibrary(L"UnivUI.dll");
        modMfc100=LoadLibrary(L"mfc100.dll");
        modRvmm=LoadLibrary(L"rvmm.dll");
		hGlobalHeap=GetProcessHeap();
		if(!modUnivUV || !modVm60)
		{
			MessageBox(0,L"无法载入必需的DLL！",L"汉化补丁",0);
			return TRUE;
		}
		
        OldReadMsgString=(ReadMsgStringFunc)((BYTE*)modVm60+0x95a50);
        OldSetNamesTbl=(DWORD)modVm60+0x17bb0;
        OldReadString=(DWORD)modUnivUV+0x56b0;
        OldReadString2=(DWORD)modRvmm+0xed60;
        ATLSetString=(DWORD)GetProcAddress(modMfc100,(LPCSTR)12743);

        if(ReadCharMapTbl()==FALSE)
        {
            MessageBox(0,L"无法载入字符映射表！",L"汉化补丁",0);
        }
        else
        {
            MakeRevertTable(RevMapTbl,MapTbl);
        }

		if(ReadHashTbl()==FALSE)
		{
			MessageBox(0,L"无法载入文本！",L"汉化补丁",0);
			return TRUE;
		}

         if(ReadSelHashAndTxt()==FALSE)
         {
             MessageBox(0,L"无法载入选项文本！",L"汉化补丁",0);
         }

        ReadNames();

		DWORD oldPro;
		if(!VirtualProtect((BYTE*)modVm60+0x1000,0xbe000,PAGE_EXECUTE_READWRITE,&oldPro))
		{
			MessageBox(0,L"无法hook目标代码！",L"汉化补丁",0);
			return TRUE;
		}
        if(!VirtualProtect((BYTE*)modUnivUV+0x1000,0x4f000,PAGE_EXECUTE_READWRITE,&oldPro))
        {
            MessageBox(0,L"无法hook目标代码！",L"汉化补丁",0);
            return TRUE;
        }
        if(!VirtualProtect((BYTE*)modRvmm+0x1000,0x8b000,PAGE_EXECUTE_READWRITE,&oldPro))
        {
            MessageBox(0,L"无法hook目标代码！",L"汉化补丁",0);
            return TRUE;
        }

		BYTE* p=(BYTE*)modVm60+0x2c6cf+1;
		*p=0x25;
		p=(BYTE*)modVm60+0x2c6f6;
		*p=0xe9;
		*(DWORD*)(p+1)=(BYTE*)NewReadRsa-((BYTE*)modVm60+0x2c6fb);

		p=(BYTE*)modVm60+0x29480;
		*p=0xe9;
		*(DWORD*)(p+1)=(BYTE*)NewReadRsa2-((BYTE*)modVm60+0x29485);

		p=(BYTE*)modVm60+0x9d38c;
		*(DWORD*)(p+1)=(BYTE*)NewReadMsgString-((BYTE*)modVm60+0x9d391);

		p=(BYTE*)modVm60+0x1e910;
		*p=0xe9;
		*(DWORD*)(p+1)=(BYTE*)NewProcessVmMsg-((BYTE*)modVm60+0x1e915);

        p=(BYTE*)modVm60+0x33d1b;
        *(DWORD*)(p+1)=(BYTE*)NewSetNamesTbl-(p+5);

        p=(BYTE*)modUnivUV+0x2A0E8;
        *(DWORD*)(p+1)=(BYTE*)NewReadStringInCall-(p+5);

        p=(BYTE*)modRvmm+0x54d44;
        *(DWORD*)(p+1)=(BYTE*)NewReadTextImgString-(p+5);
        
        //图片显示函数hook
        oldextR6ti=(ExPic6Routine)((BYTE*)modRvmm+0x69760);
        p=(BYTE*)modRvmm+0x6b305;
        *(DWORD*)(p+1)=(BYTE*)NewExtR6ti-(p+5);

        oldExtPicType7=(ExPic7Routine)((BYTE*)modRvmm+0x69e70);
        p=(BYTE*)modRvmm+0x6b137;
        *(DWORD*)(p+1)=(BYTE*)NewExtR7-(p+5);

        oldExtPicTypeRip008=(ExPicR8Routine)((BYTE*)modRvmm+0x66870);
        p=(BYTE*)modRvmm+0x68cbb;
        *(DWORD*)(p+1)=(BYTE*)NewExtRip008-(p+5);

        //New函数hook（字体模型载入时）
        p=(BYTE*)modUnivUV+0x33459;
        *(DWORD*)(p+1)=(BYTE*)NewNew-(p+5);
        memset((BYTE*)modUnivUV+0x34f93,0x90,6);
        *((BYTE*)modUnivUV+0x3346d)=0xeb;
        p=(BYTE*)modUnivUV+0x47652;
        *p=0xe9;
        *(DWORD*)(p+1)=(BYTE*)NewDelete-(p+5);

		BOOL ret;
		for(int i=0;i<ARRAYSIZE(PatchAreaVm60);i++)
		{
			PatchAreaVm60[i].address+=(DWORD)modVm60;
			ret=PatchMemory(&PatchAreaVm60[i]);
			if(ret!=0)
			{
				MessageBox(0,L"无法patch目标代码！",L"汉化补丁",0);
				ExitProcess(0);
			}
		}
		
		ret=InitApiHooks();
		if(!ret)
		{
			MessageBox(0,L"无法hook目标代码2！",L"汉化补丁",0);
			ExitProcess(0);
		}

        InitCrcTable();

	}
	return TRUE;
}

void Log(WCHAR* str)
{
#ifdef _DEBUG
	int len=lstrlen(str);
	DWORD nWrite;
	WriteFile(hLogFile,str,len*2,&nWrite,0);
	WriteFile(hLogFile,L"\r\n",4,&nWrite,0);
#endif
}

int __cdecl compare1(const void * v1, const void * v2)
{
    auto p1=(DWORD*)v1;
    auto p2=(DWORD*)v2;

    if(*p1>*p2)
        return 1;
    else if(*p1<*p2)
        return -1;
    return 0;
}

void foo()
{
}
