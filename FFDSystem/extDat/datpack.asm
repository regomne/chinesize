.486
.model flat,stdcall
option casemap:none


include datpack.inc
include com.inc
include _BrowseFolder.asm

.code

start:
;……………………………………………………………………………………
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax
invoke GetProcessHeap
mov hHeap,eax

invoke DialogBoxParam,hInstance,DLG_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;……………………………………………………………………………………

_Decode proc uses esi ebx edi _lpDec,_nLen,_lpKey
	LOCAL @hash1
	mov ecx,_lpKey
	mov ebx,[ecx]
	mov eax,[ecx+4]
	mov @hash1,eax
	mov ecx,_nLen
	shr ecx,2
	mov esi,_lpDec
	mov edi,esi
	@@:
		lodsd
		xor eax,ebx
		stosd
		mov edx,eax
		shr edx,16
		shl eax,16
		or eax,edx
		xor eax,@hash1
		add ebx,eax
	loop @B
	ret
_Decode endp

_Encode proc uses esi ebx edi _lpEnc,_nLen,_lpKey
	LOCAL @hash1
	mov ecx,_lpKey
	mov ebx,[ecx]
	mov eax,[ecx+4]
	mov @hash1,eax
	mov ecx,_nLen
	shr ecx,2
	mov esi,_lpEnc
	mov edi,esi
	@@:
		lodsd
		mov edx,eax
		xor eax,ebx
		stosd
		mov eax,edx
		shr edx,16
		shl eax,16
		or eax,edx
		xor eax,@hash1
		add ebx,eax
	loop @B
	ret
_Encode endp

_LzssUnc proc uses esi edi ebx _lpDest,_nDestLen,_lpSrc,_nSrcLen
	LOCAL @win[4096]:byte
	LOCAL @pDestEnd,@pSrcEnd
	lea edi,@win
	mov ecx,4096/4
	xor eax,eax
	rep stosd
	mov esi,_lpSrc
	mov ecx,esi
	add ecx,_nSrcLen
	mov @pSrcEnd,ecx
	mov edi,_lpDest
	mov eax,edi
	add eax,_nDestLen
	mov @pDestEnd,eax
	lea ebx,@win
	xor edx,edx
	push ebp
	mov ebp,0feeh
	.repeat
		shr dx,1
		.if !(dx&0100h)
			.break .if esi>=[esp+10h]
			lodsb
			mov dl,al
			or dx,0ff00h
		.endif
		.if dl&1
			.break .if esi>=[esp+10h]
			.break .if edi>=[esp+14h]
			lodsb
			stosb
			mov [ebx+ebp],al
			inc ebp
			and ebp,0fffh
			.continue
		.else
			lea eax,[esi+1]
			.break .if eax>=[esp+10h]
			xor eax,eax
			xor ecx,ecx
			lodsw
			mov cl,ah
			shr ah,4
			and cl,0fh
			add ecx,3
			push edx
			mov edx,eax
			@@:
				.if edi>=[esp+18h]
					pop eax
					.break
				.endif
				mov al,[ebx+edx]
				stosb
				mov [ebx+ebp],al
				inc edx
				and edx,0fffh
				inc ebp
				and ebp,0fffh
			loop @B
			pop edx
		.endif
	.until 0
	pop ebp
	sub edi,_lpDest
	mov eax,_nDestLen
	sub eax,edi
	sub esi,_lpSrc
	sub esi,_nSrcLen
	or eax,esi
	ret
_LzssUnc endp

_LzssPack proc uses esi edi _lpDest,_lpdwDestLen,_lpSrc,_nSrcLen
	mov ecx,_nSrcLen
	mov edi,_lpDest
	mov esi,_lpSrc
	.while ecx
		.if ecx>=8
			sub ecx,8
			mov al,0ffh
			stosb
			movsd
			movsd
		.else
			mov al,0ffh
			stosb
			rep movsb
			.break
		.endif
	.endw
	sub edi,_lpDest
	mov eax,_lpdwDestLen
	mov [eax],edi
	xor eax,eax
	ret
_LzssPack endp

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_REXP
			invoke GetDlgItem,hwnd,IDC_DIR
			invoke EnableWindow,eax,FALSE
			invoke GetDlgItem,hwnd,IDC_BROWSEDIR
			invoke EnableWindow,eax,FALSE
		.elseif ax==IDC_RCOMP
			invoke GetDlgItem,hwnd,IDC_DIR
			invoke EnableWindow,eax,TRUE
			invoke GetDlgItem,hwnd,IDC_BROWSEDIR
			invoke EnableWindow,eax,TRUE
		.elseif ax==IDC_BROWSEORI
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat2
			mov @opFileName.lpstrFile,offset szArcName
			mov @opFileName.nMaxFile,1024
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			.if eax
				invoke SetDlgItemText,hwnd,IDC_ORI,offset szArcName
			.endif
		.elseif ax==IDC_BROWSEDIR
			invoke _BrowseFolder,hwnd,offset szDirName
			.if eax
				invoke SetDlgItemText,hwnd,IDC_DIR,offset szDirName
			.endif
		.elseif ax==IDC_START
			invoke IsDlgButtonChecked,hwnd,IDC_REXP
			.if eax==BST_CHECKED
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szArcName,1024
				invoke _ArcUpk,offset szArcName
			.else
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szArcName,1024
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke _ArcPack,offset szArcName,offset szDirName
			.endif
		.endif
	.elseif eax==WM_INITDIALOG
		invoke LoadIcon,hInstance,IDI_APPLICATION
		invoke SendMessage,hwnd,WM_SETICON,ICON_BIG,eax
		mov eax,hwnd
		mov hWinMain,eax
		invoke CheckDlgButton,hwnd,IDC_REXP,BST_CHECKED
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		mov eax,FALSE
		ret
	.endif
	
	mov eax,TRUE
	ret
