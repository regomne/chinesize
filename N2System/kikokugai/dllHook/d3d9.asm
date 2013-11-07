.486
.model flat,stdcall
option casemap:none

ADDR_RDATA			EQU	50e000H
LEN_RDATA			EQU	23000H
ADDR_TEXTSEG		EQU	401000h
LEN_TEXTSEG			EQU	10d000H

ADDR_SWT			EQU	4A2ABFh
ADDR_CFI			EQU	49af0eh


include d3d9.inc


.code
assume fs:nothing

PatchCallImport macro _Addr,_Dest
	mov eax,_Addr+0
	add eax,nDist
	mov ecx,_Dest-5
	sub ecx,eax
	mov [eax+1],ecx
	mov byte ptr [eax],0e8h
	mov byte ptr [eax+5],90h
endm

PatchCall macro _Addr,_Dest
	mov eax,_Addr+0
	add eax,nDist
	mov ecx,_Dest-5
	sub ecx,eax
	mov [eax+1],ecx
	mov byte ptr [eax],0e8h
endm

HookApi proc uses ebx _lpszModuleName,_lpszName,_NewAddr
	invoke GetModuleHandle,_lpszModuleName
	or eax,eax
	je _Err
	invoke GetProcAddress,eax,_lpszName
	mov ebx,eax
	cmp word ptr [ebx],0ff8bh
	jne _Err
	lea ecx,[ebx-5]
	invoke VirtualProtect,ecx,7,PAGE_EXECUTE_READWRITE,offset dwTemp
	or eax,eax
	je _Err
	mov word ptr [ebx],0f9ebh
	mov eax,_NewAddr
	sub eax,ebx
	lea ecx,[ebx-5]
	mov [ecx+1],eax
	mov byte ptr [ecx],0e9h
	ret
_Err:
	xor eax,eax
	ret
HookApi endp

NewCrFile proc uses ebx esi fna,da,sm,sa,cd,att,ht
	mov eax,fna
	cmp word ptr [eax],'gc'
	jne _Old
	invoke lstrlen,fna
	inc eax
	mov esi,eax
	shl eax,1
	lea ebx,szGName
	.if eax>=256
		invoke GlobalAlloc,GMEM_FIXED,eax
		mov ebx,eax
	.endif
	invoke MultiByteToWideChar,932,0,fna,-1,ebx,esi
	invoke CreateFileW,ebx,da,sm,sa,cd,att,ht
	.if ebx!=offset szGName
		mov esi,eax
		invoke GlobalFree,ebx
		mov eax,esi
	.endif
	ret
_Old:
	mov eax,CreateFileA
	mov ecx,[eax+2]
	mov eax,[ecx]
	add eax,2
	push ht
	push att
	push cd
	push sa
	push sm
	push da
	push fna
	call eax
	ret
NewCrFile endp

NewCWE proc
	mov eax,CreateWindowExA
	mov ecx,[eax+2]
	mov eax,[ecx]
	add eax,2
	mov ecx,offset szTitle
	mov [esp+0ch],ecx
	jmp eax
NewCWE endp

NewCFI proc
	mov eax,CreateFontIndirectA
	mov ecx,[eax+2]
	mov eax,[ecx]
	add eax,2
	mov ecx,[esp+4]
	mov byte ptr [ecx+17h],86h
	jmp eax
NewCFI endp

DllMain proc uses edi ebx _hInstance,_dwReason,_unused
	local @mbi:MEMORY_BASIC_INFORMATION
	LOCAL @hInfo,@hName,@hMail
	LOCAL @lpInfo,@lpName,@lpMail
	.if _dwReason==DLL_PROCESS_ATTACH
		invoke GetModuleHandleW,0
		mov hInstance,eax
		lea ecx,[eax-400000h]
		mov nDist,ecx
		
		mov eax,ADDR_RDATA
		ADD EAX,nDist
		invoke VirtualProtect,eax,LEN_RDATA,PAGE_READWRITE,offset dwTemp
		mov eax,ADDR_TEXTSEG
		ADD EAX,nDist
		invoke VirtualProtect,eax,LEN_TEXTSEG,PAGE_EXECUTE_READWRITE,offset dwTemp

		invoke HookApi,$CTA0('kernel32.dll'),$CTA0('CreateFileA'),offset NewCrFile
		invoke HookApi,$CTA0('user32.dll'),$CTA0('CreateWindowExA'),offset NewCWE
		invoke HookApi,$CTA0('gdi32.dll'),$CTA0('CreateFontIndirectA'),offset NewCFI
	.endif
_Ex:
	MOV EAX,TRUE
	ret
DllMain endp


Direct3DCreate9 proc dwVersion
	LOCAL @szSystem[64]:word
	invoke GetSystemDirectoryW,addr @szSystem,64
	or eax,eax
	je _Ex
	invoke lstrcatW,addr @szSystem,$CTW0("\\d3d9.dll")
	invoke LoadLibraryW,addr @szSystem
	or eax,eax
	je _Ex
	invoke GetProcAddress,eax,$CTA0("Direct3DCreate9")
	or eax,eax
	je _Ex
	push dwVersion
	call eax
_Ex:
	ret
Direct3DCreate9 endp


end DllMain