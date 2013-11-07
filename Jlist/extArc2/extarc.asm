.486
.model flat,stdcall
option casemap:none


include extarc.inc
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

_LzssUnc proc uses esi edi ebx _lpDest,_nDestLen,_lpSrc,_nSrcLen
	LOCAL @win[4096]:byte
	LOCAL @pDestEnd,@pSrcEnd
	lea edi,@win
	mov ecx,4096/4
	xor eax,eax
	rep stosd
	mov esi,_lpSrc
	mov ecx,esi
	add ecx,_nSrcLen
	mov @pSrcEnd,ecx
	mov edi,_lpDest
	mov eax,edi
	add eax,_nDestLen
	mov @pDestEnd,eax
	lea ebx,@win
	xor edx,edx
	push ebp
	mov ebp,1
	.repeat
		shr dx,1
		.if !(dx&0100h)
			.break .if esi>=[esp+10h]
			mov al,[esi]
			inc esi
			mov dl,al
			or dx,0ff00h
		.endif
		.if dl&1
			.break .if esi>=[esp+10h]
			.break .if edi>=[esp+14h]
			mov al,[esi]
			inc esi
			mov [edi],al
			inc edi
			mov [ebx+ebp],al
			inc ebp
			and ebp,0fffh
			.continue
		.else
			lea eax,[esi+1]
			.break .if eax>=[esp+10h]
			xor eax,eax
			xor ecx,ecx
			mov ah,[esi]
			mov al,[esi+1]
			add esi,2
			mov cl,al
			shr ax,4
			and cl,0fh
			add ecx,2
			push edx
			mov edx,eax
			@@:
				.if edi>=[esp+18h]
					pop eax
					.break
				.endif
				mov al,[ebx+edx]
				mov [edi],al
				inc edi
				mov [ebx+ebp],al
				inc edx
				and edx,0fffh
				inc ebp
				and ebp,0fffh
				dec ecx
			jnz @B
			pop edx
		.endif
	.until 0
	pop ebp
	sub edi,_lpDest
	mov eax,_nDestLen
	sub eax,edi
	sub esi,_lpSrc
	sub esi,_nSrcLen
	or eax,esi
	ret
_LzssUnc endp

_LzssPack proc uses esi edi _lpDest,_lpdwDestLen,_lpSrc,_nSrcLen
	mov ecx,_nSrcLen
	mov edi,_lpDest
	mov esi,_lpSrc
	.while ecx
		.if ecx>=8
			sub ecx,8
			mov al,0ffh
			stosb
			movsd
			movsd
		.else
			mov al,0ffh
			stosb
			rep movsb
			.break
		.endif
	.endw
	sub edi,_lpDest
	mov eax,_lpdwDestLen
	mov [eax],edi
	xor eax,eax
	ret
_LzssPack endp

_DlgMainProc proc uses ebx edi esi hwnd,uMsg,wParam,lParam
	local @opFileName:OPENFILENAME
	mov eax,uMsg
	.if eax==WM_COMMAND
		mov eax,wParam
		.if ax==IDC_REXP
			invoke GetDlgItem,hwnd,IDC_DIR
			invoke EnableWindow,eax,FALSE
			invoke GetDlgItem,hwnd,IDC_BROWSEDIR
			invoke EnableWindow,eax,FALSE
		.elseif ax==IDC_RCOMP
			invoke GetDlgItem,hwnd,IDC_DIR
			invoke EnableWindow,eax,TRUE
			invoke GetDlgItem,hwnd,IDC_BROWSEDIR
			invoke EnableWindow,eax,TRUE
		.elseif ax==IDC_BROWSEORI
			mov ecx,sizeof @opFileName
			lea edi,@opFileName
			xor eax,eax
			rep stosb
			mov @opFileName.lStructSize,sizeof @opFileName
			push hWinMain
			pop @opFileName.hwndOwner
			mov @opFileName.lpstrFilter,offset szFormat2
			mov @opFileName.lpstrFile,offset szArcName
			mov @opFileName.nMaxFile,1024
			mov @opFileName.Flags,OFN_FILEMUSTEXIST OR OFN_PATHMUSTEXIST
			lea eax,@opFileName
			invoke GetOpenFileName,eax
			.if eax
				invoke SetDlgItemText,hwnd,IDC_ORI,offset szArcName
			.endif
		.elseif ax==IDC_BROWSEDIR
			invoke _BrowseFolder,hwnd,offset szDirName
			.if eax
				invoke SetDlgItemText,hwnd,IDC_DIR,offset szDirName
			.endif
		.elseif ax==IDC_START
			invoke IsDlgButtonChecked,hwnd,IDC_REXP
			.if eax==BST_CHECKED
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szArcName,1024
				invoke IsDlgButtonChecked,hwnd,IDC_RAWFORMAT
				mov bRawFormat,eax
				invoke CreateThread,0,0,_ArcUpk,0,0,0
