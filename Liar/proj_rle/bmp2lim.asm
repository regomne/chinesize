.486
.model flat,stdcall
option casemap:none


include rlecmpr.inc
include _BrowseFolder.asm

.code

start:
;……………………………………………………………………………………
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax


invoke DialogBoxParam,hInstance,IDD_DLG1,0,offset _DlgMainProc,NULL
invoke ExitProcess,NULL

;……………………………………………………………………………………

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
				;int 3
			.endif
			mov hBmpFile,eax
			
			mov ecx,16
			lea edi,@pMemory
			xor eax,eax
			rep stosd
			invoke _FoldPic,hBmpFile,addr @pMemory
			.if !eax
				mov ebx,6
				.while ebx
					lea edi,@pMemory
					lea eax,[edi+ecx*4]
					invoke GlobalFree,eax
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
			mov dword ptr [edi],'mil.'
			invoke CreateFile,offset szFileNameOri,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
			.if eax==-1
				;int3
			.endif
			push eax
			invoke WriteFile,eax,lpLimBuff,nLimSize,offset dwTemp,0
			call CloseHandle
			invoke CloseHandle,hBmpFile
			invoke GlobalFree,lpLimBuff
			
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
						mov ebx,6
						.while ebx
							lea edi,@pMemory
							lea eax,[edi+ecx*4]
							invoke GlobalFree,eax
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
					mov dword ptr [edi],'mil.'
					invoke CreateFile,esi,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
					.if eax==-1
						;int3
					.endif
					push eax
					invoke WriteFile,eax,lpLimBuff,nLimSize,offset dwTemp,0
					call CloseHandle
					invoke CloseHandle,hBmpFile
					invoke GlobalFree,lpLimBuff
					invoke CloseHandle,hBmpFile
					@@:
					invoke FindNextFile,hFindFile,addr @stFindData
				.until eax==FALSE
				invoke FindClose,hFindFile
			.endif
			
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
_FoldPic proc _hBmpFile,_pMemory
	local @nBmpFileSize,@lpBmpFile
	local @lpDIB16,@lpAlphaData
	local @lpBGRTable,@lpCmprBGR,@lpCmprAlpha
	local @dbAlphaTable[256]:byte,@dbAlphaFreqTable[1024]:byte
	local @lpBGRFreqTable
	local @nBGRCodes,@nAlphaCodes
	local @sTotal:_MyRLEData,@sAlpha:_MyRLEData,@sBGR:_MyRLEData
	local @lpFinalFile
	
	
	invoke GetFileSize,_hBmpFile,NULL
	mov @nBmpFileSize,eax
	invoke GlobalAlloc,GPTR,@nBmpFileSize
	test eax,eax
	je Err02
	mov @lpBmpFile,eax
	mov ecx,_pMemory
	mov [ecx],eax
	invoke ReadFile,_hBmpFile,@lpBmpFile,@nBmpFileSize,offset dwTemp,0
	test eax,eax
	je Err02
	mov edi,@lpBmpFile
	cmp word ptr [edi+1ch],20h
	jne Err03
	cmp word ptr [edi+1eh],0
	jne Err03
	mov eax,[edi+0ah]
	add eax,edi
	mov @sTotal.pSrc,eax
	mov eax,[edi+22h]
	mov @sTotal.nSrcLen,eax
	shr eax,1
	mov ebx,eax
	mov @sTotal.nDestSize,eax
	mov @sBGR.nSrcLen,eax
	invoke GlobalAlloc,GPTR,eax;24位转16位之后存储区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+4],eax
	mov @lpDIB16,eax
	mov @sTotal.pDest,eax
	mov @sBGR.pSrc,eax
	shr ebx,1
	mov @sTotal.nAlphaSize,ebx
	mov @sAlpha.nSrcLen,ebx
	invoke GlobalAlloc,GPTR,ebx;Alpha通道存储区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+8],eax
	mov @lpAlphaData,eax
	mov @sTotal.pDestAlpha,eax
	mov @sAlpha.pSrc,eax
	mov eax,[edi+12h]
	mov ecx,[edi+16h]
	invoke _ConvertDIB32to16,addr @sTotal,eax,ecx
	test eax,eax
	je Err02
	
	invoke GlobalAlloc,GPTR,65536*4;BGR Frequence Table区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+12],eax
	mov @lpBGRFreqTable,eax
	mov edi,@sBGR.pSrc
	mov ecx,@sBGR.nSrcLen
	invoke _PrepareFreqTable,edi,ecx,@lpBGRFreqTable,65536*4
	test eax,eax
	je Err02
	mov @nBGRCodes,eax
	
	shl eax,1
	mov @sBGR.nTableSize,eax
	invoke GlobalAlloc,GPTR,eax;16位DIB压缩后的Code Table区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+16],eax
	mov @lpBGRTable,eax
	mov @sBGR.pCodeTable,eax
	
	mov eax,@sTotal.nDestSize
	mov @sBGR.nDestSize,eax
	invoke GlobalAlloc,GPTR,eax;16位DIB压缩后的数据区
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+20],eax
	mov @lpCmprBGR,eax
	mov @sBGR.pDest,eax
	invoke _RLECompress,addr @sBGR,@lpBGRFreqTable,@nBGRCodes
	test eax,eax
	je Err04
	
	invoke GlobalFree,@lpDIB16
	mov ecx,_pMemory
	mov dword ptr [ecx+4],0
	invoke GlobalFree,@lpBGRFreqTable
	mov ecx,_pMemory
	mov dword ptr [ecx+16],0
	
	mov edi,@sAlpha.pSrc
	mov ecx,@sAlpha.nSrcLen
	invoke _PrepareFreqTable2,edi,ecx,addr @dbAlphaFreqTable,1024
	test eax,eax
	je Err02
	mov @nAlphaCodes,eax
	
	mov eax,@sTotal.nAlphaSize
	mov @sAlpha.nDestSize,eax
	invoke GlobalAlloc,GPTR,eax;Alpha数据被压缩后的数据区
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+24],eax
	mov @sAlpha.pDest,eax
	mov @lpCmprAlpha,eax
	lea eax,@dbAlphaTable
	mov @sAlpha.pCodeTable,eax
	mov @sAlpha.nTableSize,256
	invoke _RLECompress2,addr @sAlpha,addr @dbAlphaFreqTable,@nAlphaCodes
	test eax,eax
	je Err04
	
	invoke GlobalFree,@lpAlphaData
	mov ecx,_pMemory
	mov dword ptr [ecx+8],0
	
	mov eax,@sAlpha.nDestSize
	add eax,@sAlpha.nTableSize
	add eax,@sBGR.nDestSize
	add eax,@sBGR.nTableSize
	add eax,1000
	invoke GlobalAlloc,GPTR,eax;最终文件区域
	test eax,eax
	je Err02
	mov @lpFinalFile,eax

	mov ecx,eax
	mov edi,@lpFinalFile
	mov eax,3324d4ch
	stosd
	mov eax,80010h
	stosd
	mov esi,@lpBmpFile
	add esi,12h
	movsd
	movsd
	mov eax,@sBGR.nSrcLen
	stosd
	mov eax,@sBGR.nDestSize
	stosd
	mov eax,@nBGRCodes
	stosw
	mov ax,76h
	stosw
	mov ecx,@sBGR.nTableSize
	mov esi,@sBGR.pCodeTable
	call _memcpy
	mov ecx,@sBGR.nDestSize
	mov esi,@sBGR.pDest
	call _memcpy
	mov eax,@sAlpha.nSrcLen
	stosd
	mov eax,@sAlpha.nDestSize
	stosd
	mov eax,@nAlphaCodes
	stosd
	mov ecx,@sAlpha.nTableSize
	mov esi,@sAlpha.pCodeTable
	call _memcpy
	mov ecx,@sAlpha.nDestSize
	mov esi,@sAlpha.pDest
	call _memcpy
	
	invoke GlobalFree,@lpBmpFile
	invoke GlobalFree,@lpBGRTable
	invoke GlobalFree,@lpCmprBGR
	invoke GlobalFree,@lpCmprAlpha
	push edi
	mov edi,_pMemory
	mov ecx,16
	xor eax,eax
	rep stosd
	pop edi	
	
	sub edi,@lpFinalFile
	mov ecx,edi
	mov eax,@lpFinalFile
	
	ret
	Err02:
	mov ecx,1
	xor eax,eax
	ret
	Err03:
	mov ecx,2
	xor eax,eax
	ret
	Err04:
	mov ecx,3
	xor eax,eax
	ret
