#include<Windows.h>
#include "parser.h"
#include<stack>

wchar_t* szInsts[0x20]={
	L"End",L"",L"push",L"pushp",L"pushpp",L"",L"inst6",L"Add",
	L"Sub",L"Imul",L"Div",L"Mod",L"And",L"Or",L"Xor",L"IsZero",
	L"Neg",L"IsEqual",L"IsNotEqual",L"IsGreater",L"IsGE",L"IsLittle",L"IsLE",L"CondAnd",
	L"CondOr",L"pop",L"jump",L"jne",L"je",L"TextNumber",L"Line:",
};

int Parser(char* lpCS,int nCSSize,char* lpStrings,int nStringsSize,HANDLE hTxt)
{
	DWORD dwTemp;
	int* pC=(int*)lpCS;
	wchar_t szTemp[0x200];
	WORD uBOM=0xfeff;
	WriteFile(hTxt,&uBOM,2,&dwTemp,0);

	wchar_t* lpTxt=(wchar_t*)VirtualAlloc(0,30000000,MEM_COMMIT,PAGE_READWRITE);
	if(!lpTxt)
		return 1;
	wchar_t* pT=lpTxt;

	std::stack<char*> s;

	int m,n;
	int flag;
	while((char*)pC<lpCS+nCSSize)
	{
		if(*pC==0x1e)
		{
			pC+=2;
			continue;
		}
		pT+=wsprintfW(pT,TEXT("%X: "),(char*)pC-lpCS);
		if((pT-lpTxt)>=9000000/2)
		{
			WriteFile(hTxt,lpTxt,(pT-lpTxt)*2,&dwTemp,0);
			pT=lpTxt;
		}
_NextPara:
		int inst=*pC++;
		if(inst==5)
		{
			m=*pC++;
			n=*pC++;
			s.push(lpStrings+n);
			goto _NextPara;
		}
		else if(inst==1)
		{
			m=*pC++;
			if(!MultiByteToWideChar(932,0,lpStrings+m,-1,szTemp,0x200))
				__asm int 3
			pT+=wsprintfW(pT,TEXT("%s("),szTemp);
			flag=0;
			while(!s.empty())
			{
				if(!MultiByteToWideChar(932,0,s.top(),-1,szTemp,0x200))
					__asm int 3
				s.pop();
				for(int i=0;i<lstrlenW(szTemp);i++)
					if(szTemp[i]==L'\n')
						szTemp[i]=L'$';
				pT+=wsprintfW(pT,TEXT("%s,"),szTemp);
				flag=1;
			}
			if(flag)
				pT--;
			lstrcpyW(pT,L")\r\n");
			pT+=3;
			continue;
		}
		switch(inst)
		{
		case 0:
			goto _End;
		case 2:
		case 3:
		case 4:
		case 0x1d:
			m=*pC++;
			pT+=wsprintfW(pT,L"%s(0x%X)\r\n",szInsts[inst],m);
			break;
		case 0x1a:
		case 0x1b:
		case 0x1c:
			m=*pC++;
			if(*(int*)(lpCS+m)==0x1e)
				m+=8;
			pT+=wsprintfW(pT,L"%s(0x%X)\r\n",szInsts[inst],m);
			break;
		case 0x6:
		case 0x7:
		case 0x8:
		case 0x9:
		case 0xa:
		case 0xb:
		case 0xc:
		case 0xd:
		case 0xe:
		case 0xf:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:
			pT+=wsprintfW(pT,L"%s\r\n",szInsts[inst]);
			break;
		case 0x1e:
			m=*pC++;
			//pT+=wsprintfW(pT,L"Line: %d\r\n",m);
			break;
		default:
			__asm int 3
		}
	}

_End:
	if(pT!=lpTxt)
		WriteFile(hTxt,lpTxt,(pT-lpTxt)*2,&dwTemp,0);

	VirtualFree(lpTxt,0,MEM_RELEASE);
	return 0;
}

void Decode(char* lpBuff,int nSize)
{
	for(int i=0;i<nSize;i++)
	{
		lpBuff[i]=~lpBuff[i];
	}
}

