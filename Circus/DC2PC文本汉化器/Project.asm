; ?

      .486p                      ; create 32 bit code
      .model flat, stdcall      ; 32 bit memory model
      option casemap :none      ; case sensitive

      include Project.inc        ; local includes for this file

.code

start:

; ?

      invoke InitCommonControls

    ; ------------------
    ; set global values
    ; ------------------
      mov hInstance,   rv(GetModuleHandle, NULL)
      mov CommandLine, rv(GetCommandLine)
      mov hIcon,       rv(LoadIcon,hInstance,500)
      mov hCursor,     rv(LoadCursor,NULL,IDC_ARROW)
      mov sWid,        rv(GetSystemMetrics,SM_CXSCREEN)
      mov sHgt,        rv(GetSystemMetrics,SM_CYSCREEN)

      call Main

      invoke ExitProcess,eax

; ?

Main proc

    LOCAL Wwd:DWORD,Wht:DWORD,Wtx:DWORD,Wty:DWORD

    STRING szClassName,"CompacterClass"

  ; --------------------------------------------
  ; register class name for CreateWindowEx call
  ; --------------------------------------------
    invoke RegisterWinClass,ADDR WndProc,ADDR szClassName,
                       hIcon,hCursor,COLOR_BTNFACE+1

  ; -------------------------------------------------
  ; macro to autoscale window co-ordinates to screen
  ; percentages and centre window at those sizes.
  ; -------------------------------------------------


    
;    AutoScale 75, 70

    invoke CreateWindowEx,WS_EX_LEFT or WS_EX_ACCEPTFILES,
                          ADDR szClassName,
                          ADDR szDisplayName,
                          WS_CAPTION or WS_SYSMENU or WS_MINIMIZEBOX,
                          CW_USEDEFAULT,CW_USEDEFAULT,1100,720,
                          NULL,NULL,
                          hInstance,NULL
    mov hWnd,eax
  ; ---------------------------
  ; macros for unchanging code
  ; ---------------------------
    DisplayMenu hWnd,600
    DisplayWindow hWnd,SW_SHOWNORMAL

    call MsgLoop
    ret

Main endp

; ?

RegisterWinClass proc lpWndProc:DWORD, lpClassName:DWORD,
                      Icon:DWORD, Cursor:DWORD, bColor:DWORD

    LOCAL wc:WNDCLASSEX

    mov wc.cbSize,         sizeof WNDCLASSEX
    mov wc.style,          CS_BYTEALIGNCLIENT or \
                           CS_BYTEALIGNWINDOW
    m2m wc.lpfnWndProc,    lpWndProc
    mov wc.cbClsExtra,     NULL
    mov wc.cbWndExtra,     NULL
    m2m wc.hInstance,      hInstance
    m2m wc.hbrBackground,  bColor
    mov wc.lpszMenuName,   NULL
    m2m wc.lpszClassName,  lpClassName
    m2m wc.hIcon,          Icon
    m2m wc.hCursor,        Cursor
    m2m wc.hIconSm,        Icon

    invoke RegisterClassEx, ADDR wc

    ret

RegisterWinClass endp

; ?

MsgLoop proc

    LOCAL msg:MSG

    push esi
    push edi
    xor edi, edi                        ; clear EDI
    lea esi, msg                        ; Structure address in ESI
    jmp jumpin

    StartLoop:
      invoke TranslateMessage, esi
    ; --------------------------------------
    ; perform any required key processing here
    ; --------------------------------------
      invoke DispatchMessage,  esi
    jumpin:
      invoke GetMessage,esi,edi,edi,edi
      test eax, eax
      jnz StartLoop

    mov eax, msg.wParam
    pop edi
    pop esi

    ret

MsgLoop endp

; ?

WndProc proc hWin:DWORD,uMsg:DWORD,wParam:DWORD,lParam:DWORD

 ;   LOCAL var    :DWORD
 ;   LOCAL caW    :DWORD
 ;   LOCAL caH    :DWORD
    LOCAL patn   :DWORD
 ;   LOCAL Rct    :RECT
 ;   local nBytesofEdit2 :dword
 ;   LOCAL buffer1[260]:BYTE  ; these are two spare buffers
 ;   LOCAL buffer2[260]:BYTE  ; for text manipulation etc..

    Switch uMsg
      Case WM_COMMAND

        Switch wParam
        ;======== toolbar commands ========
         
        ;======== menu commands ========

          Case 1001
			cmp bFileModified,0
			je open974
			invoke MessageBox,hWin,offset szWarning,offset szDisplayName,MB_YESNO OR MB_DEFBUTTON2
			cmp eax,IDYES
			je open974
			ret
			open974:
            sas patn, "MES File",0,"*.mes",0
            mov fname, OpenFileDlg(hWin,hInstance,"Open File",patn)
           cmp BYTE PTR [fname], 0
             jne Openkao
            return 0
            
            Openkao:
            call MyGetPath
            OpenSuccess:
            call MySetPath
	       invoke CloseHandle,hFile