_FoldPic endp
;
_RLECompress proc uses ebx esi edi,_psCmpr:ptr _MyRLEData,_pFreqTable,_nCodes
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov eax,_nCodes
	shl eax,1
	.if eax>[edx].nTableSize
		mov [edx].nErrCode,1
		xor eax,eax
		ret
	.endif
	mov [edx].nTableSize,eax
	mov edi,[edx].pCodeTable
	mov esi,_pFreqTable
	assume edx:nothing
	
	xor ecx,ecx
	.while ecx<=65535
		movzx edx,word ptr [esi+ecx*4+2]
		.if edx
			dec edx
			mov [edi+edx*2],cx
		.endif
		inc ecx
	.endw
	
	push _nCodes
	push _pFreqTable
	push _psCmpr
	.if _nCodes<1000h
		call _RLECompressShort
	.else
		call _RLECompressLong
	.endif
	ret
_RLECompress endp
;
_RLECompressShort proc uses ebx edi esi,_psCmpr:ptr _MyRLEData,_pFreqTable,_nCodes
	local @pSrcEnd
	local @Bits:_Bits
	mov @Bits.curbits,8
	mov @Bits.curbyte,1
	mov @Bits.cache,0
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov esi,[edx].pSrc
	mov edi,[edx].pDest
	mov @Bits.stream,edi
	mov ebx,_pFreqTable
	mov eax,[edx].nDestSize
	mov @Bits.stream_length,eax
	mov eax,[edx].nSrcLen
	add eax,esi
	mov @pSrcEnd,eax
	xor eax,eax
	assume edx:nothing
	
	;int 3
	
	.while esi<@pSrcEnd
		movzx eax,word ptr [esi]
		.if ax==[esi+2]
			movzx edx,word ptr [ebx+eax*4+2]
			mov ecx,2
			push edx
			lea edx,[esi+ecx*2]
			.while ax==[edx]
				inc ecx
				.break .if ecx>=17
				lea edx,[esi+ecx*2]
				.if edx>=@pSrcEnd
					dec ecx
					.break
				.endif
			.endw
			pop edx
			lea esi,[esi+ecx*2-2]
			invoke _BitsPutHigh,addr @Bits,3,0
			test eax,eax
			je ErrNotEnoughMemory
			sub ecx,2
			shl ecx,28
			invoke _BitsPutHigh,addr @Bits,4,ecx
			test eax,eax
			je ErrNotEnoughMemory
			jmp @F

		.else
			movzx edx,word ptr [ebx+eax*4+2]
			@@:
			dec edx
			jne @F
			mov ecx,1
			jmp label02
			@@:
			shl edx,20
			mov ecx,12
			@@:
				test edx,80000000h
				jne @F
				shl edx,1
			loop @B
			@@:
			.if ecx<=6
			label02:
				mov eax,ecx
				shl eax,29
				invoke _BitsPutHigh,addr @Bits,3,eax
				test eax,eax
				je ErrNotEnoughMemory
				.if ecx!=1
					xor edx,80000000h
					shl edx,1
					dec ecx
				.endif
				invoke _BitsPutHigh,addr @Bits,ecx,edx
				test eax,eax
				je ErrNotEnoughMemory
			.else
				or eax,-1
				invoke _BitsPutHigh,addr @Bits,3,eax
				test eax,eax
				je ErrNotEnoughMemory
				mov eax,ecx
				sub eax,7
				invoke _BitsPutHigh,addr @Bits,eax,-1
				invoke _BitsPutHigh,addr @Bits,1,0
				test eax,eax
				je ErrNotEnoughMemory
				xor edx,80000000h
				shl edx,1
				dec ecx
				invoke _BitsPutHigh,addr @Bits,ecx,edx
				test eax,eax
				je ErrNotEnoughMemory
			.endif
		.endif
		add esi,2
	.endw
	;int 3
	cmp esi,@pSrcEnd
	jne @F
	mov eax,@Bits.curbyte
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov [edx].nDestSize,eax
	mov eax,1
	ret
	@@:
	mov edx,_psCmpr
	mov [edx].nErrCode,2
	xor eax,eax
	ret
	ErrNotEnoughMemory:
	mov edx,_psCmpr
	mov [edx].nErrCode,1
	xor eax,eax
	assume edx:nothing
	ret
