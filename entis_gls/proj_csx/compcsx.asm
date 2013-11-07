.486
.model flat,stdcall
option casemap:none

include compcsx.inc

DEBUG equ 1

.code
_memcpy proc
	mov     eax, ecx
	shr     ecx, 2
	rep movsd
	mov     ecx, eax
	and     ecx, 3
	rep movsb
	ret
_memcpy endp

_new proc _nSize
	invoke HeapAlloc,hProcessHeap,0,_nSize
	ret
_new endp

_free proc _p
	invoke HeapFree,hProcessHeap,0,_p
	ret
_free endp

.code

_IsEndMess proc uses esi _pointer
	mov esi,_pointer
	lodsb
	cmp al,8
	jne _ret1EM
	add esi,5
	lodsd
	cmp eax,4
	jnz _ret1EM
	lodsd
	mov ecx,eax
	lodsd
	cmp ecx,65004dh
	jne _ret1EM
	cmp eax,730073h
	jne _ret1EM
	lodsd
	cmp eax,06000201h
	jne _ret1EM
	lea ecx,[esi-3]
	lodsd
	lea esi,[esi+eax*2]
	add esi,0ah
	cmp dword ptr [esi],65004dh
	jne _ret1EM
	cmp dword ptr [esi+4],730073h
	jne _ret1EM
	xor eax,eax
	ret
_ret1EM:
	mov eax,1
	ret
_IsEndMess endp

_CorrectFuncTbl proc uses esi ebx _nOffset,_nDiff
	.if !_nDiff
		ret
	.endif
	mov esi,dword ptr [dbBlockAddr+4]
	mov ecx,[esi]
	add esi,4
	mov edx,_nOffset
	mov eax,_nDiff
	.while ecx
		.if [esi]>edx
			add [esi],eax
		.endif
		add esi,4
		dec ecx
	.endw
	
	add esi,4
	mov ecx,[esi]
	add esi,4
	mov ebx,eax
	.while ecx
		.if [esi]>edx
			add [esi],ebx
		.endif
		add esi,4
		lodsd
		lea esi,[esi+eax*2]
		dec ecx
	.endw
	
	ret
_CorrectFuncTbl endp

_CorrectJumpTbl proc uses esi _ppJT,_nOffset,_nDiff,_nOldLen
	mov edx,_ppJT
	mov esi,[edx]
	assume esi:ptr _JumpEntry
	assume edx:ptr _JumpEntry
	.while esi
		mov eax,[esi].nOffset
		lea ecx,[eax+4]
		add eax,lpNewImage
		add ecx,[eax]
		.if ecx<=_nOffset
			mov eax,[esi].Flink
			mov [edx].Flink,eax
			push eax
			push edx
			invoke _free,esi
			pop edx
			pop esi
		.else
			mov ecx,_nDiff
			add [eax],ecx
			mov edx,[edx].Flink
			mov esi,[esi].Flink
		.endif
	.endw

	lea edx,pBJumpTable
	mov esi,pBJumpTable
	
	.while esi
		mov eax,[esi].nOffset
		lea ecx,[eax+4]
		add eax,dbBlockAddr
		add ecx,[eax]
		sub ecx,_nOldLen
		.if eax<_nOffset
			mov eax,[esi].Flink
			mov [edx].Flink,eax
			push eax
			push edx
			invoke _free,esi
			pop edx
			pop esi
		.elseif ecx<=_nOffset
			mov ecx,_nDiff
			sub [eax],ecx
			mov edx,[edx].Flink
			mov esi,[esi].Flink
		.else
			mov edx,[edx].Flink
			mov esi,[esi].Flink
		.endif
	.endw
	assume edx:nothing
	assume esi:nothing
	ret
_CorrectJumpTbl endp

