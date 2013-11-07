.code

_AddLine proc uses esi edi ebx _idxToInsert,_lpBin,_Flags
	LOCAL @lpBuffTemp,@nSize
	LOCAL @nLen
	mov edi,sGscInfo.lpIndexCS
	mov eax,_idxToInsert
	mov esi,[edi+eax*4]
	mov ebx,sGscInfo.sHeader.nControlStreamSize
	sub ebx,esi
	invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
	.if !eax
		ret
	.endif
	mov @lpBuffTemp,eax
	mov edi,eax
	add esi,sGscInfo.lpControlStream
	push esi
	mov ecx,ebx
	mov @nSize,ecx
	call _memcpy
	pop esi
	mov eax,_lpBin
	movzx eax,word ptr [eax]
	invoke _GetInstLen,eax
	.if eax==-1
		xor eax,eax
		ret
	.endif
	add eax,2
	mov @nLen,eax
	mov ecx,eax
	mov edi,esi
	mov ebx,esi
	mov esi,_lpBin
	rep movsb
	mov esi,@lpBuffTemp
	mov ecx,@nSize
	call _memcpy
	invoke _CorrectRTCS,_idxToInsert,@nLen,CT_ADD
	mov eax,@nLen
	add sGscInfo.sHeader.nControlStreamSize,eax
	add sGscInfo.sHeader.nFileSize,eax
	sub ebx,sGscInfo.lpControlStream
	.if ebx<=nCurOffset
		add nCurOffset,eax
	.endif
	invoke VirtualFree,@lpBuffTemp,0,MEM_RELEASE
	mov eax,1
	ret
_AddLine endp
;
_DelLine proc uses esi edi ebx _idxToDel
	mov edi,sGscInfo.lpIndexCS
	mov eax,_idxToDel
	mov esi,[edi+eax*4]
	add esi,sGscInfo.lpControlStream
	lodsw
	and eax,0ffffh
	invoke _GetInstLen,eax
	.if eax==-1
		xor eax,eax
		ret
	.endif
	
	mov ebx,eax
	mov ecx,sGscInfo.sHeader.nControlStreamSize
	add ecx,sGscInfo.lpControlStream
	sub ecx,esi
	sub ecx,eax
	lea edi,[esi-2]
	add esi,eax
	rep movsb
	mov [edi],ecx
	
	add ebx,2
	invoke _CorrectRTCS,_idxToDel,ebx,CT_DEL
	sub sGscInfo.sHeader.nControlStreamSize,ebx
	sub sGscInfo.sHeader.nFileSize,ebx
	mov edi,sGscInfo.lpIndexCS
	mov eax,_idxToDel
	mov esi,[edi+eax*4]
	.if esi<=nCurOffset
		sub nCurOffset,ebx
	.endif
	ret
_DelLine endp

;
_ModifyText proc uses edi esi ebx _idxOfString,_lpszNew
	LOCAL @lpBuffTemp,@pString,@nLenNew,@nLenOld
	mov edi,sGscInfo.lpIndex
	mov eax,_idxOfString
	mov edi,[edi+eax*4]
	add edi,sGscInfo.lpText
	mov @pString,edi
	invoke lstrlen,edi
	mov @nLenOld,eax
	lea esi,[edi+eax+1]
	mov ecx,sGscInfo.sHeader.nTextSize
	add ecx,sGscInfo.lpText
	sub ecx,esi
	mov ebx,ecx
	invoke VirtualAlloc,0,ecx,MEM_COMMIT,PAGE_READWRITE
	.if !eax
		ret
	.endif
	mov @lpBuffTemp,eax
	mov edi,eax
	mov ecx,ebx
	call _memcpy
	mov edi,@pString
	invoke lstrcpy,edi,_lpszNew
	invoke lstrlen,_lpszNew
	mov @nLenNew,eax
	lea edi,[edi+eax+1]
	mov esi,@lpBuffTemp
	mov ecx,ebx
	call _memcpy
	invoke VirtualFree,@lpBuffTemp,0,MEM_RELEASE
	
	mov ecx,@nLenNew
	sub ecx,@nLenOld
	mov eax,_idxOfString
	.if ecx
		mov edi,sGscInfo.lpIndex
		lea edi,[edi+eax*4+4]
		.while dword ptr [edi]
			add dword ptr [edi],ecx
			add edi,4
		.endw
		add sGscInfo.sHeader.nFileSize,ecx
		add sGscInfo.sHeader.nTextSize,ecx
	.endif
	MOV EAX,1
	ret
