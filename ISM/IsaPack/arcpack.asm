.486
.model flat,stdcall
option casemap:none


include arcpack.inc
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

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_REXP
;			invoke GetDlgItem,hwnd,IDC_DIR
;			invoke EnableWindow,eax,FALSE
;			invoke GetDlgItem,hwnd,IDC_BROWSEDIR
;			invoke EnableWindow,eax,FALSE
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
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke CreateThread,0,0,offset _ArcUpk,0,0,0
			.else
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szArcName,1024
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke CreateThread,0,0,offset _ArcPack,0,0,0
			.endif
		.endif
	.elseif eax==WM_INITDIALOG
		invoke LoadIcon,hInstance,IDI_APPLICATION
		invoke SendMessage,hwnd,WM_SETICON,ICON_BIG,eax
		mov eax,hwnd
		mov hWinMain,eax
		invoke CheckDlgButton,hwnd,IDC_REXP,BST_CHECKED
		invoke GetDlgItem,hwnd,IDC_PGB
		mov hPGB,eax
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		mov eax,FALSE
		ret
	.endif
	
	mov eax,TRUE
	ret
_DlgMainProc endp

_ArcUpk proc uses esi edi ebx _lpParameter
	LOCAL @hArcFile,@nFileSize,@hArcFileMap,@lpArcFile
	LOCAL @lpIndex,@szHdr[10h]
	LOCAL @hNewFile,@lpNewFile
	LOCAL @nElem
	LOCAL @lpNewFileName
	invoke SetCurrentDirectory,offset szDirName
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("目录无效！"),0,MB_OK
		ret
	.endif
	invoke CreateFile,offset szArcName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hArcFile,eax
;	invoke CreateFileMapping,@hArcFile,0,PAGE_READONLY,0,0,0
;	.if !eax
;		@@:
;		invoke MessageBox,hWinMain,$CTA0("文件访问发生错误！"),0,MB_ICONERROR or MB_OK
;		ret
;	.endif
;	mov @hArcFileMap,eax
;	invoke MapViewOfFile,eax,FILE_MAP_READ,0,0,0
;	or eax,eax
;	je @B
;	mov @lpArcFile,eax
	invoke ReadFile,@hArcFile,addr @szHdr,10h,offset dwTemp,0
	
	lea esi,@szHdr
	lea edi,szMagic
	mov ecx,3
	repe cmpsd
	.if !ZERO?
	_NotMatch:
		invoke MessageBox,hWinMain,$CTA0("格式不匹配！"),0,MB_OK
		jmp _Err
	.endif
	xor eax,eax
	lodsw
	mov @nElem,eax
	lodsw
	cmp eax,1
	jne _NotMatch
	
	mov eax,@nElem
	shl eax,6
	invoke HeapAlloc,hHeap,0,eax
	or eax,eax
	je _Nomem
	mov @lpIndex,eax
	mov eax,@nElem
	shl eax,6
	invoke ReadFile,@hArcFile,@lpIndex,eax,offset dwTemp,0
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("文件访问发生错误！"),0,MB_ICONERROR or MB_OK
		jmp _Err
	.endif
	mov esi,@lpIndex
	
	invoke ShowWindow,hPGB,SW_SHOW
	invoke SendDlgItemMessage,hWinMain,IDC_PGB,PBM_SETRANGE32,0,@nElem
	
	invoke HeapAlloc,hHeap,0,40h*2
	.if !eax
	_Nomem:
		invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_OK
		jmp _Err
	.endif
	mov @lpNewFileName,eax
	
	xor ebx,ebx
	.while ebx<@nElem
		invoke MultiByteToWideChar,932,0,esi,-1,@lpNewFileName,40h
		.if eax
			invoke CreateFileW,@lpNewFileName,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax!=-1
				mov @hNewFile,eax
				invoke SetFilePointer,@hArcFile,[esi+34h],0,FILE_BEGIN
				invoke HeapAlloc,hHeap,0,[esi+38h]
				.if !eax
					invoke CloseHandle,@hNewFile
					jmp _Nomem
				.endif
				mov @lpNewFile,eax
				invoke ReadFile,@hArcFile,eax,[esi+38h],offset dwTemp,0
				.if !eax
					invoke CloseHandle,@hNewFile
					invoke HeapFree,hHeap,0,@lpNewFile
					jmp _Nomem
				.endif
				invoke WriteFile,@hNewFile,@lpNewFile,[esi+38h],offset dwTemp,0
				invoke CloseHandle,@hNewFile
				invoke HeapFree,hHeap,0,@lpNewFile
			.endif
		.endif
		lea eax,[ebx+1]
		invoke SendDlgItemMessage,hWinMain,IDC_PGB,PBM_SETPOS,eax,0
		add esi,40h
		inc ebx
	.endw
	
	invoke HeapFree,hHeap,0,@lpNewFileName
