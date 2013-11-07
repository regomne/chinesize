#include<Windows.h>
#include"defines.h"

wchar_t *g_CodeMne[0x100]={
	L"PUSH_XX",0,0,0,0,L"POP",L"POP",0,0,L"DEATH",0,L"SHL",L"COND_AND",L"COND_OR",L"XOR",L"SWITCH",
	L"SETVAR",L"ADD",L"SUB",L"IMUL",L"IDIV",L"MOD",L"SETE",L"SETG",L"SETL",L"SETNE",L"SETGE",L"SETLE",L"AND",L"OR",L"SETZ",L"NEG",
	L"JMP",L"JZ",L"PUSH_XX",L"CALL",L"CODE_SIZE",L"JMP25",0,0,L"CREATE_THD",L"KILL_THD",L"SET_THD_STATE",L"PUSH_2B",L"",L"",L"NEW_ISM",L"EXIT",
	L"PUSHI",L"PUSHIM",L"PUSHD",L"PUSHS",L"DEATH",L"NOP",L"NOP",L"REFVAR",L"REFVAR2",L"REFVAR3",L"PUSHREF",L"ADDVAR",L"SEARCHVAR",L"SEARCHVAR2",L"PUSHREF",L"ADDVAR",
	L"IS_INT",L"IS_STR",L"IS_DOUBLE",L"STRLEN",L"SUBSTRING",L"PUSHSREF",L"SPRINTF",L"",0,0,0,0,0,0,0,0,
	L"PUSHPOP",L"PUSH_XX",L"PUSH_DEBUG_INFO",0,0,0,0,0,0,0,0,0,0,0,0,0,
	L"CALL?60",L"CALL?61",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	L"DISPLAY_80",L"",L"",L"GETPIC_83",L"",L"",L"POP*3",L"POP*3",L"",L"",L"",L"",L"PIC_8C",L"",L"AUDIO_8E",L"PIC_8F",
	L"PIC_90",L"",L"",L"",L"",L"",0,0,0,0,0,0,0,0,0,0,
	L"",L"",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	L"DISPLAY_CHAR",L"",L"",L"",0,0,0,0,0,0,0,0,0,0,0,0,
	L"",L"CURSOR",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	L"DISPLAY_D0",0,L"",L"",0,0,0,0,0,0,0,0,0,0,0,0,
	L"",L"",L"",L"",L"",L"",L"POST_MESSAGE",0,0,0,0,0,0,0,0,0,
	L"",L"",L"",L"",L"SET_SYSTEM_VAR",L"",L"",L"PUSH_TIME",L"",0,L"PUSHPOP*2",L"NOP",L"",L"",L"NOP",L"CALL?FF"
};

int g_CodeLen[0x100]={
	-1,	-1,	-1,	-1,	-1,	0,	0,	-1,	-1,	-1,	-1,	0,	0,	0,	0,	-3,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	-2,	-2,	-1,	4,	-2,	0,	-1,	-1,	0,	0,	0,	0,	0,	0,	0,	0,
	4,	4,	8,	-4,	-1,	0,	0,	0,	4,	5,	4,	5,	4,	5,	4,	5,
	0,	0,	0,	0,	0,	-5,	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	0,	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1	,
	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	0,	0,	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1	,
	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	0,	-1,	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	0,	0,	0,	0,	0,	0,	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1	,
	0,	0,	0,	1,	0,	0,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	0
};

