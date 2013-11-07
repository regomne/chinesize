.486
.model flat,stdcall
option casemap:none


include ext.inc

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetProcessHeap
mov hHeap,eax
call MyMain
invoke ExitProcess,NULL

_FindStar proc uses esi _lpTxt,_nSize
	mov esi,_lpTxt
	xor ecx,ecx
	.while ecx<_nSize
		.if word ptr [esi+ecx]!=0f4a1h
			.if word ptr [esi+ecx+1]==0f4a1h
				inc ecx
				jmp @F
			.endif
			.while word ptr [esi+ecx]!=0a0dh
				.break .if ecx>=_nSize
				inc ecx
			.endw
			add ecx,2
			.continue
		.else
		@@:
			add ecx,2
			mov eax,ecx
			.while ecx<_nSize
				.if word ptr [esi+ecx]==0f4a1h
					mov byte ptr [esi+ecx],0
					add eax,esi
					ret
				.endif
				.if word ptr [esi+ecx]==0a0dh
					add ecx,2
					.break
				.endif
				inc ecx
			.endw
		.endif
	.endw
	xor eax,eax
	ret
_FindStar endp

_MatchLines proc uses ebx esi edi _lpTxt,_nSize,_lpFileInfo
	LOCAL @szStr[512]:byte
	LOCAL @hFile,@lpBuff
	invoke lstrcpy,addr @szStr,$CTA0("..\\ori\\")
	mov ecx,_lpFileInfo
	invoke lstrcat,addr @szStr,addr _FileInfo.fname[ecx]
	invoke CreateFile,addr @szStr,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	cmp eax,-1
	je _Err
	mov @hFile,eax
	invoke GetFileSize,@hFile,0
	mov ebx,eax
	invoke HeapAlloc,hHeap,0,ebx
	mov @lpBuff,eax
	invoke ReadFile,@hFile,@lpBuff,ebx,offset dwTemp,0
	invoke CloseHandle,@hFile
	
	mov esi,@lpBuff
	mov edx,esi
	add edx,ebx
	xor ecx,ecx
	.while esi<edx
		.if word ptr [esi]==0a0dh
			inc ecx
		.endif
		inc esi
	.endw
	mov ebx,ecx
	invoke HeapFree,hHeap,0,@lpBuff
	
	mov edi,_lpTxt
	mov edx,edi
	add edx,_nSize
	xor ecx,ecx
	.while edi<edx
		.if word ptr [edi]==0a0dh
			inc ecx
		.endif
		inc edi
	.endw
	
	xor eax,eax
	dec ebx
	cmp ebx,ecx
	sete al
	ret
_Err:
	xor eax,eax
	ret
_MatchLines endp
 

