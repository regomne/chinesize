.486
.model flat,stdcall
option casemap:none

include font.inc

.code

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
DllEntry proc _hInstance,_dwReason,_dwReserved
	.if _dwReason==DLL_PROCESS_ATTACH
		push _hInstance
		pop hInstance
	.endif
	mov eax,TRUE
	ret
DllEntry endp

_ReadFont proc
	LOCAL @hKey
	LOCAL @szFontName[64]:byte
	LOCAL @nLen
	invoke RegCreateKeyExW,HKEY_CURRENT_USER,$CTW0("Software\\Overflow\\SCHOOLDAYS HQ"),0,0,0,KEY_ALL_ACCESS,0,addr @hKey,0
	or eax,eax
	jne _Ex
	mov @nLen,32
	invoke RegQueryValueExW,@hKey,$CTW0("FontName"),0,0,addr @szFontName,addr @nLen
	or eax,eax
	jne _Next1
	lea ecx,@szFontName
	mov eax,@nLen
	.if word ptr [ecx+eax*2-2]!=0
		mov word ptr [ecx+eax*2],0
	.endif
	invoke lstrcpyW,offset g_LF.lfFaceName,addr @szFontName
	
_Next1:
	mov @nLen,4
	invoke RegQueryValueExW,@hKey,$CTW0("FontWeight"),0,0,addr @szFontName,addr @nLen
	or eax,eax
	jne _Ex2
	mov eax,dword ptr @szFontName
	mov g_LF.lfWeight,eax
_Ex2:
	invoke RegCloseKey,@hKey
_Ex:
	ret
_ReadFont endp

SetFont proc uses esi edi ebx _lpFont,_char
	LOCAL @nRealX
;	LOCAL @hdc
	LOCAL @this
	LOCAL @stgm:GLYPHMETRICS
	LOCAL @tm:TEXTMETRIC
	LOCAL @pLine
	mov @this,ecx
;	.if _lpFont
;		push _lpFont
;		mov eax,433060h
;		call eax
;	.endif
	.if !bHasFont
		mov eax,4ce528h
		invoke GetDC,[eax]
		push eax
		invoke CreateCompatibleDC,eax
		mov hdc,eax
		mov eax,4ce528h
		push [eax]
		call ReleaseDC
		
		invoke _ReadFont
		
		invoke CreateFontIndirectW,offset g_LF
		invoke SelectObject,hdc,eax
		invoke GetProcessHeap
		mov ebx,eax
		invoke HeapAlloc,ebx,0,900h
		mov lpTemp,eax
		invoke GetTextMetrics,hdc,offset g_tm
		mov bHasFont,1
	.endif
	mov edx,@this
	mov edi,[edx+24h]
	xor eax,eax
	mov ecx,900h/4
	rep stosd
	mov edi,[edx+28h]
	mov ecx,900h/4
	rep stosd
;	cmp _char,20h
;	je _space
;	cmp _char,3000h
;	je _space
;	mov edx,@this
	invoke GetGlyphOutlineW,hdc,_char,GGO_GRAY8_BITMAP,addr @stgm,900h,lpTemp,offset dbMat
	cmp eax,-1
	je _Ex
	
	mov ecx,g_tm.tmAscent
	sub ecx,@stgm.gmptGlyphOrigin.y
	shl ecx,4
	lea eax,[ecx+ecx*2+192+1]
	add eax,@stgm.gmptGlyphOrigin.x
	mov ecx,@this
	add eax,dword ptr [ecx+24h]
	mov edi,eax
	mov esi,lpTemp
	mov edx,@stgm.gmBlackBoxY
	mov eax,@stgm.gmBlackBoxX
	mov ecx,eax
	and ecx,3
	test ecx,ecx
	setne cl
	and eax,not 3
	lea eax,[eax+ecx*4]
	mov @nRealX,eax
	_loop1:
		mov ecx,@stgm.gmBlackBoxX
		mov @pLine,esi
		mov ebx,edi
		@@:
			lodsb
			.if al>=40h
				mov al,0ffh
			.else
				shl al,2
			.endif
			stosb
		loop @B
		mov esi,@pLine
		add esi,@nRealX
		lea edi,[ebx+48]
	dec edx
	jne _loop1

	
_space:
	mov edx,@this
	mov esi,[edx+24h]
	mov edi,lpTemp
	mov ecx,900h
	@@:
		lodsb
		sub al,1
		sbb al,al
		not al
		stosb
	loop @B
	
	mov edx,@this
	mov esi,[edx+24h]
	mov edi,lpTemp
	xor ebx,ebx
	.repeat
		xor ecx,ecx
		.repeat
			mov edx,ecx
			mov al,[esi+edx]
			or al,al
			jne _loop2
			cmp edx,0
			je _loop2
			mov al,[esi+edx-1]
			or al,al
			jne _loop3
			cmp edx,30h-1
			je _loop2
			mov al,[esi+edx+1]
			or al,al
			jne _loop3
			cmp ebx,0
			je _loop2
			sub edx,30h
			mov al,[esi+edx]
			or al,al
			jne _loop3
			cmp ebx,2fh
			je _loop2
			mov al,[esi+ecx+30h]
			or al,al
			je _loop2
			_loop3:
			mov byte ptr [edi+ecx],0ffh
			_loop2:
			inc ecx
		.until ecx>=30h
		add esi,30h
		add edi,30h
		inc ebx
	.until ebx>=30h
	
	mov edx,@this
	mov esi,lpTemp
	mov edi,[edx+28h]
	mov ecx,900h/4
	rep movsd
	
	mov edx,@this
	mov esi,lpTemp
	mov edi,[edx+28h]
	xor ebx,ebx
	.repeat
		xor ecx,ecx
		.repeat
			mov edx,ecx
			mov al,[esi+edx]
			or al,al
			jne _loop4
			cmp edx,0
			je _loop4
			mov al,[esi+edx-1]
			or al,al
			jne _loop5
			cmp edx,30h-1
			je _loop4
			mov al,[esi+edx+1]
			or al,al
			jne _loop5
			cmp ebx,0
			je _loop4
			sub edx,30h
			mov al,[esi+edx]
			or al,al
			jne _loop5
			cmp ebx,2fh
			je _loop4
			mov al,[esi+ecx+30h]
			or al,al
			je _loop4
			_loop5:
			mov byte ptr [edi+ecx],080h
			_loop4:
			inc ecx
		.until ecx>=30h
		add esi,30h
		add edi,30h
		inc ebx
	.until ebx>=30h
_Ex:
	ret
SetFont endp

end DllEntry