_RLECompressShort endp
;
_RLECompressLong proc uses ebx edi esi,_psCmpr:ptr _MyRLEData,_pFreqTable,_nCodes
	local @pSrcEnd
	local @Bits:_Bits
	mov @Bits.curbits,8
	mov @Bits.curbyte,1
	mov @Bits.cache,0
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov esi,[edx].pSrc
	mov edi,[edx].pDest
	mov @Bits.stream,edi
	mov ebx,_pFreqTable
	mov eax,[edx].nDestSize
	mov @Bits.stream_length,eax
	mov eax,[edx].nSrcLen
	add eax,esi
	mov @pSrcEnd,eax
	xor eax,eax
	assume edx:nothing
	
	;int 3
	
	.while esi<@pSrcEnd
		movzx eax,word ptr [esi]
		.if ax==[esi+2]
			movzx edx,word ptr [ebx+eax*4+2]
			mov ecx,2
			push edx
			lea edx,[esi+ecx*2]
			.while ax==[edx]
				inc ecx
				.break .if ecx>=17
				lea edx,[esi+ecx*2]
				.if edx>=@pSrcEnd
					dec ecx
					.break
				.endif
			.endw
			pop edx
			lea esi,[esi+ecx*2-2]
			invoke _BitsPutHigh,addr @Bits,4,0
			test eax,eax
			je ErrNotEnoughMemory
			sub ecx,2
			shl ecx,28
			invoke _BitsPutHigh,addr @Bits,4,ecx
			test eax,eax
			je ErrNotEnoughMemory
			jmp @F

		.else
			movzx edx,word ptr [ebx+eax*4+2]
			@@:
			dec edx
			jne @F
			mov ecx,1
			jmp label02
			@@:
			shl edx,16
			mov ecx,16
			@@:
				test edx,80000000h
				jne @F
				shl edx,1
			loop @B
			@@:
			.if ecx<=14
			label02:
				mov eax,ecx
				shl eax,28
				invoke _BitsPutHigh,addr @Bits,4,eax
				test eax,eax
				je ErrNotEnoughMemory
				.if ecx!=1
					xor edx,80000000h
					shl edx,1
					dec ecx
				.endif
				invoke _BitsPutHigh,addr @Bits,ecx,edx
				test eax,eax
				je ErrNotEnoughMemory
			.else
				or eax,-1
				invoke _BitsPutHigh,addr @Bits,4,eax
				test eax,eax
				je ErrNotEnoughMemory
				mov eax,ecx
				sub eax,15
				invoke _BitsPutHigh,addr @Bits,eax,-1
				invoke _BitsPutHigh,addr @Bits,1,0
				test eax,eax
				je ErrNotEnoughMemory
				xor edx,80000000h
				shl edx,1
				dec ecx
				invoke _BitsPutHigh,addr @Bits,ecx,edx
				test eax,eax
				je ErrNotEnoughMemory
			.endif
		.endif
		add esi,2
	.endw
	;int 3
	cmp esi,@pSrcEnd
	jne @F
	mov eax,@Bits.curbyte
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov [edx].nDestSize,eax
	mov eax,1
	ret
	@@:
	mov edx,_psCmpr
	mov [edx].nErrCode,2
	xor eax,eax
	ret
	ErrNotEnoughMemory:
	mov edx,_psCmpr
	mov [edx].nErrCode,1
	xor eax,eax
	assume edx:nothing
	ret