MyMain proc
	LOCAL @stFd:WIN32_FIND_DATA
	LOCAL @hReport
	LOCAL @hFind
	LOCAL @hTxt,@lpTxt
	LOCAL @nSize
	LOCAL @szReport[512]:byte
	LOCAL @szStr[512]:byte
	LOCAL @nCount
	LOCAL @nCnt2
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,2000*512
	mov lpTitles,eax
	invoke CreateFile,$CTA0("Report.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov @hReport,eax
	
	invoke SetCurrentDirectory,$CTA0('ori')
	invoke FindFirstFile,$CTA0('*.txt'),addr @stFd
	.if eax
		mov @hFind,eax
		mov @nCount,0
		.repeat
			lea ecx,@stFd
			invoke CreateFile,addr WIN32_FIND_DATA.cFileName[ecx],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			mov @hTxt,eax
			invoke GetFileSize,@hTxt,0
			mov ebx,eax
			invoke HeapAlloc,hHeap,0,ebx
			mov @lpTxt,eax
			invoke ReadFile,@hTxt,@lpTxt,ebx,offset dwTemp,0
			invoke _FindStar,@lpTxt,ebx
			.if eax
				mov ecx,@nCount
				mov esi,lpTitles
				shl ecx,9
				add esi,ecx
				invoke lstrcpy,esi,eax
				lea ecx,@stFd
				invoke lstrcpy,addr _FileInfo.fname[esi],addr WIN32_FIND_DATA.cFileName[ecx]
			.else
				mov ecx,@nCount
				mov esi,lpTitles
				shl ecx,9
				add esi,ecx
				lea edx,@stFd
				invoke lstrcpy,addr _FileInfo.fname[esi],addr WIN32_FIND_DATA.cFileName[edx]
			.endif
			invoke HeapFree,hHeap,0,@lpTxt
			invoke CloseHandle,@hTxt
			
			inc @nCount
			invoke FindNextFile,@hFind,addr @stFd
		.until !eax
		invoke FindClose,@hFind
	.endif
	
	mov @nCnt2,0
	invoke SetCurrentDirectory,$CTA0('..\\dc2psp')
	invoke FindFirstFile,$CTA0("*.txt"),addr @stFd
	.if eax
		mov @hFind,eax
		xor ebx,ebx
		.repeat
			lea ecx,@stFd
			invoke CreateFile,addr WIN32_FIND_DATA.cFileName[ecx],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			mov @hTxt,eax
			invoke GetFileSize,@hTxt,0
			mov ebx,eax
			mov @nSize,ebx
			invoke HeapAlloc,hHeap,0,ebx
			mov @lpTxt,eax
			invoke ReadFile,@hTxt,@lpTxt,ebx,offset dwTemp,0
			invoke _FindStar,@lpTxt,ebx
			.if eax
				mov esi,eax
				mov edi,lpTitles
				xor ebx,ebx
				.while ebx<=@nCount 
					invoke lstrcmp,esi,edi
					.if !eax
						lea ecx,@stFd
						invoke _MatchLines,@lpTxt,@nSize,edi
						.if !eax
							lea ecx,@stFd
							invoke wsprintf,addr @szReport,$CTA0("%s\t\t%s\n"),ADDR WIN32_FIND_DATA.cFileName[ecx],addr _FileInfo.fname[edi]
							invoke WriteFile,@hReport,addr @szReport,eax,offset dwTemp,0
							invoke lstrcpy,addr @szReport,$CTA0("LineNotMatch\\")
							lea ecx,@stFd
							invoke lstrcat,addr @szReport,ADDR WIN32_FIND_DATA.cFileName[ecx]
							lea ecx,@stFd
							invoke CopyFile,ADDR WIN32_FIND_DATA.cFileName[ecx],addr @szReport,FALSE
						.else
							lea esi,@szReport
							invoke lstrcpy,esi,$CTA0("Match\\")
							lea ecx,@stFd
							invoke lstrcat,esi,ADDR WIN32_FIND_DATA.cFileName[ecx]
							invoke lstrcpy,addr @szStr,$CTA0("..\\cn\\")
							invoke lstrcat,addr @szStr,addr _FileInfo.fname[edi]
							invoke CopyFile,addr @szStr,esi,FALSE
						.endif
						mov _FileInfo.flag[edi],1
						.break
					.endif
					add edi,sizeof _FileInfo
					inc ebx
				.endw
				.if ebx>@nCount
					lea esi,@szReport
					invoke lstrcpy,esi,$CTA0("NoMatch\\")
					lea ecx,@stFd
					invoke lstrcat,esi,ADDR WIN32_FIND_DATA.cFileName[ecx]
					lea ecx,@stFd
					invoke CopyFile,ADDR WIN32_FIND_DATA.cFileName[ecx],esi,FALSE
				.endif
			.else
				invoke lstrcpy,addr @szReport,$CTA0("Special\\")
				lea ecx,@stFd
				invoke lstrcat,addr @szReport,ADDR WIN32_FIND_DATA.cFileName[ecx]
				lea ecx,@stFd
				invoke CopyFile,ADDR WIN32_FIND_DATA.cFileName[ecx],addr @szReport,FALSE
			.endif
			invoke HeapFree,hHeap,0,@lpTxt
			invoke CloseHandle,@hTxt
			invoke FindNextFile,@hFind,addr @stFd
			inc @nCnt2
		.until !eax
	.endif
	
	invoke wsprintf,addr @szReport,$CTA0('\npc°æÎ´Æ¥ÅäÎÄ¼þ£º\n')
	invoke WriteFile,@hReport,addr @szReport,eax,offset dwTemp,0
	
	xor ebx,ebx
	mov edi,lpTitles
	lea esi,@szReport
	invoke SetCurrentDirectory,$CTA0("..\\cn")
	.while ebx<=@nCount
		.if !_FileInfo.flag[edi]
			invoke wsprintf,esi,$CTA0('%s\n'),addr _FileInfo.fname[edi]
			invoke WriteFile,@hReport,esi,eax,offset dwTemp,0
			invoke lstrcpy,esi,$CTA0('NoMatch\\')
			invoke lstrcat,esi,addr _FileInfo.fname[edi]
			invoke CopyFile,addr _FileInfo.fname[edi],esi,FALSE
		.endif
		add edi,sizeof _FileInfo
		inc ebx
	.endw
	
	invoke CloseHandle,@hReport
	ret
MyMain endp

end start