.486
.model flat,stdcall
option casemap:none


include lnedit.inc
include _CreateDIBitmap.asm
include ExcepHandler.asm
include trbutton.asm

.code

start proc
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax

invoke _CreateTrButton,hInstance

invoke DialogBoxParamW,hInstance,DLG_MAIN,0,offset _DlgMainProc,NULL
invoke ExitProcess,0

start endp
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	local @dwTemp,@dwTemp2
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
			invoke _Encrypt,lpFileBuff2,nFileSizeNew
			invoke WriteFile,hFile2,lpFileBuff2,nFileSizeNew,addr @opFileName,0
			invoke SetEndOfFile,hFile2
			invoke _Encrypt,lpFileBuff2,nFileSizeNew
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
			invoke lstrcpy,offset szNewName,offset szFileName
			invoke lstrcat,offset szNewName,offset szGang
			invoke lstrcat,offset szNewName,offset szDisplayName
			invoke SetWindowText,hwnd,offset szNewName
			invoke GlobalFree,lpFileBuff2
			invoke GlobalFree,lpFileBuff
			invoke GlobalFree,lpLineRecords2
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
			sub esi,4000h
			invoke _Encrypt,lpFileBuff,esi
			sub edi,4000h
			mov eax,lpFileBuff2
			.if dword ptr [eax]
				invoke _Encrypt,lpFileBuff2,edi
			.endif
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
			invoke _ExportTxt,lpLineRecords2,eax
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
	
	.elseif eax==WM_DRAWITEM
		mov esi,lParam
		assume esi:ptr DRAWITEMSTRUCT
		.if [esi].CtlType==ODT_BUTTON
			invoke _DrawButton,esi
		.endif
		assume esi:nothing
		
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
		
;		invoke GetDlgItem,hwnd,IDC_OPEN
;		invoke SetWindowLong,eax,GWL_WNDPROC,offset _NewButtonProc
;		mov lpOldButtonProc,eax
		
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
		
		invoke RegCreateKeyEx,HKEY_LOCAL_MACHINE,offset szRegKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,offset hRegKey,addr @dwTemp
		.if eax==ERROR_SUCCESS
			.if @dwTemp==REG_CREATED_NEW_KEY
				invoke RegSetValueEx,hRegKey,offset szRegValueVersion,0,REG_DWORD,offset nVersion,4
			.else
				invoke RegQueryValueEx,hRegKey,offset szRegValueVersion,0,addr @dwTemp2,addr @dwTemp,offset nDword
				mov eax,@dwTemp
				.if eax<=nVersion
					invoke RegSetValueEx,hRegKey,offset szRegValueVersion,0,REG_DWORD,offset nVersion,4
				.else
					invoke MessageBox,hwnd,offset szNotNewest,offset szDisplayName,MB_ICONINFORMATION or MB_OK
				.endif
			.endif
			invoke RegCloseKey,hRegKey
		.endif
		
		
	.elseif eax==WM_CLOSE
		invoke _SaveOrNot
		.if eax
			invoke GlobalFree,lpFileBuff
			invoke GlobalFree,lpFileBuff2
			invoke GlobalFree,lpTemp
			invoke GlobalFree,lpLineRecords2
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
	invoke SendDlgItemMessageW,hwnd,IDC_EDITN,EM_SETSEL,0,-1
	lea esi,szStringN
	.if word ptr [esi]==5eh
;		xor edi,edi
;		.repeat
;			inc edi
;			add esi,2
;		.until word ptr [esi]==3011h
;		add edi,2
		push eax
		invoke SendDlgItemMessageW,hwnd,IDC_EDITN,EM_GETSEL,0,esp
		pop eax
		invoke SendDlgItemMessageW,hwnd,IDC_EDITN,EM_SETSEL,5,eax
	.endif
	inc ebx
	invoke SetDlgItemInt,hwnd,IDC_LINE2,ebx,FALSE
	
	invoke _UpdateWindow,hEditN
	invoke _UpdateWindow,hEditO
	invoke GetDlgItem,hwnd,IDC_EDITN
	invoke SetFocus,eax
	ret
