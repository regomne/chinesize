.486
.model flat,stdcall
option casemap:none


include arcpack.inc
include com.inc
include _BrowseFolder.asm

CLP_INIT_OFF			EQU		11H
CLP_SEC_OFF			EQU		46H
szCPKey				db		'ClOVeRrE'
szCPCode			db		1Fh, 03h, 4Fh, 56h, 0B9h, 4Ch, 72h, 45h
szCPArchive			db		'ARCHIVE',0,0,0,0,0

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax
invoke GetProcessHeap
mov hHeap,eax

invoke DialogBoxParam,hInstance,DLG_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

_XorDecode proc uses esi ebx _buff,_len,_key,_offset
	mov esi,_buff
	mov ecx,_len
	mov edx,_offset
	mov ebx,_key
	@@:
		mov eax,edx
		and eax,7
		mov al,[ebx+eax]
		xor byte ptr [esi],al
		inc esi
		inc edx
	loop @B
	ret
_XorDecode endp

_SnrPack proc uses esi ebx edi _lpszDirName
	LOCAL @hSnr,@lpIndex
	LOCAL @datHdr:_DatHeader
	LOCAL @key[8]:byte
	LOCAL @nBuffSize
	LOCAL @hTxt,@lpTxt
	LOCAL @nCurrentOffset
	LOCAL @nFiles
	LOCAL @stFd:WIN32_FIND_DATA,@hFind
	
	
	invoke HeapAlloc,hHeap,0,1024
	or eax,eax
	je _Err
	mov ebx,eax
	invoke lstrcpy,ebx,_lpszDirName
	invoke lstrcat,ebx,$CTA0(".dat")
	invoke CreateFile,ebx,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov esi,eax
	invoke HeapFree,hHeap,0,ebx
	cmp esi,-1
	je _Err
	mov @hSnr,esi
	
	lea edi,@key
	lea esi,szCPCode
	mov ecx,2
	rep movsd
	invoke SetFilePointer,@hSnr,CLP_INIT_OFF,0,FILE_BEGIN
	invoke WriteFile,@hSnr,addr @key,8,offset dwTemp,0
	invoke _XorDecode,addr @key,8,offset szCPKey,0
	
	invoke SetCurrentDirectory,_lpszDirName
	or eax,eax
	je _Err
	xor ebx,ebx
	invoke FindFirstFile,$CTA0("*.txt"),addr @stFd
	.if eax
		mov @hFind,eax
		.repeat
			inc ebx
			invoke FindNextFile,@hFind,addr @stFd			
		.until !eax
		invoke FindClose,@hFind
	.endif
	mov @nFiles,ebx
	mov eax,sizeof _DatEntry
	mul ebx
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,eax
	or eax,eax
	je _Err
	mov @lpIndex,eax
	
	mov edi,eax
	assume edi:ptr _DatEntry
	invoke HeapAlloc,hHeap,0,1000000
	.if !eax
		invoke HeapFree,hHeap,0,@lpIndex
		invoke CloseHandle,@hSnr
		jmp _Err
	.endif
	mov @lpTxt,eax
	mov @nBuffSize,1000000
	mov @nCurrentOffset,sizeof _DatHeader
	invoke SetFilePointer,@hSnr,CLP_SEC_OFF+sizeof _DatHeader,0,FILE_BEGIN
	invoke FindFirstFile,$CTA0("*.txt"),addr @stFd
	.if eax
		mov @hFind,eax
		.repeat
			lea ecx,@stFd.cFileName
			invoke CreateFile,ecx,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je _Ex2
			push eax
			invoke GetFileSize,eax,0
			.if eax>@nBuffSize
				mov @nBuffSize,eax
				mov esi,eax
				invoke HeapReAlloc,hHeap,0,@lpTxt,eax
				.if !eax
					call CloseHandle
					_Ex2:
					invoke HeapFree,hHeap,0,@lpTxt
					invoke HeapFree,hHeap,0,@lpIndex
					invoke CloseHandle,@hSnr
					jmp _Err
				.endif
				mov @lpTxt,eax
			.else
				mov esi,eax
			.endif
			invoke ReadFile,[esp+10h],@lpTxt,esi,offset dwTemp,0
			call CloseHandle
			invoke _XorDecode,@lpTxt,esi,addr @key,@nCurrentOffset
			mov ecx,@nCurrentOffset
			mov [edi].nOffset,ecx
			mov [edi].nLen,esi
			add @nCurrentOffset,esi
			invoke lstrcpy,addr [edi].szName,addr @stFd.cFileName
			invoke WriteFile,@hSnr,@lpTxt,esi,offset dwTemp,0
			
			add edi,sizeof _DatEntry
			invoke FindNextFile,@hFind,addr @stFd
		.until !eax
		invoke FindClose,@hFind
	.endif
	assume edi:nothing
	invoke HeapFree,hHeap,0,@lpTxt
	mov eax,sizeof _DatEntry
	mul @nFiles
	mov ebx,eax
	lea ecx,@key
	invoke _XorDecode,@lpIndex,eax,ecx,@nCurrentOffset
	invoke WriteFile,@hSnr,@lpIndex,ebx,offset dwTemp,0
	invoke HeapFree,hHeap,0,@lpIndex
	
	mov ecx,dword ptr szCPArchive
	mov dword ptr [@datHdr.szMagic],ecx
	mov eax,dword ptr szCPArchive+4
	mov dword ptr [@datHdr.szMagic+4],eax
	mov ecx,dword ptr szCPArchive+8
	mov dword ptr [@datHdr.szMagic+8],ecx
	mov @datHdr.nHeaderLen,sizeof _DatHeader
	mov ecx,@nCurrentOffset
	mov @datHdr.nIndexOffset,ecx
	mov @datHdr.nIndexEntryLen,sizeof _DatEntry
	mov eax,@nFiles
	mov @datHdr.nIndexEntries,eax
	xor ecx,ecx
	mov @datHdr.resvd,ecx
	mov dword ptr [@datHdr.resvd+4],ecx
	invoke _XorDecode,addr @datHdr,sizeof _DatHeader,addr @key,0
	invoke SetFilePointer,@hSnr,CLP_SEC_OFF,0,FILE_BEGIN
	invoke WriteFile,@hSnr,addr @datHdr,sizeof _DatHeader,offset dwTemp,0
	invoke CloseHandle,@hSnr
	
	xor eax,eax
	ret
_Err:
	mov eax,1
	ret
_SnrPack endp


;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

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
			mov @opFileName.lpstrFile,offset szSnrName
			mov @opFileName.nMaxFile,1024
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			.if eax
				invoke SetDlgItemText,hwnd,IDC_ORI,offset szSnrName
			.endif
		.elseif ax==IDC_BROWSEDIR
			invoke _BrowseFolder,hwnd,offset szDirName
			.if eax
				invoke SetDlgItemText,hwnd,IDC_DIR,offset szDirName
			.endif
		.elseif ax==IDC_START
			invoke IsDlgButtonChecked,hwnd,IDC_REXP
			.if eax==BST_CHECKED
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szSnrName,1024
;				invoke _SnrUpk,offset szSnrName
			.else
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szSnrName,1024
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke _SnrPack,offset szDirName
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

end start