;            invoke CloseHandle,hTempFile
 ;           invoke DeleteFile,offset szTempFileName
 ;          invoke CreateFile,offset szTempFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
;			mov hTempFile,eax
			OpenSuccess1:
            invoke CreateFile,offset dtConfBuff.prevfn,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL
            cmp eax,0
            je FailRead
            mov hFile,eax
			
			mov bFileModified,0     
            call CleanBuff
            invoke ReadFile,hFile,addr nTotal,4,addr pBytesRead,0
            invoke wsprintf,offset lpszStr3,offset szStr3,nTotal
            mov ebx,dword ptr [nTotal]
            shl ebx,2
 ;           lea esi,dtConfBuff.prevfn
 ;           lea edi,dtConfBuff.tempfn
 ;           mov ecx,255
 ;           repz cmpsb
 ;           jmp only434
 ;           invoke ReadFile,hTempFile,addr dtBuff1m,ebx,addr pBytesRead,0
;            invoke ReadFile,hTempFile,addr dtBuff2m,100000,addr pBytesRead,0
 ;           jmp label87

            only434:
            
            invoke ReadFile,hFile,addr dtBuff1,ebx,addr pBytesRead,0
            invoke ReadFile,hFile,addr dtBuff2,100000,addr pBytesRead,0
            push 4096
            push offset dtBuff1
            push offset dtBuff1m
            call memcpy
            push 100000
            push offset dtBuff2
            push offset dtBuff2m
            call memcpy
            add esp,18h
            jmp label1
 ;           label87:
 ;           invoke ReadFile,hFile,addr dtBuff1,ebx,addr pBytesRead,0
 ;           invoke ReadFile,hFile,addr dtBuff2,100000,addr pBytesRead,0
 ;           jmp label1
		
			FailRead:
            invoke MessageBox,hWin,addr szFailRead,ADDR szDisplayName,MB_OK
            return 0
            
            label1:
            mov nLength1,0
            mov eax,dword ptr [dtConfBuff.ncnt]
            mov nCount,eax
            call ReadCount
            call ReadCountm
            invoke EnableWindow,hwndButtonPrev,TRUE
            invoke EnableWindow,hwndButtonGoto,TRUE
            invoke EnableWindow,hwndButtonNext,TRUE
            invoke EnableWindow,hwndButtonSave,TRUE
            invoke InvalidateRect,hWin,offset rectDigit,TRUE
            
            
          Case 1002
			push hFile
			call MyWriteFile
			add esp,4
			cmp eax,0
			jne f478
			invoke MessageBox,hWin,offset szFailWrite,offset szDisplayName,MB_OK
			f478:
			mov bFileModified,0


          Case 1010
            invoke SendMessage,hWin,WM_DESTROY,0,NULL

          Case 1900
            invoke MessageBox,hWin,offset szDisplayName,offset szDisplayName,MB_OK
              
		Case 3001
			cmp nCount,1
			jg en30011
			invoke MessageBox,hWin,offset szPrevError,offset szDisplayName,MB_OK
			ret
			en30011:
			xor eax,eax
			mov ecx,4
			lea edi,szTargetName
			rep stosd
			mov ecx,4
			lea edi,szNewName
			rep stosd
			dec nCount
			mov nLength1,0
			call ReadCount
			call ReadCountm
			mov eax,nCount
			mov dword ptr [dtConfBuff.ncnt],eax

		Case 3002
			mov eax,nCount
			cmp eax,nTotal
			jb en30021
			invoke MessageBox,hWin,offset szNextError,offset szDisplayName,MB_OK
			ret
			en30021:
			inc eax
			mov dword ptr [dtConfBuff.ncnt],eax
			xor eax,eax
			mov ecx,4
			lea edi,szTargetName
			rep stosd
			mov ecx,4
			lea edi,szNewName
			rep stosd
			inc nCount
			mov nLength1,0
			call ReadCount
			call ReadCountm
			
		Case 3003
			;加密
			invoke GetWindowText,hwndName2,offset szNewName,32
			xor ecx,ecx
			cmp byte ptr [szTargetName],0
			je en30031
			cmp byte ptr [szNewName],0
			jne en30035
				invoke MessageBox,hWin,offset szInputName,offset szDisplayName,MB_OK
				ret
			en30035:
				cmp bNameMatched,0
				jne crypt45
				
				mov edx,-32
				togo68:
				add edx,32
				cmp byte ptr [offset names+edx],0
				jnz togo68
				lea esi,szTargetName
				lea edi,[offset names+edx]
				mov ecx,16
				rep movsb
				lea esi,szNewName
				mov ecx,16
				rep movsb
				mov bNameModified,1
				
			crypt45:
			mov al,byte ptr [offset szNewName+ecx]
			cmp al,0
			je en30031
			sub al,20h
			mov byte ptr [offset szNewName+ecx],al
			inc ecx
			jmp crypt45
			en30031:
			mov nLength2,ecx
			cmp byte ptr [szNewName],0
			je jjh9
			inc nLength2
			jjh9:
			invoke GetWindowText,hwndEdit2,offset szNewText,512
			cmp byte ptr [szNewText],0
			jne crypt567
			invoke MessageBox,hWin,offset szInput,offset szDisplayName,MB_OK
			ret
			crypt567:
			xor ecx,ecx
			crypt877:
			mov al,byte ptr [offset szNewText+ecx]
			cmp al,0
			je end965
			sub al,20h
			mov byte ptr [offset szNewText+ecx],al
			inc ecx
			jmp crypt877
			
			;复制到写入缓冲区
			end965:
			add nLength2,ecx
			inc nLength2

			;第二缓冲清零
			push ecx
			xor eax,eax
			mov ecx,61a8h
			mov edi,offset dtBuff2mm
			rep stosd
			pop ecx
			
			
			mov eax,nCount
			dec eax
			shl eax,2
			mov edx,dword ptr [eax+offset dtBuff1m]
			mov nOffsetm,edx
			rep416:
			mov edx,nOffsetm
			lea edi,dword ptr [edx+offset dtBuff2m]
			mov al,byte ptr [edi]
			cmp al,36h
			jne goon34
				push ecx
				or ecx,0ffffffffh
				xor eax,eax
				REPNE SCAS BYTE PTR [edi]
				NOT ECX
				add nOffsetm,ecx
				jmp end2531
				
				protect47:
				mov ebx,70000
				sub ebx,nOffsetm
				push ebx
				add edx,nOffsetm
				add edx,nLength1
				push edx
				push offset dtBuff2mm
				call memcpy	
				add esp,0ch
				
				end2531:
				pop ecx
				jmp rep416
			goon34:
			cmp al,4bh
			jne goon876
				cmp byte ptr [dtBuff2mm],0
				jne next394
				push ecx
				mov edx,offset dtBuff2m
				add edx,2
				jmp protect47
				next394:
				push eax
				push ecx
				push edi
				mov edi,offset szNewName
				or ecx,0ffffffffh
				xor eax,eax
				REPNE SCAS BYTE PTR [EdI]
				NOT ECX
				SUB EDI,ECX
				MOV ESI,EDI
				pop edi
				inc edi
				REP MOVS byte PTR [EDI],byte PTR [ESI]
				mov byte ptr [edi],4ch
				sub edi,offset dtBuff2m
				mov nOffsetm,edi
				pop ecx
				pop eax
			goon876:
				cmp byte ptr [dtBuff2mm],0
				jne next174
				push ecx
				mov edx,offset dtBuff2m
				inc edx
				jmp protect47
				next174:

			push ecx
			push offset szNewText
			mov edx,nOffsetm
			lea ecx,dword ptr [edx+offset dtBuff2m+1]
			push ecx
			call memcpy
			add esp,0ch
			mov byte ptr [edi],0
			sub edi,offset dtBuff2m
			inc edi
			mov nOffsetm,edi
			mov edi,nLength2
			sub edi,nLength1
			mov ecx,nTotal
			dec ecx
			mov edx,nCount
			
			;向缓存复制buffer1
			dec edx
			add549:
				cmp ecx,edx
				je end256
				mov eax,dword ptr [ecx*4+offset dtBuff1m]
				add eax,edi
				mov dword ptr [ecx*4+offset dtBuff1m],eax
				dec ecx
				jmp add549
			end256:
			

			push ebx
			push offset dtBuff2mm
			mov eax,nOffsetm
			add eax,offset dtBuff2m
			push eax
			call memcpy
			add esp,0ch
	
			cmp word ptr [bNameModified],0
			je haha34e
			invoke SetFilePointer,hwndNameFile,0,0,FILE_BEGIN
			invoke WriteFile,hwndNameFile,offset names,2048,offset pBytesRead,0
			mov bNameModified,0
			haha34e:
