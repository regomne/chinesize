.486
.model flat,stdcall
option casemap:none


include comp.inc
include _BrowseFolder.asm

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax


invoke DialogBoxParam,hInstance,IDD_DLG2,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	local @hFileTxt,@hFileGsc,@hFile,@lpFile
	local @nSuc,@nFail
	LOCAL @stFindData:WIN32_FIND_DATA
	LOCAL @hFindFile
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_BROWSE1
			invoke _BrowseFolder,hwnd,offset szPathOri
			.if eax
				invoke SetDlgItemText,hwnd,IDC_EDIT1,offset szPathOri
			.endif
			
		.elseif ax==IDC_BROWSE2
			invoke _BrowseFolder,hwnd,offset szPathNew
			.if eax
				invoke SetDlgItemText,hwnd,IDC_EDIT2,offset szPathNew
			.endif
		.elseif ax==IDC_EDIT1
			shr eax,16
			.if ax==EN_CHANGE
				invoke GetDlgItemText,hwnd,IDC_EDIT1,offset szPathOri,512
				invoke GetDlgItem,hwnd,IDC_START
				.if !byte ptr [szPathOri]
					invoke EnableWindow,eax,FALSE
				.elseif byte ptr [szPathNew]
					invoke EnableWindow,eax,TRUE
				.endif
			.endif
		.elseif ax==IDC_EDIT2
			shr eax,16
			.if ax==EN_CHANGE
				invoke GetDlgItemText,hwnd,IDC_EDIT2,offset szPathNew,512
				invoke GetDlgItem,hwnd,IDC_START
				.if !byte ptr [szPathNew]
					invoke EnableWindow,eax,FALSE
				.elseif byte ptr [szPathOri]
					invoke EnableWindow,eax,TRUE
				.endif
			.endif
		.elseif ax==IDC_START
			invoke SetCurrentDirectory,addr szPathNew
			.if !eax
				invoke MessageBox,hwnd,offset szCantSetPath,offset szDisplayName,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			xor eax,eax
			mov @nSuc,eax
			mov @nFail,eax
			invoke FindFirstFile,offset szTxt,addr @stFindData
			.if eax!=INVALID_HANDLE_VALUE
				mov @hFindFile,eax
				.repeat
					lea edi,@stFindData.cFileName
					invoke CreateFile,edi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
					mov @hFileTxt,eax
					mov eax,7478742eh
					.while dword ptr [edi]!=eax
						inc edi
					.endw
					mov dword ptr [edi],6373672eh
					invoke SetCurrentDirectory,offset szPathOri
					lea edi,@stFindData.cFileName
					invoke CreateFile,edi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
					cmp eax,-1
					je @F
					mov @hFileGsc,eax
					invoke _WriteGsc,@hFileTxt,@hFileGsc
					.if eax==-1
						inc @nFail
					.else
						inc @nSuc
					.endif
					@@:
					invoke SetCurrentDirectory,offset szPathNew
					invoke CloseHandle,@hFileTxt
					invoke CloseHandle,@hFileGsc
					invoke FindNextFile,@hFindFile,addr @stFindData
				.until eax==FALSE
				invoke FindClose,@hFindFile
			.endif
			invoke MessageBox,hwnd,offset szOver,szDisplayName,MB_OK
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
_WriteGsc proc _hTxt,_hGsc
	LOCAL @nSizeTxt,@nSizeGsc
	invoke GetFileSize,_hTxt,0
	mov @nSizeTxt,eax
	invoke GlobalAlloc,GPTR,eax
	mov lpFileBuffTxt,eax
	invoke ReadFile,_hTxt,eax,@nSizeTxt,offset dwTemp,0
	invoke GetFileSize,_hGsc,0
	add eax,1000h
	mov @nSizeGsc,eax
	invoke GlobalAlloc,GPTR,eax
	mov lpFileBuff,eax
	invoke GlobalAlloc,GPTR,@nSizeGsc
	mov lpTemp,eax
	invoke ReadFile,_hGsc,lpFileBuff,@nSizeGsc,offset dwTemp,0
	invoke _AddToList,_hGsc,lpFileBuff
	invoke SetFilePointer,_hGsc,0,0,FILE_BEGIN
	invoke WriteFile,_hGsc,lpFileBuff,nFileSizeNew,offset dwTemp,0
	invoke SetEndOfFile,_hGsc
	invoke GlobalFree,lpLineRecords
	invoke GlobalFree,lpFileBuff
	invoke GlobalFree,lpFileBuffTxt
	ret
