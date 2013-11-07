.486
.model flat,stdcall
option casemap:none


include testext.inc

.code


start:
;……………………………………………………………………………………
call MyMain
invoke ExitProcess,NULL

_WriteErr proc _hErr,_lpfn,_lpErr,_nLine
	LOCAL @str[128]:byte
	.if _nLine!=-1
		invoke wsprintf,addr @str,$CTA0("%s\t\t%s\t\t%d\n"),_lpfn,_lpErr,_nLine
	.else
		invoke wsprintf,addr @str,$CTA0("%s\t\t%s\n"),_lpfn,_lpErr
	.endif
	invoke WriteFile,_hErr,addr @str,eax,offset dwTemp,0
	ret
_WriteErr endp

_Encode proc _lpBuff,_nSize
	mov eax,_lpBuff
	.if word ptr [eax]==0feffh
		mov eax,1
		ret
	.endif
	xor eax,eax
	or ecx,-1
	ret
_Encode endp

_CRLF proc uses esi _lpBuff,_nSize
	mov esi,_lpBuff
	xor ecx,ecx
	mov edx,_nSize
	.while ecx<edx
		mov al,[esi+ecx]
		.if al>80h
			add ecx,2
			.continue
		.elseif al==0ah
			mov eax,1
			ret
		.elseif al==0dh
			.if word ptr [esi+ecx]==0a0dh
				add ecx,2
				.continue
			.endif
			mov eax,1
			ret
		.else
			inc ecx
		.endif
	.endw
	xor eax,eax
	ret
_CRLF endp

_Half proc uses esi _lpBuff,_nSize
	LOCAL @nLine
	mov esi,_lpBuff
	xor ecx,ecx
	mov @nLine,ecx
	mov al,[esi]
	.if al=='T' || al=='N' || al=='S'
		inc ecx
	.endif
	mov edx,_nSize
	.while ecx<edx
		mov ax,[esi+ecx]
		.if al<80h
			.if word ptr [esi+ecx-2]==0a0dh
				inc ecx
				.continue
			.endif
			.if ax==0a0dh
				inc @nLine
				add ecx,2
				.continue
			.endif
			.if ax!=6e5ch
				mov eax,1
				mov ecx,@nLine
				ret
			.endif
		.endif
		add ecx,2
	.endw
	xor eax,eax
	ret
_Half endp

_CalChar proc _lpBuffer,_nSize,_lpChar
	mov edi,_lpChar
	mov esi,_lpBuffer
	mov edx,_nSize
	add edx,esi
	.while esi<edx
		.if byte ptr [esi]<80h
			xor eax,eax
			lodsb
			mov byte ptr [edi+eax],1
		.endif
		.while word ptr [esi]!=0a0dh
			.break .if esi>=edx 
			.if byte ptr [esi]<=7fh
				inc esi
			.else
				add esi,2
			.endif
		.endw
		add esi,2
	.endw
	ret
_CalChar endp


MyMain proc
	local @stFindData:WIN32_FIND_DATA	
	LOCAL @pMesEnd
	LOCAL @hMesTxt
	LOCAL @nMesTxtSize
	LOCAL @str1[64]:byte
	LOCAL @str2[64]:byte
	LOCAL @str3[64]:byte
	LOCAL @str4[128]:byte
			lea edi,@str4
			xor eax,eax
			mov ecx,128/4
			rep stosd
	
	invoke CreateFile,$CTA0("errors.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov hErr,eax
	
	invoke VirtualAlloc,0,2000000,MEM_COMMIT,PAGE_READWRITE
	mov lpMesTxt,eax
	lea edi,@stFindData
	mov ecx,sizeof WIN32_FIND_DATA
	xor eax,eax
	rep stosb
	invoke SetCurrentDirectory,offset szdirTxt
	invoke FindFirstFile,$CTA0("*.txt"),addr @stFindData
	.if eax!=INVALID_HANDLE_VALUE
		mov hFindFile,eax
		.repeat
			lea edi,@stFindData.cFileName
			invoke CreateFile,edi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==INVALID_HANDLE_VALUE
				invoke _WriteErr,hErr,edi,$CTA0("无法打开文件"),-1
				jmp _CtnTest
			.endif
			mov hFile,eax
			invoke GetFileSize,eax,0
			mov ebx,eax
			invoke GlobalAlloc,GPTR,eax
			mov lpBuffer,eax
			invoke ReadFile,hFile,lpBuffer,ebx,offset dwTemp,0
			.if !eax
				invoke _WriteErr,hErr,edi,$CTA0("无法打开文件"),-1
				jmp _CtnTest2
			.endif
			
			
			invoke _Encode,lpBuffer,ebx
			.if eax
				invoke _WriteErr,hErr,edi,$CTA0("编码错误"),-1
				jmp _CtnTest2
			.endif
			invoke _CRLF,lpBuffer,ebx
			.if eax
				invoke _WriteErr,hErr,edi,$CTA0("换行符错误"),-1
				jmp _CtnTest2
			.endif
			invoke _Half,lpBuffer,ebx
			.if eax
				invoke _WriteErr,hErr,edi,$CTA0("存在半角字符"),ecx
			.endif
			
			invoke _CalChar,lpBuffer,ebx,addr @str4
			
_CtnTest2:
			invoke CloseHandle,hFile
			invoke GlobalFree,lpBuffer
_CtnTest:
			invoke SetCurrentDirectory,offset szdirTxt
			invoke FindNextFile,hFindFile,addr @stFindData
		.until eax==FALSE
		invoke FindClose,hFindFile
	.endif
	ret
MyMain endp

;
_FindStar proc uses edi _lpBuff,_nSize
	mov edi,_lpBuff
	mov ecx,edi
	add ecx,_nSize
	mov byte ptr [ecx],0
	.while edi<ecx
		.if word ptr [edi]==0f4a1h
			lea eax,[edi+2]
			.repeat
				inc edi
			.until word ptr [edi]==0a0dh || !byte ptr [edi]
			.if word ptr [edi-2]==0f4a1h
				sub edi,2
			.endif
			mov byte ptr [edi],0
			ret
		.endif
		inc edi
	.endw
	xor eax,eax
	ret
_FindStar endp

;
_ReplaceStr proc uses ebx esi edi _lpDest,_lpszNew,_pEnd
	LOCAL @nLen1,@nLen2
	 
	invoke lstrlen,_lpDest
	mov @nLen1,eax
	invoke lstrlen,_lpszNew
	.if eax==@nLen1
		invoke lstrcpy,_lpDest,_lpszNew
		mov eax,_pEnd
		mov ecx,@nLen1
		ret
	.endif
	mov @nLen2,eax
	
	mov eax,@nLen1
	inc eax
	mov ecx,_pEnd
	sub ecx,_lpDest
	sub ecx,eax
	mov ebx,ecx
	mov edi,lpTemp
	mov esi,_lpDest
	add esi,eax
	invoke _memcpy
	invoke lstrcpy,_lpDest,_lpszNew
	mov eax,@nLen2
	mov edi,_lpDest
	lea edi,[edi+eax+1]
	mov esi,lpTemp
	mov ecx,ebx
	invoke _memcpy
	
	mov ecx,@nLen2
	mov eax,_pEnd
	sub ecx,@nLen1
	add eax,ecx
	mov ecx,@nLen2
	ret
_ReplaceStr endp

;
_memcpy proc
	mov eax,ecx
	shr ecx,2
	REP MOVS DWORD PTR [EDI],DWORD PTR [ESI]
	mov ecx,eax
	and ecx,3
	REP MOVS byte PTR [EDI],byte PTR [ESI]
	ret
_memcpy endp

end start