;			mov edi,offset dtConfBuff.prevfn
;			dec edi
;			push offset dtConfBuff.tempfn
;			call StringCopy
;			add esp,4
			mov bFileModified,1
			mov eax,nCount
			cmp eax,nTotal
			je dff2
			invoke SendMessage,hWin,WM_COMMAND,3002,0
			ret
			dff2:
			
		case 3004
			invoke GetWindowText,hwndCount,offset lpszCount,4
				xor eax,eax
				xor ecx,ecx
				
				lea esi,lpszCount
				mov al,byte ptr [esi]
				sub al,'0'-0
				movzx eax,al
				cmp byte ptr [esi+1],0
				je end555
				movzx eax,al
				shl eax,1
				mov ecx,eax
				shl eax,2
				add ecx,eax
				mov al,byte ptr [esi+1]
				sub al,'0'-0
				movzx eax,al
				add eax,ecx
				cmp byte ptr [esi+2],0
				je end555
				shl eax,1
				mov ecx,eax
				shl eax,2
				add ecx,eax
				mov al,byte ptr [esi+2]
				sub al,'0'-0
				movzx eax,al
				add eax,ecx
				
				
				end555:
				cmp eax,nTotal
				jg endshit
				mov nCount,eax
				mov nLength1,0
				xor eax,eax
			mov ecx,4
			lea edi,szTargetName
			rep stosd
			mov ecx,4
			lea edi,szNewName
			rep stosd
				call ReadCount
				call ReadCountm
				mov eax,nCount
				mov dword ptr [dtConfBuff.ncnt],eax
				endshit:
				ret
				
		Case 24001h
			cmp bFileModified,0
			je feh
			invoke MessageBox,hWin,offset szWarning,offset szDisplayName,MB_YESNO OR MB_DEFBUTTON2
			cmp eax,IDYES
			je feh
			ret
			feh:
			invoke SendMessage,hwndListbox,LB_GETCURSEL,0,0
			cmp eax,-1
			jne list587
			ret
			list587:
			invoke SendMessage,hwndListbox,LB_GETTEXT,eax,offset fnbuffer
			lea esi,fnbuffer
			lea edi,dtConfBuff.prevfn
			mov dtConfBuff.ncnt,1
			mov ecx,8
			rep movsd
			jmp OpenSuccess
			
          ;====== end menu commands ======
      Endsw

