.486
.model flat,stdcall
option casemap:none


include ext.inc

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
call MyMain
invoke ExitProcess,NULL


MyMain proc
	local @stFindData:WIN32_FIND_DATA	
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
			push GPTR
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
			mov edi,lpBuffer
			mov eax,dword ptr [edi]
			mov dwIndexes,eax
			shl eax,2
			add edi,4
			mov ebx,edi
			mov pIndex,ebx
			add edi,eax
			mov lpText,edi
			.while ebx<lpText
				mov edi,lpText
				add edi,dword ptr [ebx]
				@@:
				mov al,byte ptr [edi]
				.if al==36h
					xor eax,eax
					or ecx,-1
					repne scasb
					jmp @B
				.elseif al==4bh
					inc edi
					mov esi,edi
					xor eax,eax
					or ecx,-1
					repne scasb
					not ecx
					dec ecx
					mov edi,pTxt
					aa:
					lodsb
					add al,20h
					stosb
					loop aa
					mov word ptr [edi],0a0dh
					add edi,2
					mov pTxt,edi
					mov edi,esi
					inc edi
					jmp @B
				.elseif al==4ch
					inc edi
;					.if word ptr [edi]==2561h && !byte ptr [edi+2]
;						jmp @F
;					.endif
					mov esi,edi
					xor ecx,ecx
					.while byte ptr [edi]
						add byte ptr [edi],20h
						inc edi
						inc ecx
					.endw
					mov edi,pTxt
					.while ecx
						.if byte ptr [esi]>80h
							movsw
							sub ecx,2
						.elseif byte ptr [esi]==0ah
							mov ax,6e5ch
							stosw
							inc esi
							dec ecx
						.else
							movsb
							dec ecx
						.endif
					.endw
					mov word ptr [edi],0a0dh
					add edi,2
					mov pTxt,edi
				.elseif al==08h
					add edi,3
					.while byte ptr [edi]==44h
						add edi,5
						mov esi,edi
						xor eax,eax
						or ecx,-1
						repne scasb
						not ecx
						dec ecx
						mov edi,pTxt
						ac:
						lodsb
						add al,20h
						stosb
						loop ac
						mov word ptr [edi],0a0dh
						add edi,2
						mov pTxt,edi
						mov edi,esi
						inc edi
					.endw
				.endif
				@@:
				add ebx,4
			.endw
			mov edi,pTxt
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