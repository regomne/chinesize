#include <Windows.h>
#include "Code.h"
#include "system4.h"
#include "PreScan.h"
#include <assert.h>

extern "C" HANDLE _stdcall _MakeFile(char* lpszFileName);

#define CODEPAGE_M		936
#define CODEPAGE_F			932

WCHAR *g_CodeSTable[0x100]={
	L"PUSH",L"POP",L"REF",L"REFREF",L"PUSHGLOBALPAGE",L"PUSHLOCALPAGE",L"INV",L"NOT",
	L"COMPL",L"ADD",L"SUB",L"MUL",L"DIV",L"MOD",L"AND",L"OR",
	L"XOR",L"LSHIFT",L"RSHIFT",L"LT",L"GT",L"LTE",L"GTE",L"NOTE",
	L"EQUALE",L"ASSIGN",L"PLUSA",L"MINUSA",L"MULA",L"DIVA",L"MODA",L"ANDA",
	L"ORA",L"XORA",L"LSHIFTA",L"RSHIFTA",L"F_ASSIGN",L"F_PLUSA",L"F_MINUSA",L"F_MULA",
	L"F_DIVA",L"DUP2",L"DUP_X2",L"CMP",L"JUMP",L"IFZ",L"IFNZ",L"RETURN",
	L"CALLFUNC",L"INC",L"DEC",L"FTOI",L"ITOF",L"F_INV",L"F_ADD",L"F_SUB",
	L"F_MUL",L"F_DIV",L"F_LT",L"F_GT",L"F_LTE",L"F_GTE",L"F_NOTE",L"F_EQUALE",
	L"F_PUSH",L"S_PUSH",L"S_POP",L"S_ADD",L"S_ASSIGN",L"S_PLUSA",L"S_REF",L"S_REFREF",
	L"S_NOTE",L"S_EQUALE",L"SF_CREATE",L"SF_CREATEPIXEL",L"SF_CREATEALPHA",L"SR_POP",L"SR_ASSIGN",L"SR_REF",
	L"SR_REFREF",L"A_ALLOC",L"A_REALLOC",L"A_FREE",L"A_NUMOF",L"A_COPY",L"A_FILL",L"C_REF",
	L"C_ASSIGN",L"MES",L"CALLHLL",L"PUSHSTRUCTPAGE",L"CALLMETHOD",L"SH_GLOBALREF",L"SH_LOCALREF",L"SWITCH",
	L"STRSWITCH",L"FUNC",L"EOF",L"CALLSYS",L"SJUMP",L"CALLONJUMP",L"SWAP",L"SH_STRUCTREF",
	L"S_LENGTH",L"S_LENGTHBYTE",L"I_STRING",L"CALLFUNC2",L"CMD_DUP2_X1",L"R_ASSIGN",L"FT_ASSIGN",L"ASSERT",
	L"S_LT",L"S_GT",L"S_LTE",L"S_GTE",L"S_LENGTH2",L"S_LENGTHBYTE2",L"NEW",L"DELETE",
	L"CHECKUDO",L"A_REF",L"DUP",L"DUP_U2",L"SP_INC",L"SP_DEC",L"ENDFUNC",L"R_EQUALE",
	L"R_NOTE",L"SH_LOCALCREATE",L"SH_LOCALDELETE",L"STOI",L"A_PUSHBACK",L"A_POPBACK",L"S_EMPTY",L"A_EMPTY",
	L"A_ERASE",L"A_INSERT",L"SH_LOCALINC",L"SH_LOCALDEC",L"SH_LOCALASSIGN",L"ITOB",L"S_FIND",L"S_GETPART",
	L"A_SORT",L"S_PUSHBACK",L"S_POPBACK",L"FTOS",L"S_MOD",L"S_PLUSA2",L"OBJSWAP",L"S_ERASE",

	L"SR_REF2",L"S_ERASE2",L"S_PUSHBACK2",L"S_POPBACK2",L"ITOLI",L"LI_ADD",L"LI_SUB",L"LI_MUL",
	L"LI_DIV",L"LI_MOD",L"LI_ASSIGN",L"LI_PLUSA",L"LI_MINUSA",L"LI_MULA",L"LI_DIVA",L"LI_MODA",
	L"LI_ANDA",L"LI_ORA",L"LI_XORA",L"LI_LSHIFTA",L"LI_RSHIFTA",L"LI_INC",L"LI_DEC",L"A_FIND",
	L"A_REVERSE",L"SH_SR_ASSIGN",L"SH_MEM_ASSIGN_LOCAL",L"A_NUMOF_GLOB_1",L"A_NUMOF_STRUCT_1",L"SH_MEM_ASSIGN_IMM",L"SH_LOCALREFREF",L"SH_LOCAL_ASSIGN_SUM_IMM",
	L"SH_IF_LOC_LT_IMM",L"SH_IF_LOC_GE_IMM",L"SH_LOCALREF_ASSIGN_MEM",L"PAGE_REF",L"SH_GLOBALASSIGN_LOCAL",L"SH_STRUCTREF_GT_IMM",L"SH_STRUCT_ASSIGN_LOCALREF_ITOB",L"SH_LOCAL_ASSIGN_STRUCTREF",
	L"SH_IF_STRUCTREF_NE_LOCALREF",L"SH_IF_STRUCTREF_GT_IMM",L"SH_STRUCTREF_CALLMETHOD_NO_PARAM",L"SH_STRUCTREF2",L"SH_REF_STRUCTREF2",L"SH_STRUCTREF3",L"SH_STRUCTREF2_CALLMETHOD_NO_PARAM",L"SH_IF_STRUCTREF_Z",
	L"SH_IF_STRUCT_A_NOT_EMPTY",L"SH_IF_LOC_GT_IMM",L"SH_IF_STRUCTREF_NE_IMM",L"THISCALLMETHOD_NOPARAM",L"SH_IF_LOC_NE_IMM",L"SH_IF_STRUCTREF_EQ_IMM",L"SH_GLOBAL_ASSIGN_IMM",L"SH_LOCALSTRUCT_ASSIGN_IMM",
	L"SH_STRUCT_A_PUSHBACK_LOCAL_STRUCT",L"SH_GLOBAL_A_PUSHBACK_LOCAL_STRUCT",L"SH_LOCAL_A_PUSHBACK_LOCAL_STRUCT",L"SH_IFSREF_NE_STR0",L"SH_S_ASSIGN_REF",L"SH_A_FIND_SREF",L"SH_SREF_EMPTY",L"SH_STRUCTREF_EQ_LOCALSREF",
	L"SH_LOCALSREF_EQ_STR0",L"SH_STRUCTSREF_NE_LOCALSREF",L"SH_LOCALSREF_NE_STR0",L"SH_STRUCT_SR_REF",L"SH_STRUCT_S_REF",L"S_REF2",L"SH_REF_LOCAL_ASSIGN_STRUCTREF2",L"SH_GLOBAL_S_REF",
	L"SH_LOCAL_S_REF",L"SH_LOCALREF_SASSIGN_LOCALSREF",L"H_LOCAL_APUSHBACK_LOCALSREF",L"SH_S_ASSIGN_CALLSYS19",L"SH_S_ASSIGN_STR0",L"SH_SASSIGN_LOCALSREF",L"SH_STRUCTREF_SASSIGN_LOCALSREF",L"SH_LOCALSREF_EMPTY",
	L"SH_GLOBAL_APUSHBACK_LOCALSREF",L"SH_STRUCT_APUSHBACK_LOCALSREF",L"SH_STRUCTSREF_EMPTY",L"SH_GLOBALSREF_EMPTY",L"SH_SASSIGN_STRUCTSREF",L"SH_SASSIGN_GLOBALSREF",L"SH_STRUCTSREF_NE_STR0",L"SH_GLOBALSREF_NE_STR0",
	L"SH_LOC_LT_IMM_OR_LOC_GE_IMM",L"INST_F1"
};

