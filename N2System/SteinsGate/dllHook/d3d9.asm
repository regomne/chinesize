.486
.model flat,stdcall
option casemap:none

ADDR_INFOS			equ	621d08h
NUM_INFOS			equ	82
ADDR_NAMES		equ	621c28h
NUM_NAMES			equ	13
ADDR_MAILS			equ	643D68H
NUM_MAILS			equ	27EH
NUM_MAILLINES		equ	1752
ADDR_BBS			equ 61cb08h
NUM_BBS			equ 1bh

ADDR_RDATA			EQU	596000H
LEN_RDATA			EQU	67000H
ADDR_TEXTSEG		EQU	401000h
LEN_TEXTSEG			EQU	195000H

ADDR_SWT			EQU	4A2ABFh
ADDR_CFI			EQU	49af0eh

ADDR_NPAOBJECT	EQU	69490ch
F_FINDNPA			EQU	4a6760h
F_GETFNAME			EQU	405de0h
F_FINDFILEINNPA		EQU	4423f0h
F_MEMCMP			EQU 408250h

ADDR_FN			EQU	4a742bh
ADDR_GN			EQU	4a7445h

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

DllMain proc uses edi ebx _hInstance,_dwReason,_unused
	local @mbi:MEMORY_BASIC_INFORMATION
	LOCAL @hInfo,@hName,@hMail,@hBbs
	LOCAL @lpInfo,@lpName,@lpMail,@lpBbs
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
		
		PatchCallImport ADDR_SWT,offset NewSWT