_WriteGsc endp
;
_AddToList proc uses ebx esi edi _hFile,_lpFileBuff
	local @lpEndOf1,@lpLineR
	
	invoke GetFileSize,_hFile,0
	mov nFileSizeNew,eax
	mov @lpEndOf1,eax
	mov ecx,_lpFileBuff
	add @lpEndOf1,ecx

	mov edi,_lpFileBuff
	mov edx,[edi+0ch]
	add edx,100
	invoke GlobalAlloc,GPTR,edx
	mov lpLineRecords,eax
	
	mov esi,lpFileBuff
	mov eax,esi
	add eax,dword ptr [esi+8]
	add eax,24h
	mov lpIndex,eax
	mov ecx,[esi+0ch]
	add eax,ecx
	shr ecx,2
	mov nIndex,ecx
	mov esi,eax
	mov lpText,eax
	mov eax,lpLineRecords
	mov @lpLineR,eax
	
	xor ebx,ebx
	.while ebx<nIndex
		
		mov eax,lpIndex
		mov eax,[eax+ebx*4]
		add eax,esi
		.if byte ptr [eax]
			mov ecx,@lpLineR
			mov [ecx],eax
			add @lpLineR,4
		.endif
		
		inc ebx
	.endw
	
	mov esi,lpLineRecords
	xor ebx,ebx
	mov dwTemp,0
	.while dword ptr [esi]
		invoke _GetText,offset szStringO
		push eax
		invoke lstrlen,[esi]
		pop ecx
		inc eax
		invoke _RpcText,ebx,0,offset szStringO,ecx,eax
		inc ebx
		add esi,4
	.endw
	ret
_AddToList endp
;
_RpcText proc uses esi ebx edi,nLine,nLineOffset,lpszNew,nLenNew,nLenOri
	mov ecx,nLenNew
	sub ecx,nLenOri
	push ecx
	mov esi,lpLineRecords
	mov eax,nLine
	lea esi,[esi+eax*4+4]
	mov edx,[esi-4]
	push edx
	add edx,nLineOffset
	.if !ecx
		mov ecx,nLenNew
		mov edi,edx
		mov esi,lpszNew
		rep movsb
		add esp,8
		ret
	.endif
	.while dword ptr [esi]!=0
		add dword ptr [esi],ecx
		add esi,4
	.endw
	mov esi,lpLineRecords
	mov eax,nLine
	mov eax,[esi+eax*4]
	sub eax,lpText
	mov esi,lpIndex
	xor ebx,ebx
	.while ebx<nIndex
		.if eax<dword ptr [esi+ebx*4]
			add dword ptr [esi+ebx*4],ecx
		.endif
		inc ebx
	.endw
	mov esi,nFileSizeNew
	add esi,lpFileBuff
	sub esi,edx
	mov edi,lpTemp
	mov ecx,esi
	mov ebx,esi
	mov esi,edx
	add esi,nLenOri
	call _memcpy
	mov edi,edx
	mov ecx,nLenNew
	mov esi,lpszNew
	rep movsb
	mov esi,lpTemp
	mov ecx,ebx
	call _memcpy
	
	pop edi
	pop ebx
	add nFileSizeNew,ebx;
	mov esi,lpFileBuff
	add dword ptr [esi+10h],ebx
	ret
_RpcText endp
;
_GetText proc uses esi _lpszStr
	mov eax,dwTemp
	mov esi,lpFileBuffTxt
	.while byte ptr [esi+eax]!=';'
		inc eax
	.endw
	
	.repeat
		inc eax
	.until word ptr [esi+eax]==0a0dh
	.repeat
		inc eax
	.until byte ptr [esi+eax]=='='
	inc eax
	mov ecx,eax
	.repeat
		inc eax
	.until word ptr [esi+eax]==0a0dh
	mov byte ptr [esi+eax],0
	add eax,2
	mov dwTemp,eax
	add ecx,esi
	invoke lstrcpy,_lpszStr,ecx
	invoke lstrlen,_lpszStr
	inc eax
	ret
_GetText endp
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