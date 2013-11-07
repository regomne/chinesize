.486
.model flat,stdcall
option casemap:none


include lnedit.inc
include _CreateDIBitmap.asm
include ExcepHandler.asm

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax


invoke DialogBoxParamW,hInstance,DLG_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,0

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	assume fs:nothing
	push ebp
	push offset Ex
	push offset _ProcHandler
	push fs:[0]
	mov fs:[0],esp

	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		mov edx,lParam
		.if ax==IDC_MODIFY
			invoke GetDlgItemTextW,hwnd,IDC_EDITN,offset szStringN,512
			.if !word ptr [szStringN]
				jmp Ex
			.endif
;			invoke _StringBack,offset szStringN
			invoke GetDlgItemInt,hwnd,IDC_LINE2,addr @opFileName,FALSE
			mov ebx,eax
			dec ebx
			invoke SendDlgItemMessageW,hwnd,IDC_LISTN,LB_DELETESTRING,ebx,0
			invoke SendDlgItemMessageW,hwnd,IDC_LISTN,LB_INSERTSTRING,ebx,offset szStringN
			invoke WideCharToMultiByte,936,0,offset szStringN,-1,offset szTemp,512,0,0
			invoke _ModifyOriText,ebx,offset szTemp
			mov bIsModified,TRUE
			inc ebx
			.if ebx==nRealLine
				dec ebx
			.endif
			invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_SETCURSEL,ebx,0
			invoke SendMessage,hwnd,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTN,hListN
			
		.elseif ax==IDC_EDITN || ax==IDC_EDITO
			mov eax,wParam
			shr eax,16
			.if eax==EN_CHANGE
				invoke _UpdateWindow,lParam
			.endif
			
		.elseif ax==IDC_LISTN
			invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_GETCOUNT,0,0
			.if !eax
				jmp Ex
			.endif
			mov eax,wParam
			shr eax,16
			.if eax==LBN_SELCHANGE
				invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_GETCURSEL,0,0
				mov ebx,eax
				invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_SETCURSEL,ebx,0
				invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_GETTOPINDEX,0,0
				invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_SETTOPINDEX,eax,0
				invoke _SetText,hwnd
				invoke _UpdateWindow,hListN
				invoke _UpdateWindow,hListO
			.endif
					
		.elseif ax==IDC_LISTO
			invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_GETCOUNT,0,0
			.if !eax
				jmp Ex
			.endif
			mov eax,wParam
			shr eax,16
			.if eax==LBN_SELCHANGE
				invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_GETCURSEL,0,0
				mov ebx,eax
				invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_SETCURSEL,ebx,0
				invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_GETTOPINDEX,0,0
				invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_SETTOPINDEX,eax,0
				invoke _SetText,hwnd
				invoke _UpdateWindow,hListN
				invoke _UpdateWindow,hListO
			.endif
			
		.elseif ax==IDC_SAVE
			invoke SetFilePointer,hFile2,0,0,FILE_BEGIN
			invoke WriteFile,hFile2,lpFileBuff2,nFileSizeNew,addr @opFileName,0
			invoke SetEndOfFile,hFile2
			mov bIsModified,FALSE
			invoke CreateThread,0,0,offset _DisplayMsg,0,0,0
			invoke CloseHandle,eax

		.elseif ax==IDC_FIND
			invoke DialogBoxParam,hInstance,DLG_RPC,hwnd,offset _ReplaceDlgProc,0
			
		.elseif ax==IDC_OPEN
			invoke _SaveOrNot
			.if !eax
				jmp Ex
			.endif
			.if byte ptr [szFileName]
				lea esi,szFileName
				mov ebx,507
				.repeat
					mov eax,esi
					lea edi,szNewSc
					mov ecx,5
					repe cmpsb
					.if !ecx
						mov byte ptr [eax],0
						invoke SetCurrentDirectory,offset szFileName
						jmp @F
					.endif
					inc eax
					mov esi,eax
					dec ebx
				.until !ebx
			.endif
			@@:
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov byte ptr [szFileName],0
			mov @opFileName.lStructSize,sizeof @opFileName
			push hwnd
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat2
			mov @opFileName.lpstrFile,offset szFileName
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			.if !eax
				jmp Ex
			.endif