_ModifyText endp
;
_CorrectRTCS proc uses edi esi _idx,_nBytes,_Flags
	mov edi,sGscInfo.lpIndexCS
	mov esi,sGscInfo.lpRelocTable
	.if _Flags==CT_ADD
		xor ecx,ecx
		.repeat
			add edi,4
			inc ecx
		.until !dword ptr [edi]
		.while ecx>_idx
			mov eax,[edi-4]
			mov [edi],eax
			sub edi,4
			dec ecx
		.endw
		add edi,4
		mov eax,_nBytes
		.while dword ptr [edi]
			add dword ptr [edi],eax
			add edi,4
		.endw
		
		mov edi,sGscInfo.lpIndexCS
		mov eax,_idx
		mov edx,[edi+eax*4]
		mov ecx,_nBytes
		.while dword ptr [esi]
			.if dword ptr [esi]>=edx
				add dword ptr [esi],ecx
			.endif
			add esi,4
		.endw
	.else
		mov eax,_idx
		lea edi,[edi+eax*4]
		.while dword ptr [edi]
			mov eax,[edi+4]
			mov [edi],eax
			add edi,4
		.endw
		mov edi,sGscInfo.lpIndexCS
		mov eax,_idx
		lea edi,[edi+eax*4]
		mov eax,_nBytes
		.while dword ptr [edi]
			sub dword ptr [edi],eax
			add edi,4
		.endw
		
		mov edi,sGscInfo.lpIndexCS
		mov eax,_idx
		mov edx,[edi+eax*4]
		mov ecx,_nBytes
		.while dword ptr [esi]
			.if dword ptr [esi]>=edx
				sub dword ptr [esi],ecx
			.endif
			add esi,4
		.endw
		
	.endif
	invoke _Relocate,_idx,_nBytes,_Flags
	ret
_CorrectRTCS endp
;
_Relocate proc uses edi esi _idx,_nBytes,_Flags
	mov edi,sGscInfo.lpIndexCS
	mov eax,_idx
	mov edx,[edi+eax*4]
	mov esi,sGscInfo.lpRelocTable
	lodsd
	mov edi,sGscInfo.lpControlStream
	.if _Flags==CT_ADD
		.while eax
			mov ecx,[edi+eax]
			.if ecx>edx
				add ecx,_nBytes
				mov [edi+eax],ecx
			.endif
			lodsd
		.endw
	.else
		.while eax
			mov ecx,[edi+eax]
			.if ecx>edx
				sub ecx,_nBytes
				mov [edi+eax],ecx
			.endif
			lodsd
		.endw
	.endif
	ret
_Relocate endp
;
_InsertString proc uses edi esi _lpszStr
	
	mov edi,sGscInfo.lpIndex
	mov eax,sGscInfo.sHeader.nIndexSize
	sub eax,4
	mov esi,[edi+eax]
	add esi,sGscInfo.lpText
	invoke lstrlen,esi
	inc eax
	add esi,eax
	invoke lstrcpy,esi,_lpszStr
	invoke lstrlen,_lpszStr
	inc eax
	add sGscInfo.sHeader.nTextSize,eax
	add sGscInfo.sHeader.nFileSize,eax
	
	mov ecx,esi
	sub ecx,sGscInfo.lpText
	mov eax,sGscInfo.sHeader.nIndexSize
	mov [edi+eax],ecx
	add sGscInfo.sHeader.nIndexSize,4
	add sGscInfo.sHeader.nFileSize,4

	shr eax,2
	ret
_InsertString endp

;
_GetPointerFromLine proc _idx
	mov eax,_idx
	mov ecx,sGscInfo.lpIndexCS
	mov eax,[ecx+eax*4]
	add eax,sGscInfo.lpControlStream
	ret
_GetPointerFromLine endp

;
_GetLineFromOffset proc _nOffset
	mov edx,sGscInfo.lpIndexCS
	xor eax,eax
	mov ecx,_nOffset
	.repeat
		cmp ecx,[edx]
		je @F
		inc eax
		add edx,4
	.until !dword ptr [edx]
	or eax,-1
@@:	
	ret
_GetLineFromOffset endp

;
_GetTextByIdx proc _idx
	mov ecx,sGscInfo.lpIndex
	mov eax,_idx
	mov eax,[ecx+eax*4]
	mov ecx,sGscInfo.lpText
	add eax,ecx
	ret
_GetTextByIdx endp

