.code

;
_CreateMainWindow proc _hInst
	local @stMsg:MSG

	.if hWinMain
		mov eax,1
		ret
	.endif
	invoke CreateDialogParam,_hInst,IDD_MAINDLG,0,offset _MainWndProc,0
	.if !eax
		inc eax
		ret
	.endif
	mov hWinMain,eax
	invoke ShowWindow,hWinMain,SW_SHOW
	
	.while TRUE
		invoke GetMessage,addr @stMsg,NULL,0,0
		.break .if eax==0
		invoke IsDialogMessage,hWinMain,addr @stMsg
		.if !eax
			invoke TranslateMessage,addr @stMsg
			invoke DispatchMessage,addr @stMsg
		.endif
	.endw

	xor eax,eax
	ret
_CreateMainWindow endp

;
_CreateListWindow proc _hInst
	local @stMsg:MSG

	.if hWinList
		mov eax,1
		ret
	.endif
	invoke CreateDialogParam,_hInst,IDD_LIST,0,offset _ListDlgProc,0
	.if !eax
		inc eax
		ret
	.endif
	mov hWinList,eax
	
	.while TRUE
		invoke GetMessage,addr @stMsg,NULL,0,0
		.break .if eax==0
		invoke IsDialogMessage,hWinList,addr @stMsg
		.if !eax
			invoke TranslateMessage,addr @stMsg
			invoke DispatchMessage,addr @stMsg
		.endif
	.endw

	xor eax,eax
	ret
_CreateListWindow endp

