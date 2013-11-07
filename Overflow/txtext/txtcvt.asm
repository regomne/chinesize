.486
.model flat,stdcall
option casemap:none


include txtcvt.inc
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
		.if ax==IDC_BROWSEORS
			invoke _BrowseFolder,hwnd,offset szOrsDir
			.if eax
				invoke SetDlgItemText,hwnd,IDC_DIRORS,offset szOrsDir
			.endif
		.elseif ax==IDC_BROWSETXT
			invoke _BrowseFolder,hwnd,offset szTxtDir
			.if eax
				invoke SetDlgItemText,hwnd,IDC_DIRTXT,offset szTxtDir
			.endif
		.elseif ax==IDC_TOORS
			mov ebx,offset _CvtOrs
		_Label1:
			invoke GetDlgItemTextW,hwnd,IDC_DIRORS,offset szOrsDir,512
			invoke GetDlgItemTextW,hwnd,IDC_DIRTXT,offset szTxtDir,512
			invoke SetCurrentDirectoryW,offset szTxtDir
			.if !eax
				invoke MessageBox,hwnd,$CTA0("TXT目录不存在！"),0,0
				jmp _Ex
			.endif
			invoke SetCurrentDirectoryW,$CTW0("..")
			invoke SetCurrentDirectoryW,offset szOrsDir
			.if !eax
				invoke MessageBox,hwnd,$CTA0("ORS目录不存在！"),0,0
				jmp _Ex
			.endif
			invoke _FindFiles,offset szNull,ebx
			invoke MessageBox,hwnd,$CTA0("转换完成"),$CTA0("converter"),0
		.elseif ax==IDC_TOTXT
			mov ebx,offset _CvtTxt
			jmp _Label1
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

_Ex:
	mov eax,TRUE
	ret
_DlgMainProc endp

_CvtTxt proc uses esi edi ebx _lpszOrs
	LOCAL @lpTxtFileName
	LOCAL @hOrs,@hTxt,@lpOrs,@lpTxt
	LOCAL @pOrsEnd
	LOCAL @pN,@pT
	invoke HeapAlloc,hHeap,0,4096
	or eax,eax
	je _Err
	mov @lpTxtFileName,eax
	mov esi,eax
	invoke lstrcpyW,eax,offset szTxtDir
	invoke lstrlenW,esi
	lea ecx,[esi+eax*2]
	.if word ptr [ecx-2]!='\'
		mov dword ptr [ecx],'\'
	.endif
	invoke lstrcatW,esi,_lpszOrs
	
	invoke CreateFileW,_lpszOrs,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	cmp eax,-1
	je _Err1
	mov @hOrs,eax
	invoke GetFileSize,@hOrs,0
	mov ebx,eax
	invoke HeapAlloc,hHeap,0,ebx
	or eax,eax
	je _Err2
	mov @lpOrs,eax
	invoke HeapAlloc,hHeap,0,ebx
	or eax,eax
	je _Err3
	mov @lpTxt,eax
	invoke ReadFile,@hOrs,@lpOrs,ebx,offset dwTemp,0
	mov esi,@lpOrs
	mov edi,@lpTxt
	mov ecx,esi
	add ecx,ebx
	mov @pOrsEnd,ecx
	.while esi<@pOrsEnd
		.if (dword ptr [esi]=='irP[' && dword ptr [esi+4]=='eTtn') || (dword ptr [esi]=='teS[' && dword ptr [esi+4]=='ELES')
			.while byte ptr [esi]!=','
				cmp word ptr [esi],0d0dh
				je _NextLine
				cmp byte ptr [esi],0
				je _NextLine
				.if byte ptr [esi]>80h
					add esi,2
				.else
					inc esi
				.endif
			.endw
			inc esi
			mov @pN,esi
			.while byte ptr [esi]!=','
				cmp word ptr [esi],0d0dh
				je _NextLine
				cmp byte ptr [esi],0
				je _NextLine
				.if byte ptr [esi]>80h
					add esi,2
				.else
					inc esi
				.endif
			.endw
			inc esi
			mov @pT,esi
			
			mov esi,@pN
			.if byte ptr [esi]==' '
				inc esi
			.endif
			.while 1
				lodsw
				.break .if al==',' || ax==0d0dh || al==0
				.if al>80h
					stosw
				.else
					stosb
					dec esi
				.endif
			.endw
			mov ax,0a0dh
			stosw
			
			mov esi,@pT
			.if byte ptr [esi]==' '
				inc esi
			.endif
			.while 1
				lodsw
				.break .if al==',' || ax==0d0dh || al==0
				.if al>80h
					stosw
				.else
					stosb
					dec esi
				.endif
			.endw
			mov ax,0a0dh
			stosw
			
			.while word ptr [esi]!=0d0dh
				.break .if !byte ptr [esi]
				inc esi
			.endw
		.else
			.while word ptr [esi]!=0d0dh
				.break .if esi>=@pOrsEnd
				inc esi
			.endw
		.endif
		_NextLine:
		add esi,3
	.endw
	
	sub edi,@lpTxt
