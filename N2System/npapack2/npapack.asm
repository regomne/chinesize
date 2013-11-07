.486
.model flat,stdcall
option casemap:none


include npapack.inc
include com.inc
include _BrowseFolder.asm
include alg.asm

.code

option language:C
main proc
;……………………………………………………………………………………
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax
invoke GetProcessHeap
mov hHeap,eax

invoke DialogBoxParam,hInstance,DLG_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL
main endp

option language:stdcall
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
;				invoke _NpaUpk,offset szArcName
				invoke CreateThread,0,0,offset _NpaUpk,0,0,0
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
		invoke _InitEncTable
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

_NpaUpk proc uses esi edi ebx _para
	LOCAL @hFile,@nFileSize
	LOCAL @lpIndex,@lpEntries
	LOCAL @lpDirNames
	LOCAL @sHdr:_NpaHdr
	LOCAL @szFileName[512]:byte
	LOCAL @lpC,@nDirs
	mov esi,offset szArcName
	invoke CreateFile,esi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hFile,eax
	invoke lstrlen,esi
	.if byte ptr [esi+eax-4]=='.'
		mov byte ptr [esi+eax-4],0
		invoke CreateDirectory,esi,0
		invoke SetCurrentDirectory,esi
	.endif
	invoke GetFileSize,@hFile,0
	mov @nFileSize,eax
	
	invoke ReadFile,@hFile,addr @sHdr,sizeof @sHdr,offset dwTemp,0
	.if dword ptr [@sHdr]!=141504eh
		invoke MessageBox,hWinMain,$CTA0("格式不匹配！"),0,0
		invoke CloseHandle,@hFile
		ret
	.endif
	
	mov eax,@sHdr.nDirEntries
	shl eax,2
	invoke HeapAlloc,hHeap,0,eax
	.if !eax
		int 3
	.endif
	mov @lpDirNames,eax
	
	mov eax,@sHdr.nEntries
	mov ecx,sizeof _NpaEntry
	mul ecx
	invoke HeapAlloc,hHeap,0,eax
	.if !eax
		int 3
	.endif
	mov @lpEntries,eax

	invoke HeapAlloc,hHeap,0,@sHdr.nIndexLen
	.if !eax
		int 3
	.endif
	mov @lpIndex,eax
	invoke ReadFile,@hFile,eax,@sHdr.nIndexLen,offset dwTemp,0
	
	xor ebx,ebx
	mov esi,@lpIndex
	mov edi,@lpEntries
	mov @nDirs,0
	assume edi:ptr _NpaEntry
	.while ebx<@sHdr.nEntries
		lodsd
		.if eax>=256
			int 3
		.endif
		mov [edi].nNameLen,eax
		mov ecx,eax
		mov edx,edi
		rep movsb
		mov byte ptr [edi],0
		mov edi,edx
		xor eax,eax
		lodsb
		mov [edi].nType,eax
		mov ecx,4
		lea edi,[edi].nDirNum
		rep movsd
		mov edi,edx
		mov al,@sHdr.isCrypted
		mov [edi].bIsCrypted,eax
		mov al,@sHdr.isCompressed
		mov [edi].bIsComp,eax
		mov eax,@sHdr.hash0
		mov [edi].nHash0,eax
		mov eax,@sHdr.hash1
		mov [edi].nHash1,eax
		invoke _PrepareDec,edi,ebx
		.if [edi].nType==1
			mov eax,@lpDirNames
			mov ecx,@nDirs
			mov [eax+ecx*4],edi
			inc @nDirs
		.endif
		
		add edi,sizeof _NpaEntry
		inc ebx
	.endw
	invoke HeapFree,hHeap,0,@lpIndex 
	
	mov esi,@lpDirNames
	xor ebx,ebx
	.while ebx<@sHdr.nDirEntries
		invoke MultiByteToWideChar,932,0,[esi+ebx*4],-1,addr @szFileName,256
		invoke CreateDirectoryW,addr @szFileName,0
		.if !eax
			int 3
		.endif
		inc ebx
	.endw
	
	invoke ShowWindow,hPGB,SW_SHOW
	invoke SendMessage,hPGB,PBM_SETRANGE32,0,@sHdr.nEntries

	mov edi,@lpEntries
	xor ebx,ebx
	.while ebx<@sHdr.nEntries
		.if [edi].nType==2