;
_MainWndProc proc uses edi esi ebx hwnd,uMsg,wParam,lParam
	LOCAL @nCurLine,@bHave45h
	LOCAL @szPath[128]:byte,@szPath2[128]:byte
	LOCAL @CB[64]:byte

	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_MODIFY
			invoke _GetLineFromOffset,nCurOffset
			dec eax
			invoke _GetPointerFromLine,eax
			.if word ptr [eax]!=51h
				invoke SetWindowText,hStatusMain,offset szNotMatch
				jmp MWndEx
			.endif
			mov ebx,[eax+22]
			sub esp,512
			mov eax,esp
			mov byte ptr [eax],0
			invoke GetWindowText,hEditMain,eax,512
			.if !byte ptr [esp]
				invoke SetWindowText,hStatusMain,offset szCantGetText
				add esp,512
				jmp MWndEx
			.endif
			invoke _ModifyText,ebx,esp
			add esp,512
			.if !eax
				invoke SetWindowText,hStatusMain,offset szNoMemory2
			.endif
			
		.elseif ax==IDC_INSCUR
			invoke _GetLineFromOffset,nCurOffset
			mov ebx,eax
			inc ebx
			invoke _GetPointerFromLine,ebx
			mov esi,eax
			sub ebx,2
			invoke _GetPointerFromLine,ebx
			.if word ptr [eax]!=42h || word ptr [esi]!=43h
				invoke SetWindowText,hStatusMain,offset szNoVoice
				jmp MWndEx
			.endif
			
			mov esi,[eax+2]
			sub esp,512
			mov eax,esp
			mov byte ptr [eax],0
			invoke GetWindowText,hEditMain,eax,512
			.if !byte ptr [esp]
				invoke SetWindowText,hStatusMain,offset szCantGetText
				add esp,512
				jmp MWndEx
			.endif
			add ebx,2
			invoke _DelLine,ebx
			sub ebx,2
			invoke _DelLine,ebx
			
			lea edi,@CB
			xor eax,eax
			mov ecx,16
			rep stosd
			mov word ptr [edi],51h
			mov eax,esp
			invoke _InsertString,eax
			add esp,512
			.if !eax
				invoke SetWindowText,hStatusMain,offset szInsertErr
				jmp MWndEx
			.endif
			mov [edi+22],eax
			mov [edi+6],esi
			mov dword ptr [edi+26],1
			
			invoke _AddLine,ebx,addr @CB,0
			.if !eax
				invoke SetWindowText,hStatusMain,offset szAddFailed
				jmp MWndEx
			.endif
			invoke SetWindowText,hStatusMain,offset szAddSuccess
			invoke EnableWindow,hInsprevMain,FALSE
			mov bListChanged,1
			
		.elseif eax==IDC_INSPREV
			invoke _GetLineFromOffset,nCurOffset
			lea edi,[eax-2]
			lea esi,[eax-1]
			invoke _GetPointerFromLine,edi
			mov @bHave45h,0
			.if word ptr [eax]!=42h
				.if word ptr [eax]==45h
					mov @bHave45h,1				
				.endif
				dec edi
				invoke _GetPointerFromLine,edi
				.if word ptr [eax]!=42h
					invoke SetWindowText,hStatusMain,offset szNoVoice
					jmp MWndEx
				.endif
			.endif
			mov ebx,[eax+2]
			sub esp,512
			mov eax,esp
			mov byte ptr [eax],0
			invoke GetWindowText,hEditMain,eax,512
			.if !byte ptr [esp]
				invoke SetWindowText,hStatusMain,offset szCantGetText
				add esp,512
				jmp MWndEx
			.endif
			invoke _DelLine,edi
			.if @bHave45h
				invoke _DelLine,edi
			.endif
			
			mov esi,edi
			lea edi,@CB
			xor eax,eax
			mov ecx,16
			rep stosd
			lea edi,@CB
			mov word ptr [edi],51h
			mov eax,esp
			invoke _InsertString,eax
			add esp,512
			.if !eax
				invoke SetWindowText,hStatusMain,offset szInsertErr
				jmp MWndEx
			.endif
			mov [edi+22],eax
			mov [edi+6],ebx
			mov dword ptr [edi+26],1
			
			invoke _AddLine,esi,addr @CB,0
			.if !eax
				invoke SetWindowText,hStatusMain,offset szAddFailed
				jmp MWndEx
			.endif
			invoke SetWindowText,hStatusMain,offset szAddSuccess
			invoke EnableWindow,hInsprevMain,FALSE
			mov bListChanged,1
			
		.elseif ax==IDC_MORE
			invoke ShowWindow,hwnd,SW_HIDE
			invoke ShowWindow,hWinList,SW_SHOW
			invoke PostMessage,hWinList,WM_REFRESHLIST,0,0
			
		.elseif ax==IDC_SAVE
			invoke _SaveGsc
			.if eax
				invoke SetWindowText,hStatusMain,offset szSaveSuccess
			.else
				invoke SetWindowText,hStatusMain,offset szSaveFailed
			.endif
		.endif
		
	.elseif eax==WM_STREAMBYTE
		mov eax,wParam
		.if eax==51h 
			invoke _EnableMainAll
			invoke SetWindowText,hStatusMain,offset ANull
			invoke _GetLineFromOffset,lParam
			.if eax==-1
				invoke SetWindowText,hStatusMain,offset szNotMatch2
				invoke _DisableMainAll
				invoke EnableWindow,hMoreMain,TRUE
				jmp MWndEx
			.endif
			mov @nCurLine,eax
			mov eax,lParam
			add eax,sGscInfo.lpControlStream
			add eax,22
			mov ecx,[eax]
			mov edi,sGscInfo.lpIndex
			lea esi,[edi+ecx*4]
			mov ecx,[esi]
			add ecx,sGscInfo.lpText			
			invoke SetWindowText,hEditMain,ecx
			
			mov eax,@nCurLine
			dec eax
			invoke _GetPointerFromLine,eax
			mov bx,[eax]
			.if bx!=45h
				invoke EnableWindow,hInsprevMain,FALSE
			.endif
			.if bx!=42h
				invoke EnableWindow,hInscurMain,FALSE
			.endif

		.elseif eax==0eh
			invoke _EnableMainAll
			invoke SetWindowText,hStatusMain,offset ANull
			invoke _GetLineFromOffset,lParam
			.if eax==-1
				invoke SetWindowText,hStatusMain,offset szNotMatch2
				invoke _DisableMainAll
				invoke EnableWindow,hMoreMain,TRUE
				jmp MWndEx
			.endif
			sub esp,512
			mov edi,esp
			mov @nCurLine,eax
			mov esi,lParam
			add esi,sGscInfo.lpControlStream
			add esi,4
			invoke _GetTextByIdx,[esi]
			invoke lstrcpy,edi,eax
			add esi,24
			mov ebx,5
			.while ebx && dword ptr [esi]
				invoke lstrcat,edi,offset szEnter
				invoke _GetTextByIdx,[esi]
				invoke lstrcat,edi,eax
				dec ebx
				add esi,4
			.endw
			invoke SetWindowText,hEditMain,edi
			add esp,512
			
			mov eax,@nCurLine
			dec eax
			invoke _GetPointerFromLine,eax
			mov ebx,[eax]
			.if ebx!=45h
				invoke EnableWindow,hInsprevMain,FALSE
			.endif
			.if ebx!=42h
				invoke EnableWindow,hInscurMain,FALSE
			.endif
		.endif
		
	.elseif eax==WM_PREGSCERR
		mov eax,wParam
		.if eax==0
			invoke SetWindowText,hStatusMain,offset szFileOprFail
		.elseif eax==1
			invoke SetWindowText,hStatusMain,offset szNoMemory
		.elseif eax==2
			invoke SetWindowText,hStatusMain,offset szPreReadErr
		.elseif eax==10
			invoke SetWindowText,hStatusMain,offset szPreReadErr
		.elseif eax==11
			invoke SetWindowText,hStatusMain,offset szPreReadErr
		.endif
		invoke _DisableMainAll
		mov bFatalErr,1
		
	.elseif eax==WM_SAVEERR
	
	.elseif eax==WM_NOTAVBGSC
		invoke SetWindowText,hStatusMain,offset szNotAvbGsc
		invoke _DisableMainAll
		
	.elseif eax==WM_STREAMERR
		mov eax,wParam
		.if eax==0
			invoke SetWindowText,hStatusMain,offset szNotMatch
		.elseif eax==1
			invoke SetWindowText,hStatusMain,offset szNotMatch2
		.endif
		mov bFatalErr,1
		invoke _DisableMainAll
		invoke EnableWindow,hMoreMain,TRUE
		
	.elseif eax==WM_INITDIALOG
		invoke GetDlgItem,hwnd,IDC_MODIFY
		mov hModifyMain,eax
		invoke GetDlgItem,hwnd,IDC_EDIT
		mov hEditMain,eax
		invoke GetDlgItem,hwnd,IDC_STATUS
		mov hStatusMain,eax
		invoke GetDlgItem,hwnd,IDC_INSCUR
		mov hInscurMain,eax
		invoke GetDlgItem,hwnd,IDC_INSPREV
		mov hInsprevMain,eax
		invoke GetDlgItem,hwnd,IDC_MORE
		mov hMoreMain,eax
		invoke GetDlgItem,hwnd,IDC_SAVE
		mov hSaveMain,eax
		
		invoke GetCurrentDirectory,128,addr @szPath
		invoke _ConnectGscPathA,addr @szPath,offset szConfigFile
		invoke CreateFile,addr @szPath,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax==-1
			invoke _BrowseFolder,0,addr @szPath2,offset szSelectNewPos
			.if !eax
				invoke ExitProcess,0
			.endif
			invoke lstrcpy,offset szNewGscFolder,addr @szPath2
			invoke WritePrivateProfileString,offset szSecName,offset szKeyName,addr @szPath2,addr @szPath
		.else
			invoke CloseHandle,eax
			invoke GetPrivateProfileString,offset szSecName,offset szKeyName,offset ANull,offset szNewGscFolder,128,addr @szPath
		.endif
		
	.elseif eax==WM_CLOSE
		invoke ShowWindow,hwnd,SW_HIDE
		
	.elseif eax==WM_DESTROY
		mov hWinMain,0
	
	.else
		xor eax,eax
		ret
	.endif