;		PatchCallImport ADDR_CFI,offset NewCFI
		PatchCall ADDR_GN,OFFSET NewGetFname
		PatchCall ADDR_FN,offset NewFindNpa 
		
		invoke CreateFileW,$CTW0("info.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		cmp eax,-1
		je _Name1
		mov @hInfo,eax
		invoke GetFileSize,eax,0
		mov ebx,eax
		invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Name1
		mov @lpInfo,eax
		invoke ReadFile,@hInfo,@lpInfo,ebx,offset dwTemp,0
		invoke CloseHandle,@hInfo
		invoke ReadInfo,@lpInfo,ebx
	_Name1:
		invoke CreateFileW,$CTW0("name.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		cmp eax,-1
		je _Mail1
		mov @hName,eax
		invoke GetFileSize,eax,0
		mov ebx,eax
		invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Mail1
		mov @lpName,eax
		invoke ReadFile,@hName,@lpName,ebx,offset dwTemp,0
		invoke CloseHandle,@hName
		invoke ReadName,@lpName,ebx
	_Mail1:
		invoke CreateFileW,$CTW0("mail.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		cmp eax,-1
		je _BBS
		mov @hMail,eax
		invoke GetFileSize,eax,0
		mov ebx,eax
		invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _BBS
		mov @lpMail,eax
		invoke ReadFile,@hMail,@lpMail,ebx,offset dwTemp,0
		invoke CloseHandle,@hMail
		invoke ReadMail,@lpMail,ebx
	_BBS:
		invoke CreateFileW,$CTW0("bbs.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		cmp eax,-1
		je _Ex
		mov @hBbs,eax
		invoke GetFileSize,eax,0
		mov ebx,eax
		invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Ex
		mov @lpBbs,eax
		invoke ReadFile,@hBbs,@lpBbs,ebx,offset dwTemp,0
		invoke CloseHandle,@hBbs
		invoke ReadBbs,@lpBbs,ebx
	.endif
_Ex:
	MOV EAX,TRUE
	ret
DllMain endp

NewSWT proc _hwnd,_lpText
	LOCAL @szStr[200]:byte
	invoke MultiByteToWideChar,932,0,_lpText,-1,addr @szStr,100
	invoke SetWindowTextW,_hwnd,addr @szStr
	ret
NewSWT endp

NewCFI proc _lplf
	mov eax,_lplf
	invoke lstrcpyW,addr LOGFONT.lfFaceName[eax],$CTW0('SimHei')
	invoke CreateFontIndirectW,_lplf
	ret
NewCFI endp

ReadInfo proc uses edi ebx esi _lpFile,_nSize
	LOCAL @pEnd
	mov esi,_lpFile
	.if word ptr [esi]==0feffh
		invoke VirtualAlloc,0,_nSize,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Ex
		mov ebx,eax
		add esi,2
		mov eax,_nSize
		sub eax,2
		shr eax,1
		invoke WideCharToMultiByte,936,0,esi,eax,ebx,_nSize,0,0
		mov _nSize,eax
		invoke VirtualFree,_lpFile,0,MEM_RELEASE
		mov _lpFile,ebx
	.endif
	
	mov esi,_lpFile
	mov eax,esi
	add eax,_nSize
	mov @pEnd,eax
	xor ebx,ebx
	.while esi<@pEnd
		.if word ptr [esi]==0a0dh
			mov word ptr [esi],0
			inc ebx
		.endif
		inc esi
	.endw
	.if esi>_lpFile && word ptr [esi-2]!=0
		inc ebx
	.endif
	
	cmp ebx,NUM_INFOS
	jne _Ex
	
	mov edi,ADDR_INFOS
	add edi,nDist
	xor ebx,ebx
	mov esi,_lpFile
	.while ebx<NUM_INFOS
		invoke lstrlen,[edi]
		push eax
		invoke lstrlen,esi
		pop ecx
		push eax
		.if ecx>=eax
			invoke lstrcpy,[edi],esi
		.endif
		pop eax
		lea esi,[esi+eax+2]
		add edi,4
		inc ebx
	.endw
	
_Ex:
	ret
ReadInfo endp

ReadName proc uses edi ebx esi _lpFile,_nSize
	LOCAL @pEnd
	mov esi,_lpFile
	.if word ptr [esi]==0feffh
		invoke VirtualAlloc,0,_nSize,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Ex
		mov ebx,eax
		add esi,2
		mov eax,_nSize
		sub eax,2
		shr eax,1
		invoke WideCharToMultiByte,936,0,esi,eax,ebx,_nSize,0,0
		mov _nSize,eax
		invoke VirtualFree,_lpFile,0,MEM_RELEASE
		mov _lpFile,ebx
	.endif
	
	mov esi,_lpFile
	mov eax,esi
	add eax,_nSize
	mov @pEnd,eax
	xor ebx,ebx
	.while esi<@pEnd
		.if word ptr [esi]==0a0dh
			mov word ptr [esi],0
			inc ebx
		.endif
		inc esi
	.endw
	.if esi>_lpFile && word ptr [esi-2]!=0
		inc ebx
	.endif
	
	cmp ebx,NUM_NAMES
	jne _Ex
	
	mov edi,ADDR_NAMES
	add edi,nDist
	assume edi:ptr _Name
	xor ebx,ebx
	mov esi,_lpFile
	.while ebx<NUM_NAMES
		invoke lstrlen,[edi].lpszName
		push eax
		invoke lstrlen,esi
		pop ecx
		push eax
		.if ecx>=eax
			invoke lstrcpy,[edi].lpszName,esi
		.endif
		pop eax
		lea esi,[esi+eax+2]
		add edi,sizeof _Name
		inc ebx
	.endw
	assume edi:nothing
_Ex:
	ret
ReadName endp

GetStrNoPrefix proc _lpsz
	mov ecx,_lpsz
	mov ax,[ecx]
	.if ax==' S' || ax==' N' || ax==' T'
		lea eax,[ecx+2]
		ret
	.endif
	.if al==' '
		lea eax,[ecx+1]
		ret
	.endif
	mov eax,ecx
	ret
GetStrNoPrefix endp

_AllReplace proc uses esi edi _lpOld,_nSize,_lpNew
	LOCAL @pEnd
	mov esi,_lpOld
	mov edi,_lpNew
	mov eax,esi
	add eax,_nSize
	mov @pEnd,eax
	.while esi<@pEnd
		.if byte ptr [esi]>80h
			mov ax,[esi]
			add esi,2
			mov [edi],ax
			add edi,2
		.elseif word ptr [esi]=='n\'
			add esi,2
			mov al,0ah
			mov [edi],al
			inc edi
		.elseif word ptr [esi]=='r\'
			add esi,2
			mov al,0dh
			mov [edi],al
			inc edi
		.else
			mov al,[esi]
			inc esi
			mov [edi],al
			inc edi
		.endif
	.endw
	sub edi,_lpNew
	mov eax,edi
	ret
_AllReplace endp

ReadMail proc uses edi ebx esi _lpFile,_nSize
	LOCAL @pEnd
	LOCAL @lpNew
	mov esi,_lpFile
	.if word ptr [esi]==0feffh
		invoke VirtualAlloc,0,_nSize,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Ex
		mov ebx,eax
		add esi,2
		mov eax,_nSize
		sub eax,2
		shr eax,1
		invoke WideCharToMultiByte,936,0,esi,eax,ebx,_nSize,0,0
		mov _nSize,eax
		invoke VirtualFree,_lpFile,0,MEM_RELEASE
		mov _lpFile,ebx
	.endif
	
	invoke VirtualAlloc,0,_nSize,MEM_COMMIT,PAGE_READWRITE
	or eax,eax
	je _Ex
	mov ebx,eax
	invoke _AllReplace,_lpFile,_nSize,ebx
	mov _nSize,eax
	invoke VirtualFree,_lpFile,0,MEM_RELEASE
	mov _lpFile,ebx
	
	mov esi,_lpFile
	mov eax,esi
	add eax,_nSize
	mov @pEnd,eax
	xor ebx,ebx
	.while esi<@pEnd
		.if word ptr [esi]==0a0dh
			mov word ptr [esi],0
			inc ebx
		.endif
		inc esi
	.endw
	.if esi>_lpFile && word ptr [esi-2]!=0
		inc ebx
	.endif
	
	cmp ebx,NUM_MAILLINES
	jne _Ex
	
	mov edi,ADDR_MAILS
	add edi,nDist
	mov esi,_lpFile
	xor ebx,ebx
	assume edi:ptr _Mail
	.while ebx<NUM_MAILS
		.if [edi].lpszSub
			invoke GetStrNoPrefix,esi
			mov [edi].lpszSub,eax
			.while byte ptr [eax]
				inc eax
			.endw
			lea esi,[eax+2]
		.endif
		.if [edi].lpszTo
			invoke GetStrNoPrefix,esi
			mov [edi].lpszTo,eax
			.while byte ptr [eax]
				inc eax
			.endw
			lea esi,[eax+2]
		.endif
		.if [edi].lpszText
			invoke GetStrNoPrefix,esi
			mov [edi].lpszText,eax
			.while byte ptr [eax]
				inc eax
			.endw
			lea esi,[eax+2]
		.endif
		add edi,sizeof _Mail
		inc ebx
	.endw
	assume edi:nothing
_Ex:
	ret
ReadMail endp

ReadBbs proc uses esi edi ebx _lpFile,_nSize
	LOCAL @pEnd
	LOCAL @lpNew
	LOCAL @nPage
	mov esi,_lpFile
	.if word ptr [esi]==0feffh
		invoke VirtualAlloc,0,_nSize,MEM_COMMIT,PAGE_READWRITE
		or eax,eax
		je _Ex
		mov ebx,eax
		add esi,2
		mov eax,_nSize
		sub eax,2
		shr eax,1
		invoke WideCharToMultiByte,936,0,esi,eax,ebx,_nSize,0,0
		mov _nSize,eax
		invoke VirtualFree,_lpFile,0,MEM_RELEASE
		mov _lpFile,ebx
	.endif
	
	mov eax,_lpFile
	mov esi,eax
	add eax,_nSize
	mov @pEnd,eax
	
	.while esi<@pEnd
		.if word ptr [esi]==0a0dh
			mov word ptr [esi],0
		.endif
		inc esi
	.endw
	mov esi,_lpFile
	
	mov edi,ADDR_BBS
	mov ebx,edi
	mov @nPage,0
	.while esi<@pEnd
		.if dword ptr [esi]=='***/' && dword ptr [esi+4]=='****'
			xor eax,eax
			mov [edi],eax
			lea edi,[ebx+768]
			mov ebx,edi
			invoke lstrlen,esi
			lea esi,[esi+eax+2]
			inc @nPage
			.continue
		.endif
		mov eax,esi
		mov [edi],eax
		add edi,4
		invoke lstrlen,eax
		lea esi,[esi+eax+2]
	.endw
_Ex:
	ret
ReadBbs endp

NewFindNpa proc uses ebx scPack
	LOCAL @pTree
	mov @pTree,ebx
	mov eax,scPack
	cmp word ptr [eax+4],'gc'
	jne _Old
	cmp byte ptr [eax+6],0
	jne _Old
	
	push offset scCgcn
	mov eax,F_FINDNPA
	add eax,nDist
	call eax
	mov ecx,[eax]
	mov lpCgcnTbl,ecx
	
_Old:
	mov ebx,@pTree
	push scPack
	mov eax,F_FINDNPA
	add eax,nDist
	call eax
	ret
NewFindNpa endp

NewGetFname proc uses edi name1
	push esi
	mov edx,F_GETFNAME
	add edx,nDist
	push name1
	call edx
	lea esi,[esp+14h]
	.if dword ptr [esi+18h]>=10h
		mov edx,[esi+4]
	.else
		lea edx,[esi+4]
	.endif
	cmp word ptr [edx],'gc'
	jne _Ex
	cmp byte ptr [edx+2],'/'
	jne _Ex
	
	mov byte ptr [edx+1],'c'
	mov ecx,lpCgcnTbl
	lea eax,[ecx+10h]
	mov edi,[eax+18h]
	mov ecx,esi
	mov edx,F_FINDFILEINNPA
	add edx,nDist
	call edx
	cmp eax,edi
	je _Ex2
	mov edi,eax
	mov eax,[edi+24h]
	.if eax>=10h
		mov edx,[edi+10h]
	.else
		lea edx,[edi+10h]
	.endif
	mov ecx,[esi+14h]
	push ecx
	push edx
	mov eax,esi
	.if dword ptr [eax+18h]>=10h
		mov eax,[eax+4]
	.else
		lea eax,[eax+4]
	.endif
	push eax
	mov ecx,F_MEMCMP
	add ecx,nDist
	call ecx
	add esp,0ch
	mov edx,[edi+20h]
	test eax,eax
	jnz _Ex2
	cmp edx,[esi+14h]
	jne _Ex2
	
	mov esi,lpCgcnTbl
	add esp,4
	ret
_Ex2:
	.if dword ptr [esi+18h]>=10h
		mov edx,[esi+4]
	.else
		lea edx,[esi+4]
	.endif
	mov byte ptr [edx+1],'g'
_Ex:
	pop esi
	ret
NewGetFname endp

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