;      Case WM_DROPFILES
;        mov fname, DropFileName(wParam)
;        fn MessageBox,hWin,fname,"WM_DROPFILES",MB_OK

      Case WM_CREATE
;       invoke Do_ToolBar,hWin
;       invoke Do_Status,hWin

	    invoke CreateFont,24,12,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,128,OUT_TT_PRECIS,32,0,4,addr szJpfont
		mov hfJp,eax
		invoke CreateFont,24,12,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,134,OUT_TT_PRECIS,32,0,4,addr szChnfont
		mov hfChn,eax
		invoke CreateFont,18,9,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,0,OUT_TT_PRECIS,32,0,4,addr szChnfont
		mov hfSys,eax

        invoke CreateWindowEx,0,addr szEditObject,0,WS_CHILD or WS_VISIBLE or WS_BORDER OR ES_READONLY,\
			10,40,200,40,hWin,2001,hInstance,NULL
        mov hwndName1,eax

        invoke CreateWindowEx,0,addr szEditObject,NULL,WS_CHILD or WS_VISIBLE or \
			WS_VSCROLL or WS_BORDER or ES_MULTILINE or ES_AUTOVSCROLL or ES_LEFT or ES_READONLY or ES_NOHIDESEL,\
			10,100,750,200,hWin,2002,hInstance,NULL
        mov hwndEdit1,eax

        invoke CreateWindowEx,0,addr szEditObject,NULL,WS_CHILD or WS_VISIBLE or \
			WS_BORDER or ES_LEFT,\
			10,340,200,40,hWin,2003,hInstance,NULL
        mov hwndName2,eax
        invoke SendMessage,hwndName2,WM_SETFONT,hfChn,0

        invoke CreateWindowEx,0,addr szEditObject,NULL,WS_CHILD or WS_VISIBLE or \
			WS_VSCROLL or WS_BORDER or ES_MULTILINE or ES_AUTOVSCROLL or ES_LEFT or ES_NOHIDESEL,\
			10,400,750,200,hWin,2004,hInstance,NULL
        mov hwndEdit2,eax
        invoke SendMessage,hwndEdit2,WM_SETFONT,hfChn,0
		invoke SetWindowLong,hwndEdit2,GWL_WNDPROC,Edit2Proc
		mov OldEdit2Proc,eax
        
        invoke CreateWindowEx,0,offset szButtonObject,offset szPrev,WS_CHILD OR WS_VISIBLE OR WS_DISABLED OR BS_PUSHBUTTON,\
			450,340,100,40,hWin,3001,hInstance,NULL
		mov hwndButtonPrev,eax
        invoke CreateWindowEx,0,offset szButtonObject,offset szNext,WS_CHILD OR WS_VISIBLE OR WS_DISABLED OR BS_PUSHBUTTON,\
			600,340,100,40,hWin,3002,hInstance,NULL
		mov hwndButtonNext,eax
		
		invoke CreateWindowEx,0,offset szButtonObject,offset szSave,WS_CHILD OR WS_VISIBLE OR WS_DISABLED OR BS_PUSHBUTTON,\
			620,620,120,40,hWin,3003,hInstance,NULL
		mov hwndButtonSave,eax
		
		invoke CreateWindowEx,0,offset szEditObject,NULL,WS_CHILD or WS_VISIBLE or \
			WS_BORDER or ES_LEFT or ES_NUMBER,\
			300,330,40,20,hWin,2005,hInstance,NULL
		mov hwndCount,eax
		
		invoke CreateWindowEx,0,offset szButtonObject,offset szGoto,WS_CHILD OR WS_VISIBLE OR WS_DISABLED OR BS_PUSHBUTTON,\
			240,330,40,20,hWin,3004,hInstance,NULL
		mov hwndButtonGoto,eax
		
		invoke CreateWindowEx,0,offset szListbox,NULL,WS_CHILDWINDOW OR WS_HSCROLL OR WS_VISIBLE OR LBS_STANDARD,\
			800,100,250,550,hWin,4001h,hInstance,NULL
		mov hwndListbox,eax
		invoke SendMessage,hwndListbox,WM_SETFONT,hfSys,0
		invoke SendMessage,hwndListbox,LB_SETHORIZONTALEXTENT,200,0
        invoke SendMessage,hwndListbox,LB_DIR,DDL_READWRITE,offset szFileType
		
		invoke CreateFile,offset szConfFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
		mov hConfFile,eax
		invoke CreateFile,offset szNameTable,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
		mov hwndNameFile,eax
		
		invoke ReadFile,hConfFile,offset dtConfBuff,sizeof Conf,offset pBytesRead,0
		invoke ReadFile,hwndNameFile,offset names,2048,offset pBytesRead,0
		cmp dword ptr [dtConfBuff],23a57e9fh
		je label398
		call CreateConf
		label398:
		cmp byte ptr [dtConfBuff.workpath],0
		je label902
		cmp byte ptr [dtConfBuff.prevfn],0
		je label902
		call MySetPath