_SetText endp
;
_AddToList proc uses ebx esi edi hwnd
	local @lpEndOf1,@lpLineR,@bFirst
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
	invoke SetDlgItemText,hwnd,IDC_WORKING,offset szAddingString
	invoke _UpdateWindow,IDC_LINE
	
	invoke GetFileSize,hFile,0
	mov @lpEndOf1,eax
	mov ecx,lpFileBuff
	add @lpEndOf1,ecx
	mov edi,lpFileBuff2
	.if !dword ptr [edi]
		mov nFileSizeNew,eax
		mov esi,lpFileBuff
		mov ecx,eax
		call _memcpy
		mov @bFirst,1
	.else
		invoke GetFileSize,hFile2,0
		mov nFileSizeNew,eax
		mov @bFirst,0
	.endif
	mov edi,lpFileBuff
	mov edx,[edi+0ch]
	add edx,100
	invoke GlobalAlloc,GPTR,edx
	mov lpLineRecords2,eax
	mov eax,edi
	add eax,dword ptr [edi+8]
	add eax,dword ptr [edi+4]
	mov lpIndex,eax
	mov ecx,[edi+0ch]
	add eax,ecx
	shr ecx,2
	mov nIndex,ecx
	mov edi,eax
	
	mov esi,lpFileBuff2
	mov eax,esi
	add eax,dword ptr [esi+8]
	add eax,dword ptr [esi+4]
	mov lpIndex2,eax
	mov ecx,[esi+0ch]
	add eax,ecx
	mov esi,eax
	mov lpText2,eax
	mov eax,lpLineRecords2
	mov @lpLineR,eax
	
	xor ebx,ebx
	.while ebx<nIndex
		mov eax,lpIndex
		mov eax,[eax+ebx*4]
		add eax,edi
		.if byte ptr [eax]
			invoke _StringToUnicode
			invoke SendDlgItemMessageW,hwnd,IDC_LISTO,LB_ADDSTRING,0,offset szTemp
		.endif
		
		mov eax,lpIndex2
		mov eax,[eax+ebx*4]
		add eax,esi
		.if byte ptr [eax]
			mov ecx,@lpLineR
			mov [ecx],eax
			add @lpLineR,4
			.if @bFirst
				invoke _StringToUnicode
			.else
				invoke MultiByteToWideChar,936,MB_PRECOMPOSED,eax,-1,offset szTemp,256
			.endif
			invoke SendDlgItemMessageW,hwnd,IDC_LISTN,LB_ADDSTRING,0,offset szTemp
		.endif
		
		inc ebx
	.endw
	
	invoke SetDlgItemText,hwnd,IDC_WORKING,offset szConvertingString
	invoke _UpdateWindow,IDC_LINE
	mov esi,lpLineRecords2
	xor ebx,ebx
	.if @bFirst
		.while dword ptr [esi]
			mov eax,[esi]
			invoke _StringToUnicode
			invoke WideCharToMultiByte,936,0,offset szTemp,eax,offset szStringO,512,0,0
			push eax
			invoke lstrlen,[esi]
			pop ecx
			inc eax
			invoke _RpcText,ebx,0,offset szStringO,ecx,eax
			inc ebx
			add esi,4
		.endw
	.else
		.while dword ptr [esi]
			inc ebx
			add esi,4
		.endw
	.endif
	mov nTotalLine,ebx
	invoke SetDlgItemText,hwnd,IDC_WORKING,offset szError+4
	invoke SendDlgItemMessage,hwnd,IDC_LISTO,LB_GETCOUNT,0,0
	mov nRealLine,eax
	invoke EnableWindow,hBtnModify,TRUE
	invoke EnableWindow,hBtnSave,TRUE
	invoke ShowWindow,hBtnGoto,SW_SHOW
	invoke ShowWindow,hStatusLine,SW_SHOW
	invoke EnableWindow,hBtnFind,TRUE
;	invoke EnableWindow,hBtnImp,TRUE
;	invoke EnableWindow,hBtnExp,TRUE
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
	mov edi,lpLineRecords2
	lea edi,[edi+eax*4]
	mov edi,[edi]
	or ecx,-1
	xor eax,eax
	repne scasb
	not ecx
	push ecx
	mov edi,lpszNew
	xor eax,eax
	or ecx,-1
	repne scasb
	not ecx
	push ecx
	push lpszNew
	push 0
	push nListIndex
	call _RpcText
	ret