_RLECompressLong endp
;
_RLECompress2 proc uses ebx edi esi,_psCmpr:ptr _MyRLEData,_pFreqTable,_nCodes
	local @pSrcEnd
	local @Bits:_Bits
	
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov eax,_nCodes
	.if eax>[edx].nTableSize
		mov [edx].nErrCode,1
		xor eax,eax
		ret
	.endif
	mov [edx].nTableSize,eax
	mov edi,[edx].pCodeTable
	mov esi,_pFreqTable
	assume edx:nothing
	
	xor ecx,ecx
	.while ecx<=255
		movzx edx,byte ptr [esi+ecx*4+2]
		.if edx
			dec edx
			mov [edi+edx],cl
		.endif
		inc ecx
	.endw
	
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	
	mov @Bits.curbits,8
	mov @Bits.curbyte,1
	mov @Bits.cache,0
	mov esi,[edx].pSrc
	mov edi,[edx].pDest
	mov @Bits.stream,edi
	mov eax,[edx].nDestSize
	mov @Bits.stream_length,eax
	mov eax,[edx].nSrcLen
	add eax,esi
	mov @pSrcEnd,eax
	mov ebx,_pFreqTable
	xor eax,eax
	assume edx:nothing

	.while esi<@pSrcEnd
		movzx eax,byte ptr [esi]
		.if al==[esi+1]
			movzx edx,word ptr [ebx+eax*4+2]
			mov ecx,2
			push edx
			lea edx,[esi+ecx*2]
			.while al==[edx]
				inc ecx
				.break .if ecx>=17
				lea edx,[esi+ecx]
				.if edx>=@pSrcEnd
					dec ecx
					.break
				.endif
			.endw
			pop edx
			lea esi,[esi+ecx-1]
			invoke _BitsPutHigh,addr @Bits,3,0
			test eax,eax
			je ErrNotEnoughMemory2
			sub ecx,2
			shl ecx,28
			invoke _BitsPutHigh,addr @Bits,4,ecx
			test eax,eax
			je ErrNotEnoughMemory2
			jmp @F

		.else
			movzx edx,word ptr [ebx+eax*4+2]
			@@:
			dec edx
			jne @F
			mov ecx,1
			jmp label02
			@@:
			shl edx,20
			mov ecx,12
			@@:
				test edx,80000000h
				jne @F
				shl edx,1
			loop @B
			@@:
			.if ecx<=6
			label02:
				mov eax,ecx
				shl eax,29
				invoke _BitsPutHigh,addr @Bits,3,eax
				test eax,eax
				je ErrNotEnoughMemory2
				.if ecx!=1
					xor edx,80000000h
					shl edx,1
					dec ecx
				.endif
				invoke _BitsPutHigh,addr @Bits,ecx,edx
				test eax,eax
				je ErrNotEnoughMemory2
			.else
				or eax,-1
				invoke _BitsPutHigh,addr @Bits,3,eax
				test eax,eax
				je ErrNotEnoughMemory2
				mov eax,ecx
				sub eax,7
				invoke _BitsPutHigh,addr @Bits,eax,-1
				invoke _BitsPutHigh,addr @Bits,1,0
				test eax,eax
				je ErrNotEnoughMemory2
				xor edx,80000000h
				shl edx,1
				dec ecx
				invoke _BitsPutHigh,addr @Bits,ecx,edx
				test eax,eax
				je ErrNotEnoughMemory2
			.endif
		.endif
		inc esi
	.endw

	mov eax,@Bits.curbyte
	mov edx,_psCmpr
	assume edx:ptr _MyRLEData
	mov [edx].nDestSize,eax
	mov eax,1
	ret
	ErrNotEnoughMemory2:
	mov edx,_psCmpr
	mov [edx].nErrCode,1
	xor eax,eax
	assume edx:nothing
	ret
