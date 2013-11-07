.486
.model flat,stdcall
option casemap:none


include packt.inc

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
	LOCAL @hFile,@hTxt
	LOCAL @nFuncs
	LOCAL @lpBuff,@lpTemp
	LOCAL @lpTxt,@lpMsg0
	LOCAL @lpAinInfo
	LOCAL @hFindFile
	LOCAL @stFindData:WIN32_FIND_DATA
	LOCAL @szNewFile[1024]:byte
	invoke LoadLibrary,$CTA0("zlib1.dll")
	mov ebx,eax
	invoke GetProcAddress,eax,$CTA0("uncompress")
	mov fUncompress,eax
	invoke GetProcAddress,ebx,$CTA0("compress")
	mov fCompress,eax
	
	invoke SetCurrentDirectory,$CTA0("acx")
	
	invoke VirtualAlloc,0,2000000,MEM_COMMIT,PAGE_READWRITE
	mov @lpTxt,eax
	invoke VirtualAlloc,0,2000000,MEM_COMMIT,PAGE_READWRITE
	mov @lpBuff,eax
	invoke VirtualAlloc,0,2000000,MEM_COMMIT,PAGE_READWRITE
	mov @lpTemp,eax
	invoke FindFirstFile,$CTA0("*.acx"),addr @stFindData
	.if eax
		mov @hFindFile,eax
		.repeat
			lea edi,@stFindData
			invoke CreateFile,addr WIN32_FIND_DATA.cFileName[edi],GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je _NextFind
			mov @hFile,eax
			invoke GetFileSize,@hFile,0
			mov ebx,eax
			invoke ReadFile,@hFile,@lpBuff,ebx,offset dwTemp,0
			
			invoke lstrcpy,addr @szNewFile,addr WIN32_FIND_DATA.cFileName[edi]
			invoke lstrcat,addr @szNewFile,$CTA0('.txt')
			invoke CreateFile,addr @szNewFile,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			mov @hTxt,eax
			invoke ReadFile,@hTxt,@lpTxt,100000,offset dwTemp,0
			invoke CloseHandle,@hTxt
			
			mov esi,@lpBuff
			add esi,10h
			mov ecx,[esi-4]
			push dword ptr [esi-8]
			mov dwTemp,ecx
			push esi
			push offset dwTemp
			push @lpTemp
			call fUncompress
			.if eax
				int 3
			.endif
			
			invoke _CompactAcx,@lpTemp,dwTemp,@lpTxt
			.if eax==-1
				int 3
			.endif
			
			mov dword ptr [dbHdr+0ch],eax
			push eax
			push @lpTemp
			mov dwTemp,2000000
			push offset dwTemp
			push @lpBuff
			call fCompress
			.if eax
				int 3
			.endif
			mov ecx,dwTemp
			mov dword ptr [dbHdr+8],ecx
			
			
			invoke SetFilePointer,@hFile,0,0,FILE_BEGIN
			mov edi,dwTemp
			invoke WriteFile,@hFile,offset dbHdr,10h,offset dwTemp,0
			
			invoke WriteFile,@hFile,@lpBuff,edi,offset dwTemp,0
			invoke SetEndOfFile,@hFile
			invoke CloseHandle,@hFile
		_NextFind:
			invoke FindNextFile,@hFindFile,addr @stFindData
		.until !eax
		invoke FindClose,@hFindFile
	.endif
	assume esi:nothing
	invoke MessageBox,0,$CTA0('Success'),$CTA0("packer"),0
	ret
MyMain endp

_memcpy proc
	mov eax,ecx
	shr ecx,2
	REP MOVSd
	mov ecx,eax
	and ecx,3
	REP MOVSb
	ret
_memcpy endp

_ReplaceInMem proc uses esi edi _lpNew,_nNewLen,_lpOriPos,_nOriLen,_nLeftLen
	mov eax,_nNewLen
	.if eax==_nOriLen || !_nLeftLen
		mov esi,_lpNew
		mov edi,_lpOriPos
		mov ecx,eax
		rep movsb
		xor eax,eax
	.elseif eax>_nOriLen
	@@:
		invoke HeapAlloc,hHeap,0,_nLeftLen
		.if !eax
			mov eax,1
			jmp _ExRIM
		.endif
		push eax
		mov ecx,_nLeftLen
		mov esi,_lpOriPos
		add esi,_nOriLen
		mov edi,eax
		invoke _memcpy
		mov esi,_lpNew
		mov ecx,_nNewLen
		mov edi,_lpOriPos
		rep movsb
		mov esi,[esp]
		mov ecx,_nLeftLen
		invoke _memcpy
		push 0
		push hHeap
		call HeapFree
		xor eax,eax
	.else
		mov ecx,_nOriLen
		sub ecx,eax
		cmp ecx,4
		jb @B
		mov esi,_lpNew
		mov ecx,_nNewLen
		mov edi,_lpOriPos
		rep movsb
		mov esi,_lpOriPos
		add esi,_nOriLen
		mov ecx,_nLeftLen
		invoke _memcpy
		xor eax,eax
	.endif
_ExRIM:
	ret
_ReplaceInMem endp

_NextLine proc uses edi _lpStr
	mov edi,_lpStr
	mov edx,edi
	.while word ptr [edi]!=0a0dh
		inc edi
	.endw
	
	mov byte ptr [edi],0
	add edi,2
	mov eax,edi
	ret
_NextLine endp

_CompactAcx proc uses esi edi ebx _lpAcx,_nSize,_lpTxt
	LOCAL @stFormat[50h]
	LOCAL @nFmt,@nItem
	LOCAL @szStr[512]:CHAR
	LOCAL @lpStr
	LOCAL @nLen2
	mov esi,_lpAcx
	lodsd
	or eax,eax
	je _Err
	cmp eax,50h
	ja _Err
	mov ecx,eax
	mov @nFmt,ecx
	lea edi,@stFormat
	rep movsd
	lodsd
	mov @nItem,eax
	mov edi,_lpTxt
	.while @nItem
		xor ebx,ebx
		.repeat
			lea ecx,@stFormat
			mov eax,[ecx+ebx*4]
			.if eax==0
				add esi,4
			.elseif eax==2
				mov edx,edi
				mov @lpStr,edx
				.while word ptr [edi]!=0a0dh
					inc edi
				.endw
				mov byte ptr [edi],0
				mov ecx,edi
				add edi,2
				sub ecx,edx
				mov @nLen2,ecx
				invoke lstrlen,esi
				mov ecx,_nSize
				add ecx,_lpAcx
				sub ecx,esi
				sub ecx,eax
				mov edx,@nLen2
				sub edx,eax
				add _nSize,edx
				invoke _ReplaceInMem,@lpStr,@nLen2,esi,eax,ecx
				add esi,@nLen2
				inc esi
			.else
				int 3
			.endif
			inc ebx
		.until ebx>=@nFmt
		dec @nItem
	.endw
	sub esi,_lpAcx
	mov eax,esi
	ret
_Err:
	or eax,-1
	ret
_CompactAcx endp


end start