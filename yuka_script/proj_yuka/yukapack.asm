.486
.model flat,stdcall
option casemap:none

include yukapack.inc
include com.inc

include _Browsefolder.asm

.code

start:
;……………………………………………………………………………………
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax


invoke DialogBoxParam,hInstance,IDD_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;……………………………………………………………………………………

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	LOCAL @opFileName:OPENFILENAME
	LOCAL @szErrMsg[128]:byte
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_BROWSEORI
			
			lea edi,@opFileName
			xor eax,eax
			mov ecx,sizeof @opFileName
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			lea eax,szFileName
			mov word ptr [eax],0
			mov @opFileName.lpstrFile,eax
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST or OFN_EXPLORER or OFN_HIDEREADONLY
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			or eax,eax
			je _ExMain
			invoke SetDlgItemText,hwnd,IDC_ORIDAT,offset szFileName
		.elseif ax==IDC_BROWSENEW
			invoke _BrowseFolder,hwnd,offset szDirName
			or eax,eax
			je _ExMain
			invoke SetDlgItemText,hwnd,IDC_DIRNEW,offset szDirName
		.elseif ax==IDC_PACK
			invoke GetDlgItemText,hwnd,IDC_DIRNEW,offset szDirName,512
			invoke GetDlgItemText,hwnd,IDC_ORIDAT,offset szFileName,512
			invoke CreateFile,offset szFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				invoke MessageBox,hwnd,$CTA0("原始dat访问错误！"),0,MB_OK or MB_ICONERROR
				jmp _ExMain
			.endif
			mov ebx,eax
			invoke SetCurrentDirectory,offset szDirName
			.if !eax
				invoke MessageBox,hwnd,$CTA0("目录不存在！"),0,MB_OK or MB_ICONERROR
				invoke CloseHandle,ebx
				jmp _ExMain
			.endif
			invoke _Pack,ebx,offset szDirName
			.if eax
				invoke MessageBox,hwnd,$CTA0("封装成功！"),$CTA0("yuka packer"),MB_OK
			.else
				invoke MessageBox,hwnd,$CTA0("封装失败！"),$CTA0("yuka packer"),MB_OK or MB_ICONERROR
			.endif
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
_ExMain:
	mov eax,TRUE
	ret
	
_DlgMainProc endp

;
_Pack proc uses ebx esi edi _hFile,_Folder
	LOCAL @hdr:_ykc_header
	LOCAL @lpIndex
	LOCAL @szSingleName[128]:byte
	LOCAL @pEnd
	LOCAL @hYks,@nSize,@lpYks
	LOCAL @tmp
	invoke ReadFile,_hFile,addr @hdr,18h,offset dwTemp,0
	.if dword ptr @hdr!=30434b59h
		xor eax,eax
		ret
	.endif
	invoke SetFilePointer,_hFile,@hdr.index_offset,0,FILE_BEGIN
	invoke VirtualAlloc,0,@hdr.index_len,MEM_COMMIT,PAGE_READWRITE
	or eax,eax
	je _ErrPack
	mov @lpIndex,eax
	invoke ReadFile,_hFile,@lpIndex,@hdr.index_len,offset dwTemp,0
	mov esi,@lpIndex
	mov eax,esi
	add eax,@hdr.index_len
	mov @pEnd,eax
	assume esi:ptr _entry
	.while esi<@pEnd
		invoke SetFilePointer,_hFile,[esi].str_offset,0,FILE_BEGIN
		invoke ReadFile,_hFile,addr @szSingleName,[esi].str_len,offset dwTemp,0
		lea eax,@szSingleName
		add eax,[esi].str_len
		mov byte ptr [eax],0
		invoke CreateFile,addr @szSingleName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax!=-1
			mov @hYks,eax
			invoke GetFileSize,@hYks,0
			mov @nSize,eax
			invoke VirtualAlloc,0,@nSize,MEM_COMMIT,PAGE_READWRITE
			mov @lpYks,eax
			invoke ReadFile,@hYks,@lpYks,@nSize,offset dwTemp,0
			mov eax,@lpYks
			.if dword ptr [eax]==30534b59h
				mov ecx,[eax+20h]
				add ecx,@lpYks
				invoke _Encode,ecx,[eax+24h]
				mov eax,@nSize
				.if eax<=[esi].file_len
					mov [esi].file_len,eax
					invoke SetFilePointer,_hFile,[esi].file_offset,0,FILE_BEGIN
					mov @tmp,0
				.else
					mov [esi].file_len,eax
					invoke SetFilePointer,_hFile,0,0,FILE_END
					invoke SetFilePointer,_hFile,0,0,FILE_END
					mov [esi].file_offset,eax
					mov @tmp,1
				.endif
				invoke WriteFile,_hFile,@lpYks,@nSize,offset dwTemp,0
				.if @tmp
					invoke SetEndOfFile,_hFile
				.endif
			.endif
			invoke VirtualFree,@lpYks,0,MEM_RELEASE
			invoke CloseHandle,@hYks
		.endif
		add esi,sizeof _entry
	.endw
	assume esi:nothing
	invoke SetFilePointer,_hFile,@hdr.index_offset,0,FILE_BEGIN
	invoke WriteFile,_hFile,@lpIndex,@hdr.index_len,offset dwTemp,0

	ret
_ErrPack:
xor eax,eax
ret
_Pack endp


_Encode proc uses edi _lpBuf,_nSize
	mov edi,_lpBuf
	mov ecx,_nSize
	@@:
	xor byte ptr [edi],0aah
	inc edi
	loop @B
	ret
_Encode endp


end start