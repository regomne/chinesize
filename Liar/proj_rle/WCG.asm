.code
;
_BitsPutHigh proc _pBits:ptr _Bits,_nBitsToSet,_ddVal
	pushad
	mov edi,_pBits
	assume edi:ptr _Bits
	mov ebx,_nBitsToSet
	mov edx,_ddVal
	test ebx,ebx
	jz _Ex
	_loop:
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
	jnz _loop
_Ex:
	assume edi:nothing
	popad
	mov eax,1
	ret
Err01:
	popad
	xor eax,eax
	ret
_BitsPutHigh endp
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
		lea ecx,[esi+2]
		.if ax==[ecx] && ecx<@pSrcEnd
			movzx edx,word ptr [ebx+eax*4+2]
			mov ecx,2
			push edx
			lea edx,[esi+ecx*2]
			cmp edx,@pSrcEnd
			jae @F
			.while ax==[edx]
				inc ecx
				.break .if ecx>=17
				lea edx,[esi+ecx*2]
				.break .if edx>=@pSrcEnd
;					dec ecx
;					.break
;				.endif
			.endw
			@@:
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
		lea ecx,[esi+2]
		.if ax==[ecx] && ecx<@pSrcEnd
			movzx edx,word ptr [ebx+eax*4+2]
			mov ecx,2
			push edx
			lea edx,[esi+ecx*2]
			cmp edx,@pSrcEnd
			jae @F
			.while ax==[edx]
				inc ecx
				.break .if ecx>=17
				lea edx,[esi+ecx*2]
				.break .if edx>=@pSrcEnd
;					dec ecx
;					.break
;				.endif
			.endw
			@@:
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
_ConvertDIB proc uses ebx edi esi,_psCvrt:ptr _MyRLEData,_Width:dword,_Height:dword
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

	.if ecx>[edx].nDestSize
	@@:
		mov [edx].nErrCode,1
		xor eax,eax
		ret
	.endif
	cmp ecx,[edx].nAlphaSize
	ja @B
	mov [edx].nDestSize,ecx
	mov [edx].nAlphaSize,ecx
	
	mov esi,[edx].pSrc
	add esi,eax
	mov ebx,_Height
	mov edi,[edx].pDest
	mov eax,[edx].pDestAlpha
	mov @nAlphaOff,eax
	assume edx:nothing
	.while ebx
		xor eax,eax
		.while eax<@nWid4
			mov cx,[esi+eax+2]
;			not ch
			mov [edi],cx
			add edi,2
			push edi
			mov edi,@nAlphaOff
			mov cx,[esi+eax]
			mov [edi],cx
			add edi,2
			mov @nAlphaOff,edi
			pop edi
			add eax,4
		.endw
		sub esi,@nWid4
		dec ebx
	.endw
	mov eax,1
	ret
_ConvertDIB endp
;
_DeAlphaBlending proc uses edi esi ebx,_pDest,_nWidth,_nHeight
	mov eax,_nWidth
	mov ecx,_nHeight
	mul cx
	shl edx,16
	or eax,edx
	mov esi,eax
	mov edi,_pDest
	.while esi
		movzx ebx,byte ptr [edi+3]
		or bl,bl
		je @F
		movzx eax,byte ptr [edi]
		add al,bl
		sub al,0ffh
		mov cx,0ffh
		xor dx,dx
		mul cx
		div bx
		mov [edi],al
		
		movzx eax,byte ptr [edi+1]
		add al,bl
		sub al,0ffh
		mov cx,0ffh
		xor dx,dx
		mul cx
		div bx
		mov [edi+1],al
		
		movzx eax,byte ptr [edi+2]
		add al,bl
		sub al,0ffh
		mov cx,0ffh
		xor dx,dx
		mul cx
		div bx
		mov [edi+2],al
		
		@@:
		not byte ptr [edi+3]
		add edi,4
		dec esi
	.endw
	ret