;	invoke UnmapViewOfFile,@lpArcFile
;	invoke CloseHandle,@hArcFileMap
	invoke CloseHandle,@hArcFile
	invoke MessageBox,hWinMain,$CTA0("解包完成！"),$CTA0("Extracter"),0
	invoke ShowWindow,hPGB,SW_HIDE
	ret
_Err:
	invoke CloseHandle,@hArcFile
	ret
_ArcUpk endp

_ArcPack proc _lpPara
	LOCAL @stFd:WIN32_FIND_DATA,@hFind
	LOCAL @hPack,@nFiles,@lpIndex
	LOCAL @hIsm,@lpIsm,@pStr
	invoke SetCurrentDirectory,offset szDirName
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("无法访问目录！"),0,0
		ret
	.endif
	invoke CreateFile,offset szArcName,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
		invoke MessageBox,hWinMain,$CTA0("无法创建文件！"),0,0
		ret
	.endif
	mov @hPack,eax
	
	xor ebx,ebx
	invoke FindFirstFile,$CTA0("*"),addr @stFd
	.if eax!=-1
		mov @hFind,eax
		.repeat
			lea ecx,@stFd.cFileName
			.if !(word ptr [ecx]=='.' || word ptr [ecx]=='..' && byte ptr [ecx+2]==0)
				inc ebx
			.endif
			invoke FindNextFile,@hFind,addr @stFd
		.until !eax
		invoke FindClose,@hFind
	.endif
	
	mov @nFiles,ebx
	.if ebx>07fffh
		invoke MessageBox,hWinMain,$CTA0("文件数目不能超过32767！"),0,0
		invoke CloseHandle,@hPack
		ret
	.endif
	mov word ptr [szMagic+0ch],bx
	
	shl ebx,6
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,ebx
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,0
		invoke CloseHandle,@hPack
		ret
	.endif
	mov @lpIndex,eax
	
	invoke HeapAlloc,hHeap,0,100h
	mov @pStr,eax
	
	add ebx,10h
	invoke SetFilePointer,@hPack,ebx,0,FILE_BEGIN
	
	mov esi,@lpIndex
	invoke FindFirstFile,$CTA0("*"),addr @stFd
	.if eax!=-1
		mov @hFind,eax
		.repeat
			lea ecx,@stFd.cFileName
			.if word ptr [ecx]=='.' || word ptr [ecx]=='..' && byte ptr [ecx+2]==0
				jmp _Next
			.endif
			invoke CreateFile,ecx,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			mov @hIsm,eax
			invoke GetFileSize,@hIsm,0
			mov [esi+34h],ebx
			mov [esi+38h],eax
			invoke lstrlen,addr @stFd.cFileName
			.if eax>30h
				int 3
			.endif
			invoke MultiByteToWideChar,936,0,addr @stFd.cFileName,-1,@pStr,80h
			invoke WideCharToMultiByte,932,0,@pStr,-1,esi,34h,0,0
;			invoke lstrcpy,esi,addr @stFd.cFileName
			invoke HeapAlloc,hHeap,0,[esi+38h]
			.if !eax
				int 3
			.endif
			mov @lpIsm,eax
			invoke ReadFile,@hIsm,@lpIsm,[esi+38h],offset dwTemp,0
			invoke CloseHandle,@hIsm
			invoke WriteFile,@hPack,@lpIsm,[esi+38h],offset dwTemp,0
			add ebx,dword ptr [esi+38h]
			add esi,40h
			invoke HeapFree,hHeap,0,@lpIsm
		_Next:
			invoke FindNextFile,@hFind,addr @stFd
		.until !eax
		invoke FindClose,@hFind
	.endif
	invoke SetFilePointer,@hPack,0,0,FILE_BEGIN
	invoke WriteFile,@hPack,offset szMagic,16,offset dwTemp,0
	mov ecx,@nFiles
	shl ecx,6
	invoke WriteFile,@hPack,@lpIndex,ecx,offset dwTemp,0
	invoke CloseHandle,@hPack
	
	invoke MessageBox,hWinMain,$CTA0("封装完成"),$CTA0("Packer"),0
	
	ret
_ArcPack endp

end start