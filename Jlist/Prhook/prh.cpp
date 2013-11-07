#include <windows.h>

#define ISWIPINARC_ADDR 0x4651b0
#define GETWIPINFO_ADDR 0x465260
#define READMEM_ADDR 0x4654d0
#define READMEM2_ADDR 0x465820

#define REG_MOV(var1,var2) __asm{mov var1,var2}

#pragma comment(linker,"/entry:DllMain")

BOOL PatchFunc(BYTE* Dest, void* lpNewFunc)
{
	DWORD temp;
	BOOL rslt=VirtualProtect(Dest,5,PAGE_EXECUTE_READWRITE,&temp);
	if(!rslt)
		return FALSE;

	*Dest=0xe9;
	*(DWORD*)(Dest+1)=(BYTE*)lpNewFunc-(Dest+5);
	return TRUE;
}

BOOL HookApi(TCHAR* lpszDll,TCHAR* lpszFunc,void* lpNewFunc)
{
	HMODULE hm=GetModuleHandle(lpszDll);
	if(hm==NULL)
		return FALSE;
	
	BYTE* vf=(BYTE*)GetProcAddress(hm,lpszFunc);
	if(vf==NULL)
		return FALSE;
	
	if(*(WORD*)vf!=0xff8b)
		return FALSE;
	
	if(*(DWORD*)(vf-5)!=0x90909090 || *(vf-1)!=0x90)
		return FALSE;
	
	DWORD oldProtect;
	BOOL rslt=VirtualProtect(vf-5,7,PAGE_EXECUTE_READWRITE,&oldProtect);
	if(!rslt)
		return FALSE;
	
	//BYTE* nf=lpNewFunc;
	*(WORD*)vf=0xf9eb;
	*(vf-5)=0xe9;
	*(DWORD*)(vf-4)=(BYTE*)lpNewFunc-vf;
	return TRUE;
}

int __declspec(naked) MyCFI()
{
	__asm
	{
		mov eax,[esp+4]
		mov dword ptr [eax+1ch],486d6953h
		mov dword ptr [eax+20h],6965h
		mov eax,CreateFontIndirectA
		add eax,2
		jmp eax
	}
}

BOOL __declspec(naked) __cdecl OldIsWipInArc()
{
	__asm
	{
		sub esp,10h
		push esi
		test edx,edx
		mov eax,ISWIPINARC_ADDR+6
		jmp eax
	}
}

BOOL __cdecl IsWipInArc()
{
	char* lpszArc,*lpszWip;
	REG_MOV(lpszArc,edx);
	REG_MOV(lpszWip,edi);

	if(!lpszArc || strcmpi("CHIP.ARC",lpszArc))
	{
Ori:
		REG_MOV(edx,lpszArc);
		REG_MOV(edi,lpszWip);
		return OldIsWipInArc(); //Œ£œ’
	}
	
	char* myArc="CHIP_CH.ARC";
	__asm
	{
		mov edx,myArc
		mov edi,lpszWip
	}
	BOOL rslt=OldIsWipInArc();
	if(rslt)
		return rslt;
	else
		goto Ori;
}

BOOL __declspec(naked) __cdecl OldGetWipInfo(char* lpszArc,DWORD* bitCount,void* rect,DWORD unk1,DWORD unk2)
{
	__asm
	{
		sub esp,230h
		mov ecx,GETWIPINFO_ADDR+6
		jmp ecx
	}
}

BOOL __cdecl GetWipInfo(char* lpszArc,DWORD* bitCount,void* rect,DWORD unk1,DWORD unk2)
{
	char* lpszWip;
	BOOL rslt;
	REG_MOV(lpszWip,eax);
	if(!lpszArc || strcmpi("CHIP.ARC",lpszArc))
	{
Ori:
		//REG_MOV(eax,lpszWip);
		//return OldGetWipInfo(lpszArc,bitCount,rect,unk1,unk2);
		__asm{
			push unk2
			push unk1
			push rect
			push bitCount
			push lpszArc
			mov eax,lpszWip
			call OldGetWipInfo
			mov rslt,eax
		}
		return rslt;
	}
	char* myArc="CHIP_CH.ARC";
	__asm{
		push unk2
		push unk1
		push rect
		push bitCount
		push myArc
		mov eax,lpszWip
		call OldGetWipInfo
		mov rslt,eax
	}
	if(rslt)
		return rslt;
	else
		goto Ori;
}

BOOL __declspec(naked) __cdecl OldReadMem(void* lpRect,BYTE* lpDib,DWORD count,DWORD unk2)
{
	__asm
	{
		sub esp,564h
		mov ecx,READMEM_ADDR+6
		jmp ecx
	}
}

BOOL __cdecl ReadToMemory(void* lpRect,BYTE* lpDib,DWORD count,DWORD unk2)
{
	char* lpszArc;
	char* lpszWip;
	BOOL rslt;
	REG_MOV(lpszArc,edx);
	REG_MOV(lpszWip,eax);
	if(!lpszArc || strcmpi("CHIP.ARC",lpszArc))
	{
Ori:
		//REG_MOV(edx,lpszArc);
		//REG_MOV(eax,lpszWip);
		//return OldReadMem(lpRect,lpDib,count,unk2);
		__asm{
			push unk2
			push count
			push lpDib
			push lpRect
			mov edx,lpszArc
			mov eax,lpszWip
			call OldReadMem
			mov rslt,eax
		}
		return rslt;
	}
	char* myArc="CHIP_CH.ARC";
	__asm{
		push unk2
		push count
		push lpDib
		push lpRect
		mov edx,myArc
		mov eax,lpszWip
		call OldReadMem
		mov rslt,eax
	}
	if(rslt)
		return rslt;
	else
		goto Ori;
}

BOOL __declspec(naked) __cdecl OldReadMem2(char* lpszArc,void* lpDib,int unk1,int unk2)
{
	__asm{
		push -1
		push 4b67beh
		mov eax,READMEM2_ADDR+7
		jmp eax
	}
}

BOOL __cdecl ReadToMemory2(char* lpszArc,void* lpDib,int unk1,int unk2)
{
	int* lpUnk2;
	char* lpszWip;
	BOOL rslt;
	REG_MOV(lpszWip,edx);
	REG_MOV(lpUnk2,ecx);

	if(!lpszArc || strcmpi("CHIP.ARC",lpszArc))
	{
Ori:
		__asm{
			push unk2
			push unk1
			push lpDib
			push lpszArc
			mov edx,lpszWip
			mov ecx,lpUnk2
			call OldReadMem2
			mov rslt,eax
		}
		return rslt;
	}

	char* myArc="CHIP_CH.ARC";
	__asm{
		push unk2
		push unk1
		push lpDib
		push myArc
		mov edx,lpszWip
		mov ecx,lpUnk2
		call OldReadMem2
		mov rslt,eax
	}
	if(rslt)
		return rslt;
	else
		goto Ori;

}

int WINAPI DllMain(HANDLE hInst, DWORD dwReason, LPVOID lpResvd)
{
	if(dwReason==DLL_PROCESS_ATTACH)
	{
		PatchFunc((BYTE*)ISWIPINARC_ADDR,IsWipInArc);
		PatchFunc((BYTE*)GETWIPINFO_ADDR,GetWipInfo);
		PatchFunc((BYTE*)READMEM_ADDR,ReadToMemory);
		PatchFunc((BYTE*)READMEM2_ADDR,ReadToMemory2);

		HookApi("gdi32.dll","CreateFontIndirectA",MyCFI);
	}
	return TRUE;
}

extern "C" int __declspec(dllexport) Func();
int Func()
{
	return 0;
}