MWndEx:
	mov eax,1
	ret

_MainWndProc endp

;
_ListDlgProc proc uses edi esi ebx hwnd,uMsg,wParam,lParam
	LOCAL @szTemp[MAX_LEN]:byte,@bin[MAX_LEN]:byte
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_LISTDLG
			shr eax,16
			.if ax==LBN_SELCHANGE
				invoke SendMessage,hListList,LB_GETCURSEL,0,0
				invoke _GetPointerFromLine,eax
				mov ebx,eax
				mov ax,[eax]
				.if ax==42h || (ax==51h && !dword ptr [ebx+6])
					invoke EnableWindow,hPlayList,TRUE
				.else
					invoke EnableWindow,hPlayList,FALSE				
				.endif
				mov ax,[ebx]
				.if (ax>=03 && ax<=05) || ax==0eh
					invoke EnableWindow,hFollowList,TRUE
				.else
					invoke EnableWindow,hFollowList,FALSE
				.endif
				invoke _Bin2Scr,ebx,addr @szTemp
				invoke SetWindowText,hEditList,addr @szTemp
			.endif
			
		.elseif ax==IDC_INSERT
			invoke GetWindowText,hEditList,addr @szTemp,512
			lea edi,@szTemp
			mov al,[edi]
			or al,al
			je _ListEx
			.if al=='!'
				invoke SetWindowText,hStatusList,offset szCantAddSel
				jmp _ListEx
			.endif
			invoke _Scr2Bin,addr @szTemp,addr @bin,TO_INSERT
			.if !eax
				invoke SetWindowText,hStatusList,offset szWrongScr
				jmp _ListEx
			.endif
			invoke SendMessage,hListList,LB_GETCURSEL,0,0
			mov ebx,eax
			invoke _AddLine,ebx,addr @bin,0
			invoke _TranslateCB,addr @bin,addr @szTemp
			invoke SendMessage,hListList,LB_INSERTSTRING,ebx,addr @szTemp
		
		.elseif ax==IDC_MODF
			invoke GetWindowText,hEditList,addr @szTemp,512
			Lea edi,@szTemp
			.if !byte ptr [edi]
				jmp _ListEx
			.endif
			invoke SendMessage,hListList,LB_GETCURSEL,0,0
			mov ebx,eax
			invoke _GetPointerFromLine,ebx
			mov esi,eax
			lea edi,@szTemp
			mov al,[edi]
			.if al=='!'
				.if word ptr [esi]!=0eh
					invoke SetWindowText,hStatusList,offset szCantAddSel
					jmp _ListEx
				.endif
				inc edi
				mov ecx,edi
				.while byte ptr [edi]
					.if word ptr [edi]==0a0dh
						mov byte ptr [edi],0
						add edi,2
						.break
					.endif
					inc edi
				.endw
				invoke _ModifyText,[esi+4],ecx
				add esi,28
				mov ebx,5
				.while ebx
					lodsd
					.if !byte ptr [edi]
						invoke SetWindowText,hStatusList,offset szSelCntErr
					.endif
					mov ecx,edi
					.while byte ptr [edi]
						.if word ptr [edi]==0a0dh
							mov byte ptr [edi],0
							add edi,2
							.break
						.endif
						inc edi
					.endw
					invoke _ModifyText,eax,ecx
					dec ebx
				.endw
			.elseif al!='@' && word ptr [esi]==51h
				invoke _Scr2Bin,addr @szTemp,addr @bin,NOT_INSERT
				lea edi,@bin
				mov eax,[edi+6]
				mov [esi+6],eax
				lea edi,@szTemp
				.if byte ptr [edi]=='#'
					mov ecx,7
					@@:
						inc edi
						cmp byte ptr [edi],' '
						je @F
					loop @B
					invoke SetWindowText,hStatusList,offset szWrongScr
					jmp _ListEx
					@@:
					inc edi
				.endif
				invoke _ModifyText,[esi+22],edi
			.else
				invoke _Scr2Bin,addr @szTemp,addr @bin,TO_INSERT
				.if !eax
					invoke SetWindowText,hStatusList,offset szWrongScr
					jmp _ListEx
				.endif
				invoke _DelLine,ebx
				invoke SendMessage,hListList,LB_DELETESTRING,ebx,0
				invoke _AddLine,ebx,addr @bin,0
				invoke _TranslateCB,addr @bin,addr @szTemp
				invoke SendMessage,hListList,LB_INSERTSTRING,ebx,addr @szTemp
			.endif
			invoke SendMessage,hwnd,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTDLG,hListList

		
		.elseif ax==IDC_DEL
			invoke SendMessage,hListList,LB_GETCURSEL,0,0
			mov ebx,eax
			invoke _DelLine,eax
			invoke SendMessage,hListList,LB_DELETESTRING,ebx,0
			invoke SendMessage,hListList,LB_SETCURSEL,ebx,0
			invoke SendMessage,hwnd,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTDLG,hListList
			
		.elseif ax==IDC_PLAYVOICE
			invoke SendMessage,hListList,LB_GETCURSEL,0,0
			invoke _GetPointerFromLine,eax
			.if word ptr [eax]==42h
				mov ecx,[eax+2]
			.elseif
				mov ecx,[eax+6]
			.endif
			mov ebx,418ef0h
			xor eax,eax
			push eax
			push eax
			push eax
			push ecx
			call ebx
			
		.elseif ax==IDC_FOLLOW
			invoke SendMessage,hListList,LB_GETCURSEL,0,0
			invoke _GetPointerFromLine,eax
			mov esi,eax
			lodsw
			.if ax>=03 && ax<=05
				invoke _GetLineFromOffset,[esi]
				.if eax==-1
					invoke SetWindowText,hStatusList,offset szZErr
					jmp _ListEx
				.endif
				invoke SendMessage,hListList,LB_SETCURSEL,eax,0 
				invoke SendMessage,hwnd,WM_COMMAND,LBN_SELCHANGE*65536+IDC_LISTDLG,hListList
			.elseif ax==0eh
				invoke DialogBoxParam,hInstance,IDD_FOLLOWSEL,hwnd,offset _FollowDlgProc,esi
				or eax,eax
				je _ListEx
				add esi,6
				mov eax,[esi+eax*4-4]
				invoke _GetLineFromOffset,eax
				.if eax==-1
					invoke SetWindowText,hStatusList,offset szZErr
					jmp _ListEx
				.endif
				invoke SendMessage,hListList,LB_SETCURSEL,eax,0
			.endif
			
		.elseif ax==IDC_SETCUR
			invoke SendMessage,hListList,LB_GETCURSEL,0,0
			inc eax
			invoke _GetPointerFromLine,eax
			sub eax,sGscInfo.lpControlStream
			mov nCurOffset,eax
			invoke SetWindowText,hStatusMain,0
			mov bFatalErr,0
			
		.elseif ax==IDC_GOTOCUR
			invoke _GetLineFromOffset,nCurOffset
			dec eax
			invoke SendMessage,hListList,LB_SETCURSEL,eax,0
			
		.elseif ax==IDC_RET
			invoke ShowWindow,hwnd,SW_HIDE
			invoke ShowWindow,hWinMain,SW_SHOW
			invoke SetActiveWindow,hWinMain
		.endif
		
	.elseif eax==WM_REFRESHLIST
		invoke SendMessage,hListList,LB_GETCOUNT,0,0
		.if bListChanged || !eax
			invoke CreateThread,0,0,offset _RefreshList,0,0,0
			invoke CloseHandle,eax
		.else
			invoke _GetLineFromOffset,nCurOffset
			dec eax
			invoke SendMessage,hListList,LB_SETCURSEL,eax,0
			invoke SendMessage,hwnd,WM_COMMAND,LBN_SELCHANGE *0FFFFH+IDC_LISTDLG,hListList
		.endif
		
	.elseif eax==WM_INITDIALOG
		invoke GetDlgItem,hwnd,IDC_LISTDLG
		mov hListList,eax
		invoke GetDlgItem,hwnd,IDC_HELP1
		mov hStatusList,eax
		invoke GetDlgItem,hwnd,IDC_INSERT
		mov hInsList,eax
		invoke GetDlgItem,hwnd,IDC_DEL
		mov hDelList,eax
		invoke GetDlgItem,hwnd,IDC_MODF
		mov hModList,eax
		invoke GetDlgItem,hwnd,IDC_GOTOCUR
		mov hGotocurList,eax
		invoke GetDlgItem,hwnd,IDC_SETCUR
		mov hSetcurList,eax
		invoke GetDlgItem,hwnd,IDC_PLAYVOICE
		mov hPlayList,eax
		invoke GetDlgItem,hwnd,IDC_RET
		mov hRetList,eax
		invoke GetDlgItem,hwnd,IDC_NEW
		mov hEditList,eax
		invoke GetDlgItem,hwnd,IDC_FOLLOW
		mov hFollowList,eax
		
	.elseif eax==WM_DESTROY
		mov hWinList,0
		
	.elseif eax==WM_CLOSE
		invoke ShowWindow,hwnd,SW_HIDE
		invoke ShowWindow,hWinMain,SW_SHOW
		
	.endif

