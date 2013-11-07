
.code

_GetFileNameInPathA proc uses edi _lpszName
	mov edi,_lpszName
	xor al,al
	repne scasb
	std
	mov al,'\'
	mov ecx,32
	repne scasb
	cld
	add edi,2
	mov eax,edi
	ret
_GetFileNameInPathA endp

;
_ConnectGscPathA proc uses edi _lpszPath,_lpszFile
	mov edi,_lpszPath
	xor al,al
	or ecx,-1
	repne scasb
	sub edi,2
	.if byte ptr [edi]!='\'
		mov word ptr [edi+1],'\'
	.endif
	
	invoke _GetFileNameInPathA,_lpszFile
	invoke lstrcat,_lpszPath,eax
	ret
_ConnectGscPathA endp

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
_Bin2Scr proc uses esi ebx _lpBin,_lpScr
	LOCAL @tmpStr[16]:byte
	mov esi,_lpBin
	lodsw
	and eax,0ffffh
	mov ebx,eax
	.if eax==51h
		.if dword ptr [esi+4]
			invoke wsprintf,_lpScr,offset szVoiceIn51,dword ptr [esi+4]
			invoke _GetTextByIdx,dword ptr [esi+20]
			invoke lstrcat,_lpScr,eax
		.else
			invoke _GetTextByIdx,dword ptr [esi+20]
			invoke lstrcpy,_lpScr,eax
		.endif
	.elseif eax==0eh
		mov ecx,_lpScr
		mov word ptr [ecx],'!'
		invoke _GetTextByIdx,[esi+2]
		invoke lstrcat,_lpScr,eax
		add esi,26
		mov ebx,5
		.while ebx && dword ptr [esi]
			invoke lstrcat,_lpScr,offset szEnter
			invoke _GetTextByIdx,[esi]
			invoke lstrcat,_lpScr,eax
			dec ebx
			add esi,4
		.endw
	.else
		invoke wsprintf,_lpScr,offset szBeginCode,ebx
		invoke _GetInstLen,ebx
		.if eax==-1
			xor eax,eax
			ret
		.endif
		mov ebx,eax
		.while ebx>=4
			lodsd
			invoke wsprintf,addr @tmpStr,offset szCtnCode,eax
			invoke lstrcat,_lpScr,addr @tmpStr
			sub ebx,4
		.endw
		.if ebx>=2
			lodsw
			and eax,0ffffh
			invoke wsprintf,addr @tmpStr,offset szCtnCode,eax
			invoke lstrcat,_lpScr,addr @tmpStr 
		.endif
	.endif

_Bin2ScrSuc:
	mov eax,1
	ret
_Bin2Scr endp

;
_Scr2Bin proc uses esi edi _lpScr,_lpBin,_Flags
	LOCAL @tmpStr[16]:byte,@nTmp
	mov esi,_lpScr
	mov edi,_lpBin
	lodsb
	.if al=='@'
		invoke StrToIntEx,esi,1,addr @nTmp
		or eax,eax
		je _Bin2ScrFail
		mov eax,@nTmp
		stosw
		invoke _GetInstLen,@nTmp
		cmp eax,-1
		je _Bin2ScrFail
		mov ebx,eax
		mov ecx,512
		push edi
		mov edi,esi
		pop esi
		.while ebx>=4
			mov al,' '
			repne scasb
			push ecx
			invoke StrToIntEx,edi,1,addr @nTmp
			pop ecx
			or eax,eax
			je _Bin2ScrFail
			mov eax,@nTmp
			mov [esi],eax
			add esi,4
			sub ebx,4
		.endw
		.if ebx>=2
			mov al,' '
			repne scasb
			invoke StrToIntEx,edi,1,addr @nTmp
			or eax,eax
			je _Bin2ScrFail
			mov eax,@nTmp
			mov [esi],eax
		.endif
	.elseif al=='!'
		mov word ptr [edi],0
	.else
		mov word ptr [edi],51h
		mov @nTmp,0
		.if al=='#'
			invoke StrToIntEx,esi,1,addr @nTmp
			mov ecx,6
			@@:
				cmp byte ptr [esi],' '
				je @F
				inc esi
			loop @B
			jmp _Bin2ScrFail
			@@:
			inc esi
		.else
			dec esi
		.endif
		cmp byte ptr [esi],0
		je _Bin2ScrFail
		.if _Flags==NOT_INSERT
			xor eax,eax
		.else
			invoke _InsertString,esi
			or eax,eax
			je _Bin2ScrFail
		.endif
		mov [edi+22],eax
		mov dword ptr [edi+26],1
		mov eax,@nTmp
		mov [edi+6],eax
	.endif
	mov eax,1
	ret
_Bin2ScrFail:
	xor eax,eax
	ret
_Scr2Bin endp

;
_GetInstLen proc _ddInst
	mov eax,_ddInst
	mov cx,ax
	and ax,0f000h
	.if !ZERO?
		lea edx,dtParamSize1
		shr ax,12
		and eax,0fh
		movzx eax,byte ptr [edx+eax]
	.else
		lea edx,dtParamSize2
		movzx eax,cl
		movzx eax,byte ptr [edx+eax]
	.endif
	ret
_GetInstLen endp