;        invoke CreateFile,offset szTempFileName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
;		mov hTempFile,eax
		jmp OpenSuccess1
		label902:
		
      Case WM_PAINT
        invoke Paint_Proc,hWin
        return 0

      case WM_SETFOCUS
		invoke SetFocus,hwndEdit2

		Case WM_DESTROY
      Case WM_CLOSE
      next495:
        cmp bFileModified,0
        je fdie
        invoke MessageBox,hWin,offset szWarning,offset szDisplayName,MB_YESNO OR MB_DEFBUTTON2
        cmp eax,IDYES
        jne fdie
        ret
        fdie:
        invoke SetFilePointer,hConfFile,0,0,FILE_BEGIN
		invoke WriteFile,hConfFile,offset dtConfBuff,sizeof Conf,offset pBytesRead,0
        invoke CloseHandle,hConfFile
        invoke CloseHandle,hwndNameFile
;        invoke CloseHandle,hTempFile
        invoke CloseHandle,hFile
        invoke PostQuitMessage,NULL
        return 0

    Endsw

    invoke DefWindowProc,hWin,uMsg,wParam,lParam

    ret

WndProc endp



Paint_Proc proc hWin:DWORD

   LOCAL hdc      :DWORD
  LOCAL btn_hi   :DWORD
 LOCAL btn_lo   :DWORD