_ListEx:
	xor eax,eax
	ret
_ListDlgProc endp

;
_FollowDlgProc proc uses edi esi ebx hwnd,uMsg,wParam,lParam
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax>=IDC_SEL1 && AX<=IDC_SEL5
			sub ax,IDC_SEL1-1
			and eax,0ffffh
			invoke EndDialog,hwnd,eax
		.endif

	.elseif eax==WM_INITDIALOG
		mov esi,lParam
		add esi,2
		invoke _GetTextByIdx,[esi]
		invoke SetDlgItemText,hwnd,IDC_SELTITLE,eax
		add esi,24
		mov ebx,5
		MOV EDI,IDC_SEL1
		.while ebx
			lodsd
			.break .if !eax
			invoke _GetTextByIdx,eax
			invoke SetDlgItemText,hwnd,edi,eax
			inc edi
			dec ebx
		.endw
		.while edi<=IDC_SEL5
			invoke GetDlgItem,hwnd,edi
			invoke EnableWindow,eax,FALSE
			inc edi
		.endw
	.elseif eax==WM_DESTROY
		invoke EndDialog,hwnd,0
	.endif
	XOR EAX,EAX
	ret
_FollowDlgProc endp

;
_DisableMainAll proc
	invoke GetDlgItem,hWinMain,IDC_EDIT
	invoke EnableWindow,EAX,FALSE
	invoke GetDlgItem,hWinMain,IDC_MODIFY
	invoke EnableWindow,EAX,FALSE
	invoke GetDlgItem,hWinMain,IDC_INSCUR
	invoke EnableWindow,EAX,FALSE
	invoke GetDlgItem,hWinMain,IDC_INSPREV
	invoke EnableWindow,EAX,FALSE
	invoke GetDlgItem,hWinMain,IDC_MORE
	invoke EnableWindow,EAX,FALSE
	invoke EnableWindow,hSaveMain,FALSE
	ret