_DeAlphaBlending endp
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
_FoldPic proc uses edi esi ebx,_hBmpFile,_pMemory
	local @nBmpFileSize,@lpBmpFile
	local @lpDIB1,@lpDIB2
	local @lpTable1,@lpCmpr1,@lpTable2,@lpCmpr2
	local @lpFreqTable1,@lpFreqTable2
	local @nCodes1,@nCodes2
	local @sTotal:_MyRLEData,@s2:_MyRLEData,@s1:_MyRLEData
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
	
	mov eax,edi
	add eax,dword ptr [edi+0ah]
	invoke _DeAlphaBlending,eax,dword ptr [edi+12h],dword ptr [edi+16h]
	
	mov eax,[edi+0ah]
	add eax,edi
	mov @sTotal.pSrc,eax
	mov eax,[edi+22h]
	mov @sTotal.nSrcLen,eax
	shr eax,1
	mov ebx,eax
	mov @sTotal.nDestSize,eax
	mov @s1.nSrcLen,eax
	invoke GlobalAlloc,GPTR,eax;RA通道存储区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+4],eax
	mov @lpDIB1,eax
	mov @sTotal.pDest,eax
	mov @s1.pSrc,eax
	mov @sTotal.nAlphaSize,ebx
	mov @s2.nSrcLen,ebx
	invoke GlobalAlloc,GPTR,ebx;BG通道存储区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+8],eax
	mov @lpDIB2,eax
	mov @sTotal.pDestAlpha,eax
	mov @s2.pSrc,eax
	mov eax,[edi+12h]
	mov ecx,[edi+16h]
	invoke _ConvertDIB,addr @sTotal,eax,ecx
	test eax,eax
	je Err02
	mov eax,@sTotal.nDestSize
	mov @s1.nSrcLen,eax
	mov eax,@sTotal.nAlphaSize
	mov @s2.nSrcLen,eax
	
	invoke GlobalAlloc,GPTR,65536*4;RA Frequence Table区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+12],eax
	mov @lpFreqTable1,eax
	mov edi,@s1.pSrc
	mov ecx,@s1.nSrcLen
	invoke _PrepareFreqTable,edi,ecx,@lpFreqTable1,65536*4
	test eax,eax
	je Err02
	mov @nCodes1,eax
	
	shl eax,1
	mov @s1.nTableSize,eax
	invoke GlobalAlloc,GPTR,eax;RA通道压缩后的Code Table区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+16],eax
	mov @lpTable1,eax
	mov @s1.pCodeTable,eax
	
	mov eax,@sTotal.nDestSize
	mov @s1.nDestSize,eax
	invoke GlobalAlloc,GPTR,eax;RA通道压缩后的数据区
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+20],eax
	mov @lpCmpr1,eax
	mov @s1.pDest,eax
	invoke _RLECompress,addr @s1,@lpFreqTable1,@nCodes1
	test eax,eax
	je Err04
	
	invoke GlobalFree,@lpDIB1
	mov ecx,_pMemory
	mov dword ptr [ecx+4],0
	invoke GlobalFree,@lpFreqTable1
	mov ecx,_pMemory
	mov dword ptr [ecx+16],0
	
	invoke GlobalAlloc,GPTR,65536*4;BG通道 Frequence Table区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+28],eax
	mov @lpFreqTable2,eax
	mov edi,@s2.pSrc
	mov ecx,@s2.nSrcLen
	invoke _PrepareFreqTable,edi,ecx,@lpFreqTable2,65536*4
	test eax,eax
	je Err02
	mov @nCodes2,eax
	
	shl eax,1
	mov @s2.nTableSize,eax
	invoke GlobalAlloc,GPTR,eax;RA通道压缩后的Code Table区域
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+32],eax
	mov @lpTable2,eax
	mov @s2.pCodeTable,eax

	mov eax,@sTotal.nAlphaSize
	mov @s2.nDestSize,eax
	invoke GlobalAlloc,GPTR,eax;BG数据被压缩后的数据区
	test eax,eax
	je Err02
	mov ecx,_pMemory
	mov [ecx+24],eax
	mov @s2.pDest,eax
	mov @lpCmpr2,eax
	invoke _RLECompress,addr @s2,@lpFreqTable2,@nCodes2
	test eax,eax
	je Err04
	
	invoke GlobalFree,@lpDIB2
	mov ecx,_pMemory
	mov dword ptr [ecx+8],0
	invoke GlobalFree,@lpFreqTable2
	mov ecx,_pMemory
	mov dword ptr [ecx+28],0
	
	mov eax,@s2.nDestSize
	add eax,@s2.nTableSize
	add eax,@s1.nDestSize
	add eax,@s1.nTableSize
	add eax,1000
	invoke GlobalAlloc,GPTR,eax;最终文件区域
	test eax,eax
	je Err02
	mov @lpFinalFile,eax

	mov ecx,eax
	mov edi,@lpFinalFile
	mov eax,2714757h
	stosd
	mov eax,40000020h
	stosd
	mov esi,@lpBmpFile
	add esi,12h
	movsd
	movsd
	mov eax,@s1.nSrcLen
	stosd
	mov eax,@s1.nDestSize
	stosd
	mov eax,@nCodes1
	stosw
	mov ax,0eh
	stosw
	mov ecx,@s1.nTableSize
	mov esi,@s1.pCodeTable
	call _memcpy
	mov ecx,@s1.nDestSize
	mov esi,@s1.pDest
	call _memcpy
	mov eax,@s2.nSrcLen
	stosd
	mov eax,@s2.nDestSize
	stosd
	mov eax,@nCodes2
	stosw
	mov ax,0cch
	stosw
	mov ecx,@s2.nTableSize
	mov esi,@s2.pCodeTable
	call _memcpy
	mov ecx,@s2.nDestSize
	mov esi,@s2.pDest
	call _memcpy
	
	invoke GlobalFree,@lpBmpFile
	invoke GlobalFree,@lpTable1
	invoke GlobalFree,@lpTable2
	invoke GlobalFree,@lpCmpr1
	invoke GlobalFree,@lpCmpr2
	push edi
	mov edi,_pMemory
	mov ecx,9
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