;	invoke CreateFileW,@lpTxtFileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov ecx,@lpTxtFileName
	.while word ptr [ecx]
		add ecx,2
	.endw
	sub ecx,6
	mov eax,[ecx]
	or eax,200020h
	mov dx,[ecx+4]
	or dx,20h
	.if eax==72006fh && dx=='s'
		mov dword ptr [ecx],780074h
		mov word ptr [ecx+4],'t'
	.endif
	invoke _MakeFile,@lpTxtFileName
	or eax,eax
	je _Err4
	mov @hTxt,eax
	invoke WriteFile,@hTxt,@lpTxt,edi,offset dwTemp,0
	
	invoke CloseHandle,@hTxt
	invoke HeapFree,hHeap,0,@lpTxt
	invoke HeapFree,hHeap,0,@lpOrs
	invoke CloseHandle,@hOrs
	invoke HeapFree,hHeap,0,@lpTxtFileName
	xor eax,eax
	ret
_Err4:
	invoke HeapFree,hHeap,0,@lpTxt
_Err3:
	invoke HeapFree,hHeap,0,@lpOrs
_Err2:
	invoke CloseHandle,@hOrs
_Err1:
	invoke HeapFree,hHeap,0,@lpTxtFileName
_Err:
	mov eax,1
	ret
_CvtTxt endp