label983:
			invoke CreateFile,offset szFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==INVALID_HANDLE_VALUE
				invoke MessageBox,hwnd,offset szFailOpen,offset szError,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			push eax
			invoke _IsFormatted,eax
			.if !eax
				invoke MessageBox,hwnd,offset szNotTxt,offset szDisplayName,MB_YESNO or MB_ICONWARNING or MB_DEFBUTTON2
				.if eax==IDNO
					call CloseHandle
					jmp Ex
				.endif
			.endif
			invoke CloseHandle,hFile
			pop hFile
			invoke GlobalFree,lpFileBuff2
			invoke GlobalFree,lpFileBuff
			invoke GlobalFree,lpLineRecords
			invoke GlobalFree,lpTemp
			
			lea edi,szFileName
			add edi,256
			std
			mov al,'\'
			or ecx,-1
			repnz scasb
			cld
			not ecx
			lea esi,[edi+2]
			mov edx,esi
			sub esp,512
			mov edi,esp
			sub ecx,2
			mov ebx,ecx
			rep movsb
			lea esi,szNewSc
			mov edi,edx
			mov ecx,6
			rep movsb
			mov byte ptr [edi],0
			invoke SetCurrentDirectory,offset szFileName
			.if !eax
				invoke CreateDirectory,offset szFileName,0
			.endif
			mov ecx,ebx
			mov esi,esp
			rep movsb
			add esp,512
			invoke CloseHandle,hFile2
			invoke CreateFile,offset szFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==INVALID_HANDLE_VALUE
				invoke MessageBox,hwnd,offset szFailOpen,offset szError,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			mov hFile2,eax
			 
			invoke KillTimer,hwnd,ID_TIMER
			invoke GetFileSize,hFile,0
			mov esi,eax
			add esi,4000h
			invoke GetFileSize,hFile2,0
			mov edi,eax
			invoke GlobalAlloc,GPTR,esi
			mov lpFileBuff,eax
			.if edi
				add edi,4000h
			.else
				mov edi,esi
			.endif
			invoke GlobalAlloc,GPTR,edi
			mov lpFileBuff2,eax
			invoke GlobalAlloc,GPTR,edi
			mov lpTemp,eax
			invoke SetFilePointer,hFile,0,0,FILE_BEGIN
			invoke ReadFile,hFile,lpFileBuff,esi,addr @opFileName,NULL
			invoke ReadFile,hFile2,lpFileBuff2,edi,addr @opFileName,NULL
			invoke CreateThread,0,0,offset _AddToList,hwnd,0,0
;			invoke _AddToList,hwnd
			invoke CloseHandle,eax
			mov bIsModified,0
			invoke GetDlgItem,hwnd,IDC_EDITN
			invoke SetFocus,eax

		.elseif ax==IDC_GOTO
			invoke DialogBoxParam,hInstance,DLG_GOTO,hWinMain,offset _GotoDlgProc,0
			
		.elseif ax==IDC_IMPORT
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov byte ptr [szFileName],0
			mov @opFileName.lStructSize,sizeof @opFileName
			push hwnd
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat3
			mov @opFileName.lpstrFile,offset szFileName
			mov @opFileName.nMaxFile,512
			mov @opFileName.Flags,OFN_PATHMUSTEXIST or OFN_FILEMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			or eax,eax
			je Ex
			invoke CreateFile,offset szFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				invoke MessageBox,hwnd,offset szFailOpen,offset szError,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			sub esp,0ch
			mov [esp+8],eax
			invoke GetFileSize,eax,0
			mov [esp+4],eax
			invoke GlobalAlloc,GPTR,eax
			mov [esp],eax
			invoke ReadFile,[esp+18h],eax,[esp+0ch],offset dwTemp,0
			invoke _ImportTxt,[esp]
			.if eax
				invoke MessageBox,hwnd,offset szImpTxtSuccess,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			.else
				invoke MessageBox,hwnd,offset szImpTxtFail,offset szDisplayName,MB_OK or MB_ICONERROR
				invoke SendMessage,hwnd,WM_QUIT,0,0
			.endif
			invoke CloseHandle,[esp+8]
			invoke GlobalFree,[esp]
			add esp,0ch
			mov bIsModified,1
		
		.elseif ax==IDC_EXPORT
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			lea edi,szFileName
			mov ecx,512
			mov al,'.'
			repne scasb
			dec edi
			mov byte ptr [edi],0
;			mov byte ptr [szFileName],0
			mov @opFileName.lStructSize,sizeof @opFileName
			push hwnd
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat3
			mov @opFileName.lpstrFile,offset szFileName
			mov @opFileName.nMaxFile,512
			mov @opFileName.lpstrDefExt,offset szTxt1
			mov @opFileName.Flags,OFN_PATHMUSTEXIST or OFN_OVERWRITEPROMPT
			lea eax,@opFileName
			invoke GetSaveFileName,eax
			or eax,eax
			je Ex
			invoke CreateFile,offset szFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				invoke MessageBox,hwnd,offset szFailOpen,offset szError,MB_OK or MB_ICONERROR
				jmp Ex
			.endif
			push eax
			invoke _ExportTxt,lpLineRecords,eax
			.if eax
				invoke MessageBox,hwnd,offset szSaveTxtSuccess,offset szDisplayName,MB_OK or MB_ICONINFORMATION
			.else
				invoke MessageBox,hwnd,offset szSaveTxtFail,offset szDisplayName,MB_OK or MB_ICONERROR
			.endif
			call CloseHandle
					
		.elseif ax==IDC_REDO
			invoke GetDlgItemTextW,hwnd,IDC_EDITO,offset szStringO,512
			invoke SetDlgItemTextW,hwnd,IDC_EDITN,offset szStringO
			invoke SendMessage,hEditN,EM_SETSEL,0,-1
			invoke _UpdateWindow,hEditN
			
