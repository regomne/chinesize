.486
.model flat,stdcall
option casemap:none


include scecomp.inc

.code

;封装sce*文件，scene replay中的小标题

start:
;……………………………………………………………………………………
call MyMain
invoke ExitProcess,NULL


MyMain proc
	local @stFindData:WIN32_FIND_DATA	
	LOCAL @pMesEnd
	LOCAL @hMesTxt
	LOCAL @nMesTxtSize
	LOCAL @str1[64]:byte
	LOCAL @str2[64]:byte
	LOCAL @str3[64]:byte
	LOCAL @str4[64]:byte
	
	invoke VirtualAlloc,0,2000000,MEM_COMMIT,PAGE_READWRITE
	mov lpMesTxt,eax
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
			invoke CreateFile,edi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==INVALID_HANDLE_VALUE
				xor eax,eax
				ret
			.endif
			mov hFile,eax
			invoke GetFileSize,eax,0
			mov ebx,eax
			mov esi,eax
			invoke GlobalAlloc,GPTR,eax
			mov lpBuffer,eax
			add eax,ebx
			mov @pMesEnd,eax
			invoke GlobalAlloc,GPTR,esi
			mov lpTxtBuffer,eax
			mov pTxt,eax
			invoke GlobalAlloc,GPTR,esi
			mov lpTemp,eax
			invoke ReadFile,hFile,lpBuffer,ebx,offset dwTemp,0
			lea edi,@stFindData.cFileName
			mov eax,73656d2eh
			.while dword ptr [edi]!=eax
				inc edi
			.endw
			mov dword ptr [edi],7478742eh
			lea edi,@stFindData.cFileName
			invoke lstrcpy,addr @str1,$CTA0("..\\scetxt\\")
			invoke lstrcat,addr @str1,edi
			invoke CreateFile,addr @str1,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			mov hFiletxt,eax
			invoke ReadFile,hFiletxt,lpTxtBuffer,30000,offset dwTemp,0
			
			mov esi,lpBuffer
			mov edi,lpTxtBuffer
			.if dword ptr [esi]!=1
				int 3
			.endif
			add esi,dword ptr [esi+4]
			add esi,8
			.while esi<@pMesEnd
				.if word ptr [esi]==4500h && byte ptr [esi-1]==1
					add esi,2
					.if dword ptr [esi]==48814881h && dword ptr [esi+4]==48814881h
						mov dword ptr [esi],0bfa3bfa3h
						mov dword ptr [esi+4],0bfa3bfa3h
						mov word ptr [esi+8],0bfa3h
						add esi,11
						.continue
					.elseif dword ptr [esi]==0bfa3bfa3h
						add esi,11
						.continue
					.endif
					mov ebx,edi
					.while word ptr [edi]!=0a0dh
						.if !byte ptr [edi]
							int 3
						.endif
						inc edi
					.endw
					mov byte ptr [edi],0
					add edi,2
					invoke lstrcpy,addr @str2,$CTA0("..\\mestxt\\x")
					invoke lstrcat,addr @str2,ebx
					invoke lstrcat,addr @str2,$CTA0(".txt")
					invoke CreateFile,addr @str2,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
					.if eax==-1
						xor eax,eax
						mov dword ptr [eax],0
					.endif
					mov @hMesTxt,eax
					invoke GetFileSize,@hMesTxt,0
					mov @nMesTxtSize,eax
					invoke ReadFile,@hMesTxt,lpMesTxt,@nMesTxtSize,offset dwTemp,0
					.if !eax
						mov dword ptr [eax],0
					.endif
					invoke _FindStar,lpMesTxt,@nMesTxtSize
					test byte ptr [eax],0
					invoke _ReplaceStr,esi,eax,@pMesEnd
					mov @pMesEnd,eax
					add esi,ecx
					invoke CloseHandle,@hMesTxt
				.else
					inc esi
				.endif
			.endw

			sub edi,lpTxtBuffer
			mov edi,@pMesEnd
			sub edi,lpBuffer
			invoke SetFilePointer,hFile,0,0,FILE_BEGIN
			invoke WriteFile,hFile,lpBuffer,edi,offset dwTemp,0
			invoke SetEndOfFile,hFile
			invoke CloseHandle,hFiletxt
			invoke CloseHandle,hFile
			invoke GlobalFree,lpBuffer
			invoke GlobalFree,lpTemp
			invoke GlobalFree,lpTxtBuffer
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