LOCAL Rct      :RECT
LOCAL Ps       :PAINTSTRUCT

    mov hdc, rv(BeginPaint,hWin,ADDR Ps)

	invoke SetBkMode,hdc,TRANSPARENT
	invoke TextOut,hdc,280,330,offset szStr1,2
	invoke TextOut,hdc,340,330,offset szStr2,6
	invoke TextOut,hdc,280,370,offset lpszStr3,13
  ; ----------------------------------------

;    mov btn_hi, rv(GetSysColor,COLOR_BTNHIGHLIGHT)

;    mov btn_lo, rv(GetSysColor,COLOR_BTNSHADOW)


  ; ----------------------------------------

   invoke EndPaint,hWin,ADDR Ps

  ret

Paint_Proc endp

; ?

Extract proc
	push eax
	mov eax,nCount
	push ecx
	push edx
	push ebx
	push edi
	sub eax,1
	shl eax,2
	mov bNameMatched,0
	mov ebx,DWORD PTR [eax+offset dtBuff1]
	mov nOffset,ebx
	GetString:
	mov ebx,nOffset
	lea edi,dword ptr [ebx+offset dtBuff2]
	mov al,byte ptr [edi]
	movzx eax,al
	switch eax
		case 36h
			or ecx,0ffffffffh
			xor eax,eax
			REPNE SCAS BYTE PTR [edi]
			NOT ECX
			lea ecx,dword ptr [ebx+ecx]
			mov nOffset,ecx
			jmp GetString
		case 4bh
			push offset szTargetName
			call StringCopy
			mov nOffset,eax
			add esp,4
			mov al,szTargetName
			xor ecx,ecx
			label324:
			cmp al,0
			je end324
			add al,20h
			mov byte ptr [offset szTargetName+ecx],al
			inc ecx
			mov al,byte ptr [offset szTargetName+ecx]
			jmp label324
			end324:
;			mov nLength1,ecx
			xor edx,edx
			match54:
				lea esi,dword ptr [edx+offset names]
				cmp byte ptr [esi],0
				je GetString
				lea edi,szTargetName
				mov ecx,16
				repz cmpsb
				jnz fail452
				lea edi,szNewName
				lea esi,[offset names+edx+16]
				mov ecx,16
				rep movsb
				mov bNameMatched,1
				jmp GetString
				fail452:
				add edx,32
				jmp match54
		case 4ch
			push offset szTarget
			call StringCopy
			mov nOffset,eax
			add esp,4
			
	mov al,szTarget
	mov ecx,offset szTarget
	mov dl,[ecx+1]
	xor ecx,ecx
	xor ebx,ebx
	Uncrypt:
	cmp al,0
	je endCr
	cmp al,0eah
	je enterChar
	add al,20h
	add dl,20h
	mov byte ptr [offset szUncrypted+ebx],al
	inc ebx
	mov byte ptr [offset szUncrypted+ebx],dl
	inc ebx
	add ecx,2
	mov al,byte ptr [offset szTarget+ecx]
	mov dl,byte ptr [offset szTarget+ecx+1]
	jmp Uncrypt
	enterChar:
	inc ecx
	mov al,byte ptr [offset szTarget+ecx]
	mov dl,byte ptr [offset szTarget+ecx+1]
	jmp Uncrypt
	endCr:
	mov word ptr [offset szUncrypted+ebx],0
;	add nLength1,ecx
;	add nLength1,3
	endsw
	
	pop edi
	pop ebx
	pop edx
	pop ecx
	pop eax
	ret
Extract endp

StringCopy proc
			push ebp
			mov ebp,[esp+8]
			or ecx,0ffffffffh
			xor eax,eax
			inc edi
			REPNE SCAS BYTE PTR [EDI]
			NOT ECX
			SUB EDI,ECX
			MOV EAX,ECX
			mov edx,eax
			MOV ESI,EDI
			MOV EDI,ebp
			SHR ECX,2
			REP MOVS DWORD PTR [EDI],DWORD PTR [ESI]
			MOV ECX,EAX
			XOR EAX,EAX
			AND ECX,3
			REP MOVS BYTE PTR [EDI],BYTE PTR [ESI]
			mov EDI,ebp
			OR ECX,0FFFFFFFFh
			REPNE SCAS BYTE PTR [EDI]
			NOT ECX
			DEC ECX
			LEA EaX,DWORD PTR [EBX+ECX+2]
			pop ebp

	ret