;		.elseif ax==IDC_OPTION
;			invoke DialogBoxParam,hInstance,DLG_OPTION,hWinMain,offset _OptionDlgProc,0
			
		.elseif ax==IDC_ACCKEY
			invoke MessageBox,hwnd,offset szInstruction,offset szDisplayName,MB_OK
			
		.endif
		
	.elseif eax==WM_CTLCOLORSTATIC || eax==WM_CTLCOLOREDIT || eax==WM_CTLCOLORLISTBOX
		invoke SetBkMode,wParam,TRANSPARENT
		mov eax,hBrushTr
		ret
		
	.elseif eax==WM_ERASEBKGND
		.if !hBackBmp
			return FALSE
		.endif
		mov esi,wParam
		invoke CreateCompatibleDC,esi
		mov ebx,eax
		invoke SelectObject,eax,hBackBmp
		invoke BitBlt,esi,0,0,800,600,ebx,0,0,SRCCOPY
		invoke DeleteDC,ebx
		
	.elseif eax==WM_DROPFILES
		invoke _SaveOrNot
		.if !eax
			jmp Ex
		.endif
		invoke DragQueryFile,wParam,0,offset szFileName,512
		jmp label983
	.elseif eax==WM_MOUSEWHEEL	
		invoke SendMessage,hListO,WM_MOUSEWHEEL,wParam,lParam
		
	.elseif eax==WM_TIMER
		invoke SendMessage,hwnd,WM_COMMAND,IDC_SAVE,0
			
	.elseif eax==WM_INITDIALOG
		invoke SendMessage,hwnd,WM_SETICON,ICON_BIG,hIcon
		mov eax,hwnd
		mov hWinMain,eax
		invoke GetDlgItem,hwnd,IDC_LISTO
		mov hListO,eax
		invoke GetDlgItem,hwnd,IDC_LISTN
		mov hListN,eax
		invoke GetDlgItem,hwnd,IDC_LINE1
		mov hStatusLine,eax
;		invoke GetDlgItem,hwnd,IDC_BYTEO
;		mov hStatusByteO,eax
;		invoke GetDlgItem,hwnd,IDC_BYTEN
;		mov hStatusByteN,eax
		invoke GetDlgItem,hwnd,IDC_EDITO
		mov hEditO,eax
		invoke GetDlgItem,hwnd,IDC_EDITN
		mov hEditN,eax
		invoke GetDlgItem,hwnd,IDC_SAVE
		mov hBtnSave,eax
		invoke GetDlgItem,hwnd,IDC_MODIFY
		mov hBtnModify,eax
		invoke GetDlgItem,hwnd,IDC_GOTO
		mov hBtnGoto,eax
		invoke GetDlgItem,hwnd,IDC_FIND
		mov hBtnFind,eax
		invoke GetDlgItem,hwnd,IDC_IMPORT
		mov hBtnImp,eax
		invoke GetDlgItem,hwnd,IDC_EXPORT
		mov hBtnExp,eax
		invoke SetWindowLong,hListO,GWL_WNDPROC,addr _NewListProc
		mov lpOldListProc,eax
		invoke SetWindowLong,hListN,GWL_WNDPROC,addr _NewListProc
		invoke GetDlgItem,hwnd,IDC_EDITN
		invoke SetWindowLong,eax,GWL_WNDPROC,offset _NewEditProc
		mov lpOldEditProc,eax
		
		invoke GetStockObject,NULL_BRUSH
		mov hBrushTr,eax
		
		jmp @F
		bitmapname:
		db 'back.bmp',0
		@@:
		invoke CreateFile,offset bitmapname,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax!=INVALID_HANDLE_VALUE
			mov ebx,eax
			invoke GlobalAlloc,GPTR,1500000
			mov lpBackBmp,eax
			invoke ReadFile,ebx,lpBackBmp,1500000,addr @opFileName,0
			invoke CloseHandle,ebx
			invoke _CreateDIBitmap,hwnd,lpBackBmp
			mov hBackBmp,eax
		.endif 
		
	.elseif eax==WM_CLOSE
		invoke _SaveOrNot
		.if eax
			invoke GlobalFree,lpFileBuff
			invoke GlobalFree,lpFileBuff2
			invoke GlobalFree,lpTemp
			invoke GlobalFree,lpLineRecords
			invoke CloseHandle,hFile
			invoke KillTimer,hwnd,ID_TIMER
			invoke EndDialog,hwnd,0
		.endif
	.else
		mov eax,FALSE
		ret
	.endif
	
	Ex:
	pop fs:[0]
	add esp,0ch
	mov eax,TRUE
	ret
	
_DlgMainProc endp
;
_SetText proc uses esi edi,hwnd
	invoke _UpdateWindow,IDC_LINE
	invoke SendDlgItemMessageW,hwnd,IDC_LISTO,LB_GETTEXT,ebx,offset szStringO
	invoke SendDlgItemMessageW,hwnd,IDC_LISTN,LB_GETTEXT,ebx,offset szStringN