_DisableMainAll endp 

;
_EnableMainAll proc
	invoke GetDlgItem,hWinMain,IDC_EDIT
	invoke EnableWindow,EAX,TRUE
	invoke GetDlgItem,hWinMain,IDC_MODIFY
	invoke EnableWindow,EAX,TRUE
	invoke GetDlgItem,hWinMain,IDC_INSCUR
	invoke EnableWindow,EAX,TRUE
	invoke GetDlgItem,hWinMain,IDC_INSPREV
	invoke EnableWindow,EAX,TRUE
	invoke GetDlgItem,hWinMain,IDC_MORE
	invoke EnableWindow,EAX,TRUE
	invoke EnableWindow,hSaveMain,TRUE
	ret
_EnableMainAll endp 

;
_RefreshList proc _lParam
	LOCAL @szText[512]:byte
	invoke SendMessage,hListList,LB_RESETCONTENT,0,0
	mov esi,sGscInfo.lpIndexCS
	.repeat
		lodsd
		add eax,sGscInfo.lpControlStream
		mov ecx,eax
		invoke _TranslateCB,ecx,addr @szText
		invoke SendMessage,hListList,LB_ADDSTRING,0,addr @szText
	.until !dword ptr [esi]
	invoke _GetLineFromOffset,nCurOffset
	dec eax
	invoke SendMessage,hListList,LB_SETCURSEL,eax,0
	invoke SendMessage,hWinList,WM_COMMAND,LBN_SELCHANGE *10000H+IDC_LISTDLG,hListList

	mov bListChanged,0
	ret