int ParseScript(TCHAR* lpszScName,TCHAR* lpszTxtName)
{
	DWORD dwTemp;
	scHeader hdr;
	HANDLE hSc=CreateFile(lpszScName,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hSc==INVALID_HANDLE_VALUE)
		return 1;
	int nFileSize=GetFileSize(hSc,0);
	char dbMagic;
	ReadFile(hSc,&dbMagic,1,&dwTemp,0);
	if(dbMagic!=(char)0xff)
	{
		CloseHandle(hSc);
		return 1;
	}
	ReadFile(hSc,&hdr,sizeof(hdr),&dwTemp,0);
	char* lpCS=new char[hdr.nControlStreamSize];
	if(!lpCS)
	{
		CloseHandle(hSc);
		return 1;
	}
	char* lpStrings=new char[hdr.nStringSize];
	if(!lpStrings)
	{
		delete [] lpCS;
		CloseHandle(hSc);
		return 1;
	}
	ReadFile(hSc,lpCS,hdr.nControlStreamSize,&dwTemp,0);
	ReadFile(hSc,lpStrings,hdr.nStringSize,&dwTemp,0);
	CloseHandle(hSc);

	Decode(lpStrings,hdr.nStringSize);

	HANDLE hTxt=CreateFile(lpszTxtName,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hTxt==INVALID_HANDLE_VALUE)
	{
		delete[] lpCS;
		delete[] lpStrings;
		return 1;
	}
	Parser(lpCS,hdr.nControlStreamSize,lpStrings,hdr.nStringSize,hTxt);

	CloseHandle(hTxt);
	delete[] lpCS;
	delete[] lpStrings;
	return 0;
}

//int __stdcall WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
//{
////	ParseScript(TEXT("scenario\\scenario.sc"),TEXT("scenario\\scenario.txt"));
//	WIN32_FIND_DATA stFd;
//	SetCurrentDirectory(L"scenario");
//	HANDLE hFind=FindFirstFile(L"*.sc",&stFd);
//	WCHAR szTemp[0x100];
//	if(hFind!=INVALID_HANDLE_VALUE)
//	{
//		do{
//			lstrcpy(szTemp,stFd.cFileName);
//			for(int i=0;i<lstrlen(szTemp)-2;i++)
//			{
//				if(!lstrcmp(&szTemp[i],L".sc"))
//				{
//					lstrcpy(&szTemp[i],L".txt");
//					if(ParseScript(stFd.cFileName,szTemp))
//						__asm int 3
//					break;
//				}
//			}
//		}while(FindNextFile(hFind,&stFd));
//		FindClose(hFind);
//	}
//
//	return 0;
//}

int main(int argc, char* argv[])
{
//	ParseScript(TEXT("scenario\\scenario.sc"),TEXT("scenario\\scenario.txt"));
	if(argc!=2)
	{
		printf("Format: parser <sc filename>\n");
		return 0;
	}
	TCHAR szTemp[512];
	int num;
	LPWSTR* realArgv=CommandLineToArgvW(GetCommandLine(),&num);
	lstrcpy(szTemp,realArgv[1]);
	for(int i=0;i<lstrlen(szTemp)-2;i++)
	{
		if(!lstrcmp(&szTemp[i],L".sc"))
		{
			lstrcpy(&szTemp[i],L".txt");
			if(ParseScript(realArgv[1],szTemp))
				__asm int 3
			break;
		}
	}

	//WIN32_FIND_DATA stFd;
	//SetCurrentDirectory(L"scenario");
	//HANDLE hFind=FindFirstFile(L"*.sc",&stFd);
	//WCHAR szTemp[0x100];
	//if(hFind!=INVALID_HANDLE_VALUE)
	//{
	//	do{
	//		lstrcpy(szTemp,stFd.cFileName);
	//		for(int i=0;i<lstrlen(szTemp)-2;i++)
	//		{
	//			if(!lstrcmp(&szTemp[i],L".sc"))
	//			{
	//				lstrcpy(&szTemp[i],L".txt");
	//				if(ParseScript(stFd.cFileName,szTemp))
	//					__asm int 3
	//				break;
	//			}
	//		}
	//	}while(FindNextFile(hFind,&stFd));
	//	FindClose(hFind);
	//}

	return 0;
}