;	invoke _StringBrief,offset szStringO
;	invoke _StringBrief,offset szStringN
	invoke SetDlgItemTextW,hwnd,IDC_EDITO,offset szStringO
	invoke SetDlgItemTextW,hwnd,IDC_EDITN,offset szStringN
;	invoke SendDlgItemMessageW,hwnd,IDC_EDITN,EM_SETSEL,0,-1
;	movzx esi,word ptr [szStringN]
;	.if esi==0300ch || esi==03010h
;		sub esp,4
;		mov eax,esp
;		invoke SendDlgItemMessageW,hwnd,IDC_EDITN,EM_GETSEL,0,eax
;		dec dword ptr [esp]
;		.if esi==0300ch
;			mov edi,1
;		.else
;			mov ax,3011h
;			lea edi,szStringN
;			or ecx,-1
;			@@:
;			repne scasw
;			cmp word ptr [edi],2fh
;			je @B
;			sub edi,offset szStringN
;			shr edi,1
;			inc edi
;		.endif
;		invoke SendDlgItemMessageW,hwnd,IDC_EDITN,EM_SETSEL,edi,[esp]
;		add esp,4
;	.endif
	inc ebx
	invoke SetDlgItemInt,hwnd,IDC_LINE2,ebx,FALSE
	
	invoke _UpdateWindow,hEditN
	invoke _UpdateWindow,hEditO
	invoke GetDlgItem,hwnd,IDC_EDITN
	invoke SetFocus,eax
	ret
_SetText endp
;
_AddToList proc hwnd
	local @lpEndOf1,@lpLineR,@bIsText
	assume fs:nothing
	push offset _SafeOfAdd
	push offset _AddHandler
	push fs:[0]
	mov fs:[0],esp
	mov bIsOpened,FALSE
	invoke EnableWindow,hBtnModify,FALSE
	invoke EnableWindow,hBtnSave,FALSE
	invoke ShowWindow,hStatusLine,SW_HIDE
	invoke ShowWindow,hBtnGoto,SW_HIDE
	invoke EnableWindow,hBtnFind,FALSE
	invoke EnableWindow,hBtnImp,FALSE
	invoke EnableWindow,hBtnExp,FALSE

	
	invoke SendMessage,hListN,LB_RESETCONTENT,0,0
	invoke SendMessage,hListO,LB_RESETCONTENT,0,0
	invoke GetFileSize,hFile,0
	push eax
	mov @lpEndOf1,eax
	mov ecx,lpFileBuff
	add @lpEndOf1,ecx
	mov edi,lpFileBuff2
	.if !dword ptr [edi]
		mov nFileSizeNew,eax
		mov esi,lpFileBuff
		mov ecx,eax
		call _memcpy
	.else
		invoke GetFileSize,hFile2,0
		mov nFileSizeNew,eax
	.endif
	pop edx
	shr edx,4
	invoke GlobalAlloc,GPTR,edx
	mov lpLineRecords,eax
	invoke SetDlgItemText,hwnd,IDC_WORKING,offset szAddingString
	mov edi,lpFileBuff
	mov eax,[edi+2]
	add edi,eax
	mov lpscOri,edi
	mov esi,lpFileBuff2
	add esi,eax
	mov lpscNew,esi
	mov eax,lpLineRecords
	mov @lpLineR,eax
	mov @bIsText,0
	
	.repeat
label23:
		.break .if edi>@lpEndOf1
		movzx eax,word ptr [edi]
		add edi,2
		movzx ecx,al
		shr eax,6
		.if ecx>6 || eax>0dch
			xor eax,eax
			mov dword ptr [eax],0
		.endif
		shl ecx,8
		add eax,ecx
		lea edx,ddTable
		add edx,eax
		mov eax,[edx]
		mov ebx,eax
		.while ebx
			movzx eax,bl
			.if eax<0f0h
				add edi,eax
			.elseif eax==0ffh
				mov ecx,[edi]
				lea edi,[edi+ecx+4]
			.elseif eax==0feh
				add edi,4
				cmp dword ptr [edi-4],0
				je @F
				invoke _StringToUnicode
				shl eax,1
				add eax,offset szTemp
				mov word ptr [eax],0
				invoke SendMessageW,hListO,LB_ADDSTRING,0,offset szTemp
				mov @bIsText,1
				add edi,dword ptr [edi-4]
			.endif
			@@:
			shr ebx,8
		.endw
		cmp @bIsText,0
		je label23
		mov @bIsText,0