_RLECompress2 endp
;
_ConvertDIB32to16 proc uses ebx edi esi,_psCvrt:ptr _MyRLEData,_Width:dword,_Height:dword
	local @nWid4,@nAlphaOff
	mov eax,_Width
	mov ecx,_Height
	dec ecx
	mul ecx
	shl eax,2
	mov ecx,_Width
	shl ecx,2
	mov @nWid4,ecx
	add ecx,eax
	mov edx,_psCvrt
	assume edx:ptr _MyRLEData
;	.if ecx!=[edx].nSrcLen
;	.endif
	shr ecx,1
	mov [edx].nSrcLen,ecx
	.if ecx>[edx].nDestSize
	@@:
		mov [edx].nErrCode,1
		xor eax,eax
		ret
	.endif
	shr ecx,1
	cmp ecx,[edx].nAlphaSize
	ja @B
	
	mov esi,[edx].pSrc
	add esi,eax
	mov ecx,_Height
	mov edi,[edx].pDest
	mov eax,[edx].pDestAlpha
	mov @nAlphaOff,eax
	assume edx:nothing
	.while ecx
		xor eax,eax
		.while eax<@nWid4
			xor edx,edx
			mov bl,[esi+eax]
			shl bx,8
			or dx,bx
			and dx,0f800h
			movzx ebx,byte ptr [esi+eax+1]
			shl bx,3
			or dx,bx
			and dx,0ffe0h
			movzx ebx,byte ptr [esi+eax+2]
			shr bl,3
			or dx,bx
			mov [edi],dx
			add edi,2
			push edi
			mov edi,@nAlphaOff
			mov bl,byte ptr [esi+eax+3]
