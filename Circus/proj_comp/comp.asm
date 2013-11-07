.486
.model flat,stdcall
option casemap:none


include comp.inc

include _BrowseFolder.asm
.code

start:
;……………………………………………………………………………………
invoke GetModuleHandle,NULL
mov hInstance,eax


invoke DialogBoxParam,hInstance,IDD_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;……………………………………………………………………………………

_WriteMes proc uses esi edi,_hMes,_hTxt
	local @lpBufferM,@lpBufferT,@nSizeM,@nSizeT
	local @nNewSize
	local @nLength
	local @lpText,@pTxt,@nTemp,@lpbfTemp
	LOCAL @nLine
	invoke GetFileSize,_hTxt,0
	mov @nSizeT,eax
	add eax,10h
	invoke GlobalAlloc,GPTR,eax
	mov @lpBufferT,eax
	invoke ReadFile,_hTxt,@lpBufferT,@nSizeT,offset dwTemp,0
	invoke _CorrectText,@lpBufferT,@nSizeT
	mov @lpBufferT,eax
	mov @nSizeT,ecx
	
	invoke GetFileSize,_hMes,0
	mov @nNewSize,eax
	add eax,10000
	mov @nSizeM,eax
	invoke GlobalAlloc,GPTR,eax
	mov @lpBufferM,eax
	invoke GlobalAlloc,GPTR,@nSizeM
	mov @lpbfTemp,eax
	invoke ReadFile,_hMes,@lpBufferM,@nSizeM,offset dwTemp,0
	mov edi,@lpBufferM
	mov eax,dword ptr [edi]
	shl eax,2
	add edi,4
	mov ebx,edi
	add edi,eax
	mov @lpText,edi
	mov edi,@lpBufferT
	mov @nLine,0
;	.while word ptr [edi]!=0a0dh
;		inc edi
;	.endw
;	add edi,2
;	.while word ptr [edi]!=0a0dh
;		inc edi
;	.endw
;	add edi,2
	mov @pTxt,edi
	.while ebx<@lpText
		mov edi,@lpText
		add edi,dword ptr [ebx]
		@@:
		mov al,byte ptr [edi]
		.if al==36h
			xor eax,eax
			or ecx,-1
			repne scasb
			jmp @B
		.elseif al==4bh
			inc edi
			mov esi,edi
			xor eax,eax
			or ecx,-1
			repne scasb
			not ecx
			dec ecx
			
			xor eax,eax
			mov edi,@pTxt
			.while word ptr [edi]==0a0dh
				.break .if !word ptr [edi]
				add edi,2
			.endw
			cmp byte ptr [edi],0
			je Err02
			.if byte ptr [edi]<80h
;				cmp byte ptr [edi],'T'
;				je _BadPos
				inc edi
				inc @pTxt
			.endif
			.while word ptr [edi]!=0a0dh
				.if !byte ptr [edi]
					sub edi,2
					.break
				.endif
				.if byte ptr [edi]==20h && byte ptr [edi+1]==0dh
					add byte ptr [edi],20h
				.endif
				cmp byte ptr [edi],80h
				jb _InvalidChar
				sub byte ptr [edi],20h
				sub byte ptr [edi+1],20h
				add edi,2
				add eax,2
			.endw
			
			mov @nLength,eax
			sub eax,ecx
			jnz aa
				mov edi,esi
				mov esi,@pTxt
				rep movsb
				add esi,2
				mov @pTxt,esi
				inc edi
				jmp @B
			aa:
			push ebx
			add ebx,4
			.while ebx<@lpText
				add dword ptr [ebx],eax
				add ebx,4
			.endw
			mov ebx,@nNewSize
			add @nNewSize,eax
			add ebx,@lpBufferM
			sub ebx,esi
			sub ebx,ecx
			mov edx,esi
			add esi,ecx
			mov ecx,ebx
			mov edi,@lpbfTemp
			call _memcpy
			mov ecx,@nLength
			mov esi,@pTxt
			mov edi,edx
			rep movsb
			push edi
			add esi,2
			mov @pTxt,esi
			mov esi,@lpbfTemp
			mov ecx,ebx
			call _memcpy
			pop edi
			inc edi
			pop ebx
			inc @nLine
			jmp @B
		.elseif al==4ch
			inc edi