;			invoke lstrcpy,addr @szFileName,edi
			invoke MultiByteToWideChar,932,0,edi,-1,addr @szFileName,256
			invoke HeapAlloc,hHeap,0,[edi].nCompLen
			.if !eax
				int 3
			.endif
			mov @lpC,eax
			mov eax,[edi].nDataOffset
			add eax,sizeof _NpaHdr
			add eax,@sHdr.nIndexLen
			invoke SetFilePointer,@hFile,eax,0,FILE_BEGIN
			invoke ReadFile,@hFile,@lpC,[edi].nCompLen,offset dwTemp,0
			.if [edi].bIsCrypted
				invoke _Decrypt,edi,@lpC,[edi].nDecLen
			.endif
			.if [edi].bIsComp
				invoke HeapAlloc,hHeap,0,[edi].nUncLen
				.if !eax
					int 3
				.endif
				mov esi,eax
				mov ecx,[edi].nUncLen
				mov dwTemp,ecx
				invoke uncompress,esi,offset dwTemp,@lpC,[edi].nCompLen
				.if eax
					int 3
				.endif
				invoke HeapFree,hHeap,0,@lpC
				mov @lpC,esi
			.endif
			invoke CreateFileW,addr @szFileName,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			push eax
			invoke WriteFile,eax,@lpC,[edi].nUncLen,offset dwTemp,0
			call CloseHandle
			invoke HeapFree,hHeap,0,@lpC
		.endif
		add edi,sizeof _NpaEntry
		inc ebx
		invoke SendMessage,hPGB,PBM_SETPOS,ebx,0
	.endw
	assume edi:nothing
	
	invoke HeapFree,hHeap,0,@lpEntries
	invoke HeapFree,hHeap,0,@lpDirNames
	invoke CloseHandle,@hFile
	
	invoke ShowWindow,hPGB,SW_HIDE
	invoke MessageBox,hWinMain,$CTA0("解包完成！"),$CTA0("extracter"),0
	ret
_NomemAU:
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_ICONERROR or MB_OK
	ret
_NpaUpk endp

_NpaPack proc _lpszName,_lpszDir
	LOCAL @hFile,@nFileSize,@lpEndOfFile
	LOCAL @hNewFile,@lpNewFile,@nNewFileSize
	LOCAL @lpIndex
	LOCAL @lpEntries,@nOffset
	LOCAL @szFn[512]:byte
	LOCAL @sHdr:_NpaHdr
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
	
	invoke ReadFile,@hFile,addr @sHdr,sizeof _NpaHdr,offset dwTemp,0
	.if dword ptr [@sHdr]!=141504eh
		invoke MessageBox,hWinMain,$CTA0("格式不匹配！"),0,0
		invoke CloseHandle,@hFile
		ret
	.endif
	
;	mov eax,@sHdr.nDirEntries
;	shl eax,2
;	invoke HeapAlloc,hHeap,0,eax
;	.if !eax
;		int 3
;	.endif
;	mov @lpDirNames,eax
	
	mov eax,@sHdr.nEntries
	mov ecx,sizeof _NpaEntry
	mul ecx
	invoke HeapAlloc,hHeap,0,eax
	.if !eax
		int 3
	.endif
	mov @lpEntries,eax

	invoke HeapAlloc,hHeap,0,@sHdr.nIndexLen
	.if !eax
		int 3
	.endif
	mov @lpIndex,eax
	invoke ReadFile,@hFile,eax,@sHdr.nIndexLen,offset dwTemp,0
	
	xor ebx,ebx
	mov esi,@lpIndex
	mov edi,@lpEntries
