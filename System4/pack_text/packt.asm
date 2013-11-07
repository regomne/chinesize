.486
.model flat,stdcall
option casemap:none


include packt.inc
include prescan.asm

.data
	szAinName		db		'´óµÛ¹ú0.ain',0
	szNewAinName	db		'´óµÛ¹ú.ain',0
	szTxtName		db		'strings.txt',0

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,0
mov hInstance,eax
invoke GetProcessHeap
mov hHeap,eax
call MyMain
invoke ExitProcess,NULL


MyMain proc
	LOCAL @hFile,@hTxt,@nFileSize
	LOCAL @nFuncs
	LOCAL @lpBuff,@lpTemp
	LOCAL @lpTxt,@lpMsg0
	LOCAL @lpAinInfo
	
	invoke CreateFile,offset szAinName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hFile,eax
	
	invoke GetFileSize,@hFile,0
	mov ebx,eax
	mov @nFileSize,ebx
	invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
	mov @lpBuff,eax
	invoke ReadFile,@hFile,@lpBuff,ebx,offset dwTemp,0
	
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,sizeof _AinSegs
	mov @lpAinInfo,eax
	
	invoke _PreScan,@lpBuff,ebx,@lpAinInfo
	.if eax
		invoke MessageBox,0,$CTA0("Ô¤É¨ÃèÊ§°Ü£¡"),0,MB_ICONERROR
		invoke ExitProcess,0
	.endif
	
	invoke CreateFile,offset szTxtName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hTxt,eax
	invoke GetFileSize,eax,0
	mov ebx,eax
	invoke VirtualAlloc,0,@nFileSize,MEM_COMMIT,PAGE_READWRITE
	mov @lpTxt,eax
	invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
	mov @lpMsg0,eax
	invoke ReadFile,@hTxt,@lpTxt,ebx,offset dwTemp,0
	invoke CloseHandle,@hTxt
	
	invoke _MakeMsg0,@lpTxt,ebx,@lpMsg0
	mov edi,eax
	invoke VirtualAlloc,0,@nFileSize,MEM_COMMIT,PAGE_READWRITE
	mov @lpTemp,eax
	
	mov esi,@lpAinInfo
	assume esi:ptr _AinSegs
	mov eax,[esi].Strings.lpAddr
	sub eax,@lpBuff
	add eax,4
	mov edx,@lpTemp
	add edx,eax
	invoke _memcpy2,@lpTemp,@lpBuff,eax
	lea ebx,[edx+edi]
	invoke _memcpy2,edx,@lpMsg0,edi
	mov eax,@nFileSize
	mov ecx,[esi].Strings.lpAddr
	add ecx,[esi].Strings.nSize
	mov edx,ecx
	sub ecx,@lpBuff
	sub eax,ecx
	lea edi,[ebx+eax]
	invoke _memcpy2,ebx,edx,eax
	sub edi,@lpTemp
	
	mov ecx,@nFileSize
	mov dwTemp,ecx
	invoke compress,@lpTxt,offset dwTemp,@lpTemp,edi
	.if eax
		int 3
	.endif
	mov dword ptr [dbHdr+8],edi
	invoke CreateFile,offset szNewAinName,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov ebx,eax
	mov edi,dwTemp
	mov dword ptr [dbHdr+12],edi
	invoke WriteFile,ebx,offset dbHdr,16,offset dwTemp,0
	invoke WriteFile,ebx,@lpTxt,edi,offset dwTemp,0
;	invoke WriteFile,ebx,@lpTemp,edi,offset dwTemp,0
	invoke SetEndOfFile,ebx
	invoke CloseHandle,ebx
	
	assume esi:nothing
	
	ret
MyMain endp

_MakeMsg0 proc _lpTxt,_nTxtSize,_lpOutMsg0
	LOCAL @pEnd,@nLines
	mov esi,_lpTxt
	mov ecx,esi
	add ecx,_nTxtSize
	mov @pEnd,ecx
	
	mov edi,_lpOutMsg0
;	mov byte ptr [edi],0
;	inc edi
	
	mov @nLines,0
	.repeat
		mov ebx,esi
		xor ecx,ecx
		.while word ptr [esi]!=0a0dh
			.break .if esi>=@pEnd
;			.if word ptr [esi]==03a3ah
;				inc ecx
;				.if ecx==2
;					lea ebx,[esi+2]
;				.endif
;			.endif
			.if byte ptr [esi]=='$'
				mov byte ptr [esi],0ah
			.endif
			inc esi
		.endw
		mov ecx,esi
		mov esi,ebx
		sub ecx,ebx
		rep movsb
		add esi,2
		mov byte ptr [edi],0
		inc edi
		inc @nLines
	.until esi>=@pEnd
	sub edi,_lpOutMsg0
	mov eax,edi
	ret
_MakeMsg0 endp

_memcpy2 proc _dest,_src,_len
	push esi
	mov esi,_src
	mov ecx,_len
	mov eax,ecx
	shr ecx,2
	push edi
	mov edi,_dest
	rep movsd
	mov ecx,eax
	and ecx,3
	rep movsb
	mov eax,esi
	pop edi
	pop esi
	ret
_memcpy2 endp

end start