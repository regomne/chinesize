.486
.model flat,stdcall
option casemap:none


include ext.inc

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
			invoke CreateFile,addr WIN32_FIND_DATA.cFileName[edi],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je _NextFind
			mov @hFile,eax
			invoke GetFileSize,@hFile,0
			mov ebx,eax
			invoke ReadFile,@hFile,@lpBuff,ebx,offset dwTemp,0
			invoke CloseHandle,@hFile
			
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
			
			invoke _ExtractAcx,@lpTemp,dwTemp,@lpTxt
			.if eax==-1
				int 3
			.endif
			mov ebx,eax
			
			invoke lstrcpy,addr @szNewFile,addr WIN32_FIND_DATA.cFileName[edi]
			invoke lstrcat,addr @szNewFile,$CTA0('.txt')
			invoke CreateFile,addr @szNewFile,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			mov @hTxt,eax
			invoke WriteFile,@hTxt,@lpTxt,ebx,offset dwTemp,0
			invoke CloseHandle,@hTxt
			
		_NextFind:
			invoke FindNextFile,@hFindFile,addr @stFindData
		.until !eax
		invoke FindClose,@hFindFile
	.endif
	assume esi:nothing
	invoke MessageBox,0,$CTA0('Success'),$CTA0("extracter"),0
	
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

_ExtractAcx proc uses esi edi ebx _lpAcx,_nSize,_lpTxt
	LOCAL @stFormat[100h]
	LOCAL @nFmt,@nItem
	LOCAL @szStr[512]:CHAR
	LOCAL @nLen2
	mov esi,_lpAcx
	lodsd
	or eax,eax
	je _Err
	cmp eax,100h
	ja _Err
	mov ecx,eax
	mov @nFmt,ecx
	lea edi,@stFormat
	rep movsd
	lodsd
	mov @nItem,eax
	mov edi,_lpTxt
;	add edi,2
	.while @nItem
		xor ebx,ebx
		.repeat
			lea ecx,@stFormat
			mov eax,[ecx+ebx*4]
			.if eax==0
				add esi,4
			.elseif eax==2
;				mov edx,edi
;				invoke lstrlen,esi
;				mov ecx,eax
;				rep movsb
;				inc esi
;				mov word ptr [edi],0a0dh
;				add edi,2
				invoke MultiByteToWideChar,932,0,esi,-1,addr @szStr,256
				.if !eax
					int 3
				.endif
				invoke lstrlen,esi
				lea esi,[eax+esi+1]
				invoke WideCharToMultiByte,936,0,addr @szStr,-1,edi,100000,0,0
				lea edi,[edi+eax-1]
				mov word ptr [edi],0a0dh
				add edi,2
			.else
				int 3
			.endif
			inc ebx
		.until ebx>=@nFmt
		dec @nItem
	.endw
	sub edi,_lpTxt
	mov eax,edi
	ret
_Err:
	or eax,-1
	ret
_ExtractAcx endp


end start