int ParseScript(BYTE* lpScript,int nScriptSize,BYTE* lpTxt,int* lpdwTxtSize)
{
	BYTE* pS=lpScript;
	if(memcmp(pS,"ISM SCRIPT\0\0",0xC))
		return 1;
	BYTE* lpCS=pS+*(int*)(pS+0x10);
	BYTE* p1=pS+0x20+(*(int*)(pS+0x1C)-1)*12;
	p1=lpCS+*(int*)p1;
	if(*p1!=0x24)
		return 1;
	BYTE* pCSEnd=p1+*(int*)(p1+1);
	if(pCSEnd>lpScript+nScriptSize)
		return 1;

	wchar_t* pT=(wchar_t*)lpTxt;
	*pT++=0xfeff;

	pS=lpCS;
	BYTE code;
	int code_len;
	int a;
	wchar_t szCodeMne[10];
	char* pa;
	wchar_t* pb;
	BYTE* pc;
	while(pS<pCSEnd)
	{
		if((BYTE*)pT>lpTxt+*lpdwTxtSize)
			__asm int 3//TXT空间不足
		code=*pS++;
		code_len=g_CodeLen[code];
		if(code_len==P_ERROR)
			__asm int 3
		if(code==0x24)
		{
			*pT++='\r';
			*pT++='\n';
		}
		pT+=wsprintfW(pT,L"%X:\t",pS-1-lpScript);
		pb=g_CodeMne[code];
		if(pb[0]=='\0')
		{
			wsprintfW(szCodeMne,L"CODE_%X",code);
			pb=szCodeMne;
		}
		pT+=wsprintfW(pT,L"%s",pb);
		switch(code_len)
		{
		case 0:
			*(DWORD*)pT=0xa000d;
			pT+=2;
			break;
		case P_STR:
			a=(unsigned)*pS++;
			if(a==0xff)
			{
				a=*(DWORD*)pS;
				pS+=4;
			}
			pa=new char[a+1];
			if(!pa)
				__asm int 3//type 2
			memcpy(pa,pS,a);
			for(int i=0;i<a;i++)
				pa[i]^=0xff;
			pa[a]='\0';
			pb=new wchar_t[a+2];
			if(!pb)
				__asm int 3//type2
			if(!MultiByteToWideChar(SCRIPT_CP,0,pa,-1,pb,a+2))
				__asm int 3
			pT+=wsprintfW(pT,L" \"%s\"\r\n",pb);
			pS+=a;
			delete[] pb;
			delete[] pa;
			break;
		case P_REFSTR:
			pc=lpCS+*(DWORD*)pS;
			pS+=4;
			if(*pc++!=0x33)
				__asm int 3//type 3
			a=(unsigned)*pc++;
			if(a==0xff)
			{
				a=*(DWORD*)pc;
				pS+=4;
			}
			pa=new char[a+1];
			if(!pa)
				__asm int 3//type 2
			memcpy(pa,pc,a);
			for(int i=0;i<a;i++)
				pa[i]^=0xff;
			pa[a]='\0';
			pb=new wchar_t[a+2];
			if(!pb)
				__asm int 3//type2
			if(!MultiByteToWideChar(SCRIPT_CP,0,pa,-1,pb,a+2))
				__asm int 3
			pT+=wsprintfW(pT,L" \"%s\"\r\n",pb);
			delete[] pb;
			delete[] pa;
			break;
		case 4:
			pT+=wsprintfW(pT,L" %x\r\n",*(DWORD*)pS);
			pS+=4;
			break;
		case P_DISTANCE:
			a=*(int*)pS;
			pT+=wsprintfW(pT,L" %x\r\n",pS-1+a-lpScript);
			pS+=4;
			break;
		case 5:
			pT+=wsprintfW(pT,L" %x %x\r\n",*pS,*(DWORD*)(pS+1));
			pS+=5;
			break;
		case P_SWITCH:
			pT+=wsprintfW(pT,L" %x %x\r\n",lpCS+*(DWORD*)pS-lpScript,*(DWORD*)(pS+4));
			pS+=8;
			break;
		case 8:
			pT+=wsprintfW(pT,L" %lf\r\n",(double*)pS);
			pS+=8;
			break;
		case 1:
			pT+=wsprintfW(pT,L" %x\r\n",*pS++);
			break;
		default:
			__asm int 3//type1
		}

	}
	*lpdwTxtSize=(BYTE*)pT-lpTxt;
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	WIN32_FIND_DATA stFd;
	SetCurrentDirectory(TEXT("data0"));
	HANDLE hFind=FindFirstFile(L"*.ism",&stFd);
	if(hFind)
	{
		BYTE* lpNewTxt=(BYTE*)VirtualAlloc(0,10000000,MEM_COMMIT,PAGE_READWRITE);
		if(!lpNewTxt)
		{
			MessageBox(0,TEXT("草，怎么连10MB空间都木有"),0,0);
			return 0;
		}
		do{
			int nTxtSize=10000000;
			LARGE_INTEGER nScriptSize;
			DWORD nBytesRead;
			HANDLE hScript=CreateFile(stFd.cFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hScript)
			{
				GetFileSizeEx(hScript,&nScriptSize);
				BYTE* lpScript=(BYTE*)new char[nScriptSize.LowPart];
				if(!lpScript)
				{
					MessageBox(0,TEXT("内存不够！"),0,0);
					return 0;
				}
				ReadFile(hScript,lpScript,nScriptSize.LowPart,&nBytesRead,0);
				if(nBytesRead!=nScriptSize.LowPart)
				{
					MessageBox(0,TEXT("读取失败"),0,0);
					return 0;
				}
				if(ParseScript(lpScript,nScriptSize.LowPart,lpNewTxt,&nTxtSize))
				{
					MessageBox(0,TEXT("反编译失败！"),0,0);
					return 0;
				}
				TCHAR* pfn=new TCHAR[lstrlen(stFd.cFileName)+6];
				lstrcpy(pfn,stFd.cFileName);
				lstrcat(pfn,TEXT(".txt"));
				HANDLE hNewTxt=CreateFile(pfn,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				WriteFile(hNewTxt,lpNewTxt,nTxtSize,&nBytesRead,0);
				CloseHandle(hNewTxt);
				delete[] pfn;
				CloseHandle(hScript);
			}
		}while(FindNextFile(hFind,&stFd));
		FindClose(hFind);
	}
	return 0;
}