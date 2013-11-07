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


start proc
	LOCAL @hdr:_IsmHdr
	LOCAL @hFile
	LOCAL @lpIndex,@nEntries
	LOCAL @lpszT
	
	invoke MessageBox,0,$CTA0("提取data0.isa中的ism文件？"),$CTA0("AmaF"),MB_YESNO
	.if eax==IDNO
		ret
	.endif
	
	invoke CreateFile,$CTA0("data0.isa"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
		int 3
	.endif
	mov @hFile,eax
	
	invoke SetCurrentDirectory,$CTA0("data0")
	
	invoke ReadFile,@hFile,addr @hdr,sizeof @hdr,offset dwTemp,0
	movzx eax,@hdr.uEntries
	mov @nEntries,eax
	
	shl eax,6 
	invoke HeapAlloc,hHeap,0,eax
	.if !eax
		int 3
	.endif
	mov @lpIndex,eax
	
	mov eax,@nEntries
	shl eax,6
	invoke ReadFile,@hFile,@lpIndex,eax,offset dwTemp,0
	
	xor ebx,ebx
	mov esi,@lpIndex
	.while ebx<@nEntries
		mov edx,esi
		.while byte ptr [esi]
			inc esi
		.endw
		lea ecx,[esi-4]
		mov esi,edx
		invoke lstrcmpi,ecx,$CTA0(".ism")
		.if !eax
			invoke SetFilePointer,@hFile,[esi+34h],0,FILE_BEGIN
			invoke HeapAlloc,hHeap,0,[esi+38h]
			.if !eax
				int 3
			.endif
			mov edi,eax
			invoke ReadFile,@hFile,edi,[esi+38h],offset dwTemp,0
			invoke CreateFile,esi,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			push eax
			invoke WriteFile,eax,edi,[esi+38h],offset dwTemp,0
			call CloseHandle
			invoke HeapFree,hHeap,0,edi
		.endif
		add esi,40h
		inc ebx
	.endw
	
	ret
start endp

end start0