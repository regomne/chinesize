.486
.model flat,stdcall
option casemap:none

include temp.inc

.code

start0:
invoke GetProcessHeap
mov hHeap,eax
call start
invoke ExitProcess,0
ret

_DecodeString proc uses esi edi ebx _lpFile,_nSize
	LOCAL @lpCS,@lpCSEnd
	
	mov esi,_lpFile
	add esi,dword ptr [esi+10h]
	mov @lpCS,esi
	
	mov esi,_lpFile
	mov ecx,[esi+1ch]
	shl ecx,2
	lea eax,[ecx+ecx*2]
	lea esi,[esi+eax+20h-12]
	lodsd
	add eax,@lpCS
	.if byte ptr [eax]!=24h
		int 3
	.endif
	add eax,dword ptr [eax+1]
	mov @lpCSEnd,eax
	
	mov esi,@lpCS
	.while esi<@lpCSEnd
		xor eax,eax
		lodsb
		movsx eax,byte ptr [eax+offset IsmInstTable]
		cmp eax,0
		jnl _NextInst
		.if eax==-1
			int 3
		.endif
		.if eax==-3
			lea ecx,[esi-1]
			sub ecx,@lpCS
			xor eax,eax
			lodsb
			.if al==0ffh
				lodsd
			.endif
			.if cl==0ffh
				add esi,eax
				.continue
			.endif
			.while eax
				xor byte ptr [esi],cl
				inc esi
				dec eax
			.endw
		.endif
	_NextInst:
		add esi,eax
	.endw
	
	ret
_DecodeString endp

start proc
	LOCAL @stFd:WIN32_FIND_DATA
	LOCAL @hFind
	LOCAL @hFile,@nFileSize,@lpFile
	LOCAL @lpIndex,@nEntries
	LOCAL @lpszT
	
	invoke MessageBox,0,$CTA0("½«×Ö·û´®½âÃÜ£¿"),$CTA0("AmaF"),MB_YESNO
	.if eax==IDNO
		ret
	.endif
	
	invoke SetCurrentDirectory,$CTA0("data0")
	invoke FindFirstFile,$CTA0("*.ism"),addr @stFd
	.if eax!=-1
		mov @hFind,eax
		.repeat
			invoke CreateFile,addr @stFd.cFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			mov @hFile,eax
			invoke GetFileSize,@hFile,0
			mov @nFileSize,eax
			invoke HeapAlloc,hHeap,0,eax
			.if !eax
				int 3
			.endif
			mov @lpFile,eax
			invoke ReadFile,@hFile,@lpFile,@nFileSize,offset dwTemp,0
			
			invoke _DecodeString,@lpFile,@nFileSize
			
			invoke SetFilePointer,@hFile,0,0,FILE_BEGIN
			invoke WriteFile,@hFile,@lpFile,@nFileSize,offset dwTemp,0
			invoke CloseHandle,@hFile
			invoke HeapFree,hHeap,0,@lpFile
			
			invoke FindNextFile,@hFind,addr @stFd
		.until !eax
		invoke FindClose,@hFind
	.endif
	
	ret
start endp

end start0