;				invoke _ArcUpk,offset szArcName
			.else
				invoke GetDlgItemText,hwnd,IDC_ORI,offset szArcName,1024
				invoke GetDlgItemText,hwnd,IDC_DIR,offset szDirName,1024
				invoke _ArcPack,offset szArcName,offset szDirName
			.endif
		.endif
	.elseif eax==WM_INITDIALOG
		invoke LoadIcon,hInstance,IDI_APPLICATION
		invoke SendMessage,hwnd,WM_SETICON,ICON_BIG,eax
		mov eax,hwnd
		mov hWinMain,eax
		invoke CheckDlgButton,hwnd,IDC_REXP,BST_CHECKED
	.elseif eax==WM_CLOSE
		invoke EndDialog,hwnd,0
	.else
		mov eax,FALSE
		ret
	.endif
	
	mov eax,TRUE
	ret
_DlgMainProc endp

_AllocBmp proc uses esi edi _lpWEntry,_lpData,_bit
	LOCAL @lpBmp,@pallete
	mov esi,_lpWEntry
	mov eax,_bit
	.if ax!=18h && ax!=8 && ax!=32
		jmp _Err
	.endif
	
	assume esi:ptr _WipfEntry
	mov eax,[esi].width1
	mul [esi].height
	
	mov edi,esi
	.if word ptr _bit==18h
		lea eax,[eax+eax*2]
		lea esi,[eax+sizeof BITMAPFILEHEADER+sizeof BITMAPINFOHEADER]
	.elseif word ptr _bit==20h
		shl eax,2
		lea esi,[eax+sizeof BITMAPFILEHEADER+sizeof BITMAPINFOHEADER]
	.else
		lea esi,[eax+sizeof BITMAPFILEHEADER+sizeof BITMAPINFOHEADER+400h]
	.endif
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,esi
	.if !eax
		int 3
	.endif
	mov @lpBmp,eax
	mov edx,esi
	mov esi,eax
	assume esi:ptr BITMAPFILEHEADER
	mov [esi].bfType,'MB'
	mov [esi].bfSize,edx
	.if word ptr _bit!=8
		mov [esi].bfOffBits,sizeof BITMAPFILEHEADER+sizeof BITMAPINFOHEADER
	.else
		mov [esi].bfOffBits,sizeof BITMAPFILEHEADER+sizeof BITMAPINFOHEADER+400h
	.endif
	add esi,sizeof BITMAPFILEHEADER
	assume esi:ptr BITMAPINFOHEADER
	mov [esi].biSize,sizeof BITMAPINFOHEADER
	mov eax,_WipfEntry.width1[edi]
	mov [esi].biWidth,eax
	mov eax,_WipfEntry.height[edi]
	neg eax
	mov [esi].biHeight,eax
	mov [esi].biPlanes,1
	mov eax,_bit
	mov [esi].biBitCount,ax
	assume esi:nothing
	.if ax==8
		mov eax,_lpData
		lea edi,[esi+sizeof BITMAPINFOHEADER]
		mov ecx,400h/2
		xchg eax,esi
		rep movsw
		mov esi,eax
	.endif
	lea eax,[esi-sizeof BITMAPFILEHEADER]
	
	ret
_Err:
	xor eax,eax
	ret