_CvtOrs proc uses esi edi ebx _lpszOrs
	LOCAL @lpTxtFileName
	LOCAL @hOrs,@hTxt,@lpOrs,@lpTxt
	LOCAL @pOrsEnd,@pTxtEnd
	LOCAL @pN,@pT
	LOCAL @nNew,@nOri
	invoke HeapAlloc,hHeap,0,4096
	or eax,eax
	je _Err
	mov @lpTxtFileName,eax
	mov esi,eax
	invoke lstrcpyW,eax,offset szTxtDir
	invoke lstrlenW,esi
	lea ecx,[esi+eax*2]
	.if word ptr [ecx-2]!='\'
		mov dword ptr [ecx],'\'
	.endif
	invoke lstrcatW,esi,_lpszOrs
	
	mov ecx,esi
	.while word ptr [ecx]
		add ecx,2
	.endw
	sub ecx,6
	mov eax,[ecx]
	or eax,200020h
	mov dx,[ecx+4]
	or dx,20h
	.if eax==72006fh && dx=='s'
		mov dword ptr [ecx],780074h
		mov word ptr [ecx+4],'t'
	.endif
	invoke CreateFileW,@lpTxtFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	cmp eax,-1
	je _Erra
	mov @hTxt,eax
	invoke GetFileSize,eax,0
	mov ebx,eax
	inc eax
	invoke HeapAlloc,hHeap,0,eax
	or eax,eax
	je _Errb
	mov @lpTxt,eax
	mov byte ptr [eax+ebx],0
	invoke ReadFile,@hTxt,@lpTxt,ebx,offset dwTemp,0
	add ebx,@lpTxt
	mov @pTxtEnd,ebx
	
	invoke CreateFileW,_lpszOrs,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	cmp eax,-1
	je _Err1
	mov @hOrs,eax
	invoke GetFileSize,@hOrs,0
	mov ebx,eax
	shl eax,1
	invoke HeapAlloc,hHeap,0,eax
	or eax,eax
	je _Err2
	mov @lpOrs,eax
	invoke ReadFile,@hOrs,@lpOrs,ebx,offset dwTemp,0
	mov esi,@lpOrs
	mov edi,@lpTxt
	mov ecx,esi
	add ecx,ebx
	mov @pOrsEnd,ecx
	.while esi<@pOrsEnd
		.if (dword ptr [esi]=='irP[' && dword ptr [esi+4]=='eTtn') || (dword ptr [esi]=='teS[' && dword ptr [esi+4]=='ELES')
			.while byte ptr [esi]!=','
				cmp word ptr [esi],0d0dh
				je _NextLine
				cmp byte ptr [esi],0
				je _NextLine
				.if byte ptr [esi]>80h
					add esi,2
				.else
					inc esi
				.endif
			.endw
			inc esi
			mov @pN,esi
			.while byte ptr [esi]!=','
				cmp word ptr [esi],0d0dh
				je _NextLine
				cmp byte ptr [esi],0
				je _NextLine
				.if byte ptr [esi]>80h
					add esi,2
				.else
					inc esi
				.endif
			.endw
			inc esi
			mov @pT,esi
			
			mov esi,@pN
			.if byte ptr [esi]==' '
				inc esi
			.endif
			mov ecx,@pT
			sub ecx,esi
			dec ecx
			mov @nOri,ecx
			mov eax,edi
			.while word ptr [edi]!=0a0dh
				.break .if !byte ptr [edi]
				inc edi
			.endw
			mov edx,edi
			add edi,2
			sub edx,eax
			mov @nNew,edx
			mov ebx,@pOrsEnd
			sub ebx,esi
			sub ebx,ecx
			invoke _ReplaceInMem,eax,edx,esi,ecx,ebx
			mov ecx,@nNew
			sub ecx,@nOri
			add @pOrsEnd,ecx
			add @pT,ecx
			
			cmp edi,@pTxtEnd
			ja _Err3
			
			mov esi,@pT
			.if byte ptr [esi]==' '
				inc esi
			.endif
			mov ecx,esi
			.while 1
				mov ax,[ecx]
				.break .if al==',' || ax==0d0dh || al==0
				.if al>80h
					add ecx,2
				.else
					inc ecx
				.endif
			.endw
			sub ecx,esi
			mov @nOri,ecx
			mov eax,edi
			.while word ptr [edi]!=0a0dh
				.break .if !byte ptr [edi]
				inc edi
			.endw
			mov edx,edi
			add edi,2
			sub edx,eax
			mov @nNew,edx
			mov ebx,@pOrsEnd
			sub ebx,esi
			sub ebx,ecx
			invoke _ReplaceInMem,eax,edx,esi,ecx,ebx
			mov ecx,@nNew
			sub ecx,@nOri
			add @pOrsEnd,ecx
			
			cmp edi,@pTxtEnd
			ja _Err3
			
			.while word ptr [esi]!=0d0dh
				.break .if !byte ptr [esi]
				inc esi
			.endw
		.else
			.while word ptr [esi]!=0d0dh
				.break .if esi>=@pOrsEnd
				inc esi
			.endw
		.endif
		_NextLine:
		add esi,3
	.endw
	
	invoke SetFilePointer,@hOrs,0,0,FILE_BEGIN
	mov eax,@pOrsEnd
	sub eax,@lpOrs
	invoke WriteFile,@hOrs,@lpOrs,eax,offset dwTemp,0
	invoke SetEndOfFile,@hOrs
	
	invoke CloseHandle,@hTxt
	invoke HeapFree,hHeap,0,@lpTxt
	invoke HeapFree,hHeap,0,@lpOrs
	invoke CloseHandle,@hOrs
	invoke HeapFree,hHeap,0,@lpTxtFileName
	xor eax,eax
	ret
_Err3:
	invoke HeapFree,hHeap,0,@lpOrs
_Err2:
	invoke CloseHandle,@hOrs
_Err1:
	invoke HeapFree,hHeap,0,@lpTxtFileName
_Errb:
	invoke HeapFree,hHeap,0,@lpTxt
_Erra:
	invoke CloseHandle,@hTxt
_Err:
	mov eax,1
	ret
_CvtOrs endp

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
			mov ax,'\'
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
					mov word ptr [edi-2],'\'
					.if !eax
						invoke GetLastError
						.if eax!=ERROR_ALREADY_EXISTS
							mov word ptr [ebx],'\'
							pop ebx
							pop edi
							pop esi
							xor eax,eax
							ret
						.endif
					.endif
					mov ecx,esi
					mov ax,'\'
				.else
					mov edi,ebx
					invoke CreateDirectoryW,addr @szStr1,0
					mov word ptr [ebx],'\'
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