StringCopy endp

ReadCount proc
			call Extract
			invoke wsprintf,offset lpszCount,offset szCount,nCount
			invoke SetWindowText,hwndCount,offset lpszCount
			mov al,szUncrypted
			cmp al,81h
			jb rc274
			cmp al,0a0h
			jg rc274
			invoke SendMessage,hwndName1,WM_SETFONT,hfJp,0
			invoke SendMessage,hwndEdit1,WM_SETFONT,hfJp,0
			jmp rc275
			rc274:
			invoke SendMessage,hwndName1,WM_SETFONT,hfChn,0
			invoke SendMessage,hwndEdit1,WM_SETFONT,hfChn,0
			rc275:
			invoke SetWindowText,hwndEdit1,offset szUncrypted
			invoke SetWindowText,hwndName1,offset szTargetName
			
	ret
ReadCount endp

memcpy proc
	mov edi,[esp+4]
	mov esi,[esp+8]
	mov ecx,[esp+12]
	mov eax,ecx
	shr ecx,2
	REP MOVS DWORD PTR [EDI],DWORD PTR [ESI]
	mov ecx,eax
	and ecx,3
	REP MOVS byte PTR [EDI],byte PTR [ESI]

	ret
memcpy endp

CleanBuff proc
	xor eax,eax
			mov ecx,4
			lea edi,szTargetName
			rep stosd
			mov ecx,4
			lea edi,szNewName
			rep stosd

	mov ecx,1024
	mov edi,offset dtBuff1
	rep stosd
	mov edi,offset dtBuff2
	mov ecx,61a8h
	rep stosd
	mov edi,offset dtBuff1m
	mov ecx,1024
	rep stosd
	mov edi ,offset dtBuff2m
	mov ecx,61a8h
	rep stosd
	ret
CleanBuff endp

Edit2Proc proc uses ebx,hwnd:dword,message:dword,wParam:dword,lParam:dword
	invoke GetParent,hwnd
	cmp message,WM_KEYDOWN
	jne label357
	cmp wParam,VK_RETURN
	jne label366
	invoke SendMessage,eax,WM_COMMAND,3003,0
	ret
	label366:
	cmp wParam,VK_NEXT
	jne label578
	invoke SendMessage,eax,WM_COMMAND,3002,0
	ret
	label578:
	cmp wParam,VK_PRIOR
	jne label357
	invoke SendMessage,eax,WM_COMMAND,3001,0
	ret
	label357:
	cmp message,WM_CHAR
	jne end23
	cmp wParam,13
	jne end23
	ret
	end23:
	invoke CallWindowProc,OldEdit2Proc,hwnd,message,wParam,lParam
	ret
Edit2Proc endp

MyWriteFile proc
	mov ebx,[esp+4]
	mov ecx,nTotal
	dec ecx
	mov ecx,dword ptr [ecx*4+offset dtBuff1m]
	add ecx,offset dtBuff2m
	mov edi,ecx
	mov al,3ah
	rep980:
	repnz scasb
	mov ecx,dword ptr [edi]
	cmp ecx,646e65h
	je end559
	jmp rep980
	end559:
	add edi,6
	sub edi,offset dtBuff2m
	invoke SetFilePointer,ebx,0,0,FILE_BEGIN
	invoke WriteFile,ebx,offset nTotal,4,offset pBytesRead,0
	mov edx,nTotal
	shl edx,2
	invoke WriteFile,ebx,offset dtBuff1m,edx,offset pBytesRead,0
	invoke WriteFile,ebx,offset dtBuff2m,edi,offset pBytesRead,0
	ret
MyWriteFile endp

CreateConf proc
	mov dword ptr [dtConfBuff],23a57e9fh
	mov dword ptr [dtConfBuff.ncnt],1
	invoke WriteFile,hConfFile,offset dtConfBuff,sizeof Conf,offset pBytesRead,0
	invoke SetFilePointer,hConfFile,0,0,FILE_BEGIN
	ret
CreateConf endp