_AllocBmp endp

_SwapBmp proc uses esi edi ebx _lpBmp
	LOCAL @w,@h,@temp,@bc
	LOCAL @lpTemp
	mov esi,_lpBmp
	mov ebx,BITMAPFILEHEADER.bfOffBits[esi]
	
	add esi,sizeof BITMAPFILEHEADER
	assume esi:ptr BITMAPINFOHEADER
	mov eax,[esi].biWidth
	mov @w,eax
	mov ecx,[esi].biHeight
	neg ecx
	mov @h,ecx
	
	cmp [esi].biBitCount,18h
	je _lbl1
	cmp [esi].biBitCount,20h
	jne _Err
_lbl1:
	xor eax,eax
	mov ax,[esi].biBitCount
	mov @bc,eax
	 
	mov esi,_lpBmp
	mov ecx,BITMAPFILEHEADER.bfSize[esi]
	invoke HeapAlloc,hHeap,0,ecx
	.if !eax
		int 3
	.endif
	mov edx,BITMAPFILEHEADER.bfOffBits[esi]
	mov @lpTemp,eax
	mov edi,eax
	mov ecx,sizeof BITMAPFILEHEADER+sizeof BITMAPINFOHEADER
	rep movsb
	assume esi:nothing
	mov edi,@lpTemp
	add edi,edx
	
	mov ebx,edi
	mov eax,@w
	mul @h
	mov @temp,eax
	.if @bc==18h
		xor ecx,ecx
		.while ecx<3
			mov edx,@temp
			lea edi,[ebx+ecx]
			lp2:
				mov al,[esi]
				inc esi
				mov [edi],al
				add edi,3
			dec edx
			jnz lp2
			inc ecx
		.endw
	.else
		mov ecx,1
		.while ecx<=4
			mov edx,@temp
			mov eax,4
			sub eax,ecx
			lea edi,[ebx+eax]
			lp3:
				mov al,[esi]
				inc esi
				mov [edi],al
				add edi,4
			dec edx
			jnz lp3
			inc ecx
		.endw
	.endif
	invoke HeapFree,hHeap,0,_lpBmp
	
	mov eax,@lpTemp
	
	ret
_Err:
	xor eax,eax
	ret
_SwapBmp endp

_MakeSingleBmp proc uses edi _lpWipfEntry,_lpData,_bit
	mov eax,_bit
	invoke _AllocBmp,_lpWipfEntry,_lpData,eax
	test eax,eax
	jz _Err
	mov edi,eax
	mov ecx,_lpWipfEntry
	push _WipfEntry.data_len[ecx]
	mov edx,_lpData
	.if word ptr _bit==8
		lea edx,[edx+400h]
	.endif
	push edx
	mov eax,BITMAPFILEHEADER.bfOffBits[edi]
	mov ecx,BITMAPFILEHEADER.bfSize[edi]
	sub ecx,eax
	push ecx
	add eax,edi
	push eax
	call _LzssUnc
	mov eax,_bit
	.if ax==18h || ax==20h
		invoke _SwapBmp,edi
		.if !eax
			int 3
		.endif
		mov edi,eax
	.endif
	mov eax,edi
	ret
_Err:
	xor eax,eax
	ret
_MakeSingleBmp endp