_DlgMainProc endp

_ArcUpk proc uses esi edi ebx _lpszName
	LOCAL @hArcFile,@nFileSize,@hArcFileMap,@lpArcFile
	LOCAL @hdr[10h]:byte
	LOCAL @nEntries
	LOCAL @lpEntry,@lpUnc,@lpComp
	LOCAL @bHasErr
	LOCAL @lpNewFileName
	invoke CreateFile,_lpszName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hArcFile,eax
	
	mov edi,_lpszName
	xor al,al
	or ecx,-1
	repne scasb
	sub edi,5
	.if byte ptr [edi]=='.'
		mov byte ptr [edi],0
	.else
		mov word ptr [edi],'1'
	.endif
	invoke CreateDirectory,_lpszName,0
	invoke SetCurrentDirectory,_lpszName
	.if !eax
		invoke CloseHandle,@hArcFile
		invoke MessageBox,hWinMain,$CTA0("无法访问目录！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	
	invoke ReadFile,@hArcFile,addr @hdr,10h,offset dwTemp,0
	lea esi,@hdr
	lea edi,sMagic
	mov ecx,2
	repe cmpsd
	.if !ZERO? || dword ptr [@hdr+8]!=2
		invoke CloseHandle,@hArcFile
		invoke MessageBox,hWinMain,$CTA0("文件格式错误！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov ecx,dword ptr [@hdr+12]
	invoke SetFilePointer,@hArcFile,ecx,0,FILE_CURRENT
	invoke ReadFile,@hArcFile,addr @nEntries,4,offset dwTemp,0
	mov eax,@nEntries
	mov ecx,sizeof _DatEntry
	mul ecx
	mov ebx,eax
	invoke HeapAlloc,hHeap,0,ebx
	.if !eax
		invoke CloseHandle,@hArcFile
		jmp _NomemAU 
	.endif
	mov @lpEntry,eax
	invoke ReadFile,@hArcFile,eax,ebx,offset dwTemp,0

	xor ebx,ebx
	mov esi,@lpEntry
	assume esi:ptr _DatEntry
	.while ebx<@nEntries
		invoke _Decode,esi,sizeof _DatEntry,offset sKeyIdx
		invoke HeapAlloc,hHeap,0,[esi].complen
		.if !eax
			invoke CloseHandle,@hArcFile
			invoke HeapFree,hHeap,0,@lpEntry
			jmp _NomemAU
		.endif
		mov @lpComp,eax
		mov ecx,[esi].unclen
		.if ecx!=[esi].complen
			invoke HeapAlloc,hHeap,0,[esi].unclen
			.if !eax
				invoke CloseHandle,@hArcFile
				invoke HeapFree,hHeap,0,@lpEntry
				invoke HeapFree,hHeap,0,@lpComp
				jmp _NomemAU
			.endif
			mov @lpUnc,eax
		.endif
		invoke SetFilePointer,@hArcFile,[esi].offset1,0,FILE_BEGIN
		invoke ReadFile,@hArcFile,@lpComp,[esi].complen,offset dwTemp,0
		.if [esi].mode==1
			invoke _Decode,@lpComp,[esi].complen,offset sKeyIdx
		.else
			int 3
		.endif
		mov eax,[esi].complen
		.if eax!=[esi].unclen
			invoke _LzssUnc,@lpUnc,[esi].unclen,@lpComp,[esi].complen
			.if eax
				int 3
			.endif
			invoke CreateFile,addr [esi].name1,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			push eax
			invoke WriteFile,eax,@lpUnc,[esi].unclen,offset dwTemp,0
			call CloseHandle
			invoke HeapFree,hHeap,0,@lpUnc
		.else
			invoke CreateFile,addr [esi].name1,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			push eax
			invoke WriteFile,eax,@lpComp,[esi].complen,offset dwTemp,0
			call CloseHandle
		.endif
		invoke HeapFree,hHeap,0,@lpComp
		
		add esi,sizeof _DatEntry
		inc ebx
	.endw
	assume esi:nothing
	
	invoke HeapFree,hHeap,0,@lpEntry
	invoke CloseHandle,@hArcFile
	invoke MessageBox,hWinMain,$CTA0("解包完成！"),0,MB_ICONERROR or MB_OK
	ret
_NomemAU:
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_ICONERROR or MB_OK
	ret
_ArcUpk endp

_ArcPack proc _lpszName,_lpszDir
	LOCAL @hArcFile,@nFileSize
	LOCAL @hdr[10h]:byte
	LOCAL @hNewFile,@lpNewFile,@nNewFileSize,@lpC
	LOCAL @lpEntry,@nEntries
	LOCAL @nIndexPos
	LOCAL @bHasErr
	
	invoke CreateFile,_lpszName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hArcFile,eax
	
	invoke SetCurrentDirectory,_lpszDir
	.if !eax
		invoke CloseHandle,@hArcFile
		invoke MessageBox,hWinMain,$CTA0("无法访问目录！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	
	invoke ReadFile,@hArcFile,addr @hdr,10h,offset dwTemp,0
	lea esi,@hdr
	lea edi,sMagic
	mov ecx,2
	repe cmpsd
	.if !ZERO? || dword ptr [@hdr+8]!=2
		invoke CloseHandle,@hArcFile
		invoke MessageBox,hWinMain,$CTA0("文件格式错误！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov ecx,dword ptr [@hdr+12]
	invoke SetFilePointer,@hArcFile,ecx,0,FILE_CURRENT
	add eax,4
	mov @nIndexPos,eax
	invoke ReadFile,@hArcFile,addr @nEntries,4,offset dwTemp,0
	mov eax,@nEntries
	mov ecx,sizeof _DatEntry
	mul ecx
	mov ebx,eax
	invoke HeapAlloc,hHeap,0,ebx
	.if !eax
		invoke CloseHandle,@hArcFile
		jmp _Nomem
	.endif
	mov @lpEntry,eax
	invoke ReadFile,@hArcFile,eax,ebx,offset dwTemp,0
	
	xor ebx,ebx
	mov esi,@lpEntry
	assume esi:ptr _DatEntry
	.while ebx<@nEntries
		invoke _Decode,esi,sizeof _DatEntry,offset sKeyIdx
		invoke CreateFile,addr [esi].name1,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax==-1
			int 3
		.endif
		mov @hNewFile,eax
		invoke GetFileSize,@hNewFile,0
		mov @nNewFileSize,eax
		invoke HeapAlloc,hHeap,0,@nNewFileSize
		.if !eax
			int 3
		.endif
		mov @lpNewFile,eax
		invoke ReadFile,@hNewFile,@lpNewFile,@nNewFileSize,offset dwTemp,0
		invoke CloseHandle,@hNewFile
		
		mov ecx,@nNewFileSize
		mov [esi].unclen,ecx
		shl ecx,1
		invoke HeapAlloc,hHeap,0,ecx
		.if !eax
			int 3
		.endif
		mov @lpC,eax
		invoke _LzssPack,@lpC,addr @nNewFileSize,@lpNewFile,@nNewFileSize
		mov eax,@nNewFileSize
		mov edi,[esi].complen
		mov [esi].complen,eax
		
		invoke _Encode,@lpC,[esi].complen,offset sKeyIdx
		
		.if edi<[esi].complen
			invoke SetFilePointer,@hArcFile,0,0,FILE_END
			mov [esi].offset1,eax
		.else
			invoke SetFilePointer,@hArcFile,[esi].offset1,0,FILE_BEGIN
		.endif
		invoke WriteFile,@hArcFile,@lpC,[esi].complen,offset dwTemp,0
		
		invoke HeapFree,hHeap,0,@lpC
		invoke HeapFree,hHeap,0,@lpNewFile
		
		invoke _Encode,esi,sizeof _DatEntry,offset sKeyIdx
		add esi,sizeof _DatEntry
		inc ebx
	.endw
	assume esi:nothing
	
	invoke SetFilePointer,@hArcFile,@nIndexPos,0,FILE_BEGIN
	mov eax,sizeof _DatEntry
	mul @nEntries
	invoke WriteFile,@hArcFile,@lpEntry,eax,offset dwTemp,0
	invoke HeapFree,hHeap,0,@lpEntry
	invoke CloseHandle,@hArcFile
	
	invoke MessageBox,hWinMain,$CTA0("封装完成！"),0,MB_OK
	ret
_Nomem:
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_OK or MB_ICONERROR
	ret
_ArcPack endp

_MakeDir proc uses edi _lpszName
	mov edi,_lpszName
	xor al,al
	or ecx,-1
	repne scasb
	mov edx,edi
	dec edi
	.while byte ptr [edi]!='.'
		dec edi
		.if byte ptr [edi]=='\'
			mov word ptr [edx],'1'
			jmp @F
		.endif
		cmp edi,_lpszName
		je _ErrMD
	.endw
	mov byte ptr [edi],0
	@@:
	invoke SetCurrentDirectory,_lpszName
	.if !eax
		invoke CreateDirectory,_lpszName,0
		or eax,eax
		je _ErrMD
		invoke SetCurrentDirectory,_lpszName
		or eax,eax
		je _ErrMD
	.endif
	mov eax,1
	ret
_ErrMD:
	xor eax,eax
	ret
_MakeDir endp
end start