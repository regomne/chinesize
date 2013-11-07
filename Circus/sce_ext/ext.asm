.486
.model flat,stdcall
option casemap:none


include ext.inc

.code

start:
;……………………………………………………………………………………
call MyMain
invoke ExitProcess,NULL


;解sce*文件，scene replay中的小标题
MyMain proc
	local @stFindData:WIN32_FIND_DATA	
	LOCAL @pMesEnd
	LOCAL @nLine1,@nLine2
	lea edi,@stFindData
	mov ecx,sizeof WIN32_FIND_DATA
	xor eax,eax
	rep stosb
	invoke SetCurrentDirectory,offset szdirMes
	invoke FindFirstFile,offset szMes,addr @stFindData
	.if eax!=INVALID_HANDLE_VALUE
		mov hFindFile,eax
		.repeat
			lea edi,@stFindData.cFileName
			invoke CreateFile,edi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==INVALID_HANDLE_VALUE
				xor eax,eax
				ret
			.endif
			mov hFile,eax
			invoke GetFileSize,eax,0
			mov ebx,eax
			push eax
			invoke GlobalAlloc,GPTR,eax
			mov lpBuffer,eax
			add eax,ebx
			push GPTR
			mov @pMesEnd,eax
			call GlobalAlloc
			mov lpTxtBuffer,eax
			mov pTxt,eax
			invoke ReadFile,hFile,lpBuffer,ebx,offset dwTemp,0
			lea edi,@stFindData.cFileName
			mov eax,73656d2eh
			.while dword ptr [edi]!=eax
				inc edi
			.endw
			mov dword ptr [edi],7478742eh
			lea edi,@stFindData.cFileName
			invoke CreateFile,edi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			mov hFiletxt,eax
			
			mov esi,lpBuffer
			mov edi,lpTxtBuffer
			.if dword ptr [esi]!=1
				int 3
			.endif
			add esi,dword ptr [esi+4]
			add esi,8
			mov @nLine1,0
			.while esi<@pMesEnd
				.if word ptr [esi]==4500h && byte ptr [esi-1]==1
					add esi,2
					.if dword ptr [esi]==48814881h && dword ptr [esi+4]==48814881h
						add esi,11
						.continue
					.endif
					xor al,al
					or ecx,-1
					mov ebx,edi
					mov edi,esi
					repne scasb
					mov edi,ebx
					not ecx
					dec ecx
					
					add esi,ecx
;					rep movsb
;					mov ax,0a0dh
;					stosw
					inc @nLine1
				.else
					.break .if word ptr [esi]==12eh && word ptr [esi-2]==5
					inc esi
				.endif
			.endw
			mov @nLine2,0
			.while esi<@pMesEnd
				.if word ptr [esi]==12eh && word ptr [esi-2]==5
					add esi,2
					xor al,al
					or ecx,-1
					mov ebx,edi
					mov edi,esi
					repne scasb
					mov edi,ebx
					not ecx
					dec ecx
					rep movsb
					mov ax,0a0dh
					stosw
					inc @nLine2
				.else
					inc esi
				.endif
			.endw
			mov eax,@nLine1
			.if eax!=@nLine2
				int 3
			.endif

			sub edi,lpTxtBuffer
			invoke WriteFile,hFiletxt,lpTxtBuffer,edi,offset dwTemp,0
			invoke CloseHandle,hFiletxt
			invoke CloseHandle,hFile
			invoke GlobalFree,lpBuffer
			invoke GlobalFree,lpTxtBuffer
			invoke FindNextFile,hFindFile,addr @stFindData
		.until eax==FALSE
		invoke FindClose,hFindFile
	.endif
	ret
MyMain endp

end start