_ArcUpk proc uses esi edi ebx _rar
	LOCAL @hArcFile,@nFileSize,@hArcFileMap,@lpArcFile
	LOCAL @dbBytes[100h]:byte
	LOCAL @fname[64]:byte
	LOCAL @type,@temp,@temp2,@temp3
	LOCAL @pname,@bit,@piccnt,@pData
	LOCAL @lpEntry,@lpUnc
	LOCAL @nSECount
	invoke CreateFile,offset szArcName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hArcFile,eax
	
	mov edi,offset szArcName
	xor al,al
	or ecx,-1
	repne scasb
	sub edi,5
	.if byte ptr [edi]=='.'
		mov byte ptr [edi],0
	.else
		mov word ptr [edi],'1'
	.endif
	invoke GetCurrentDirectory,1024,offset szDirName
	invoke CreateDirectory,offset szArcName,0
	invoke SetCurrentDirectory,offset szArcName
	.if !eax
		invoke CloseHandle,@hArcFile
		invoke MessageBox,hWinMain,$CTA0("无法访问目录！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	
	invoke GetDlgItem,hWinMain,IDC_START
	invoke EnableWindow,eax,FALSE
	
	invoke ReadFile,@hArcFile,addr @dbBytes,100h,offset dwTemp,0
	lea esi,@dbBytes
	mov ebx,[esi]
	add esi,4
	test ebx,ebx
	jz _Over
	lp1:
		mov eax,[esi]
		mov @type,eax
		push ebx
		
		mov ebx,[esi+4]
		invoke SetFilePointer,@hArcFile,dword ptr [esi+8],0,FILE_BEGIN
		
		invoke SendDlgItemMessage,hWinMain,IDC_PGB,PBM_SETRANGE32,0,ebx
		mov @nSECount,0
		
		imul edi,ebx,sizeof(_Entry)
		invoke HeapAlloc,hHeap,0,edi
		.if !eax
			invoke CloseHandle,@hArcFile
			pop ebx
			jmp _NomemAU
		.endif
		mov @lpEntry,eax
		invoke ReadFile,@hArcFile,@lpEntry,edi,offset dwTemp,0
		
		mov edi,@lpEntry
		assume edi:ptr _Entry
		lp2:
			invoke lstrcpy,addr @fname,edi
;			invoke lstrcmp,addr @fname,$CTA0("SEPFRM")
;			.if !eax
;				int 3
;			.endif
			
			invoke SetFilePointer,@hArcFile,[edi].offset1,0,FILE_BEGIN
			invoke HeapAlloc,hHeap,0,[edi].len
			.if !eax
				int 3
			.endif
			mov @lpUnc,eax
			invoke ReadFile,@hArcFile,@lpUnc,[edi].len,offset dwTemp,0
			mov eax,@type
			.if !bRawFormat && (eax=='PIW' || eax=='KSM' || eax=='PAW')
				mov eax,@lpUnc
				mov @temp,edi
				.if _WipfHdr.pic_count[eax]==1
					lea ecx,[eax+sizeof _WipfHdr]
					mov ax,_WipfHdr.bit_count[eax]
					.if ax==0
						mov ax,32
					.endif
					lea edx,[ecx+sizeof _WipfEntry]
					invoke _MakeSingleBmp,ecx,edx,eax
					test eax,eax
					jz @F
					mov edi,eax
					.if @type=='PIW'
						invoke lstrcat,addr @fname,$CTA0(".bmp")
					.elseif @type=='PAW'
						invoke lstrcat,addr @fname,$CTA0("~.bmp")
					.else
						invoke lstrcat,addr @fname,$CTA0("$.bmp")
					.endif
					invoke CreateFile,addr @fname,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
					.if eax==-1
						int 3
					.endif
					push eax
					mov ecx,BITMAPFILEHEADER.bfSize[edi]
					invoke WriteFile,eax,edi,ecx,offset dwTemp,0
					call CloseHandle
					invoke HeapFree,hHeap,0,edi
				.else
					mov @temp2,ebx
					mov @temp3,esi
					lea esi,[eax+sizeof _WipfHdr]
					xor ecx,ecx
					mov cx,_WipfHdr.pic_count[eax]
					mov @piccnt,ecx
					shl ecx,3
					lea ecx,[ecx+ecx*2]
					add ecx,esi
					mov @pData,ecx
					mov ax,_WipfHdr.bit_count[eax]
					.if ax==0
						mov ax,32
					.endif
					mov @bit,eax
					.if @type=='PIW'
						invoke lstrcat,addr @fname,$CTA0("\\")
					.elseif @type=='PAW'
						invoke lstrcat,addr @fname,$CTA0("~\\")
					.else
						invoke lstrcat,addr @fname,$CTA0("$\\")
					.endif
					invoke lstrlen,addr @fname
					lea ecx,@fname
					add eax,ecx
					mov @pname,eax
					invoke CreateDirectory,addr @fname,0
					
					xor ebx,ebx
					.while ebx<@piccnt
						invoke _MakeSingleBmp,esi,@pData,@bit
						.if !eax
							int 3
						.endif
						mov edi,eax
						invoke wsprintf,@pname,$CTA0("%04d.bmp"),ebx
						invoke CreateFile,addr @fname,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
						.if eax==-1
							int 3
						.endif
						push eax
						mov ecx,BITMAPFILEHEADER.bfSize[edi]
						invoke WriteFile,eax,edi,ecx,offset dwTemp,0
						call CloseHandle
						invoke HeapFree,hHeap,0,edi
						mov eax,_WipfEntry.data_len[esi]
						add @pData,eax
						.if word ptr @bit==8
							add @pData,400h
						.endif
						add esi,sizeof _WipfEntry
						inc ebx
					.endw
					mov esi,@temp3
					mov ebx,@temp2
				.endif
				mov edi,@temp
			.else
			@@:
				invoke lstrcat,addr @fname,$CTA0('.')
				invoke lstrcat,addr @fname,addr @type
				invoke CreateFile,addr @fname,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
				.if eax==-1
					int 3
				.endif
				push eax
				invoke WriteFile,eax,@lpUnc,[edi].len,offset dwTemp,0
				call CloseHandle
			.endif
			invoke HeapFree,hHeap,0,@lpUnc
			add edi,sizeof _Entry
			
			inc @nSECount
			invoke SendDlgItemMessage,hWinMain,IDC_PGB,PBM_SETPOS,@nSECount,0
			
		dec ebx
		jnz lp2
		assume edi:nothing
		invoke HeapFree,hHeap,0,@lpEntry
		
		pop ebx
		add esi,12
		dec ebx
	jnz lp1
	
	
_Over:
	invoke GetDlgItem,hWinMain,IDC_START
	invoke EnableWindow,eax,TRUE
	invoke CloseHandle,@hArcFile
	invoke SetCurrentDirectory,offset szDirName
	invoke MessageBox,hWinMain,$CTA0("解包完成！"),0,MB_OK
	ret
_NomemAU:
	invoke GetDlgItem,hWinMain,IDC_START
	invoke EnableWindow,eax,TRUE
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_ICONERROR or MB_OK
	ret
_ArcUpk endp

_ArcPack proc _lpszName,_lpszDir
	LOCAL @hArcFile,@nFileSize
	LOCAL @hdr[10h]:byte
	LOCAL @hNewFile,@lpNewFile,@nNewFileSize,@lpC
	LOCAL @lpEntry,@nEntries
	LOCAL @nIndexPos
	LOCAL @bHasErr
	
	invoke CreateFile,_lpszName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke MessageBox,hWinMain,$CTA0("无法打开文件！"),0,MB_ICONERROR or MB_OK
		ret
	.endif
	mov @hArcFile,eax
	
	invoke CloseHandle,@hArcFile
	
	invoke MessageBox,hWinMain,$CTA0("封装完成！"),0,MB_OK
	ret
_Nomem:
	invoke MessageBox,hWinMain,$CTA0("内存不足！"),0,MB_OK or MB_ICONERROR
	ret
_ArcPack endp

_MakeDir proc uses edi _lpszName
	mov edi,_lpszName
	xor al,al
	or ecx,-1
	repne scasb
	mov edx,edi
	dec edi
	.while byte ptr [edi]!='.'
		dec edi
		.if byte ptr [edi]=='\'
			mov word ptr [edx],'1'
			jmp @F
		.endif
		cmp edi,_lpszName
		je _ErrMD
	.endw
	mov byte ptr [edi],0
	@@:
	invoke SetCurrentDirectory,_lpszName
	.if !eax
		invoke CreateDirectory,_lpszName,0
		or eax,eax
		je _ErrMD
		invoke SetCurrentDirectory,_lpszName
		or eax,eax
		je _ErrMD
	.endif
	mov eax,1
	ret
_ErrMD:
	xor eax,eax
	ret
_MakeDir endp
end start