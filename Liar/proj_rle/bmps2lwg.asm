.486
.model flat,stdcall
option casemap:none


include bmps2lwg.inc
include _BrowseFolder.asm
include wcg.asm

.code

start:
;……………………………………………………………………………………
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax


invoke DialogBoxParam,hInstance,IDD_DLG2,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;……………………………………………………………………………………

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	local @hFileOri
	local @hFile,@lpFile
	LOCAL @szHint[128]:byte
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_BROWSE1
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat2
			mov @opFileName.lpstrFile,offset szFileOri
			mov byte ptr [szFileOri],0
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			or eax,eax
			je Ex
			invoke SetDlgItemText,hwnd,IDC_EDIT1,offset szFileOri
			
		.elseif ax==IDC_BROWSE2
			invoke _BrowseFolder,hwnd,offset szPathDst
			.if eax
				invoke SetDlgItemText,hwnd,IDC_EDIT2,offset szPathDst
			.endif
		.elseif ax==IDC_EDIT1
			shr eax,16
			.if ax==EN_CHANGE
				invoke GetDlgItemText,hwnd,IDC_EDIT1,offset szFileOri,512
				invoke GetDlgItem,hwnd,IDC_START
				.if !byte ptr [szFileOri]
					invoke EnableWindow,eax,FALSE
				.elseif byte ptr [szPathDst]
					invoke EnableWindow,eax,TRUE
				.endif
			.endif
		.elseif ax==IDC_EDIT2
			shr eax,16
			.if ax==EN_CHANGE
				invoke GetDlgItemText,hwnd,IDC_EDIT2,offset szPathDst,512
				invoke GetDlgItem,hwnd,IDC_START
				.if !byte ptr [szPathDst]
					invoke EnableWindow,eax,FALSE
				.elseif byte ptr [szFileOri]
					invoke EnableWindow,eax,TRUE
				.endif
			.endif
		.elseif ax==IDC_START
			invoke SetCurrentDirectory,addr szPathDst
			.if !eax
				invoke MessageBox,hwnd,offset szCantSetPath,offset szDisplayName,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			invoke CreateFile,offset szFileOri,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				invoke MessageBox,hwnd,offset szCantOpenLwgFile,offset szDisplayName,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			mov @hFileOri,eax
			invoke _FoldLwg,@hFileOri,offset szPathDst
			.if !eax
				invoke MessageBox,hwnd,offset szPackingErr,offset szDisplayName,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			mov ebx,ecx
			mov @lpFile,eax
			lea edi,szFileOri
			or ecx,-1
			xor al,al
			repne scasb
			mov al,'\'
			std
			repne scasb
			cld
			add edi,2
			
			invoke CreateFile,edi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je Ex
			mov @hFile,eax
			mov edi,@lpFile
			mov eax,sizeof _LwgHeader
			mov ecx,[edi+eax]
			add eax,ecx
			add eax,4
			add edi,eax
			add eax,dword ptr [edi]
			add eax,4
			invoke WriteFile,@hFile,@lpFile,eax,offset dwTemp,0
			.if eax
				.if !ebx
					invoke MessageBox,hwnd,offset szOver,offset szDisplayName,MB_OK or MB_ICONINFORMATION
				.else
					invoke wsprintf,addr @szHint,offset szOverErr,ebx
					invoke MessageBox,hwnd,addr @szHint,offset szDisplayName,MB_OK or MB_ICONINFORMATION
				.endif
			.endif
			invoke CloseHandle,@hFile
			invoke VirtualFree,@lpFile,0,MEM_RELEASE
		.endif
	.elseif eax==WM_INITDIALOG
		invoke LoadIcon,hInstance,IDI_APPLICATION
		invoke SendMessage,hwnd,WM_SETICON,ICON_BIG,eax
		mov eax,hwnd
		mov hWinMain,eax
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		mov eax,FALSE
		ret
	.endif

Ex:
	mov eax,TRUE
	ret
	
_DlgMainProc endp