int g_CodeLTable[0x100]={
	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	4,	4,	0,		//2
	-2,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	4,	-6,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	8,	0,	-7,	4,	4,	4,	//5
	4,	-3,	-5,	4,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-4,	0,
	0,	8,	4,	0,	0,	0,	0,	0,	0,	0,	4,	4,	8,	0,	0,	0,	//8
	0,	0,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	8,	4,	4,	8,	4,	8,	12,	12,	8,	4,	8,	8,	8,	8,	//B
	12,	12,	8,	8,	8,	12,	12,	8,	8,	12,	12,	4,	12,	12,	8,	12,
	8,	8,	8,	8,	0,	0,	0,	8,	8,	8,	8,	8,	4,	4,	12,	4,
	4,	8,	8,	0,	4,	4,	8,	4,	8,	8,	4,	4,	4,	4,	4,	8,	//E 0xee??
	12,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
};
DWORD dwTemp;

BOOL GetFuncName(AinSegs* lpAinInfo,int nFunc,LPWSTR lpszName)
{
	int* p=(int*)lpAinInfo->Function.lpAddr;
	if(nFunc>=*p)
		return FALSE;
	p=lpAinInfo->Function.lpRestruct[nFunc];
	p++;
	if(!MultiByteToWideChar(CODEPAGE_F,0,(LPCSTR)p,-1,lpszName,512))
		return FALSE;
	return TRUE;
}