;			not bl
			mov [edi],bl
			inc edi
			mov @nAlphaOff,edi
			pop edi
			add eax,4
		.endw
		sub esi,@nWid4
		dec ecx
	.endw
	mov eax,1
	ret
_ConvertDIB32to16 endp
;
_PrepareFreqTable proc uses ebx edi esi,_pSrc,_nSrcLen,_pOut,_pOutSize
	local @flag
	.if _pOutSize<65536*4
		xor eax,eax
		ret
	.endif
	mov edi,_pOut
	mov ecx,_pOutSize
	shr ecx,2
	xor eax,eax
	rep stosd
	mov ecx,_pOutSize
	and ecx,2
	rep stosb
	
	mov edi,_pOut
	mov esi,_pSrc
	mov ecx,_nSrcLen
	shr ecx,1
	.while ecx
		lodsw
		dec ecx
		mov bx,word ptr [edi+eax*4]
		.continue .if bx==0ffffh
		inc bx
		mov word ptr [edi+eax*4],bx
	.endw
	
	;int 3
	mov edx,edi
	mov ecx,1
	mov @flag,1
	.while @flag
		xor eax,eax
		mov @flag,0
		.while eax<65536
			mov bx,word ptr [edi+eax*4]
			.if bx && !word ptr [edi+eax*4+2]
				lea edx,[edi+eax*4]
				mov [edx+2],cx
				inc eax
				.break
			.endif
			inc eax
		.endw
		.while eax<65536
			mov bx,word ptr [edi+eax*4] 
			.if bx && !word ptr [edi+eax*4+2]
				mov @flag,1
				.if bx>word ptr [edx]
					mov word ptr [edx+2],0
					lea edx,[edi+eax*4]
					mov [edx+2],cx
				.endif
			.endif
			inc eax
		.endw
		inc ecx
	.endw
	
	mov eax,ecx
	dec eax
	
	ret
_PrepareFreqTable endp
;
_PrepareFreqTable2 proc uses ebx edi esi,_pSrc,_nSrcLen,_pOut,_pOutSize
	local @flag
	.if _pOutSize<256*4
		xor eax,eax
		ret
	.endif
	mov edi,_pOut
	mov ecx,_pOutSize
	shr ecx,2
	xor eax,eax
	rep stosd
	mov ecx,_pOutSize
	and ecx,2
	rep stosb
	
	mov edi,_pOut
	mov esi,_pSrc
	mov ecx,_nSrcLen
	.while ecx
		lodsb
		dec ecx
		mov bx,word ptr [edi+eax*4]
		.continue .if bx==0ffffh
		inc bx
		mov word ptr [edi+eax*4],bx
	.endw
	
	;int 3
	mov edx,edi
	mov ecx,1
	mov @flag,1
	.while @flag
		xor eax,eax
		mov @flag,0
		.while eax<=255
			mov bx,word ptr [edi+eax*4]
			.if bx && !word ptr [edi+eax*4+2]
				lea edx,[edi+eax*4]
				mov [edx+2],cx
				inc eax
				.break
			.endif
			inc eax
		.endw
		.while eax<=255
			mov bx,word ptr [edi+eax*4] 
			.if bx && !word ptr [edi+eax*4+2]
				mov @flag,1
				.if bx>word ptr [edx]
					mov word ptr [edx+2],0
					lea edx,[edi+eax*4]
					mov [edx+2],cx
				.endif
			.endif
			inc eax
		.endw
		inc ecx
	.endw
	
	mov eax,ecx
	dec eax
	
	ret
_PrepareFreqTable2 endp

;
_BitsPutHigh proc _pBits:ptr _Bits,_nBitsToSet,_ddVal
	pushad
	mov edi,_pBits
	assume edi:ptr _Bits
	mov ebx,_nBitsToSet
	mov edx,_ddVal
	.while ebx
		rol edx,1
		jnc @F
		mov ecx,[edi].curbits
		dec ecx
		mov eax,1
		shl eax,cl
		mov esi,[edi].stream
		mov ecx,[edi].curbyte
		xor byte ptr [esi+ecx-1],al
		@@:
		dec [edi].curbits
		.if ![edi].curbits
			mov [edi].curbits,8
			inc [edi].curbyte
			mov eax,[edi].curbyte
			cmp eax,[edi].stream_length
			ja Err01
		.endif
		dec ebx
	.endw
	popad
	mov eax,1
	ret
Err01:
	popad
	xor eax,eax
	ret
_BitsPutHigh endp

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

