.486
.model flat,stdcall
option casemap:none


include sdhq.inc
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
invoke LoadLibrary,$CTA0("zlib1.dll")
.if !eax
	invoke ExitProcess,0
.endif
mov ebx,eax
invoke GetProcAddress,eax,$CTA0("uncompress")
mov fUncompress,eax
invoke GetProcAddress,ebx,$CTA0("compress")
mov fCompress,eax

invoke DialogBoxParam,hInstance,DLG_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

_GpkDecode proc _lpBuff,_nSize
	mov edx,_lpBuff
	xor ecx,ecx
	.while ecx<_nSize
		mov eax,ecx
		and eax,0fh
		mov al,[dbKey+eax]
		xor byte ptr [edx+ecx],al
		inc ecx
	.endw
	ret
_GpkDecode endp

_memcmp proc uses esi edi _buff1,_buff2,_size
	mov esi,_buff1
	mov edi,_buff2
	mov ecx,_size
	xor eax,eax
	repne cmpsb
	setne al
	ret
_memcmp endp

;……………………………………………………………………………………

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
			invoke GetDlgItem,hwnd,IDC_BCOMPRESS
			invoke EnableWindow,eax,FALSE
		.elseif ax==IDC_RCOMP
			invoke GetDlgItem,hwnd,IDC_DIR
			invoke EnableWindow,eax,TRUE
			invoke GetDlgItem,hwnd,IDC_BROWSEDIR
			invoke EnableWindow,eax,TRUE
			invoke GetDlgItem,hwnd,IDC_BCOMPRESS
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
			mov @opFileName.lpstrFile,offset szPackName
			mov @opFileName.nMaxFile,1024
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			.if eax
				invoke SetDlgItemText,hwnd,IDC_ORI,offset szPackName
			.endif
		.elseif ax==IDC_BROWSEDIR
			invoke _BrowseFolder,hwnd,offset szDirName
			.if eax
				invoke SetDlgItemText,hwnd,IDC_DIR,offset szDirName
			.endif
		.elseif ax==IDC_START
			invoke IsDlgButtonChecked,hwnd,IDC_REXP
			.if eax==BST_CHECKED
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szPackName,1024
				invoke _GpkUnpack,offset szPackName
			.else
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szPackName,1024
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke IsDlgButtonChecked,hwnd,IDC_BCOMPRESS
				mov bCompress,eax
				invoke _GpkPack,offset szPackName,offset szDirName
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