;	mov @nDirs,0
	assume edi:ptr _NpaEntry
	.while ebx<@sHdr.nEntries
		lodsd
		.if eax>=64
			int 3
		.endif
		mov [edi].nNameLen,eax
		mov ecx,eax
		mov edx,edi
		rep movsb
		mov byte ptr [edi],0
		mov edi,edx
		xor eax,eax
		lodsb
		mov [edi].nType,eax
		mov ecx,4
		lea edi,[edi].nDirNum
		rep movsd
		mov edi,edx
		mov al,@sHdr.isCrypted
		mov [edi].bIsCrypted,eax
		mov al,@sHdr.isCompressed
		mov [edi].bIsComp,eax
		mov eax,@sHdr.hash0
		mov [edi].nHash0,eax
		mov eax,@sHdr.hash1
		mov [edi].nHash1,eax
		invoke _PrepareDec,edi,ebx
;		.if [edi].nType==1
;			mov eax,@lpDirNames
;			mov ecx,@nDirs
;			mov [eax+ecx*4],edi
;			inc @nDirs
;		.endif
		
		add edi,sizeof _NpaEntry
		inc ebx
	.endw
	invoke HeapFree,hHeap,0,@lpIndex 
	
	assume edi:nothing
	mov esi,@lpEntries
	assume esi:ptr _NpaEntry
	xor ebx,ebx
	mov @nOffset,0
	.while ebx<@sHdr.nEntries
		.if [esi].nType==2
			invoke MultiByteToWideChar,932,0,esi,-1,addr @szFn,256
			invoke CreateFileW,addr @szFn,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				int 3
			.endif
			mov @hNewFile,eax
			invoke GetFileSize,eax,0
			mov @nNewFileSize,eax
			invoke HeapAlloc,hHeap,0,eax
			.if !eax
				int 3
			.endif
			mov @lpNewFile,eax
			invoke ReadFile,@hNewFile,@lpNewFile,@nNewFileSize,offset dwTemp,0
			invoke CloseHandle,@hNewFile
			.if [esi].bIsComp
				mov eax,@nNewFileSize
				add eax,100h
				invoke HeapAlloc,hHeap,0,eax
				.if !eax
					int 3
				.endif
				mov edi,eax
				mov ecx,@nNewFileSize
				lea eax,[ecx+100h]
				mov dwTemp,eax
				mov [esi].nUncLen,ecx
				invoke compress,edi,offset dwTemp,@lpNewFile,@nNewFileSize
				.if eax
					int 3
				.endif
				invoke HeapFree,hHeap,0,@lpNewFile
				mov @lpNewFile,edi
				mov eax,dwTemp
				mov @nNewFileSize,eax
				mov [esi].nCompLen,eax
			.else
				mov eax,@nNewFileSize
				mov [esi].nCompLen,eax
				mov [esi].nUncLen,eax
			.endif
			.if [esi].bIsCrypted
				invoke _Encrypt,esi,@lpNewFile
			.endif
			invoke WriteFile,@hFile,@lpNewFile,@nNewFileSize,offset dwTemp,0
			mov eax,@nOffset
			mov [esi].nDataOffset,eax
			add eax,@nNewFileSize
			mov @nOffset,eax
		.endif
		add esi,sizeof _NpaEntry
		inc ebx
	.endw
	invoke SetEndOfFile,@hFile
	
	invoke HeapAlloc,hHeap,0,@sHdr.nIndexLen
	.if !eax
		int 3
	.endif
	mov @lpIndex,eax
	
	mov edi,@lpIndex
	mov esi,@lpEntries
	xor ebx,ebx
	assume esi:ptr _NpaEntry
	.while ebx<@sHdr.nEntries
		invoke _EncryptIndex,esi,ebx
		mov eax,[esi].nNameLen
		stosd
		mov ecx,eax
		mov edx,esi
		rep movsb
		mov al,byte ptr [edx+_NpaEntry.nType]
		stosb
		lea esi,[edx+_NpaEntry.nDirNum]
		mov ecx,4
		rep movsd
		lea esi,[edx+sizeof _NpaEntry]
		inc ebx
	.endw
	assume esi:nothing
	sub edi,@lpIndex
	.if edi!=@sHdr.nIndexLen
		int 3
	.endif
	invoke SetFilePointer,@hFile,sizeof _NpaHdr,0,FILE_BEGIN
	invoke WriteFile,@hFile,@lpIndex,@sHdr.nIndexLen,offset dwTemp,0
	
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

end main