_ModifyOriText endp
;
_RpcText proc uses esi ebx edi,nLine,nLineOffset,lpszNew,nLenNew,nLenOri
	mov ecx,nLenNew
	sub ecx,nLenOri
	push ecx
	mov esi,lpLineRecords2
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
	mov esi,lpLineRecords2
	mov eax,nLine
	mov eax,[esi+eax*4]
	sub eax,lpText2
	mov esi,lpIndex2
	xor ebx,ebx
	.while ebx<nIndex
		.if eax<dword ptr [esi+ebx*4]
			add dword ptr [esi+ebx*4],ecx
		.endif
		inc ebx
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
	add nFileSizeNew,ebx;
	mov esi,lpFileBuff2
	add dword ptr [esi+10h],ebx
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
	mov edx,lpLineRecords2
	mov esi,[edx]
	.repeat
		mov eax,esi
		.repeat
			mov esi,eax
			lea edi,szToFind
			mov ecx,ebx
			repe cmpsb
			.if !ecx
				pushad
				mov esi,edx
				sub esi,lpLineRecords2
				shr esi,2
				sub eax,[edx]
				mov edi,edx
				dec ebx
				invoke _RpcText,esi,eax,offset szToReplace,@nLenNew,ebx
				inc @nTotal
				invoke SendDlgItemMessageW,hWinMain,IDC_LISTN,LB_DELETESTRING,esi,0
				mov eax,[edi]
				invoke _StringToUnicode
				
				invoke SendDlgItemMessageW,hWinMain,IDC_LISTN,LB_INSERTSTRING,esi,offset szTemp
				popad
				
				add eax,@nLenNew
				jmp @f
			.endif
			inc eax
			@@:
		.until !byte ptr [eax]
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
				invoke GetDlgItemText,hwnd,IDC_TOFIND,offset szToFind,64
				.if byte ptr [szToFind]
					invoke EnableWindow,ebx,TRUE
				.else
					invoke EnableWindow,ebx,FALSE
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
;	local @Buff:dword,@temp:dword
;	invoke ReadFile, _hFile,addr @Buff,2,addr @temp,0
;	.if @Buff!=0feffh
;		xor eax,eax
;		ret
;	.endif
	mov eax,TRUE
	ret
_IsFormatted endp
;
_StringToUnicode proc
	push 256
	push offset szTemp
	push -1
	push eax
	push MB_PRECOMPOSED
	@@:
	mov cl,byte ptr [eax]
	.if cl<80h
		or cl,cl
		je @F
		inc eax
		jmp @B
	.elseif (cl<0a0h || cl>=0e0h && cl<=0eah || cl>=0edh) 
		push 932
	.else
	@@:
		push 936
	.endif
	call MultiByteToWideChar
	ret
_StringToUnicode endp
;
_Encrypt proc uses edi,_lpBuffer,_nSize
;	mov edi,_lpBuffer
;	mov ecx,_nSize
;	@@:
;		xor byte ptr [edi],0ffh
;		inc edi
;	loop @B
	ret
_Encrypt endp
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
		movzx ecx,byte ptr [esi-1]
		.if !ecx
			movzx ecx,byte ptr [esi-6]
		.endif
		.if ecx>0ffh
			xor eax,eax
			ret
		.endif
		mov edx,ecx
		mov eax,edi
		rep movsb
		mov ecx,edx
		mov edx,edi
		mov edi,eax
		@@:
			inc edi
			cmp byte ptr [edi],0
			je @F
		loop @B
		mov edi,edx
		@@:
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
		mov esi,lpLineRecords2
		lea esi,[esi+ebx*4]
		mov eax,[esi]
		call _slstrlen
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
		inc edi
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
_slstrlen proc
	movzx ecx,byte ptr [eax-1]
	.if !ecx
		movzx ecx,byte ptr [eax-6]
	.endif
	ret
_slstrlen endp

end start