;			.if word ptr [edi]==2561h && !byte ptr [edi+2]
;				jmp @F
;			.endif
			mov esi,edi
			xor eax,eax
			or ecx,-1
			repne scasb
			not ecx
			dec ecx
			mov @nLength,ecx
			
			mov eax,@nNewSize
			add eax,@lpBufferM
			sub eax,esi
			sub eax,ecx
			mov @nTemp,eax
			mov ecx,eax
			mov edx,esi
			dec edi
			mov esi,edi
			mov edi,@lpbfTemp
			call _memcpy
			mov edi,edx
			mov esi,@pTxt
			.while word ptr [esi]==0a0dh
				.break .if !word ptr [esi]
				add esi,2
			.endw
			cmp byte ptr [esi],0
			je Err02
			.if byte ptr [esi]<80h
;				cmp byte ptr [esi],'N'
;				je _BadPos
				inc esi
				inc @pTxt
			.endif
			xor ecx,ecx
			.while word ptr [esi]!=0a0dh
				.if !byte ptr [esi]
					sub esi,2
					.break
				.endif
				xor edx,edx
				.if byte ptr [esi]>=80h
					movsw
					sub byte ptr [edi-2],20h
					sub byte ptr [edi-1],20h
					add ecx,2
				.elseif word ptr [esi]==6e5ch
					add esi,2
					mov byte ptr [edi],0eah
					inc edi
					inc ecx
				.elseif byte ptr [esi]==20h
					mov word ptr [edi],08181h
					add edi,2
					inc esi
					add ecx,2
				.elseif byte ptr [esi]==3fh
					mov word ptr [edi],09f83h
					add edi,2
					inc esi
					add ecx,2
				.else
					cmp byte ptr [esi],80h
					jb _InvalidChar
					movsb
					sub byte ptr [edi-1],20h
					inc ecx
				.endif
			.endw
			add esi,2
			mov @pTxt,esi
			mov eax,ecx
			sub eax,@nLength
			add @nNewSize,eax
			mov @nLength,ecx
			push ebx
			add ebx,4
			.while ebx<@lpText
				add dword ptr [ebx],eax
				add ebx,4
			.endw
			mov esi,@lpbfTemp
			mov ecx,@nTemp
			call _memcpy
			inc @nLine
			pop ebx
		.elseif al==08h
			add edi,3
			.while byte ptr [edi]==44h
				add edi,5
				mov esi,edi
				xor eax,eax
				or ecx,-1
				repne scasb
				not ecx
				dec ecx
				
				xor eax,eax
				mov edi,@pTxt
				cmp byte ptr [edi],0
				je Err02
				.if byte ptr [edi]<80h
;					cmp byte ptr [edi],'N'
;					je _BadPos
;					cmp byte ptr [edi],'T'
;					je _BadPos
					inc edi
					inc @pTxt
				.endif
				.while word ptr [edi]!=0a0dh
					.if !byte ptr [edi]
						sub edi,2
						.break
					.endif
					.if byte ptr [edi]==20h && !byte ptr [edi+1]
						add byte ptr [edi],20h
					.endif
					cmp byte ptr [edi],80h
					jb _InvalidChar
					sub byte ptr [edi],20h
					sub byte ptr [edi+1],20h
					add edi,2
					add eax,2
				.endw
				
				mov @nLength,eax
				sub eax,ecx
				push ebx
				add ebx,4
				.while ebx<@lpText
					add dword ptr [ebx],eax
					add ebx,4
				.endw
				mov ebx,@nNewSize
				add @nNewSize,eax
				add ebx,@lpBufferM
				sub ebx,esi
				sub ebx,ecx
				mov edx,esi
				add esi,ecx
				mov ecx,ebx
				mov edi,@lpbfTemp
				call _memcpy
				mov ecx,@nLength
				mov esi,@pTxt
				mov edi,edx
				rep movsb
				push edi
				add esi,2
				mov @pTxt,esi
				mov esi,@lpbfTemp
				mov ecx,ebx
				call _memcpy
				pop edi
				inc edi
				inc @nLine
				pop ebx
			.endw