label24:
		movzx eax,word ptr [esi]
		add esi,2
		movzx ecx,al
		shr eax,6
		.if ecx>6 || eax>0dch
			xor eax,eax
			mov dword ptr [eax],0
		.endif
		shl ecx,8
		add eax,ecx
		lea edx,ddTable
		add edx,eax
		mov eax,[edx]
		mov ebx,eax
		.while ebx
			movzx eax,bl
			.if eax<0f0h
				add esi,eax
			.elseif eax==0ffh
				mov ecx,[esi]
				lea esi,[esi+ecx+4]
			.elseif eax==0feh
				cmp dword ptr [esi],0
				jne labe3
				add esi,4
				jmp @F
				labe3:
				mov eax,@lpLineR
				mov [eax],esi
				add @lpLineR,4
				add esi,4
				push edi
				mov edi,esi
				invoke _StringToUnicode
				pop edi
				shl eax,1
				add eax,offset szTemp
				mov word ptr [eax],0
				invoke SendMessageW,hListN,LB_ADDSTRING,0,offset szTemp
				add esi,dword ptr [esi-4]
				mov @bIsText,1
			.endif
			@@:
			shr ebx,8
		.endw
		cmp @bIsText,0
		je label24
		mov @bIsText,0
	.until FALSE
	invoke SetDlgItemText,hwnd,IDC_WORKING,offset szConvertingString
	invoke _UpdateWindow,IDC_LINE
	mov esi,lpLineRecords
	xor ebx,ebx
	.while dword ptr [esi]
		mov edi,[esi]
		add edi,4
		invoke _StringToUnicode
		invoke WideCharToMultiByte,936,0,offset szTemp,eax,offset szStringO,512,0,0
		push eax
		invoke _RpcText,ebx,0,offset szStringO,eax,dword ptr [edi-4]
		pop dword ptr [edi-4]
		inc ebx
		add esi,4
	.endw
	mov nTotalLine,ebx
	invoke SetDlgItemText,hwnd,IDC_WORKING,offset szError+4
	invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_GETCOUNT,0,0
	mov nRealLine,eax
	invoke EnableWindow,hBtnModify,TRUE
	invoke EnableWindow,hBtnImp,TRUE
	invoke EnableWindow,hBtnExp,TRUE
	invoke EnableWindow,hBtnSave,TRUE
	invoke ShowWindow,hBtnGoto,SW_SHOW
	invoke ShowWindow,hStatusLine,SW_SHOW
	invoke EnableWindow,hBtnFind,TRUE
	invoke SendDlgItemMessage,hwnd,IDC_LISTN,LB_SETCURSEL,0,0
	invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_SETCURSEL,0,0
	xor ebx,ebx
	invoke _SetText,hwnd
	invoke _UpdateWindow,IDC_LINE
	invoke SetTimer,hWinMain,ID_TIMER,60000,NULL
	mov bIsOpened,TRUE
	jmp label3e
_SafeOfAdd:
	invoke SetDlgItemText,hwnd,IDC_WORKING,0
	invoke ShowWindow,hStatusLine,SW_SHOW
	invoke ShowWindow,hBtnGoto,SW_SHOW
	invoke _UpdateWindow,IDC_LINE
label3e:
	pop fs:[0]
	add esp,8
	ret
_AddToList endp
;

;

;
_ModifyOriText proc uses ebx,nListIndex,lpszNew
	mov eax,nListIndex
	mov edi,lpLineRecords
	lea edi,[edi+eax*4]
	mov esi,[edi]
	mov edi,lpszNew
	xor eax,eax
	or ecx,-1
	repnz scasb
	not ecx
	dec ecx
	invoke _RpcText,nListIndex,0,lpszNew,ecx,[esi]
	ret
_ModifyOriText endp
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
	add edx,4;
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
	mov esi,nFileSizeNew
	add esi,lpFileBuff2
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
	add dword ptr [edi],ebx
	sub edi,lpscNew
	add nFileSizeNew,ebx;
	mov esi,lpFileBuff2
	mov eax,[esi+6]
	or eax,eax
	jz @F
	add eax,esi
	add esi,11
	.while esi<eax
		.if dword ptr [esi+4]>edi
			add dword ptr [esi+4],ebx
		.endif
		add esi,09
	.endw
	.if dword ptr [esi+4]>edi
		add dword ptr [esi+4],ebx
	.endif
	add esi,13
	mov edx,lpFileBuff2
	mov eax,[edx+2]
	add eax,edx
	.while esi<eax
		.if dword ptr [esi+4]>edi
			add dword ptr [esi+4],ebx
		.endif
		add esi,09
	.endw
	@@:
	ret
_RpcText endp
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
;
_NewListProc proc uses ebx esi edi,hwnd,uMsg,wParam,lParam
	mov eax,uMsg
	.if eax==WM_MOUSEWHEEL
		invoke SendMessage,hListN,LB_GETTOPINDEX,0,0
		mov ebx,eax
		mov eax,wParam
		shr eax,31
		shl eax,1
		lea eax,[eax+eax*2]
		sub eax,3
		add ebx,eax
		invoke SendMessage,hListN,LB_SETTOPINDEX,ebx,0
		invoke SendMessage,hListO,LB_SETTOPINDEX,ebx,0
		invoke _UpdateWindow,hListN
		invoke _UpdateWindow,hListO
		ret
	.elseif eax==WM_VSCROLL
		.if !bScrolling
			mov eax,hwnd
			mov bScrolling,TRUE
			.if eax==hListO
				invoke SendMessage,hListN,WM_VSCROLL,wParam,lParam
			.else
				invoke SendMessage,hListO,WM_VSCROLL,wParam,lParam
			.endif
			mov bScrolling,FALSE
		.endif
	.endif
	invoke CallWindowProc,lpOldListProc,hwnd,uMsg,wParam,lParam
	push eax
	.if uMsg==WM_VSCROLL
		invoke _UpdateWindow,hListO
		invoke _UpdateWindow,hListN
	.endif
	pop eax
	ret