_ModifyString proc uses edi esi ebx _ppPosNew,_lpNewString,_ppPosOri,_ppJT
	LOCAL @pNew,@pOri
	LOCAL @nNewLen,@nOldLen
	mov ecx,_ppPosOri
	mov eax,[ecx]
	mov @pOri,eax
	mov ecx,_ppPosNew
	mov eax,[ecx]
	mov @pNew,eax
	mov edi,eax
	invoke lstrlenW,_lpNewString
	.repeat
		mov esi,_lpNewString
		.break .if eax<=28
		mov ecx,@pOri
		add ecx,3
		.break .if eax<=[ecx]
		mov ebx,eax
		mov dword ptr [edi],60002h
		add edi,3
		mov eax,28
		stosd
		mov ecx,eax
		rep movsw
		mov ecx,19
		lea esi,dbMess
		rep movsb
		add _lpNewString,56
		lea eax,[ebx-28]
	.until 0
	mov dword ptr [edi],60002h
	add edi,3
	stosd
	mov ecx,eax
	rep movsw
	
	mov eax,_ppPosNew
	mov [eax],edi
	sub edi,@pNew
	mov @nNewLen,edi
	mov ecx,@pOri
	.repeat
		mov esi,ecx
		add esi,3
		lodsd
		lea esi,[esi+eax*2]
		invoke _IsEndMess,esi
	.until eax
	
	mov eax,_ppPosOri
	mov [eax],esi
	sub esi,@pOri
	mov @nOldLen,esi
	mov eax,@nNewLen
	sub eax,@nOldLen
	mov ecx,@pNew
	sub ecx,lpNewImage
	push @nOldLen
	push eax
	push ecx
	invoke _CorrectFuncTbl,ecx,eax
	push _ppJT
	call _CorrectJumpTbl
	ret
_ModifyString endp

_MakeBackJumpTable proc uses esi ebx _lpImage,_nSize
	LOCAL @pCsxEnd
	LOCAL @pLastString
	mov pBJumpTable,0
	mov @pLastString,0
	mov esi,_lpImage
	mov eax,esi
	add eax,_nSize
	mov @pCsxEnd,eax
	.while esi<@pCsxEnd
		xor eax,eax
		lodsb
		.if al>=0fh
			int 3
		.endif
		xor ecx,ecx
		call [eax*4+Jumptbl]
		.if eax==-1 || eax>=100h
			int 3
		.endif
		.if ecx==1
			mov edx,[esi+2]
			.if byte ptr [esi+7] && edx
				lea esi,[esi+edx*2+6]
				mov @pLastString,esi
				.continue
			.endif
			add esi,eax
		.elseif ecx==2
_j06BJT:
			lodsd
			test eax,080000000h
			.continue .if ZERO? || eax==-1
			add eax,esi
			.continue .if eax>=@pLastString
			invoke _new,sizeof _JumpEntry
			assert(eax)
			mov ebx,eax
			assume ebx:ptr _JumpEntry
			lea ecx,[esi-4]
			sub ecx,_lpImage
;			sub ecx,@pPosOri
;			add ecx,@pPosNew
;			sub ecx,lpNewImage
			mov [ebx].nOffset,ecx
			push pBJumpTable
			mov pBJumpTable,ebx
			pop [ebx].Flink
			assume ebx:nothing
		.elseif ecx==3
			inc esi
			jmp _j06BJT
		.else
			add esi,eax
		.endif
	.endw
	
	ret
_MakeBackJumpTable endp

