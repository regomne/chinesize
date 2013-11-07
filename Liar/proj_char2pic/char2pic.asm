.486
.model flat,stdcall
option casemap:none


include char2pic.inc


.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke GetProcessHeap
mov hHeap,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax

call _WinMain
invoke ExitProcess,0

_WinMain proc
	local @stWndClass:WNDCLASSEX
	local @stMsg:MSG
	mov ecx,sizeof @stWndClass
	lea edi,@stWndClass
	xor eax,eax
	rep stosb
	
	invoke LoadCursor,0,IDC_ARROW
	
	mov @stWndClass.hCursor,eax
	push hInstance
	pop @stWndClass.hInstance
	mov @stWndClass.cbSize,sizeof WNDCLASSEX
	mov @stWndClass.style,CS_HREDRAW OR CS_VREDRAW
	mov @stWndClass.lpfnWndProc,offset _WndMainProc
	invoke GetStockObject,WHITE_BRUSH
	mov @stWndClass.hbrBackground,eax
	mov @stWndClass.lpszClassName,offset szClassName
	invoke RegisterClassEx,addr @stWndClass
	
	invoke CreateWindowEx,WS_EX_CLIENTEDGE,offset szClassName,offset szDisplayName,\
		WS_OVERLAPPEDWINDOW or WS_CAPTION or WS_SYSMENU,\
		CW_USEDEFAULT,CW_USEDEFAULT,500,430,NULL,NULL,hInstance,NULL
	mov hWinMain,eax
	invoke ShowWindow,hWinMain,SW_SHOWNORMAL
	invoke UpdateWindow,hWinMain
	
	.while TRUE
		invoke GetMessage,addr @stMsg,NULL,0,0
		.break .if eax==0
		invoke TranslateMessage,addr @stMsg
		invoke DispatchMessage,addr @stMsg
	.endw
	
	ret
_WinMain endp

_WndMainProc proc uses ebx edi esi,hwnd,uMsg,wParam,lParam
	local @stPs:PAINTSTRUCT
	LOCAL @hdc,@hmdc,@hbmp
	LOCAL @lf:LOGFONT
	LOCAL @cf:CHOOSEFONT
	LOCAL @be:_BitmapEntry
	
	mov eax,uMsg
	.if eax==WM_PAINT
		invoke BeginPaint,hwnd,addr @stPs
		mov @hdc,eax
		invoke EndPaint,hwnd,addr @stPs

	.elseif eax==WM_CLOSE
		invoke GetDC,hwnd
		invoke Main,eax
		invoke DestroyWindow,hwnd
		invoke PostQuitMessage,NULL
	
	.else
		invoke DefWindowProc,hwnd,uMsg,wParam,lParam
		ret
	.endif
	
_Ex:
	xor eax,eax
	ret
_WndMainProc endp

