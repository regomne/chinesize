.code

_PrepareDec proc uses esi edi ebx _lpEntry,nIndex
	LOCAL @nHash
	mov edi,_lpEntry
	assume edi:ptr _NpaEntry
	mov ecx,[edi].nHash0
	imul ecx,[edi].nHash1
	mov @nHash,ecx
	
	mov edx,ecx
	mov esi,nIndex
	xor ebx,ebx
	.while ebx<[edi].nNameLen
		mov eax,ebx
		mov cl,0fch
		imul cl
		mov ecx,edx
		shr ecx,24
		sub al,cl
		mov ecx,edx
		shr ecx,16
		sub al,cl
		mov ecx,edx
		shr ecx,8
		sub al,cl
		sub al,dl
		mov ecx,esi
		shr ecx,24
		sub al,cl
		mov ecx,esi
		shr ecx,16
		sub al,cl
		mov ecx,esi
		shr ecx,8
		sub al,cl
		mov ecx,esi
		sub al,cl
		lea ecx,[edi].szName
		add byte ptr [ecx+ebx],al
		inc ebx
	.endw
	
	xor ebx,ebx
	mov eax,12345678h
	.while ebx<[edi].nNameLen
		movzx edx,byte ptr [ecx+ebx]
		sub eax,edx
		inc ebx
	.endw
	imul ebx
	mov [edi].nKey,eax
	add ebx,1000h
	mov ecx,[edi].nCompLen
	.if ecx>ebx
		mov [edi].nDecLen,ebx
	.else
		mov [edi].nDecLen,ecx
	.endif
	
	assume edi:nothing
	ret
_PrepareDec endp

_Decrypt proc uses ebx edi esi _lpEntry,_lpBuff,_nLen
	mov edi,_lpEntry
	assume edi:ptr _NpaEntry
	mov ecx,[edi].nHash0
	imul ecx,[edi].nHash1
	add ecx,[edi].nKey
	imul ecx,[edi].nUncLen
	
	mov esi,_lpBuff
	xor ebx,ebx
	xor eax,eax
	.while ebx<_nLen
		mov al,[esi+ebx]
		mov al,byte ptr [eax+offset dbDecTable]
		mov al,byte ptr [eax+offset dbDecTable]
		mov al,byte ptr [eax+offset dbDecTable]
		not al
		sub al,cl
		sub al,bl
		mov [esi+ebx],al
		inc ebx
	.endw
	assume edi:nothing
	ret
_Decrypt endp

_InitEncTable proc uses esi edi
	lea esi,dbDecTable
	lea edi,dbEncTable
	xor eax,eax
	xor ecx,ecx
	.while ecx<256
		lodsb
		mov [edi+eax],cl
		inc ecx
	.endw
	ret
_InitEncTable endp

_Encrypt proc uses esi edi ebx _lpEntry,_lpBuff
	mov edi,_lpEntry
	assume edi:ptr _NpaEntry
	xor ebx,ebx
	mov eax,20101118h
	lea ecx,[edi].szName
	.while ebx<[edi].nNameLen
		movzx edx,byte ptr [ecx+ebx]
		sub eax,edx
		inc ebx
	.endw
	imul ebx
	mov [edi].nKey,eax
	add ebx,1000h
	mov ecx,[edi].nCompLen
	.if ecx>ebx
		mov [edi].nDecLen,ebx
	.else
		mov [edi].nDecLen,ecx
	.endif
	
	mov ecx,[edi].nHash0
	imul ecx,[edi].nHash1
	add ecx,[edi].nKey
	imul ecx,[edi].nUncLen
	
	mov esi,_lpBuff
	xor ebx,ebx
	xor eax,eax
	.while ebx<[edi].nDecLen
		mov al,[esi+ebx]
		add al,cl
		add al,bl
		mov al,byte ptr [eax+offset dbEncTable]
		mov [esi+ebx],al
		inc ebx
	.endw
	assume edi:nothing
	ret
_Encrypt endp

_EncryptIndex proc uses esi edi ebx _lpEntry,_nIndex
	LOCAL @nHash
	mov edi,_lpEntry
	assume edi:ptr _NpaEntry
	mov ecx,[edi].nHash0
	imul ecx,[edi].nHash1
	mov @nHash,ecx
	
	mov edx,ecx
	mov esi,_nIndex
	xor ebx,ebx
	.while ebx<[edi].nNameLen
		mov eax,ebx
		mov cl,0fch
		imul cl
		mov ecx,edx
		shr ecx,24
		sub al,cl
		mov ecx,edx
		shr ecx,16
		sub al,cl
		mov ecx,edx
		shr ecx,8
		sub al,cl
		sub al,dl
		mov ecx,esi
		shr ecx,24
		sub al,cl
		mov ecx,esi
		shr ecx,16
		sub al,cl
		mov ecx,esi
		shr ecx,8
		sub al,cl
		mov ecx,esi
		sub al,cl
		lea ecx,[edi].szName
		sub byte ptr [ecx+ebx],al
		inc ebx
	.endw
	ret
_EncryptIndex endp