;		.elseif al!=4
;			jmp _UnkErr
;			db 00
;			db 00
		.endif
		@@:
		add ebx,4
	.endw
	
	mov ecx,@pTxt
	.while word ptr [ecx]==0a0dh
		add ecx,2	
	.endw
	.if byte ptr [ecx]!=0
		jmp Err02
	.endif
	
	invoke SetFilePointer,_hMes,0,0,FILE_BEGIN
	invoke WriteFile,_hMes,@lpBufferM,@nNewSize,offset dwTemp,0
	invoke SetEndOfFile,_hMes
	invoke GlobalFree,@lpBufferM
	invoke GlobalFree,@lpbfTemp
	invoke GlobalFree,@lpBufferT
	xor eax,eax
	ret
_BadPos:
	invoke GlobalFree,@lpBufferM
	invoke GlobalFree,@lpbfTemp
	invoke GlobalFree,@lpBufferT
	mov eax,E_LINENOTFIT
	mov ecx,@nLine
	ret
Err02:
	invoke GlobalFree,@lpBufferM
	invoke GlobalFree,@lpbfTemp
	invoke GlobalFree,@lpBufferT
	mov eax,E_LINENOTMATCH
	xor ecx,ecx
	ret
_InvalidChar:
	invoke GlobalFree,@lpBufferM
	invoke GlobalFree,@lpbfTemp
	invoke GlobalFree,@lpBufferT
	mov eax,E_INVALIDCHAR
	mov ecx,@nLine
	ret
_UnkErr:
	invoke GlobalFree,@lpBufferM
	invoke GlobalFree,@lpbfTemp
	invoke GlobalFree,@lpBufferT
	mov eax,10
	xor ecx,ecx
	ret
_WriteMes endp

_AddEnterA proc uses ebx esi edi,lpOri,lpNew,nCharPos,dwChar
	LOCAL @bAllowEnter
	shl nCharPos,1
	mov esi,lpOri
	mov edi,lpNew
;	mov ecx,2
;	@@:
;	.while word ptr [esi]!=0a0dh
;		mov al,byte ptr [esi]
;		.break .if !al
;		.if al>7fh
;			add esi,2
;		.else
;			inc esi
;		.endif
;	.endw
;	ADD esi,2
;	loop @B
;	sub esi,lpOri
;	mov ecx,esi
;	mov esi,lpOri
;	rep movsb
	
	mov ecx,dwChar
	.repeat
		.if byte ptr [esi]<80h
			movsb
		.endif
		movzx edx,word ptr [esi]
		.if edx==0b8a1h || edx==0baa1h
			xor edx,edx
		.else
			mov edx,1
		.endif
		xor ebx,ebx
		mov @bAllowEnter,TRUE
		.repeat
			_label1:
			mov ax,word ptr [esi]
			.break .if ax==0a0dh
			.if ax==cx
				movsw
;				.if edx
;					xor ebx,ebx
				.if word ptr [esi]==0a1a1h
					xor ebx,ebx
				.else
					mov word ptr [edi],0a1a1h
					add edi,2
					mov ebx,2
				.endif
				.continue
			.elseif ax==0fba3h
				movsw
				mov @bAllowEnter,FALSE
			.elseif ax==0FDA3H
				movsw
				MOV @bAllowEnter,TRUE
			.elseif ax==0afa3h
				push esi
				mov eax,esi
				.repeat
					dec esi
					.if word ptr [esi]==0fba3h
;						sub eax,esi
;						sub ebx,eax
						sub esi,eax
						lea ebx,[ebx+esi+2]
						.break
					.endif
				.until word ptr [esi]==0a0dh || esi<=lpOri
				pop esi
				movsw
			.elseif al>7fh
				movsw
				add ebx,2
			.else
				test al,al
				je Exi
				movsb
				add ebx,2
			.endif
			cmp ebx,nCharPos
			jl _label1
			cmp @bAllowEnter,FALSE
			je _label1