BOOL ProcessCode(AinSegs* lpAinInfo,WCHAR* lpTxt)
{
	WCHAR szMessage[512];
	WORD* pb=(WORD*)lpAinInfo->Code.lpAddr;
	WCHAR* pt=lpTxt;
	*pt++=0xfeff;
	if(*pb==0x62 && *(DWORD*)(pb+1)==0)
		pb+=3;

	WORD nInst;
	int nInstLen;
	int m,n;
	int* p1;
	do{
		pt+=wsprintfW(pt,TEXT("%X: "),(char*)pb-(char*)lpAinInfo->Code.lpAddr);
		nInst=*pb++;
		if(nInst>0xf1)
			__asm{int 3};
		nInstLen=g_CodeLTable[nInst];
		switch(nInstLen)
		{
		case P_CALL:
			m=*(DWORD*)pb;
			pb+=2;
			if(!GetFuncName(lpAinInfo,m,szMessage))
				__asm{int 3}
			pt+=wsprintfW((LPWSTR)pt,TEXT("CALLFUNC %X: %s\r\n"),m,szMessage);
			break;
		case P_CALL2:
			m=*(DWORD*)pb;
			pb+=2;
			if(!GetFuncName(lpAinInfo,m,szMessage))
				__asm{int 3}
			pt+=wsprintfW((LPWSTR)pt,TEXT("CALLMETHOD %X: %s\r\n"),m,szMessage);
			break;
		case P_MES:
			if(!MultiByteToWideChar(CODEPAGE_F,0,(LPCSTR)(lpAinInfo->Message0.lpRestruct[*(DWORD*)pb]),-1,szMessage,512))
				__asm{int 3}
			pt+=wsprintfW((LPWSTR)pt,TEXT("MES \"%s\"\r\n"),szMessage);
			pb+=2;
			break;
		case P_SPUSH:
			if(!MultiByteToWideChar(CODEPAGE_F,0,(LPCSTR)(lpAinInfo->Strings.lpRestruct[*(DWORD*)pb]),-1,szMessage,512))
				__asm{int 3}
			for(int i=0;i<lstrlenW(szMessage);i++)
				if(szMessage[i]==0xa)
					szMessage[i]=L'#';
			pt+=wsprintfW((LPWSTR)pt,TEXT("S_PUSH \"%s\"\r\n"),szMessage);
			pb+=2;
			break;
		case P_FUNC:
			m=*(DWORD*)pb;
			pb+=2;
			p1=lpAinInfo->Function.lpRestruct[m];
			if(!GetFuncName(lpAinInfo,m,szMessage))
				__asm{int 3}
			pt+=wsprintfW((LPWSTR)pt,TEXT("<FUNC %X>: %s("),m,szMessage);
			p1++;
			p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
			p1+=6;
			n=*(p1-2);
			for(int i=0;i<n;i++)
			{
				if(!MultiByteToWideChar(CODEPAGE_F,0,(LPCSTR)p1,-1,szMessage,512))
					__asm{int 3}
				pt+=wsprintfW((LPWSTR)pt,TEXT("%s,"),szMessage);
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1+3*4);
			}
			if(n)
				pt--;
			lstrcpyW(pt,TEXT(")\r\n"));
			pt+=3;
			break;
		case P_ENDF:
			pt+=wsprintfW(pt,TEXT("<ENDF %X>\r\n\r\n"),*(DWORD*)pb);
			pb+=2;
			break;
		case P_EOF:
			m=*(DWORD*)pb;
			pt+=wsprintf(pt,TEXT("<EOF %X>\r\n"),m);
			pb+=2;
			HANDLE hTxt;
			hTxt=_MakeFile((char*)lpAinInfo->FileName.lpRestruct[m]);
			if(!hTxt)
				__asm{int 3}
			WriteFile(hTxt,lpTxt,(char*)pt-(char*)lpTxt,&dwTemp,0);
			CloseHandle(hTxt);
			pt=lpTxt+1;
			break;
		default:
			pt+=wsprintfW(pt,TEXT("%s"),g_CodeSTable[nInst]);
			if(nInstLen%4)
				__asm{int 3}
			//if(nInst>=0x2c && nInst<=0x2e)
			//{
			//	pt+=wsprintfW(pt,TEXT(" %X\r\n"),*(DWORD*)pb);
			//	pb+=2;
			//	break;
			//}
			if(nInstLen)
			{
				*pt++=L' ';
				do{
					pt+=wsprintfW(pt,TEXT("%X,"),*(DWORD*)pb);
					pb+=2;
					nInstLen-=4;
				}while(nInstLen);
				pt--;
			}
			lstrcpyW(pt,TEXT("\r\n"));
			pt+=2;
		}
	}while((char*)pb<(char*)lpAinInfo->Code.lpAddr+lpAinInfo->Code.nSize);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpszCmd,int nShowCmd)
{
	int argc;
	WCHAR** argv=CommandLineToArgvW(GetCommandLine(),&argc);

	if(argc!=2)
	{
		MessageBox(0,L"参数不对",0,0);
		return 0;
	}

	BOOL found=FALSE;
	WCHAR* newdir=new WCHAR[wcslen(argv[1])+2];
	wcscpy(newdir,argv[1]);
	WCHAR* fname=argv[1];
	WCHAR* p=newdir+wcslen(newdir)-1;
	while(TRUE)
	{
		if(p<=newdir)
			break;
		if(*p==L'\\' || *p==L'/')
			break;
		if(*p==L'.')
		{
			found=TRUE;
			break;
		}
		p--;
	}
	if(found)
	{
		*p=0;
	}
	else
	{
		wcscat(newdir,L"1");
	}

	HANDLE hAin=CreateFile(fname,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hAin!=INVALID_HANDLE_VALUE)
	{
		LPVOID lpBuff;
		int nFileSize=GetFileSize(hAin,0);
		lpBuff=VirtualAlloc(0,nFileSize,MEM_COMMIT,PAGE_READWRITE);
		if(lpBuff)
		{
			ReadFile(hAin,lpBuff,nFileSize,&dwTemp,0);

			AinSegs AinInfo;
			if(!PreScan((char*)lpBuff,nFileSize,&AinInfo))
			{
				MessageBox(0,TEXT("预读取出错"),0,0);
				ExitProcess(1);
			}

			LPVOID lpTxt=::VirtualAlloc(0,10000000,MEM_COMMIT,PAGE_READWRITE);
			if(!lpTxt)
			{
				MessageBox(0,TEXT("预读取出错"),0,0);
				ExitProcess(1);
			}
			CreateDirectory(newdir,0);
			SetCurrentDirectory(newdir);
			ProcessCode(&AinInfo,(WCHAR*)lpTxt);
		}
	}
	return 0;
}