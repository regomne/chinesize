.486
.model flat,stdcall
option casemap:none


include npapack.inc
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
_memcpy proc
	mov eax,ecx
	shr ecx,2
	REP MOVS DWORD PTR [EDI],DWORD PTR [ESI]
	mov ecx,eax
	and ecx,3
	REP MOVS byte PTR [EDI],byte PTR [ESI]
	ret
_memcpy endp

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
				invoke _NpaUpk,offset szArcName
			.else
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szArcName,1024
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke _NpaPack,offset szArcName,offset szDirName
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

_XorDecode proc _lpBuff,_nSize
	mov edx,_lpBuff
	xor ecx,ecx
	.while ecx<_nSize
		mov eax,ecx
		and eax,7
		mov al,byte ptr [dbKey+eax]
		xor byte ptr [edx+ecx],al
		inc ecx
	.endw
	ret
_XorDecode endp

_NpaUpk proc uses esi edi ebx _lpszName
	LOCAL @hFile,@nFileSize,@hFileMap,@lpFile
	LOCAL @lpIndex,@nIndexLen
	LOCAL @bHasErr
	LOCAL @pStr,@hNewFile,@lpNewFile
	invoke CreateFile,_lpszName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hFile,eax
	invoke CreateFileMapping,@hFile,0,PAGE_READONLY,0,0,0
	.if !eax
		@@:
		invoke MessageBox,hWinMain,$CTA0("文件访问发生错误！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hFileMap,eax
	invoke MapViewOfFile,eax,FILE_MAP_READ,0,0,0
	or eax,eax
	je @B
	mov @lpFile,eax
	invoke GetFileSize,@hFile,0
	mov @nFileSize,eax
	
	mov esi,@lpFile
	lodsd
	mov @nIndexLen,eax
	invoke HeapAlloc,hHeap,0,eax
	or eax,eax
	je _NomemAU
	mov @lpIndex,eax
	
	mov edi,eax
	mov ecx,[esi-4]
	invoke _memcpy
	invoke _XorDecode,@lpIndex,@nIndexLen
	
	mov esi,@lpIndex
	lodsd
	mov ebx,eax
	.while ebx
		lodsd
		inc eax
		shl eax,1
		invoke HeapAlloc,hHeap,0,eax
		mov @pStr,eax
		invoke MultiByteToWideChar,932,0,esi,[esi-4],@pStr,10000
		.if !eax
			int 3
		.endif
		mov ecx,@pStr
		mov word ptr [ecx+eax*2],0
		invoke _MakeFile,@pStr
		.if !eax || eax==-1
			int 3
		.endif
		mov @hNewFile,eax
		invoke HeapFree,hHeap,0,@pStr
		add esi,dword ptr [esi-4]
		invoke HeapAlloc,hHeap,0,[esi]
		.if !eax
			int 3
		.endif
		mov @lpNewFile,eax
		mov edi,eax
		mov edx,esi
		mov ecx,[esi]
		mov eax,[esi+4]
		mov esi,@lpFile
		add esi,eax
		invoke _memcpy
		lea esi,[edx+12]
		
		invoke _XorDecode,@lpNewFile,[esi-12]
		invoke WriteFile,@hNewFile,@lpNewFile,[esi-12],offset dwTemp,0
		invoke CloseHandle,@hNewFile
		invoke HeapFree,hHeap,0,@lpNewFile
		
		dec ebx
	.endw
	
	invoke UnmapViewOfFile,@lpFile
	invoke CloseHandle,@hFileMap
	invoke CloseHandle,@hFile
	
	invoke MessageBox,hWinMain,$CTA0("解包完成！"),$CTA0("extracter"),0
	ret
_NomemAU:
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_ICONERROR or MB_OK
	ret
_NpaUpk endp

_NpaPack proc _lpszName,_lpszDir
	LOCAL @hFile,@nFileSize,@lpEndOfFile
	LOCAL @hNewFile,@lpNewFile,@nNewFileSize
	LOCAL @lpIndex,@nIndex,@nIndexLen
	LOCAL @pStr
	LOCAL @bHasErr
	invoke CreateFile,_lpszName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hFile,eax
	invoke SetCurrentDirectory,_lpszDir
	.if !eax
		invoke CloseHandle,@hFile
		invoke MessageBox,hWinMain,$CTA0("目录无效！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	invoke GetFileSize,@hFile,0
	mov @nFileSize,eax
	
	invoke ReadFile,@hFile,addr @nIndexLen,4,offset dwTemp,0
	mov eax,@nIndexLen
	invoke HeapAlloc,hHeap,0,eax
	or eax,eax
	je _NomemAP
	mov @lpIndex,eax
	invoke ReadFile,@hFile,@lpIndex,@nIndexLen,offset dwTemp,0
	invoke _XorDecode,@lpIndex,@nIndexLen
	
	mov esi,@lpIndex
	lodsd
	mov @nIndex,eax
	xor ebx,ebx
	.while ebx<@nIndex
		lodsd
		inc eax
		shl eax,1
		invoke HeapAlloc,hHeap,0,eax
		mov @pStr,eax
		invoke MultiByteToWideChar,932,0,esi,[esi-4],@pStr,10000
		.if !eax
			int 3
		.endif
		mov ecx,@pStr
		mov word ptr [ecx+eax*2],0
		invoke CreateFileW,@pStr,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax==-1
			add esi,dword ptr [esi-4]
			add esi,12
			jmp _NextFile
		.endif
		mov @hNewFile,eax
		invoke HeapFree,hHeap,0,@pStr
		
		add esi,dword ptr [esi-4]
		invoke GetFileSize,@hNewFile,0
		mov @nNewFileSize,eax
		invoke HeapAlloc,hHeap,0,eax
		.if !eax
			int 3
		.endif
		mov @lpNewFile,eax
		invoke ReadFile,@hNewFile,@lpNewFile,@nNewFileSize,offset dwTemp,0
		invoke CloseHandle,@hNewFile
		invoke _XorDecode,@lpNewFile,@nNewFileSize
		
		mov eax,@nNewFileSize
		.if eax<=[esi]
			mov [esi],eax
			invoke SetFilePointer,@hFile,[esi+4],0,FILE_BEGIN
		.else
			mov [esi],eax
			invoke SetFilePointer,@hFile,0,0,FILE_END
			mov [esi+4],eax
		.endif
		invoke WriteFile,@hFile,@lpNewFile,@nNewFileSize,offset dwTemp,0
		invoke HeapFree,hHeap,0,@lpNewFile
		add esi,12
	_NextFile:
		inc ebx
	.endw
	
	invoke _XorDecode,@lpIndex,@nIndexLen
	invoke SetFilePointer,@hFile,4,0,FILE_BEGIN
	invoke WriteFile,@hFile,@lpIndex,@nIndexLen,offset dwTemp,0
	
_ExAP:
	invoke CloseHandle,@hFile
	
	invoke MessageBox,hWinMain,$CTA0("封包完成！"),$CTA0("packer"),0
	ret
_NomemAP:
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_ICONERROR or MB_OK
	ret
_NpaPack endp

;
_MakeFile proc _lpFileName
	LOCAL @szStr1[1024]:byte
	invoke lstrcpyW,addr @szStr1,_lpFileName
	lea ecx,@szStr1
	lea ecx,[ecx+eax*2-2]
	invoke CreateFileW,addr @szStr1,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke GetLastError
		.if eax==ERROR_PATH_NOT_FOUND
			push esi
			push edi
			push ebx
			lea edi,@szStr1
			or ecx,-1
			xor ax,ax
			repne scasw
			sub edi,2
			not ecx
			dec ecx
			std
			mov ax,'/'
			repne scasw
			cld
			lea ebx,[edi+2]
			mov word ptr [ebx],0
			lea edi,@szStr1
			mov ecx,ebx
			sub ecx,edi
			shr ecx,1
			.while edi<ebx
				repne scasw
				.if ecx
					mov esi,ecx
					mov word ptr [edi-2],0
					invoke CreateDirectoryW,addr @szStr1,0
					mov word ptr [edi-2],'/'
					.if !eax
						invoke GetLastError
						.if eax!=ERROR_ALREADY_EXISTS
							mov word ptr [ebx],'/'
							pop ebx
							pop edi
							pop esi
							xor eax,eax
							ret
						.endif
					.endif
					mov ecx,esi
					mov ax,'/'
				.else
					mov edi,ebx
					invoke CreateDirectoryW,addr @szStr1,0
					mov word ptr [ebx],'/'
					invoke CreateFileW,addr @szStr1,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
					.if eax==INVALID_HANDLE_VALUE
						xor eax,eax
					.endif
				.endif
			.endw
			pop ebx
			pop edi
			pop esi
		.else
			xor eax,eax
		.endif
	.endif
	ret
_MakeFile endp

end start