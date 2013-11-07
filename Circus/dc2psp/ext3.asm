.486
.model flat,stdcall
option casemap:none


include ext3.inc

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetProcessHeap
mov hHeap,eax
call MyMain
invoke ExitProcess,NULL


MyMain proc
	LOCAL @stFindData:WIN32_FIND_DATA
	LOCAL @szFn[40h]:byte
	LOCAL @hCode,@lpBuff
	LOCAL @lpTxt
	LOCAL @hFind
	
	invoke CreateFile,$CTA0("report.txt"),GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hCode,eax
	invoke HeapAlloc,hHeap,0,1000000
	mov @lpBuff,eax
	invoke ReadFile,@hCode,@lpBuff,100000,offset dwTemp,0
	xor ebx,ebx
	mov edi,@lpBuff
	invoke SetCurrentDirectory,$CTA0("dc2psp\\LineNotMatch")
	.while ebx<145
		mov edx,edi
		mov al,9
		or ecx,-1
		repne scasb
		mov byte ptr [edi-1],0
		lea esi,[edi+1]
		mov al,0dh
		repne scasb
		mov byte ptr [edi-1],0
		inc edi
		invoke MoveFileEx,edx,esi,0
		inc ebx
	.endw
	ret
MyMain endp

end start