_NewListProc endp
;
_NewEditProc proc uses ebx esi edi,hwnd,uMsg,wParam,lParam
	.if bIsOpened
	mov eax,uMsg
	.if eax==WM_KEYDOWN
		mov eax,wParam
		.if eax==VK_RETURN
			invoke SendMessage,hWinMain,WM_COMMAND,IDC_MODIFY,0
		.elseif eax==VK_NEXT
			invoke GetDlgItemInt,hWinMain,IDC_LINE2,0,FALSE
			.if eax==nRealLine
				dec eax
			.endif
			@@:
			invoke SendDlgItemMessage,hWinMain,IDC_LISTN,LB_SETCURSEL,eax,0
			invoke SendMessage,hWinMain,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTN,hListN
			invoke _UpdateWindow,hEditN
			ret
		.elseif eax==VK_PRIOR
			invoke GetDlgItemInt,hWinMain,IDC_LINE2,0,FALSE
			sub eax,2
			.if eax==-1
				inc eax
			.endif
			jmp @B
		.elseif eax<2fh 
			invoke _UpdateWindow,hwnd
		.elseif eax==47h || eax==53h || eax==46h
			invoke GetKeyState,VK_CONTROL
			test eax,80h
			.if !ZERO?
				add wParam,2000h
				invoke SendMessage,hWinMain,WM_COMMAND,wParam,0
				ret
			.endif
;		.elseif eax==52h || eax==4eh
;			invoke GetKeyState,VK_SHIFT
;			test eax,80h
;			.if !ZERO?
;				
;			.endif
		.endif
	.elseif eax==WM_LBUTTONDOWN
		invoke _UpdateWindow,hwnd
	.elseif eax==WM_MOUSEWHEEL
		invoke SendMessage,hListO,WM_MOUSEWHEEL,wParam,lParam
		invoke _UpdateWindow,hListO
	.endif
	.endif
	invoke CallWindowProc,lpOldEditProc,hwnd,uMsg,wParam,lParam
	ret
_NewEditProc endp
;
_SaveOrNot proc
	.if bIsModified
		invoke MessageBox,hWinMain,offset szSaveOrNot,offset szDisplayName,MB_YESNOCANCEL or MB_DEFBUTTON1 or MB_ICONQUESTION
		.if eax==IDYES
			invoke SendMessage,hWinMain,WM_COMMAND,IDC_SAVE,0
		.elseif eax==IDCANCEL
			return FALSE
		.endif
	.endif
	mov eax,TRUE
	ret
_SaveOrNot endp
;
_UpdateWindow proc hwnd
	local @rect:RECT
	mov eax,hwnd
	.if eax==hListN
		invoke _UpdateWindow,IDC_LINE
		mov @rect.left,414
		mov @rect.top,27
		mov @rect.right,762
		mov @rect.bottom,395
	.elseif eax==hListO
		mov @rect.left,23
		mov @rect.top,27
		mov @rect.right,371
		mov @rect.bottom,395
	.elseif eax==hEditN
		mov @rect.left,68
		mov @rect.top,495
		mov @rect.right,720
		mov @rect.bottom,535
	.elseif eax==hEditO
		mov @rect.left,68
		mov @rect.top,432
		mov @rect.right,720
		mov @rect.bottom,472
	.elseif eax==IDC_LINE
		mov @rect.left,23
		mov @rect.top,397
		mov @rect.right,410
		mov @rect.bottom,422
	.endif
	invoke InvalidateRect,hWinMain,addr @rect,TRUE
	ret
_UpdateWindow endp
;
_GotoDlgProc proc uses ebx esi edi,hwnd,uMsg,wParam,lParam
	mov eax,uMsg
	.if eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.elseif eax==WM_COMMAND
		.if wParam==IDC_OK
			invoke GetDlgItemInt,hwnd,IDC_LINE,0,FALSE

			.if eax>nRealLine
				invoke MessageBox,hwnd,offset szLineOver,offset szDisplayName,MB_OK OR MB_ICONERROR
				jmp Ex1
			.endif
			dec eax
			invoke SendDlgItemMessage,hWinMain,IDC_LISTN,LB_SETCURSEL,eax,0
			invoke SendMessage,hWinMain,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTN,hListN
			invoke EndDialog,hwnd,0
		.endif

	.elseif eax==WM_INITDIALOG
		invoke GetDlgItemInt,hWinMain,IDC_LINE2,0,FALSE
		invoke SetDlgItemInt,hwnd,IDC_LINE,eax,FALSE
		invoke SendDlgItemMessage,hwnd,IDC_LINE,EM_SETSEL,0,-1
		invoke SetDlgItemInt,hwnd,IDC_TOTALLINE,nRealLine,FALSE
	.else
		xor eax,eax
		ret
	.endif
	Ex1:
	mov eax,TRUE
	ret