Main proc _hdc
	LOCAL @hFile,@nSize,@lpTxt
	LOCAL @lpBE
	
	LOCAL @lf:LOGFONT
	LOCAL @cf:CHOOSEFONT
	
	invoke CreateFile,$CTA0("data.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov hOutLink,eax
	invoke WriteFile,hOutLink,offset szBegin,2,offset dwTemp,0
	
	invoke CreateFile,$CTA0("links.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov ebx,eax
	invoke GetFileSize,ebx,0
	mov esi,eax
	invoke VirtualAlloc,0,esi,MEM_COMMIT,PAGE_READWRITE
	mov edi,eax
	invoke ReadFile,ebx,edi,esi,offset dwTemp,0
	.if word ptr [edi]==0feffh
		add edi,2
		invoke _GetLinkW,edi,addr Links
	.endif
	
	lea edi,@cf
	xor al,al
	mov ecx,sizeof CHOOSEFONT
	rep stosb
	lea edi,@lf
	mov ecx,sizeof LOGFONT
	rep stosb
	lea esi,@cf
	assume esi:ptr CHOOSEFONT
	mov [esi].lStructSize,sizeof @cf
	lea ecx,@lf
	mov [esi].lpLogFont,ecx
	mov [esi].Flags,CF_TTONLY or CF_INITTOLOGFONTSTRUCT or CF_NOVERTFONTS or CF_SCREENFONTS
	assume esi:nothing
	invoke ChooseFont,esi
	or eax,eax
	je _Ex
;	mov @lf.lfQuality,CLEARTYPE_QUALITY
	invoke CreateFontIndirect,addr @lf
	mov esi,eax
	invoke ChooseFont,addr @cf
	or eax,eax
	je _Ex
;	mov @lf.lfQuality,6
	invoke CreateFontIndirect,addr @lf
	mov edi,eax
	
	invoke CreateFile,$CTA0("01.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hFile,eax
	invoke GetFileSize,eax,0
	mov @nSize,eax
	add eax,100h
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,eax
	mov @lpTxt,eax
	invoke ReadFile,@hFile,eax,@nSize,offset dwTemp,0
	mov ecx,@lpTxt
	add ecx,2
	invoke _Str2PicW,_hdc,ecx,addr @lpBE,esi,edi,501
	invoke _SaveBmp,@lpBE,501
	_Ex:
	ret
Main endp

;
_Str2PicW proc uses esi edi ebx _hdc,_lpStr,_lpOutBmps,_hFontT,_hFont,_nIdx
	LOCAL @hdc,@hbmp
	LOCAL @bNewPage,@pNowPage
	LOCAL @szTitle[40]:byte
	LOCAL @szItem[80]:byte 
	LOCAL @nPage,@nLine,@nCol
	LOCAL @hFontTitle,@hFont
	LOCAL @rect:RECT
	invoke CreateCompatibleDC,_hdc
	mov @hdc,eax
	invoke CreateCompatibleBitmap,_hdc,PIC_WIDTH,PIC_HEIGHT
	mov @hbmp,eax
	invoke SelectObject,@hdc,@hbmp

;	invoke CreateFont,TITLECHAR_HEIGHT,TITLECHAR_WIDTH,0,0,0,0,0,0,86h,0,0,0,0,offset szFontTitle
;	mov @hFontTitle,eax
;	invoke CreateFont,14,9,0,0,0,0,0,0,86h,0,0,0,0,offset szFont
;	mov @hFont,eax
	
	mov @rect.left,0
	mov @rect.top,0
	mov @rect.right,PIC_WIDTH
	mov @rect.bottom,PIC_HEIGHT
	
	mov esi,_lpStr
	.while word ptr [esi]==0dh
		add esi,4
	.endw
	.repeat
		.break .if dword ptr [esi]!=2f002fh
		.while word ptr [esi]!=0dh
			.if !word ptr [esi]
				mov ebx,E_FORMAT
				jmp _Ex
			.endif
			add esi,2
		.endw
		.repeat
			add esi,4
		.until word ptr [esi]!=0dh
	.until 0
	mov edi,esi
	.while word ptr [edi]!=0dh
		.break .if !word ptr [esi]
		add edi,2
	.endw
	sub edi,esi
	mov ecx,edi
	.if ecx>38
		mov ebx,E_TOOLONG
		jmp _Ex
	.endif
	lea edi,@szTitle
	rep movsb
	mov word ptr [edi],0
	
	.repeat
		add esi,4
	.until word ptr [esi]!=0dh
	mov @bNewPage,1
	mov @nPage,0
	mov ecx,_lpOutBmps
	mov @pNowPage,ecx
	.while word ptr [esi]
		.if dword ptr [esi]==2f002fh
			.while word ptr [esi]!=0dh
				.if !word ptr [esi]
					xor ebx,ebx
					jmp _Ex
				.endif
				add esi,2
			.endw
			.repeat
				add esi,4
			.until word ptr [esi]!=0dh
			.continue
		.endif
		.if @bNewPage
			.if @nPage
				invoke WriteFile,hOutLink,$CTW("-1\n"),8,offset dwTemp,0
			.endif
			invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,sizeof(_BitmapEntry)
			mov ecx,@pNowPage
			mov [ecx],eax
			mov @pNowPage,eax
			mov ebx,eax
			inc @nPage
			mov @nLine,0
			lea ecx,[esi-4]
			.if ecx>=_lpStr && word ptr [ecx]!=0dh
				mov @nCol,0
			.else
				mov @nCol,1
			.endif
			mov @bNewPage,0
			
			mov ebx,esi
			lea esi,defHdr
			mov edx,@pNowPage
			lea edi,[edx+_BitmapEntry.Hdr]
			mov ecx,sizeof defHdr
			rep movsb
			mov esi,ebx
			
			invoke wsprintfW,addr @szItem,$CTW0("%03d%d:\n\t"),_nIdx,@nPage
			shl eax,1
			invoke WriteFile,hOutLink,addr @szItem,eax,offset dwTemp,0
;			mov @rect.left,0
;			mov @rect.top,0
;			mov @rect.right,PIC_WIDTH
;			mov @rect.bottom,PIC_HEIGHT
			invoke GetStockObject,WHITE_BRUSH
			invoke FillRect,@hdc,addr @rect,eax
			invoke SelectObject,@hdc,_hFontT
			invoke lstrlenW,addr @szTitle
			invoke TextOutW,@hdc,TITLE_X,TITLE_Y,addr @szTitle,eax
			invoke SelectObject,@hdc,_hFont
		.endif
		mov ax,[esi]
		.if ax==0ff1ch
			.if dword ptr [esi+2]==0ff1eff50h
				invoke _MakeDIB,@hdc,@hbmp,@pNowPage
				mov @bNewPage,1
				add esi,6
				.continue
			.endif
			add esi,2
			mov edi,esi
			mov ebx,esi
			.while word ptr [edi]!=0ff1eh
				.if !word ptr [edi]
					mov ebx,E_FORMAT
					jmp _Ex
				.endif
				add edi,2
			.endw
			sub edi,esi
			mov ecx,edi
			.if ecx>40
				mov ebx,E_TOOLONG
				jmp _Ex
			.endif
			.if @nLine==9
				mov edx,@nCol
				add edx,ecx
				.if edx>=21
					invoke _MakeDIB,@hdc,@hbmp,@pNowPage
					mov @bNewPage,1
					lea esi,[ebx-2]
					.continue
				.endif
			.endif
			lea edi,@szItem
			rep movsb
			mov word ptr [edi],0
			mov esi,ebx
			invoke _MakeLink,hOutLink,@nLine,@nCol,addr @szItem
			.if eax
				int 3
			.endif
			jmp _PaintChar
		.elseif ax==0ff1eh
			add esi,2
		.elseif ax==0dh
			add esi,4
			.if word ptr [esi-8]!=0dh
				inc @nLine
				.if word ptr [esi]!=3000h
					mov @nCol,1
				.else
					mov @nCol,0
				.endif
			.endif
		.else
_PaintChar:
			cmp @nLine,10
			je _NextPage
			mov bx,[esi]
			mov ecx,@nCol
			.if ecx>=20
				.if ecx==20 && (bx==0ff0ch || bx==3002h || bx==3001h || bx==300dh || bx==300bh)
					jmp @F
				.endif
				cmp word ptr [esi+2],0
				je @F
				.if @nLine==9
			_NextPage:
					invoke _MakeDIB,@hdc,@hbmp,@pNowPage
					mov @bNewPage,1
					.continue
				.endif
				inc @nLine
				mov @nCol,0
			.endif
			@@:
			mov ecx,CHAR_WIDTH
			mov eax,@nCol
			mul ecx
			lea ebx,[eax+CHAR_X]
;			mov @rect.left,ebx
;			add ebx,CHAR_WIDTH
;			mov @rect.right,ebx
			mov ecx,CHAR_HEIGHT
			mov eax,@nLine
			mul ecx
			add eax,CHAR_Y
;			mov @rect.top,eax
;			add eax,CHAR_HEIGHT
;			mov @rect.bottom,eax
			invoke TextOutW,@hdc,ebx,eax,esi,1
;			invoke DrawTextW,@hdc,esi,1,addr @rect,DT_NOPREFIX
			inc @nCol
			add esi,2
		.endif
	.endw
	invoke WriteFile,hOutLink,$CTW("-1\n"),8,offset dwTemp,0
	invoke _MakeDIB,@hdc,@hbmp,@pNowPage
	
	.if @nPage>1
		invoke SelectObject,@hdc,_hFontT
		mov ecx,_lpOutBmps
		mov edi,[ecx]
		assume edi:ptr _BitmapEntry
		mov ebx,1
		.while edi
			invoke SetDIBits,@hdc,@hbmp,0,PIC_WIDTH,[edi].lpDib,addr [edi].Hdr,DIB_RGB_COLORS
			invoke HeapFree,hHeap,0,[edi].lpDib
			invoke wsprintfW,addr @szTitle,$CTW0("(%d/%d)"),ebx,@nPage
			invoke TextOutW,@hdc,PAGE_X,TITLE_Y,addr @szTitle,5
			invoke _MakeDIB,@hdc,@hbmp,edi
			mov edi,[edi].Next
			inc ebx
		.endw
		assume edi:nothing
	.endif
	
	xor ebx,ebx
	_Ex:
;	invoke DeleteObject,@hFont
;	invoke DeleteObject,@hFontTitle
	invoke DeleteObject,@hbmp
	invoke DeleteObject,@hdc
	mov eax,ebx
	ret
_Str2PicW endp

_MakeDIB proc _hdc,_hbmp,_lpBitmap
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,PIC_WIDTH*PIC_HEIGHT*8
	mov ecx,_lpBitmap
	mov _BitmapEntry.lpDib[ecx],eax
	mov edx,eax
	invoke GetDIBits,_hdc,_hbmp,0,PIC_WIDTH,edx,addr [ecx+_BitmapEntry.Hdr],DIB_RGB_COLORS
	ret
_MakeDIB endp

_SaveBmp proc uses edi ebx _lpBE,_nNamePre
	LOCAL @bfh:BITMAPFILEHEADER
	LOCAL @fn[16]:byte
	LOCAL @hFile
	mov edi,_lpBE
	assume edi:ptr _BitmapEntry
	mov ebx,1
	.while edi
		mov eax,[edi].Hdr.biSizeImage
		mov @bfh.bfType,'MB'
		add eax,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
		mov @bfh.bfSize,eax
		xor ecx,ecx
		mov @bfh.bfReserved1,cx
		mov @bfh.bfReserved2,cx
		mov @bfh.bfOffBits,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
		
		invoke wsprintf,addr @fn,$CTA0("%03d%1d.bmp"),_nNamePre,ebx
		invoke CreateFile,addr @fn,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
		cmp eax,-1
		je _Ex
		mov @hFile,eax
		invoke WriteFile,@hFile,addr @bfh,sizeof(BITMAPFILEHEADER),offset dwTemp,0
		invoke WriteFile,@hFile,addr [edi].Hdr,sizeof(BITMAPINFOHEADER),offset dwTemp,0
		invoke WriteFile,@hFile,[edi].lpDib,[edi].Hdr.biSizeImage,offset dwTemp,0
		invoke CloseHandle,@hFile
		
		mov edi,[edi].Next
		inc ebx
	.endw
	assume edi:nothing
	
	xor eax,eax
_Ex:
	ret
_SaveBmp endp

_GetLinkW proc uses esi edi ebx _lpStr,_lpLinks
	LOCAL @nValue
	mov esi,_lpStr
	mov edi,_lpLinks
	assume edi:ptr _PicLink
	xor ebx,ebx
	.while word ptr [esi]
		.if dword ptr [esi]==2f002fh
			.while dword ptr [esi]!=0a000dh
				cmp word ptr [esi],0
				je _Ex
				add esi,2				
			.endw
			add esi,4
			.continue
		.elseif dword ptr [esi]==0a000dh
			add esi,4
			.continue
		.endif
		mov eax,esi
		.while word ptr [esi]!=9
			.if word ptr [esi]==0dh
				add esi,4
				.continue
			.endif
			add esi,2
		.endw
		mov ecx,esi
		sub ecx,eax
		.continue .if ecx>=78
		mov esi,eax
		mov edx,edi
		rep movsb
		mov word ptr [edi],0
		add esi,2
		mov edi,edx
		mov eax,esi
		.while word ptr [esi]!=0dh
			.break .if !word ptr [esi]
			add esi,2
		.endw
		mov word ptr [esi],0
		lea ecx,@nValue
		invoke StrToIntExW,eax,0,ecx
		mov word ptr [esi],0dh
		mov ecx,@nValue
		mov [edi].PicIdx,ecx
		.if ecx>=1001 && ecx<=1084
			add edi,sizeof _PicLink
			inc ebx
		.endif
		add esi,4
		.break .if ebx>=99
	.endw
	assume edi:nothing
_Ex:
	ret
_GetLinkW endp

_MakeLink proc uses esi edi ebx _hFile,_nLine,_nCol,_lpszItem
	LOCAL @szTmp[160]:byte
	
	lea esi,Links
	assume esi:ptr _PicLink
	xor ebx,ebx
	.repeat
		mov edi,_lpszItem
		xor ax,ax
		or ecx,-1
		repne scasw
		not ecx
		lea eax,[ecx-1]
		mov edi,_lpszItem
		mov edx,esi
		repe cmpsw
		.if ZERO?
			mov esi,edx
			invoke wsprintfW,addr @szTmp,$CTW0("%d,%d,%d,%d,"),_nCol,_nLine,eax,[esi].PicIdx
			shl eax,1
			invoke WriteFile,_hFile,addr @szTmp,eax,offset dwTemp,0
			xor eax,eax
			ret
		.endif
		lea esi,[edx+sizeof _PicLink]
		inc ebx
	.until !word ptr [esi] || ebx>=99
	assume esi:nothing
	mov eax,1
	ret
_MakeLink endp

end start