ReadCountm proc
	call Extractm
	cmp bNameMatched,0
	jnz efg
	invoke SetWindowText,hwndName2,offset szZero
	jmp nexe213
	efg:
	invoke SetWindowText,hwndName2,offset szNewName
	nexe213:
	lea esi,szNewText
	lea edi,szUncrypted
	mov ecx,4
	repz cmpsb
	jz ne76
	invoke SetWindowText,hwndEdit2,offset szNewText
	jmp ee578
	ne76:
	invoke SetWindowText,hwndEdit2,offset szZero
	ee578:
	cmp byte ptr [szTargetName],0
	je end4562
	invoke EnableWindow,hwndName2,TRUE
	jmp ne7654
	end4562:
	invoke EnableWindow,hwndName2,FALSE
	ne7654:
	ret
ReadCountm endp

Extractm proc
	push eax
	mov eax,nCount
	push ecx
	push edx
	push ebx
	push esi
	push edi
	sub eax,1
	shl eax,2
	mov ebx,DWORD PTR [eax+offset dtBuff1m]
	mov nOffsetm,ebx
	GetString1:
	mov ebx,nOffsetm
	lea edi,dword ptr [ebx+offset dtBuff2m]
	mov al,byte ptr [edi]
	movzx eax,al
	switch eax
		case 36h
			or ecx,0ffffffffh
			xor eax,eax
			REPNE SCAS BYTE PTR [edi]
			NOT ECX
			lea ecx,dword ptr [ebx+ecx]
			mov nOffsetm,ecx
			jmp GetString1
		case 4bh
			cmp bNameMatched,0
			je goon5l
				or ecx,0ffffffffh
				xor eax,eax
				inc edi
				REPNE SCAS BYTE PTR [edi]
				NOT ECX
				mov nLength1,ecx
				LEA EaX,DWORD PTR [EBX+ECX+1]
				mov nOffsetm,eax
				jmp GetString1
			goon5l:
			push offset szNewName
			call StringCopy
			mov nOffsetm,eax
			add esp,4
			mov nLength1,edx
			mov al,szNewName
			xor ecx,ecx
			label3224:
			cmp al,0
			je end3243
			add al,20h
			mov byte ptr [offset szNewName+ecx],al
			inc ecx
			mov al,byte ptr [offset szNewName+ecx]
			jmp label3224
			end3243:
			jmp GetString1
		case 4ch
			push offset szNewCryptText
			call StringCopy
			mov nOffsetm,eax
			add esp,4
			add nLength1,edx
			
	mov al,szNewCryptText
	mov ecx,offset szNewCryptText
	mov dl,[ecx+1]
	xor ecx,ecx
	xor ebx,ebx
	Uncrypt:
	cmp al,0
	je endCr
	cmp al,0eah
	je enterChar
	add al,20h
	add dl,20h
	mov byte ptr [offset szNewText+ebx],al
	inc ebx
	mov byte ptr [offset szNewText+ebx],dl
	inc ebx
	add ecx,2
	mov al,byte ptr [offset szNewCryptText+ecx]
	mov dl,byte ptr [offset szNewCryptText+ecx+1]
	jmp Uncrypt
	enterChar:
	inc ecx
	mov al,byte ptr [offset szNewCryptText+ecx]
	mov dl,byte ptr [offset szNewCryptText+ecx+1]
	jmp Uncrypt
	endCr:
	mov word ptr [offset szNewText+ebx],0
	endsw
	
	pop edi
	pop esi
	pop ebx
	pop edx
	pop ecx
	pop eax
	ret
Extractm endp

MyGetPath proc
            xor eax,eax
	       mov ecx,72
	       lea edi,dtConfBuff.workpath
	       rep stosd			

            mov al,'\'
            mov edi,fname
            add edi,255
            std
            or ecx,0ffffffffh
            repnz scasb
            not ecx
            cld
            add edi,2
            mov ebx,edi
            mov esi,edi
            lea edi,dtConfBuff.prevfn
            rep movsb
            mov edi,ebx
            sub edi,fname
            mov ecx,edi
            mov esi,fname
            lea edi,dtConfBuff.workpath
            rep movsb
			mov byte ptr [edi],0
	ret
MyGetPath endp

MySetPath proc
	        invoke SetCurrentDirectory,offset dtConfBuff.workpath
            invoke SendMessage,hwndListbox,LB_RESETCONTENT,0,0
            invoke SendMessage,hwndListbox,LB_DIR,DDL_READWRITE,offset szFileType
            invoke SendMessage,hwndListbox,LB_SELECTSTRING,-1,offset dtConfBuff.prevfn
            invoke SendMessage,hwndListbox,LB_SETTOPINDEX,eax,0

	ret
MySetPath endp

end start
;字体变换