_GpkUnpack proc _lpszPack
	LOCAL @hGpk,@nFileSize
	LOCAL @dbSig:_GpkTail
	LOCAL @lpIndex,@lpUncIndex
	LOCAL @nClen,@nUnclen
	LOCAL @lpC,@lpUnc
	LOCAL @hSingle
	invoke CreateFile,_lpszPack,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,0
		ret
	.endif
	mov @hGpk,eax
	invoke GetFileSize,@hGpk,0
	mov @nFileSize,eax
	
	mov ecx,@nFileSize
	sub ecx,20h
	invoke SetFilePointer,@hGpk,ecx,0,FILE_BEGIN
	invoke ReadFile,@hGpk,addr @dbSig,20h,offset dwTemp,0
	
	lea ecx,@dbSig.szTailer1
	invoke _memcmp,offset szTailer1,ecx,10h
	mov ebx,eax
	invoke _memcmp,offset szTailer2,addr @dbSig.szTailer2,0ch
	or eax,ebx
	.if !ZERO?
		invoke CloseHandle,@hGpk
		invoke MessageBox,hWinMain,$CTA0("文件格式错误！"),0,0
		ret
	.endif

	invoke HeapAlloc,hHeap,0,@dbSig.nIndexSize
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("无法分配内存！"),0,0
		invoke CloseHandle,@hGpk
		ret
	.endif
	mov @lpIndex,eax
	
	mov ecx,@nFileSize
	sub ecx,20h
	sub ecx,@dbSig.nIndexSize
	invoke SetFilePointer,@hGpk,ecx,0,FILE_BEGIN
	invoke ReadFile,@hGpk,@lpIndex,@dbSig.nIndexSize,offset dwTemp,0
	
	invoke _GpkDecode,@lpIndex,@dbSig.nIndexSize
	mov ecx,@lpIndex
	invoke HeapAlloc,hHeap,0,[ecx]
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("无法分配内存！"),0,0
		invoke CloseHandle,@hGpk
		invoke HeapFree,hHeap,0,@lpIndex
		ret
	.endif
	mov @lpUncIndex,eax
	mov ecx,@lpIndex
	mov eax,ecx
	add ecx,4
	mov edx,@dbSig.nIndexSize
	sub edx,4
	push edx
	push ecx
	push eax
	push @lpUncIndex
	call fUncompress
	add esp,16
	.if eax
		invoke MessageBox,hWinMain,$CTA0("解压缩失败！"),0,0
		invoke CloseHandle,@hGpk
		invoke HeapFree,hHeap,0,@lpIndex
		invoke HeapFree,hHeap,0,@lpUncIndex
		ret
	.endif
	invoke HeapFree,hHeap,0,@lpIndex
	
	mov edi,_lpszPack
	xor al,al
	or ecx,-1
	repne scasb
	sub edi,5
	.if byte ptr [edi]=='.'
		mov byte ptr [edi],0
	.else
		mov word ptr [edi+4],'1'
	.endif
	invoke CreateDirectory,_lpszPack,0
	invoke SetCurrentDirectory,_lpszPack
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("无法访问解压目录！"),0,0
		invoke CloseHandle,@hGpk
		invoke HeapFree,hHeap,0,@lpUncIndex
	.endif
	
	mov esi,@lpUncIndex
	.while word ptr [esi]
		xor eax,eax
		lodsw
		lea ebx,[esi+eax*2]
		mov word ptr [esi+eax*2],0
		invoke _MakeFile,esi
		.if !eax
			int 3
		.endif
		mov @hSingle,eax
		lea esi,[ebx+6]
		movzx ebx,byte ptr [esi+16]
		mov eax,ebx
		add eax,dword ptr [esi+4]
		mov ecx,[esi+4]
		mov @nClen,ecx
		invoke HeapAlloc,hHeap,0,@nClen
		mov @lpC,eax
		mov ecx,ebx
		push esi
		add esi,17
		mov edi,@lpC
		rep movsb
		pop esi
		invoke SetFilePointer,@hGpk,[esi],0,FILE_BEGIN
		mov ecx,@nClen
		sub ecx,ebx
		invoke ReadFile,@hGpk,edi,ecx,offset dwTemp,0
		
		.if dword ptr [esi+8]=='TLFD'
			mov ecx,[esi+12]
			mov @nUnclen,ecx
			invoke HeapAlloc,hHeap,0,@nUnclen
			mov @lpUnc,eax
			
			push @nClen
			push @lpC
			lea ecx,@nUnclen
			push ecx
			push @lpUnc
			call fUncompress
			add esp,16
			.if eax
				int 3
			.endif
			invoke WriteFile,@hSingle,@lpUnc,@nUnclen,offset dwTemp,0
			invoke CloseHandle,@hSingle
			invoke HeapFree,hHeap,0,@lpUnc
		.else
			invoke WriteFile,@hSingle,@lpC,@nClen,offset dwTemp,0
		.endif
		invoke HeapFree,hHeap,0,@lpC
		lea esi,[esi+ebx+17]
	.endw
	invoke CloseHandle,@hGpk
	invoke MessageBox,hWinMain,$CTA0("解包完成"),$CTA0("packer"),0
	ret
_GpkUnpack endp

_MakeFile proc _lpFileName
	LOCAL @szStr1[1024]:byte
;	invoke MultiByteToWideChar,932,0,_lpFileName,-1,addr @szStr1,512
	invoke lstrcpyW,addr @szStr1,_lpFileName
	lea ecx,@szStr1
	lea ecx,[ecx+eax*2-2]
;	.if dword ptr [ecx-4]==660061h && dword ptr [ecx-8]==6a002eh
;		mov dword ptr [ecx-4],740078h
;		mov dword ptr [ecx-8],74002eh
;	.endif
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