;
_FoldLwg proc _hFile,_lpszPath
	local @fzOri:LARGE_INTEGER
	local @lpFileOri,@lpFileNew,@dwTemp
	local @sizeIndex,@nIndex
	local @offset
	local @szFname[32]:byte
	local @pIndexOri,@pIndexNew,@pDataOri,@pDataNew
	local @pMemory
	local @dwErr,@hLog
	mov @dwErr,0
	invoke GetFileSizeEx,_hFile,addr @fzOri
	invoke VirtualAlloc,NULL,dword ptr @fzOri,MEM_COMMIT,PAGE_READWRITE
	or eax,eax
	je lwgEx1
	mov @lpFileOri,eax
	invoke ReadFile,_hFile,eax,dword ptr [@fzOri],@dwTemp,0
	mov esi,@lpFileOri
	assume esi:ptr _LwgHeader
	.if [esi].magic!=474ch || [esi].version!=1
		invoke VirtualFree,@lpFileOri,0,MEM_RELEASE
		jmp lwgEx1
	.endif
	mov eax,dword ptr @fzOri
	shl eax,1
	invoke VirtualAlloc,NULL,eax,MEM_COMMIT,PAGE_READWRITE
	or eax,eax
	je lwgEx1
	mov @lpFileNew,eax
	mov edi,eax
	mov eax,[esi].entries
	mov @nIndex,eax
	mov ecx,sizeof _LwgHeader
	rep movsb
	assume esi:nothing
	mov eax,[esi]
	mov @sizeIndex,eax
	movsd
	mov @pIndexOri,esi
	mov @pIndexNew,edi
	mov ecx,eax
	add ecx,4
	rep movsb
	mov @pDataOri,esi
	mov @pDataNew,edi
	mov edi,@pIndexNew
	assume edi:ptr _IndexAtom
	mov ebx,@nIndex
	mov @offset,0
	invoke SetCurrentDirectory,_lpszPath
	invoke CreateFile,offset szLog,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov @hLog,eax
	
	invoke VirtualAlloc,0,52,MEM_COMMIT,PAGE_READWRITE
	.if !eax
		invoke VirtualFree,@lpFileOri,0,MEM_RELEASE
		invoke VirtualFree,@lpFileNew,0,MEM_RELEASE
		jmp lwgEx1
	.endif
	mov @pMemory,eax
	.while ebx
		pushad
		movzx ecx,[edi].namelen
		lea esi,[edi].name1
		lea edi,@szFname
		rep movsb
		mov dword ptr [edi],'pmb.'
		mov byte ptr [edi+4],0
		popad
		invoke CreateFile,addr @szFname,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax!=-1
			push eax
			mov edx,eax
			mov ecx,9
			xor eax,eax
			mov esi,edi
			mov edi,@pMemory
			rep stosd
			mov edi,esi
			invoke _FoldPic,edx,@pMemory
			.if !eax
				.if ecx==1
					mov eax,$CTA0("内存错误")
				.elseif ecx==2
					mov eax,$CTA0("文件格式错误")
				.elseif ecx==3
					mov eax,$CTA0("压缩过程中出现错误")
				.else
					mov eax,$CTA0("未知错误")
				.endif
				invoke wsprintf,offset szErrLog,offset szErrFormat,addr @szFname,eax
				invoke WriteFile,@hLog,offset szErrLog,eax,offset dwTemp,0
				assume edi:nothing
				push edi
				xor ecx,ecx
				.while ecx<9
					mov edi,@pMemory
					mov eax,[edi+ecx*4]
					.if eax
						push ecx
						invoke GlobalFree,eax
						pop ecx
					.endif
					inc ecx
				.endw
				pop edi
				call CloseHandle
				inc @dwErr
				jmp lwglbl58
				assume edi:ptr _IndexAtom
			.endif
			mov edx,@offset
			mov [edi].offset1,edx
			mov [edi].length1,ecx
			push edi
			mov edi,@pDataNew
			add edi,@offset
			add @offset,ecx
			mov esi,eax
			call _memcpy
			pop edi
			call CloseHandle
		.else
	lwglbl58:
			push edi
			mov esi,@pDataOri
			add esi,[edi].offset1
			mov ecx,[edi].length1
			mov edi,@pDataNew
			mov edx,@offset
			add edi,edx
			add @offset,ecx
			call _memcpy
			pop edi
			mov [edi].offset1,edx
		.endif
		movzx ecx,[edi].namelen
		add edi,ecx
		add edi,sizeof _IndexAtom
		dec edi
		assume edi:nothing
		dec ebx
	.endw
	mov edi,@pDataNew
	mov eax,@offset
	mov dword ptr [edi-4],eax
	invoke VirtualFree,@lpFileOri,0,MEM_RELEASE
	invoke CloseHandle,@hLog
	mov eax,@lpFileNew
	mov ecx,@dwErr
	ret
lwgEx1:
	xor eax,eax
	ret
_FoldLwg endp
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