_GotoDlgProc endp
;
_OptionDlgProc proc uses ebx esi edi,hwnd,uMsg,wParam,lParam
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if eax==IDC_CANCEL
			invoke EndDialog,hwnd,0
		.endif
	.elseif eax==WM_INITDIALOG
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		xor eax,eax
		ret
	.endif
	mov eax,TRUE
	ret
_OptionDlgProc endp
;
_DisplayMsg proc lParam
	invoke SetDlgItemText,hWinMain,IDC_WORKING,offset szSaveSuccess
	invoke Sleep,2000
	invoke SetDlgItemText,hWinMain,IDC_WORKING,offset szError+4
	invoke _UpdateWindow,IDC_LINE
	ret
_DisplayMsg endp
;
_ReplaceText proc uses ebx
	local @nTotal,@nLenNew
	mov @nTotal,0
	lea edi,szToReplace
	xor eax,eax
	or ecx,-1
	repne scasb
	not ecx
	dec ecx
	mov @nLenNew,ecx
	lea edi,szToFind
	xor eax,eax
	or ecx,-1
	repne scasb
	not ecx
	mov ebx,ecx
	mov edx,lpLineRecords
	mov esi,[edx]
	.repeat
		add esi,4
		mov eax,esi
		.repeat
			mov esi,eax
			lea edi,szToFind
			mov ecx,ebx
			repe cmpsb
			.if !ecx
				pushad
				mov esi,edx
				sub esi,lpLineRecords
				shr esi,2
				sub eax,[edx]
				sub eax,4
				mov edi,edx
				dec ebx
				invoke _RpcText,esi,eax,offset szToReplace,@nLenNew,ebx
				inc @nTotal
				invoke SendDlgItemMessageW,hWinMain,IDC_LISTN,LB_DELETESTRING,esi,0
				mov edi,[edi]
				add edi,4
				invoke _StringToUnicode
				shl eax,1
				add eax,offset szTemp
				mov word ptr [eax],0
				invoke SendDlgItemMessageW,hWinMain,IDC_LISTN,LB_INSERTSTRING,esi,offset szTemp
				popad
				add eax,@nLenNew
				jmp @f
			.endif
			inc eax
			@@:
			mov esi,eax
			mov ecx,[edx]
			sub esi,ecx
			sub esi,4
		.until esi>=dword ptr [ecx]
		add edx,4
		mov esi,[edx]
	.until !esi
	mov eax,@nTotal
	ret
_ReplaceText endp
;
_ReplaceDlgProc proc uses ebx esi edi,hwnd,uMsg,wParam,lParam
	push ebp
	push offset Ex3
	push offset _ProcHandler
	push fs:[0]
	mov fs:[0],esp
	
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		movzx eax,ax
		.if ax==IDC_TOFIND
			mov eax,wParam
			shr eax,16
			.if eax==EN_CHANGE
				invoke GetDlgItem,hwnd,IDOK
				mov ebx,eax
				invoke GetDlgItem,hwnd,IDC_FINDLINE
				mov esi,eax
				invoke GetDlgItemText,hwnd,IDC_TOFIND,offset szToFind,64
				.if byte ptr [szToFind]
					invoke EnableWindow,ebx,TRUE
					invoke EnableWindow,esi,TRUE
				.else
					invoke EnableWindow,ebx,FALSE
					invoke EnableWindow,esi,FALSE
				.endif
			.endif
		.elseif ax==IDOK
			invoke GetDlgItemText,hwnd,IDC_TORPC,offset szToReplace,64
			INVOKE _ReplaceText
			mov ebx,eax
			invoke _UpdateWindow,hListN
			.if ebx
				mov bIsModified,TRUE
				invoke wsprintf,offset szReplaced,offset szRpcFormat,ebx
				invoke MessageBox,hwnd,offset szReplaced,offset szCaptionFind,MB_OK or MB_ICONINFORMATION
				invoke EndDialog,hwnd,0
			.else
				invoke MessageBox,hwnd,offset szTextNotFound,offset szCaptionFind,MB_OK or MB_ICONERROR
			.endif
		.elseif ax==IDC_FINDLINE
			invoke GetDlgItemInt,hWinMain,IDC_LINE2,0,0
			invoke _FindText,eax
			.if eax!=-1
				invoke SendDlgItemMessage,hWinMain,IDC_LISTN,LB_SETCURSEL,eax,0
				invoke SendMessage,hWinMain,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTN,hListN
				invoke EndDialog,hwnd,0
			.else
				invoke MessageBox,hwnd,offset szTextNotFound,offset szCaptionFind,MB_OK or MB_ICONERROR
			.endif
		.elseif ax==IDCANCEL
			invoke EndDialog,hwnd,0
		.endif
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		xor eax,eax
		ret
	.endif