_GpkPack proc _lpszPack,_lpszDir
	LOCAL @hGpk,@nFileSize,@nActualIndexSize
	LOCAL @dbSig:_GpkTail
	LOCAL @lpIndex,@lpUncIndex
	LOCAL @nClen,@nCClen
	LOCAL @lpC,@lpCC
	LOCAL @hSingle
	LOCAL @nCurOff
	invoke CreateFile,_lpszPack,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,0
		ret
	.endif
	mov @hGpk,eax
	invoke GetFileSize,@hGpk,0
	mov @nFileSize,eax
	
	invoke SetCurrentDirectory,_lpszDir
	.if !eax
		invoke CloseHandle,@hGpk
		invoke MessageBox,hWinMain,$CTA0("无效的目录！"),0,0
		ret
	.endif
	
	mov ecx,@nFileSize
	sub ecx,20h
	invoke SetFilePointer,@hGpk,ecx,0,FILE_BEGIN
	invoke ReadFile,@hGpk,addr @dbSig,20h,offset dwTemp,0
	
	lea ecx,@dbSig.szTailer1
	invoke _memcmp,offset szTailer1,ecx,10h
	mov ebx,eax
	invoke _memcmp,offset szTailer2,addr @dbSig.szTailer2,0ch
	or eax,ebx
	.if !ZERO?
		invoke CloseHandle,@hGpk
		invoke MessageBox,hWinMain,$CTA0("文件格式错误！"),0,0
		ret
	.endif

	invoke HeapAlloc,hHeap,0,@dbSig.nIndexSize
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("无法分配内存！"),0,0
		invoke CloseHandle,@hGpk
		ret
	.endif
	mov @lpIndex,eax
	
	mov ecx,@nFileSize
	sub ecx,20h
	sub ecx,@dbSig.nIndexSize
	invoke SetFilePointer,@hGpk,ecx,0,FILE_BEGIN
	invoke ReadFile,@hGpk,@lpIndex,@dbSig.nIndexSize,offset dwTemp,0
	
	invoke _GpkDecode,@lpIndex,@dbSig.nIndexSize
	mov ecx,@lpIndex
	invoke HeapAlloc,hHeap,0,[ecx]
	.if !eax
		invoke MessageBox,hWinMain,$CTA0("无法分配内存！"),0,0
		invoke CloseHandle,@hGpk
		invoke HeapFree,hHeap,0,@lpIndex
		ret
	.endif
	mov @lpUncIndex,eax
	mov ecx,@lpIndex
	mov eax,ecx
	add ecx,4
	mov edx,@dbSig.nIndexSize
	sub edx,4
	push edx
	push ecx
	push eax
	push @lpUncIndex
	call fUncompress
	add esp,16
	.if eax
		invoke MessageBox,hWinMain,$CTA0("解压缩失败！"),0,0
		invoke CloseHandle,@hGpk
		invoke HeapFree,hHeap,0,@lpIndex
		invoke HeapFree,hHeap,0,@lpUncIndex
		ret
	.endif
	mov ecx,@lpIndex
	mov eax,[ecx]
	mov @nActualIndexSize,eax
	invoke HeapFree,hHeap,0,@lpIndex
	
	mov @nCurOff,1400h
	invoke SetFilePointer,@hGpk,@nCurOff,0,FILE_BEGIN
	mov esi,@lpUncIndex
	.while word ptr [esi]
		xor eax,eax
		lodsw
		lea ebx,[esi+eax*2]
		.if word ptr [esi+eax*2]
			int 3
		.endif
		invoke CreateFileW,esi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax==-1
			int 3
		.endif
		mov @hSingle,eax
		invoke GetFileSize,@hSingle,0
		mov @nClen,eax
		invoke HeapAlloc,hHeap,0,@nClen
		mov @lpC,eax
		invoke ReadFile,@hSingle,@lpC,@nClen,offset dwTemp,0
		invoke CloseHandle,@hSingle
		lea esi,[ebx+6]
		
		.if bCompress
			mov ecx,@nClen
			shl ecx,1
			mov @nCClen,ecx
			invoke HeapAlloc,hHeap,0,@nCClen
			mov @lpCC,eax
			push @nClen
			push @lpC
			lea ecx,@nCClen
			push ecx
			push @lpCC
			call fCompress
			add esp,10h
			.if eax
				int 3
			.endif
		.endif
		
		movzx ebx,byte ptr [esi+16]
		mov ecx,ebx
		lea edi,[esi+17]
		push esi
		.if bCompress
			mov esi,@lpCC
			rep movsb
			mov ecx,@nCClen
		.else
			mov esi,@lpC
			rep movsb
			mov ecx,@nClen
		.endif
		sub ecx,ebx
		mov edi,ecx
		invoke WriteFile,@hGpk,esi,ecx,offset dwTemp,0
		invoke HeapFree,hHeap,0,@lpC
		.if bCompress
			invoke HeapFree,hHeap,0,@lpCC
		.endif
		pop esi
		mov eax,@nCurOff
		mov [esi],eax
		add @nCurOff,edi
		.if bCompress
			mov eax,@nCClen
			mov [esi+4],eax
			mov eax,@nClen
			mov dword ptr [esi+8],'TLFD'
			mov dword ptr [esi+12],eax
		.else
			mov eax,@nClen
			mov [esi+4],eax
			mov dword ptr [esi+8],'    '
			mov dword ptr [esi+12],0
		.endif
		lea esi,[esi+ebx+17]
	.endw
	
	invoke HeapAlloc,hHeap,0,@nActualIndexSize
	mov @lpIndex,eax
	push @nActualIndexSize
	push @lpUncIndex
	lea ecx,@nActualIndexSize
	mov edx,[ecx]
	sub dword ptr [ecx],4
	push ecx
	mov eax,@lpIndex
	mov [eax],edx
	add eax,4
	push eax
	call fCompress
	.if eax
		int 3
	.endif
	mov ebx,@nActualIndexSize
	add ebx,4
	invoke _GpkDecode,@lpIndex,ebx
	invoke WriteFile,@hGpk,@lpIndex,ebx,offset dwTemp,0
	mov @dbSig.nIndexSize,ebx
	invoke WriteFile,@hGpk,addr @dbSig,sizeof _GpkTail,offset dwTemp,0
	invoke SetEndOfFile,@hGpk
	invoke CloseHandle,@hGpk
	invoke MessageBox,hWinMain,$CTA0("封包完成"),$CTA0("packer"),0
	ret
_GpkPack endp

end start