start proc
	LOCAL @nCsxSize,@nTxtSize
	LOCAL @pTxtEnd,@pCsxEnd
	LOCAL @dbHdr[40h]:byte
	LOCAL @dbBlockHdr[60h]:byte
	LOCAL @pPosOri,@pPosNew
	LOCAL @pJumpTable
	invoke GetProcessHeap
	mov hProcessHeap,eax
	invoke CreateFile,$CTA0("haruka.csx"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
;	assert((eax!=INVALID_HANDLE_VALUE))
	.if !(eax!=INVALID_HANDLE_VALUE)
		int 3
	.endif
	mov hCsxFile,eax
	invoke CreateFile,$CTA0("haruka.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
;	assert(eax!=INVALID_HANDLE_VALUE)
	.if !(eax!=INVALID_HANDLE_VALUE)
		int 3
	.endif
	mov hTxtFile,eax
	invoke GetFileSize,hTxtFile,0
	mov @nTxtSize,eax
	mov @pTxtEnd,eax
	invoke GetFileSize,hCsxFile,0
	mov @nCsxSize,eax
	
	invoke VirtualAlloc,0,@nTxtSize,MEM_COMMIT,PAGE_READWRITE
	assert(eax)
	mov lpTxt,eax
	add @pTxtEnd,eax
	mov edx,@nCsxSize
	shl edx,1
	invoke VirtualAlloc,0,edx,MEM_COMMIT,PAGE_READWRITE
	assert(eax)
	mov lpNewImage,eax
	
	invoke ReadFile,hCsxFile,addr @dbHdr,40h,offset dwTemp,0
	xor ebx,ebx
	lea edi,dbBlockAddr
	lea esi,dbBlockSize
	.repeat
		lea eax,@dbBlockHdr
		mov ecx,ebx
		shl ecx,4
		add ecx,eax
		push ecx
		invoke ReadFile,hCsxFile,ecx,10h,offset dwTemp,0
		pop ecx
		mov eax,[ecx+8]
		mov [esi],eax
		add esi,4
		shl eax,1
		invoke VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
		assert(eax)
		stosd
		invoke ReadFile,hCsxFile,eax,[esi-4],offset dwTemp,0
		inc ebx
	.until ebx>=6
	invoke _MakeBackJumpTable,dbBlockAddr,dbBlockSize
	
	invoke ReadFile,hTxtFile,lpTxt,@nTxtSize,offset dwTemp,0
	mov edi,lpTxt
	add edi,2
	mov esi,dword ptr dbBlockAddr
	mov eax,dword ptr dbBlockSize
	mov @pPosOri,esi
	add eax,esi
	mov @pCsxEnd,eax
	mov eax,lpNewImage
	mov @pPosNew,eax
	mov @pJumpTable,0
	.while esi<@pCsxEnd
		xor eax,eax
		lodsb
		.if al>=0fh
			int 3
		.endif
		xor ecx,ecx
		call [eax*4+Jumptbl]
		.if eax==-1 || eax>=100h
			int 3
		.endif
		.if ecx==1
			mov     ebx, esi
			add     ebx, eax
			dec     esi
			push    edi
			mov     ecx, esi
			sub     ecx, @pPosOri
			mov     esi, @pPosOri
			mov     edi, @pPosNew
			invoke _memcpy
			mov     @pPosNew, edi
			mov     @pPosOri, esi
			pop     edi
			add     esi, 3
			lodsd
			.if byte ptr [esi+1] && eax
				mov eax,edi
				.while word ptr [edi]!=0dh
					add edi,2
					.if edi>=@pTxtEnd
						int 3
					.endif
				.endw
				mov word ptr [edi],0
				add edi,4
				mov ecx,eax
				invoke _ModifyString,addr @pPosNew,ecx,addr @pPosOri,addr @pJumpTable
				mov esi,@pPosOri
				.continue
			.endif
			mov esi,ebx
		.elseif ecx==2
_j06:
			lodsd
			test eax,080000000h
			.continue .if !ZERO? || eax<=7
			invoke _new,sizeof _JumpEntry
			assert(eax)
			mov ebx,eax
			assume ebx:ptr _JumpEntry
			lea ecx,[esi-4]
			sub ecx,@pPosOri
			add ecx,@pPosNew
			sub ecx,lpNewImage
			mov [ebx].nOffset,ecx
			push @pJumpTable
			mov @pJumpTable,ebx
			pop [ebx].Flink
			assume ebx:nothing
		.elseif ecx==3
			inc esi
			jmp _j06
		.else
			add esi,eax
		.endif
	.endw
	mov ecx,esi
	sub ecx,@pPosOri
	mov esi,@pPosOri
	mov edi,@pPosNew
	rep movsb
	
	sub edi,lpNewImage
	mov dword ptr [dbBlockSize],edi
	mov eax,lpNewImage
	mov dword ptr [dbBlockAddr],eax
	invoke CreateFile,$CTA0("haruka1.csx"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		int 3
	.endif
	mov hNewCsxFile,eax
	xor ecx,ecx
	xor eax,eax
	.while ecx<6
		add eax,[ecx*4+offset dbBlockSize]
		inc ecx
	.endw
	lea ecx,@dbHdr
	mov [ecx+38h],eax
	invoke WriteFile,hNewCsxFile,ecx,40h,offset dwTemp,0
	lea esi,dbBlockAddr
	lea edi,dbBlockSize
	xor ebx,ebx
	.repeat
		lea eax,@dbBlockHdr
		mov ecx,ebx
		shl ecx,4
		add ecx,eax
		mov eax,[edi+ebx*4]
		mov [ecx+8],eax
		invoke WriteFile,hNewCsxFile,ecx,10h,offset dwTemp,0
		invoke WriteFile,hNewCsxFile,[esi+ebx*4],[edi+ebx*4],offset dwTemp,0		
		inc ebx
	.until ebx>=6
	
	invoke MessageBox,0,$CTA0("封装成功！"),$CTA0("14=大便"),MB_OK
	invoke ExitProcess,0
	ret
start endp

_p00 proc
	mov eax,2
	.if word ptr [esi]==2 || word ptr [esi]==1
		mov edx,[eax+esi]
		lea eax,[eax+edx*2+4]
		mov edx,[eax+esi]
		lea eax,[eax+edx*2+4]
	.else
		mov edx,[eax+esi]
		lea eax,[eax+edx*2+4]
	.endif
	ret
_p00 endp

_p01 proc
	xor eax,eax
	ret
_p01 endp

_p02 proc
	mov ax,[esi]
	.if ax!=600h && ax!=602h && ax
		.if ax==500h
			mov eax,10
		.elseif ax!=102h && ax!=100h && ax!=200h
			.if byte ptr [esi+6]<0fh
				.if al>5 || ah>6
					int 3
				.else
					mov eax,6
				.endif
			.else
				int 3
			.endif
		.else
			mov eax,2
		.endif
	.else
		.if ax==600h
			mov ecx,1
		.endif
		mov edx,[esi+2]
		lea eax,[edx*2+6]
	.endif
	ret
_p02 endp

_p03 proc
	mov eax,1
	ret
_p03 endp

_p04 proc
	mov eax,[esi]
	lea eax,[eax*2+4]
	mov ecx,[eax+esi]
	add eax,4
	.if ecx==-1
		xor ecx,ecx
		add eax,5
		ret
	.endif
	.while ecx
		inc eax
		.if !byte ptr [esi+eax-1]
			mov edx,[eax+esi]
			lea eax,[eax+edx*2+4]
		.endif
		mov edx,[eax+esi]
		lea eax,[eax+edx*2+4]		
		dec ecx
	.endw
	ret
_p04 endp

_p05 proc
	xor eax,eax
	ret
_p05 endp

_p06 proc
	mov eax,4
	mov ecx,2
	ret
_p06 endp

_p07 proc
	mov eax,5
	mov ecx,3
	ret
_p07 endp

_p08 proc
	mov edx,[esi+5]
	lea eax,[edx*2+9]
	ret
_p08 endp

_p09 proc
	mov eax,1
	ret
_p09 endp

_p0A proc
	mov edx,[esi+1]
	lea eax,[edx*2+5]
	ret
_p0A endp

_p0B proc
	xor eax,eax
	ret
_p0B endp

_p0C proc
	mov eax,1
	ret
_p0C endp

_p0D proc
	mov eax,1
	ret
_p0D endp

_p0E proc
	mov eax,1
	ret
_p0E endp

_p0F proc
	or eax,-1
	ret
_p0F endp

Jumptbl:
	dd	offset _p00
	dd	offset _p01
	dd	offset _p02
	dd	offset _p03
	dd	offset _p04
	dd	offset _p05
	dd	offset _p06
	dd	offset _p07
	dd	offset _p08
	dd	offset _p09
	dd	offset _p0A
	dd	offset _p0B
	dd	offset _p0C
	dd	offset _p0D
	dd	offset _p0E
	dd	offset _p0F

end start