Ex3:
	pop fs:[0]
	add esp,0ch
	mov eax,TRUE
	ret
_ReplaceDlgProc endp
;
_IsFormatted proc _hFile
	local @Buff[1024]:byte,@temp:dword
	invoke ReadFile, _hFile,addr @Buff,1024,addr @temp,0
	lea edi,@Buff
	.if word ptr [edi]
		xor eax,eax
		ret
	.endif
	mov ecx,dword ptr [edi+2]
	.if ecx>1000h || ecx<dword ptr [edi+6]
		xor eax,eax
		ret
	.endif
	
	mov eax,TRUE
	ret
_IsFormatted endp
;
_StringToUnicode proc uses edi
	push 256
	push offset szTemp
	push [edi-4]
	push edi
	push MB_PRECOMPOSED
	.while byte ptr [edi]<80h
		inc edi
		.if !byte ptr [edi]
			push 936
			jmp @F
		.endif
	.endw
	movzx eax,byte ptr [edi]
	.if eax<0a0h || (eax>=0e0h && eax<=0eah) || (eax>=0edh && eax<=0eeh)
		push 932
	.else
		push 936
	.endif
@@:
	call MultiByteToWideChar
	ret
_StringToUnicode endp

_FindText proc cLine
	lea edi,szToFind
	xor eax,eax
	or ecx,-1
	repne scasb
	not ecx
	mov ebx,ecx
	mov ecx,cLine
	mov edx,lpLineRecords
	lea edx,[edx+ecx*4+4]
	mov esi,[edx]
	or esi,esi
	je @F
	.repeat
		add esi,4
		mov eax,esi
		.repeat
			mov esi,eax
			lea edi,szToFind
			mov ecx,ebx
			repe cmpsb
			.if !ecx
				sub edx,lpLineRecords
				shr edx,2
				mov eax,edx
				ret
			.endif
			inc eax
			mov esi,eax
			mov ecx,[edx]
			sub esi,ecx
			sub esi,4
		.until esi>=dword ptr [ecx]
		add edx,4
		mov esi,[edx]
	.until !esi
	@@:
	mov eax,-1
	ret
_FindText endp
;
_ExportTxt proc uses edi esi ebx,_lpLines,_hFile
	local @lpTxtBuffer
	mov edi,_lpLines
	xor ecx,ecx
	.if !dword ptr [edi]
		xor eax,eax
		ret
	.endif
	.while dword ptr [edi]
		inc ecx
		add edi,4
	.endw
	shl ecx,8
	invoke GlobalAlloc,GPTR,ecx
	mov @lpTxtBuffer,eax
	mov edi,eax
	mov ebx,_lpLines
	mov esi,[ebx]
	.repeat
		mov ecx,[esi]
		add esi,4
		.if ecx>0ffh
			xor eax,eax
			ret
		.endif
;		mov edx,ecx
		mov eax,edi
		rep movsb
;		mov ecx,edx
;		mov edx,edi
;		mov edi,eax
;		@@:
;			inc edi
;			cmp byte ptr [edi],0
;			je @F
;		loop @B
;		mov edi,edx
;		@@:
		mov ax,0a0dh
		stosw
		add ebx,4
		mov esi,[ebx]
	.until !esi
	sub edi,@lpTxtBuffer
	invoke SetFilePointer,_hFile,0,0,FILE_BEGIN
	invoke WriteFile,_hFile,@lpTxtBuffer,edi,offset dwTemp,0
	invoke SetEndOfFile,_hFile
	mov eax,1
	ret
_ExportTxt endp
;
_ImportTxt proc uses esi edi ebx,_lpTxt
	xor ebx,ebx
	mov edi,_lpTxt
	.while ebx<nRealLine
		mov esi,lpLineRecords
		lea esi,[esi+ebx*4]
		mov eax,[esi]
		mov ecx,[eax]
		mov eax,edi
		.while word ptr [edi]!=0a0dh
			inc edi
			.if !dword ptr [edi]
				xor eax,eax
				ret
			.endif
		.endw
		mov byte ptr [edi],0
		mov esi,edi
		sub edi,eax
		pushad
		push eax
		invoke SendDlgItemMessageA,hWinMain,IDC_LISTN,LB_DELETESTRING,ebx,0
		push ebx
		push LB_INSERTSTRING
		push IDC_LISTN
		push hWinMain
		call SendDlgItemMessageA
		popad
		invoke _RpcText,ebx,0,eax,edi,ecx
		mov edi,esi
		add edi,2
		inc ebx
	.endw
	mov bIsModified,TRUE
	invoke SendMessage,hWinMain,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTN,hListN
	mov eax,1
	ret
_ImportTxt endp
;

end start