_FindFiles proc uses edi _lpszDir,_lpCallback
	LOCAL @lpCurDir,@lpNewDir
	LOCAL @lpFindData
	LOCAL @hFindFile
	LOCAL @hTxtFile,@nTxtSize,@lpTxtBuffer
	invoke HeapAlloc,hHeap,0,4096
	or eax,eax
	je _Ex
	mov @lpCurDir,eax
	invoke HeapAlloc,hHeap,0,sizeof(WIN32_FIND_DATAW)
	or eax,eax
	je _Ex2
	mov @lpFindData,eax
	invoke HeapAlloc,hHeap,0,4096
	or eax,eax
	je _Ex3
	mov @lpNewDir,eax
	invoke lstrcpyW,@lpCurDir,_lpszDir
	mov ecx,@lpCurDir
	cmp word ptr [ecx],0
	je @F
	.while word ptr [ecx]
		add ecx,2
	.endw
	.if word ptr [ecx-2]!='\'
		mov word ptr [ecx],'\'
		add ecx,2
	.endif
	@@:
	invoke lstrcpyW,ecx,$CTW0('*')
	
	invoke FindFirstFileW,@lpCurDir,@lpFindData
	.if eax!=INVALID_HANDLE_VALUE
		mov @hFindFile,eax
		invoke lstrlenW,@lpCurDir
		shl eax,1
		mov ecx,@lpCurDir
		lea ecx,[ecx+eax-2]
		mov word ptr [ecx],0
		.repeat
			mov edi,@lpFindData
			assume edi:ptr WIN32_FIND_DATA
			.if dword ptr [edi].cFileName=='.' || dword ptr [edi].cFileName==2e002eh && !word ptr [[edi].cFileName+4]
				jmp _NextFind
			.endif
			.if [edi].dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY
				invoke lstrcpyW,@lpNewDir,@lpCurDir
				invoke lstrcatW,@lpNewDir,addr [edi].cFileName
				invoke _FindFiles,@lpNewDir,_lpCallback
			.else
				invoke lstrcpyW,@lpNewDir,@lpCurDir
				invoke lstrcatW,@lpNewDir,addr [edi].cFileName
				push @lpNewDir
				call _lpCallback
			.endif
			assume edi:nothing
			_NextFind:
			invoke FindNextFileW,@hFindFile,@lpFindData
		.until eax==FALSE
		invoke FindClose,@hFindFile
	.endif
	invoke HeapFree,hHeap,0,@lpNewDir
_Ex3:
	invoke HeapFree,hHeap,0,@lpFindData
_Ex2:
	invoke HeapFree,hHeap,0,@lpCurDir
_Ex:
	ret
_FindFiles endp
;
_ReplaceInMem proc uses esi edi _lpNew,_nNewLen,_lpOriPos,_nOriLen,_nLeftLen
	mov eax,_nNewLen
	.if eax==_nOriLen || !_nLeftLen
		mov esi,_lpNew
		mov edi,_lpOriPos
		mov ecx,eax
		rep movsb
		xor eax,eax
	.elseif eax>_nOriLen
	@@:
		invoke HeapAlloc,hHeap,0,_nLeftLen
		.if !eax
			mov eax,1
			jmp _ExRIM
		.endif
		push eax
		mov ecx,_nLeftLen
		mov esi,_lpOriPos
		add esi,_nOriLen
		mov edi,eax
		invoke _memcpy
		mov esi,_lpNew
		mov ecx,_nNewLen
		mov edi,_lpOriPos
		rep movsb
		mov esi,[esp]
		mov ecx,_nLeftLen
		invoke _memcpy
		push 0
		push hHeap
		call HeapFree
		xor eax,eax
	.else
		mov ecx,_nOriLen
		sub ecx,eax
		cmp ecx,4
		jb @B
		mov esi,_lpNew
		mov ecx,_nNewLen
		mov edi,_lpOriPos
		rep movsb
		mov esi,_lpOriPos
		add esi,_nOriLen
		mov ecx,_nLeftLen
		invoke _memcpy
		xor eax,eax
	.endif
_ExRIM:
	ret
_ReplaceInMem endp
;
_memcpy proc
	mov eax,ecx
	shr ecx,2
	REP MOVSd
	mov ecx,eax
	and ecx,3
	REP MOVSb
	ret
_memcpy endp

end start