_RefreshList endp

;
_TranslateCB proc uses esi _lpInst,_lpszText
	mov esi,_lpInst
	lodsw
	.if ax>=03 && ax<=05
		invoke _GetLineFromOffset,[esi]
		invoke wsprintf,_lpszText,offset sz03s,eax
	.elseif ax==08 || ax==10h || ax==3ah
		invoke lstrcpy,_lpszText,offset sz08s
	.elseif ax==0eh
		invoke _GetTextByIdx,[esi+2]
		invoke wsprintf,_lpszText,offset sz0e,eax
	.elseif ax==0fh
		invoke wsprintf,_lpszText,offset sz0f,dword ptr [esi]
	.elseif ax==3ch
		invoke wsprintf,_lpszText,offset sz3c,dword ptr [esi+4]
	.elseif ax==3eh
		invoke wsprintf,_lpszText,offset sz3e,dword ptr [esi]
	.elseif ax==42h
		mov eax,[esi+4]
		inc eax
		invoke wsprintf,_lpszText,offset sz42,dword ptr [esi],eax
	.elseif ax==43h
		invoke lstrcpy,_lpszText,offset sz43
	.elseif ax==45h
		invoke lstrcpy,_lpszText,offset sz45
	.elseif ax==51h
		invoke _GetTextByIdx,[esi+20]
		invoke wsprintf,_lpszText,offset sz51,dword ptr [esi+4],eax
	.else
		and eax,0ffffh
		invoke wsprintf,_lpszText,offset szFF,eax
	.endif
	ret
_TranslateCB endp