;			.if ebx>=nCharPos
				movzx eax,word ptr [esi]
				.break .if eax==0a0dh
				;逗号 顿号 句号 问号 引号
				.if eax==0aca3h||eax==0a2a1h||eax==0a3a1h||eax==0bfa3h||eax==0b9a1h
					.continue .if word ptr [esi+2]==0b9a1h || word ptr [esi+2]==0a0dh
					movsw
				.endif
				mov word ptr [edi],cx
				.if word ptr [esi]==0a1a1h
					add edi,2
					xor ebx,ebx
				.else
					mov word ptr [edi+2],0a1a1h
					add edi,4
					mov ebx,2
				.endif
;			.endif
		.until FALSE
		movsw
	.until FALSE
	Exi:
	sub edi,lpNew
	mov eax,edi
	ret
_AddEnterA endp

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	local @stFindData:WIN32_FIND_DATA
	LOCAL @str[64]:byte
	LOCAL @hErr
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_SINGLE
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szTxtFormat
			mov @opFileName.lpstrFile,offset szFileNameTxt
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			or eax,eax
			je Ex01
			
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szMesFormat
			mov @opFileName.lpstrFile,offset szFileNameMes
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			or eax,eax
			je Ex01
			
			invoke CreateFile,offset szFileNameTxt,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je Err01
			mov hFileTxt,eax
			invoke CreateFile,offset szFileNameMes,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			cmp eax,-1
			je Err01
			mov hFileMes,eax
			invoke _WriteMes,hFileMes,hFileTxt
			.if eax
				invoke _WriteErrA,@hErr,edi,eax,ecx
				invoke MessageBox,hwnd,offset szWritingError,0,MB_OK or MB_ICONERROR
				jmp Ex01
			.endif
			invoke MessageBox,hwnd,offset szWroteSuccess,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			invoke CloseHandle,hFileTxt
			invoke CloseHandle,hFileMes
			jmp Ex01
		Err01:
			invoke MessageBox,hwnd,offset szOpenFailed,0,MB_OK or MB_ICONERROR
			invoke CloseHandle,hFileTxt

		.elseif ax==IDC_DIR
			lea edi,szDirT
			mov ecx,256
			xor eax,eax
			rep stosd
			invoke _BrowseFolder,hwnd,offset szDirT,offset szTxtDir
			or eax,eax
			je Ex01
			invoke _BrowseFolder,hwnd,offset szDirM,offset szMesDir
			or eax,eax
			je Ex01
			invoke SetCurrentDirectory,offset szDirM
			invoke CreateFile,$CTA0("error.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			mov @hErr,eax
			invoke SetCurrentDirectory,offset szDirT
			invoke FindFirstFile,offset szTxt,addr @stFindData
			.if eax!=INVALID_HANDLE_VALUE
				mov hFindFile,eax
				.repeat
					lea edi,@stFindData.cFileName
					invoke CreateFile,edi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
					.if eax==-1
						invoke _WriteErrA,@hErr,addr @stFindData.cFileName,E_TXTERROR,-1
						JMP _NextFile
					.endif
					mov hFileTxt,eax
					
					mov eax,7478742eh
					.while eax!=dword ptr [edi]
						.if !byte ptr [edi]
							lea edi,@stFindData.cFileName
							mov eax,5458542eh
							.while eax!=dword ptr [edi]
								.if !byte ptr [edi]
									invoke _WriteErrA,@hErr,addr @stFindData.cFileName,10,-1
									JMP @F
								.endif
								inc edi
							.endw
							.break
						.endif
						inc edi
					.endw
					mov dword ptr [edi],73656d2eh
					invoke SetCurrentDirectory,offset szDirM
					lea edi,@stFindData.cFileName
					.if byte ptr [edi]=='x'
						inc edi
					.endif
					invoke CreateFile,edi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
					.if eax==-1
						invoke _WriteErrA,@hErr,addr @stFindData.cFileName,E_MESERROR,-1
						JMP @F
					.endif
					mov hFileMes,eax
					invoke _WriteMes,hFileMes,hFileTxt
					.if eax
						invoke _WriteErrA,@hErr,edi,eax,ecx
					.endif
					@@:
					invoke SetCurrentDirectory,offset szDirT
					invoke CloseHandle,hFileTxt
					invoke CloseHandle,hFileMes
				_NextFile:
					invoke FindNextFile,hFindFile,addr @stFindData
				.until eax==FALSE
				invoke FindClose,hFindFile
			.endif
			invoke SetEndOfFile,@hErr
			invoke GetFileSize,@hErr,0
			mov ebx,eax
			invoke CloseHandle,@hErr
			.if !ebx
				invoke MessageBox,hwnd,offset szWroteSuccess,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			.else
				invoke MessageBox,hwnd,offset szWrotePartSuccess,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			.endif
		.endif
	.elseif eax==WM_INITDIALOG
		mov eax,hwnd
		mov hWinMain,eax
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		mov eax,FALSE
		ret
	.endif

Ex01:
	mov eax,TRUE
	ret
	
_DlgMainProc endp

;
_CorrectText proc uses esi edi ebx _lpText,_nSize
	LOCAL @lpTemp,@nTempSize
	LOCAL @lpEnd
	mov eax,_nSize
	shl eax,1
	mov @nTempSize,eax
	invoke GlobalAlloc,GPTR,eax
	.if !eax
		ret
	.endif
	mov @lpTemp,eax
	mov esi,_lpText
	.if word ptr [esi]==0feffh
		add esi,2
		invoke WideCharToMultiByte,936,WC_COMPOSITECHECK,esi,_nSize,@lpTemp,0,0,0
		invoke WideCharToMultiByte,936,WC_COMPOSITECHECK,esi,_nSize,@lpTemp,eax,0,0
		.if !eax
			invoke GlobalFree,@lpTemp
			xor eax,eax
			ret
		.endif
		mov _nSize,eax
		invoke GlobalFree,_lpText
		mov eax,@lpTemp
		mov _lpText,eax
		invoke GlobalAlloc,GPTR,@nTempSize
		.if !eax
			ret
		.endif
		mov @lpTemp,eax
	.endif
	mov edi,_lpText
	add edi,_nSize
	mov @lpEnd,edi
	mov edi,_lpText
	xor edx,edx
;	.while edi<@lpEnd
;		.if word ptr [edi]==6e5ch
;			inc edx
;		.endif
;		inc edi
;	.endw
;	.if edx<=2
		invoke _AddEnterA,_lpText,@lpTemp,22,6e5ch
		push eax
		invoke GlobalFree,_lpText
		mov eax,@lpTemp
		pop ecx
		ret
;	.endif
;	invoke GlobalFree,@lpTemp
;	mov eax,_lpText
;	mov ecx,_nSize
;	ret
_CorrectText endp

_WriteErrA proc _hErr,_lpfn,_nErrType,_nExtra
	LOCAL @str[128]:byte
	mov eax,_nErrType
	.if eax==E_LINENOTMATCH
		mov ecx,$CTA0("行数不符")
	.elseif eax==E_MESERROR
		mov ecx,$CTA0("无法打开mes文件")
	.elseif eax==E_TXTERROR
		mov ecx,$CTA0("无法打开txt文件")
	.elseif eax==E_INVALIDCHAR
		mov ecx,$CTA0("存在半角字符")
	.elseif eax==E_LINENOTFIT
		mov ecx,$CTA0("行错位")
	.else
		mov ecx,$CTA0("未知错误")
	.endif
	.if _nExtra==-1
		invoke wsprintf,addr @str,$CTA0("%s\t\t%s\n"),_lpfn,ecx
	.else
		invoke wsprintf,addr @str,$CTA0("%s\t\t%s\t\t%d\n"),_lpfn,ecx,_nExtra
	.endif
	invoke WriteFile,_hErr,addr @str,eax,offset dwTemp,0
	ret
_WriteErrA endp

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