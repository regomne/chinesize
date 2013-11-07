#include<windows.h>
#include"Code.h"
#include"system4.h"

BOOL PreScan4(char* lpFile,int nSize,AinSegs* lpAinSegs)
{
	return FALSE;
}
BOOL PreScan6(char* lpFile,int nSize,AinSegs* lpAinSegs)
{
	for(char* pf=lpFile;pf<lpFile+nSize;)
	{
		int tab=*(int*)pf;
		int m,n,x,y;
		int* p1;
		int** p2;
		pf+=4;
		switch(tab)
		{
		case 'SREV':
			lpAinSegs->Version.lpAddr=pf;
			lpAinSegs->Version.nSize=4;
			pf+=4;
			break;
		case 'EDOC':
			lpAinSegs->Code.nSize=*(int*)pf;
			lpAinSegs->Code.lpAddr=pf+4;
			pf+=*(int*)pf+4;
			break;
		case 'CNUF':
			m=*(int*)pf;
			lpAinSegs->Function.lpRestruct=new int*[m+2];
			lpAinSegs->Function.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(lpAinSegs->Function.lpRestruct);
			for(int i=0;i<m;i++)
			{
				if(*(short*)((char*)lpAinSegs->Code.lpAddr+*p1-6)!=0x61)
					__asm{int 3}
				y=*(int*)((char*)lpAinSegs->Code.lpAddr+*p1-4);
				p2[y]=p1;
				p1++;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
				p1+=6;
				n=*(p1-2);
				for(int j=0;j<n;j++)
				{
					p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
					p1+=3;
				}
			}
			lpAinSegs->Function.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'BOLG':
			m=*(int*)pf;
			lpAinSegs->GlobalVar.lpRestruct=new int*[m];
			lpAinSegs->GlobalVar.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->GlobalVar.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
				p1+=4;
			}
			lpAinSegs->GlobalVar.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'TESG':
			m=*(int*)pf;
			lpAinSegs->GlobalSet.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			for(int i=0;i<m;i++)
			{
				p1+=2;
				if(*(p1-1)==TYPE_STRING)
					p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
				else
					p1++;
			}
			lpAinSegs->GlobalSet.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'TRTS':
			m=*(int*)pf;
			lpAinSegs->Structs.lpRestruct=new int*[m];
			lpAinSegs->Structs.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->Structs.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
				p1+=3;
				n=*(p1-1);
				for(int j=0;j<n;j++)
				{
					p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
					p1+=3;
				}
			}
			lpAinSegs->Structs.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case '0GSM':
			m=*(int*)pf;
			lpAinSegs->Message0.lpRestruct=new int*[m];
			lpAinSegs->Message0.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->Message0.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
			}
			lpAinSegs->Message0.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'NIAM':
			lpAinSegs->Main.lpAddr=pf;
			lpAinSegs->Main.nSize=4;
			pf+=4;
			break;
		case 'FGSM':
			lpAinSegs->MessageFunc.lpAddr=pf;
			lpAinSegs->MessageFunc.nSize=4;
			pf+=4;
			break;
		case '0LLH':
			m=*(int*)pf;
			lpAinSegs->HLL.lpRestruct=new int*[m];
			lpAinSegs->HLL.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->HLL.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
				n=*p1++;
				for(int j=0;j<n;j++)
				{
					p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
					p1+=2;
					x=*(p1-1);
					for(int k=0;k<x;k++)
					{
						p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
						p1++;
					}
				}
			}
			lpAinSegs->HLL.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case '0IWS':
			m=*(int*)pf;
			lpAinSegs->SwitchData.lpRestruct=new int*[m];
			lpAinSegs->SwitchData.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->SwitchData.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1+=3;
				p1+=*(p1-1)*2;
			}
			lpAinSegs->SwitchData.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'REVG':
			lpAinSegs->GameVersion.lpAddr=pf;
			lpAinSegs->GameVersion.nSize=4;
			pf+=4;
			break;
		case '0RTS':
			m=*(int*)pf;
			lpAinSegs->Strings.lpRestruct=new int*[m];
			lpAinSegs->Strings.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->Strings.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
			}
			lpAinSegs->Strings.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'MANF':
			m=*(int*)pf;
			lpAinSegs->FileName.lpRestruct=new int*[m];
			lpAinSegs->FileName.lpAddr=pf;
			pf+=4;
			p1=(int*)pf;
			p2=(int**)lpAinSegs->FileName.lpRestruct;
			for(int i=0;i<m;i++)
			{
				*p2++=p1;
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
			}
			lpAinSegs->FileName.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		case 'PMJO':
			lpAinSegs->OnJump.lpAddr=pf;
			lpAinSegs->OnJump.nSize=4;
			pf+=4;
			break;
		case 'TCNF':
			lpAinSegs->FunctionType.lpAddr=pf;
			lpAinSegs->FunctionType.nSize=*(int*)pf;
			pf+=*(int*)pf;
			break;
		case 'CYEK':
			lpAinSegs->OnJump.lpAddr=pf;
			lpAinSegs->OnJump.nSize=4;
			pf+=4;
			break;
		case 'GJBO':
			lpAinSegs->ObjG.lpAddr=pf;
			m=*(int*)pf;
			pf+=4;
			p1=(int*)pf;
			for(int i=0;i<m;i++)
			{
				p1=(int*)((char*)p1+lstrlenA((char*)p1)+1);
			}
			lpAinSegs->ObjG.nSize=(char*)p1-pf+4;
			pf=(char*)p1;
			break;
		default:
			return FALSE;
		}
	}
	return TRUE;
}

BOOL PreScan(char* lpFile,int nSize,AinSegs* lpAinSegs)
{
	char* pf=lpFile;
	int n=*(int*)pf;
	if(n==4)
		return PreScan4(lpFile,nSize,lpAinSegs);
	else
		return PreScan6(lpFile,nSize,lpAinSegs);
}