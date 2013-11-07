.486
.model flat,stdcall
option casemap:none


include ext2.inc

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetProcessHeap
mov hHeap,eax
call MyMain
invoke ExitProcess,NULL

_ExtractTxt proc uses ebx esi edi _lpBuff,_nSize,_lpTxt
	mov esi,_lpBuff
	mov edi,_lpTxt
	mov edx,_nSize
	add edx,esi
	.while esi<edx
		.if dword ptr [esi]=='EgsM'
			.if dword ptr [esi+6]=='TgsM'
				mov al,'T'
				stosb
			.elseif dword ptr [esi+6]=='NgsM'
				mov al,'N'
				stosb
			.else
				jmp _NextLine
			.endif
			add esi,15h
		_AddLine:
			.if byte ptr [esi-1]!="'"
				int 3
			.endif
			mov ebx,esi
			.while word ptr [esi]!=2927h
				.break .if esi>=edx
				inc esi
			.endw
			sub esi,ebx
			mov ecx,esi
			mov esi,ebx
			rep movsb
			mov ax,0a0dh
			stosw
			add esi,4
			.if word ptr [esi-2]!=0a0dh
				int 3
			.endif
		.elseif dword ptr [esi]==' leS'
			.if dword ptr [esi+4]=='IleS'
				mov al,'S'
				stosb
				add esi,18h
				jmp _AddLine
			.elseif dword ptr [esi+4]=='AleS'
				mov al,'S'
				stosb
				add esi,17h
				jmp _AddLine
			.else
				jmp _NextLine
			.endif
		.else
	_NextLine:
			.while word ptr [esi]!=0a0dh
				.break .if esi>=edx
				inc esi
			.endw
			add esi,2
		.endif
	.endw
	sub edi,_lpTxt
	mov eax,edi
	ret
_ExtractTxt endp

MyMain proc
	LOCAL @stFindData:WIN32_FIND_DATA
	LOCAL @szFn[40h]:byte
	LOCAL @hCode,@lpBuff
	LOCAL @lpTxt
	LOCAL @hFind
	invoke SetCurrentDirectory,$CTA0("dc2psp")
	invoke FindFirstFile,$CTA0('*.code'),addr @stFindData
	.if eax
		mov @hFind,eax
		.repeat
			lea ecx,@stFindData
			invoke CreateFile,addr WIN32_FIND_DATA.cFileName[ecx],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je _NextFind
			mov @hCode,eax
			invoke GetFileSize,@hCode,0
			mov ebx,eax
			invoke HeapAlloc,hHeap,0,ebx
			or eax,eax
			je _NextFind2
			mov @lpBuff,eax
			invoke ReadFile,@hCode,@lpBuff,ebx,offset dwTemp,0
			invoke HeapAlloc,hHeap,0,ebx
			or eax,eax
			je _NextFind3
			mov @lpTxt,eax
			invoke _ExtractTxt,@lpBuff,ebx,@lpTxt
			mov ebx,eax
			
			lea ecx,@stFindData
			lea edi,@szFn
			invoke lstrcpy,edi,addr WIN32_FIND_DATA.cFileName[ecx]
			mov dword ptr [edi+15h],'txt'
			invoke CreateFile,edi,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			push eax
			invoke WriteFile,eax,@lpTxt,ebx,offset dwTemp,0
			call CloseHandle
			
			invoke HeapFree,hHeap,0,@lpTxt
		_NextFind3:
			invoke HeapFree,hHeap,0,@lpBuff
		_NextFind2:
			invoke CloseHandle,@hCode
		_NextFind:
			invoke FindNextFile,@hFind,addr @stFindData
		.until !eax
		invoke FindClose,@hFind
	.endif
	ret
MyMain endp

end start