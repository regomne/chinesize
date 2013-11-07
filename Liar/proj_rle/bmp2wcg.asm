.486
.model flat,stdcall
option casemap:none


include bmp2wcg.inc
include _BrowseFolder.asm
include wcg.asm

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax


invoke DialogBoxParam,hInstance,IDD_DLG1,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	local @pMemory[10]:dword
	local @stFindData:WIN32_FIND_DATA
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_CMPR

			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat2
			mov @opFileName.lpstrFile,offset szFileNameOri
			mov byte ptr [szFileNameOri],0
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			or eax,eax
			je Ex
			invoke CreateFile,offset szFileNameOri,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				invoke MessageBox,hwnd,offset szCantCreateFile,offset szDisplayName,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			mov hBmpFile,eax
			
			mov ecx,16
			lea edi,@pMemory
			xor eax,eax
			rep stosd
			invoke _FoldPic,hBmpFile,addr @pMemory
			.if !eax
				push MB_OK OR MB_ICONERROR
				push offset szDisplayName
				.if ecx==1
					push offset szNoMemory
				.elseif ecx==2
					push offset szNotCorrectBmp
				.else
					push offset szCompressError
				.endif
				push hwnd
				call MessageBox
				
				mov ebx,9
				.while ebx
					lea edi,@pMemory
					mov eax,[edi+ecx*4]
					.if eax
						invoke GlobalFree,eax
					.endif
					dec ebx
				.endw
				jmp Ex
			.endif
			mov lpLimBuff,eax
			mov nLimSize,ecx
			or ecx,-1
			lea edi,szFileNameOri
			xor eax,eax
			repne scasb
			std
			mov al,'.'
			repne scasb
			cld
			inc edi
			mov dword ptr [edi],'gcw.'
			invoke CreateFile,offset szFileNameOri,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				invoke MessageBox,hwnd,offset szCantCreateFile,offset szDisplayName,MB_OK or MB_ICONERROR
				invoke CloseHandle,hBmpFile
				invoke GlobalFree,lpLimBuff
				jmp Ex
			.endif
			push eax
			invoke WriteFile,eax,lpLimBuff,nLimSize,offset dwTemp,0
			call CloseHandle
			invoke CloseHandle,hBmpFile
			invoke GlobalFree,lpLimBuff
			invoke MessageBox,hwnd,offset szOver,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			
		.elseif ax==IDC_CMPRDIR
			invoke _BrowseFolder,hwnd,offset szDir1
			invoke SetCurrentDirectory,offset szDir1
			invoke FindFirstFile,offset szBmp,addr @stFindData
			.if eax!=INVALID_HANDLE_VALUE
				mov hFindFile,eax
				.repeat
					lea esi,@stFindData.cFileName
					invoke CreateFile,esi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
					cmp eax,-1
					je @f
					mov hBmpFile,eax
					mov ecx,16
					lea edi,@pMemory
					xor eax,eax
					rep stosd
					invoke _FoldPic,hBmpFile,addr @pMemory
					.if !eax
						mov ebx,9
						.while ebx
							lea edi,@pMemory
							mov eax,[edi+ecx*4]
							.if eax
								invoke GlobalFree,eax
							.endif
							dec ebx
						.endw
						invoke CloseHandle,hBmpFile
						jmp @F
					.endif
					mov lpLimBuff,eax
					mov nLimSize,ecx
					or ecx,-1
					lea edi,@stFindData.cFileName
					mov esi,edi
					xor eax,eax
					repne scasb
					std
					mov al,'.'
					repne scasb
					cld
					inc edi
					mov dword ptr [edi],'gcw.'
					invoke CreateFile,esi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
					.if eax==-1
						jmp label33
					.endif
					push eax
					invoke WriteFile,eax,lpLimBuff,nLimSize,offset dwTemp,0
					call CloseHandle
					label33:
					invoke CloseHandle,hBmpFile
					invoke GlobalFree,lpLimBuff
					@@:
					invoke FindNextFile,hFindFile,addr @stFindData
				.until eax==FALSE
				invoke FindClose,hFindFile
			.endif
			invoke MessageBox,hwnd,offset szOver,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			
		.endif
	.elseif eax==WM_INITDIALOG
		invoke LoadIcon,100,0
		invoke SendMessage,hwnd,WM_SETICON,ICON_BIG,eax
		mov eax,hwnd
		mov hWinMain,eax
		invoke SendDlgItemMessage,hwnd,IDC_OVERWRITE,BM_SETCHECK,1,0
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

