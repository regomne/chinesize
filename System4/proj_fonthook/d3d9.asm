.486
.model flat,stdcall
option casemap:none

F_AllocMem				equ		415bb0h
F_OldGetCharOutLine		equ		5fd550h
F_OldGetCharWidth		equ		5fd780h
MIN_HEIGHT				EQU		30H
MAX_HEIGHT				EQU		0C0H
FONT_TYPES				EQU		3

ADDR_GET_OUTLINE		EQU		5FF0f4H
ADDR_GET_WIDTH			EQU		5ffb43h

include d3d9.inc


.code
assume fs:nothing
DllMain proc uses edi ebx _hInstance,_dwReason,_unused
	local @mbi:MEMORY_BASIC_INFORMATION
	.if _dwReason==DLL_PROCESS_ATTACH
		invoke GetModuleHandleW,0
		mov hInstance,eax
		lea ecx,[eax-400000h]
		mov nDist,ecx
		add ecx,ADDR_GET_OUTLINE
		invoke VirtualQuery,ecx,addr @mbi,sizeof @mbi
		or eax,eax
		je _Err
		mov ebx,PAGE_EXECUTE_READWRITE
		invoke VirtualProtect,@mbi.BaseAddress,@mbi.RegionSize,ebx,offset dwTemp
		.if !eax
_Err:
			invoke GetLastError
			invoke wsprintf,offset dwBuffer,$CTA0("v2 code: %X"),eax
			invoke MessageBox,0,offset dwBuffer,0,MB_OK
			ret
		.endif
		mov eax,offset GetCharOutLine
		mov ecx,ADDR_GET_OUTLINE+5
		add ecx,nDist
		sub eax,ecx
		mov [ecx-5+1],eax
		mov eax,offset GetCharWidth1
		mov ecx,ADDR_GET_WIDTH+5
		add ecx,nDist
		sub eax,ecx
		mov [ecx-5+1],eax
		
;		invoke HookApi,$CTA0("kernel32.dll"),$CTA0('CreateFileA'),offset NewCrFile
;		invoke HookApi,$CTA0("kernel32.dll"),$CTA0("CreateDirectoryA"),offset NewCrDir
;		invoke HookApi,$CTA0("kernel32.dll"),$CTA0("CopyFileA"),offset NewCpFile
;		invoke HookApi,$CTA0("kernel32.dll"),$CTA0("FindFirstFileA"),offset NewFind
;		invoke HookApi,$CTA0("kernel32.dll"),$CTA0("FindNextFileA"),offset NewFindN
		
		lea edi,g_FontInfo
		xor ebx,ebx
		push esi
		.while ebx<FONT_TYPES
			lea edx,dbNames
			assume edi:ptr FontInfo
			mov esi,edi
			invoke lstrcpyW,edi,[edx+ebx*4]
			lea edi,[edi].nDescent
			mov ecx,256
			or eax,-1
			rep stosd
			lea edi,[esi+sizeof FontInfo]
			inc ebx
			assume edi:nothing
		.endw
		pop esi
	.endif
	MOV EAX,TRUE
	ret
DllMain endp

HookApi proc uses ebx _lpszModuleName,_lpszName,_NewAddr
	invoke GetModuleHandle,_lpszModuleName
	or eax,eax
	je _Err
	invoke GetProcAddress,eax,_lpszName
	mov ebx,eax
	cmp word ptr [ebx],0ff8bh
	jne _Err
	lea ecx,[ebx-5]
	invoke VirtualProtect,ecx,7,PAGE_EXECUTE_READWRITE,offset dwTemp
	or eax,eax
	je _Err
	mov word ptr [ebx],0f9ebh
	mov eax,_NewAddr
	sub eax,ebx
	lea ecx,[ebx-5]
	mov [ecx+1],eax
	mov byte ptr [ecx],0e9h
	ret
_Err:
	xor eax,eax
	ret
HookApi endp

NewCrFile proc uses ebx esi fna,da,sm,sa,cd,att,ht
	invoke lstrlen,fna
	inc eax
	mov esi,eax
	shl eax,1
	lea ebx,szGName
	.if eax>=256
		invoke GlobalAlloc,GMEM_FIXED,eax
		mov ebx,eax
	.endif
	invoke MultiByteToWideChar,932,0,fna,-1,ebx,esi
	invoke CreateFileW,ebx,da,sm,sa,cd,att,ht
	.if ebx!=offset szGName
		mov esi,eax
		invoke GlobalFree,ebx
		mov eax,esi
	.endif
	ret
NewCrFile endp
NewCrDir proc uses esi ebx dn,asa
	invoke lstrlen,dn
	inc eax
	mov esi,eax
	shl eax,1
	lea ebx,szGName
	.if eax>=256
		invoke GlobalAlloc,GMEM_FIXED,eax
		mov ebx,eax
	.endif
	invoke MultiByteToWideChar,932,0,dn,-1,ebx,esi
	invoke CreateDirectoryW,ebx,asa
	.if ebx!=offset szGName
		mov esi,eax
		invoke GlobalFree,ebx
		mov eax,esi
	.endif
	ret
NewCrDir endp
NewFind proc uses esi ebx edi fna,pfd
	LOCAL @fd:WIN32_FIND_DATAW
	invoke lstrlen,fna
	inc eax
	mov esi,eax
	shl eax,1
	lea ebx,szGName
	.if eax>=256
		invoke GlobalAlloc,GMEM_FIXED,eax
		mov ebx,eax
	.endif
	invoke MultiByteToWideChar,932,0,fna,-1,ebx,esi
	invoke FindFirstFileW,ebx,addr @fd
	mov esi,eax
	.if ebx!=offset szGName
		invoke GlobalFree,ebx
	.endif
	.if esi!=-1
		mov ebx,esi
		lea esi,@fd
		mov edi,pfd
		mov ecx,sizeof WIN32_FIND_DATA/4
		rep movsd
		mov eax,pfd
		lea ecx,WIN32_FIND_DATA.cFileName[eax]
		invoke WideCharToMultiByte,932,0,addr @fd.cFileName,-1,ecx,MAX_PATH,0,0
		mov esi,ebx
	.endif
	mov eax,esi
	ret
NewFind endp
NewFindN proc uses esi ebx edi hf,pfd
	LOCAL @fd:WIN32_FIND_DATAW
	invoke FindNextFileW,hf,addr @fd
	.if eax
		mov ebx,eax
		lea esi,@fd
		mov edi,pfd
		mov ecx,sizeof WIN32_FIND_DATA/4
		rep movsd
		mov eax,pfd
		lea ecx,WIN32_FIND_DATA.cFileName[eax]
		invoke WideCharToMultiByte,932,0,addr @fd.cFileName,-1,ecx,MAX_PATH,0,0
		mov eax,ebx
	.endif
	ret
NewFindN endp
NewCpFile proc uses esi edi fn1,fn2,bo
	LOCAL @p1,@p2
	invoke lstrlen,fn1
	inc eax
	mov esi,eax
	shl eax,1
	invoke GlobalAlloc,GMEM_FIXED,eax
	mov @p1,eax
	invoke lstrlen,fn2
	inc eax
	mov edi,eax
	shl eax,1
	invoke GlobalAlloc,GMEM_FIXED,eax
	mov @p2,eax
	invoke MultiByteToWideChar,932,0,fn1,-1,@p1,esi
	invoke MultiByteToWideChar,932,0,fn2,-1,@p2,edi
	invoke CopyFileW,@p1,@p2,bo
	mov esi,eax
	invoke GlobalFree,@p1
	invoke GlobalFree,@p2
	mov eax,esi
	ret
NewCpFile endp

Direct3DCreate9 proc dwVersion
	LOCAL @szSystem[64]:word
	invoke GetSystemDirectoryW,addr @szSystem,64
	or eax,eax
	je _Ex
	invoke lstrcatW,addr @szSystem,$CTW0("\\d3d9.dll")
	invoke LoadLibraryW,addr @szSystem
	or eax,eax
	je _Ex
	invoke GetProcAddress,eax,$CTA0("Direct3DCreate9")
	or eax,eax
	je _Ex
	push dwVersion
	call eax
_Ex:
	ret
Direct3DCreate9 endp

GetCharOutLine proc uses edi esi ebx _dw1,_nHeight,_nChar,_lpBitsPerRow,_lpDescent,_lpMem
	LOCAL @nFontType,@nBytesPerRow,@nBytesPerRowRaw,@pFontInfo,@nNewH
	LOCAL @pEnd
	LOCAL @lf:LOGFONTW
	LOCAL @tm:TEXTMETRICW
	LOCAL @gm:GLYPHMETRICS
	mov @nFontType,ecx
	cmp @nFontType,FONT_TYPES
	jae _Err
	cmp _nHeight,MIN_HEIGHT
	jb _Err
	cmp _nHeight,MAX_HEIGHT
	ja _Err
	cmp _lpBitsPerRow,0
	jle _Err
	
	push offset _Handler
	push fs:[0]
	mov fs:[0],esp

	mov eax,_nHeight
	mov edx,eax
	shr edx,2
	sub eax,edx
	mov @nNewH,eax
	
	.if !g_hdc
		invoke CreateCompatibleDC,0
		or eax,eax
		je _Old
		mov g_hdc,eax
	.endif
	
	lea edi,@lf
	mov ecx,sizeof(LOGFONTW)
	xor eax,eax
	rep stosb
	
	mov edi,@nFontType
	imul edi,sizeof(FontInfo)
	add edi,offset g_FontInfo
	mov @pFontInfo,edi
	assume edi:ptr FontInfo
	mov ebx,_nHeight
	mov eax,[edi].hFont[ebx*4]
	.if !eax
		mov eax,@nNewH
		mov @lf.lfHeight,eax
		mov @lf.lfWeight,FW_NORMAL
		mov @lf.lfCharSet,086h
		mov @lf.lfQuality,CLEARTYPE_QUALITY
		mov @lf.lfPitchAndFamily,FIXED_PITCH
		invoke lstrcpyW,addr @lf.lfFaceName,edi
		invoke CreateFontIndirectW,addr @lf
		or eax,eax
		je _Old
		mov [edi].hFont[ebx*4],eax
	.endif
	
	mov eax,g_LastFont
	mov ecx,g_LastSize
	.if eax!=@nFontType || ecx!=_nHeight
		invoke SelectObject,g_hdc,[edi].hFont[ebx*4]
		cmp eax,-1
		je _Old
		mov eax,@nFontType
		mov ecx,_nHeight
		mov g_LastFont,eax
		mov g_LastSize,ecx
	.endif
	
	mov eax,[edi].nDescent[ebx*4]
	.if eax==-1
		invoke GetTextMetricsW,g_hdc,addr @tm
		xor ecx,ecx
		.if eax
			mov eax,@tm.tmDescent
			mov ecx,@tm.tmAscent
		.endif
		mov [edi].nDescent[ebx*4],eax
		mov [edi].nAscent[ebx*4],ecx
	.endif
	mov ecx,_lpDescent
	.if ecx
		mov dword ptr [ecx],eax
	.endif
	assume edi:nothing
	
	mov eax,_nChar
	.if ax>=824fh && ax<=8258h
		sub _nChar,824fh-0a3b0h
	.endif
	.if _lpMem
		invoke GetGlyphOutlineA,g_hdc,_nChar,GGO_BITMAP,addr @gm,8192,offset dwBuffer,offset dbMat
	.else
		invoke GetGlyphOutlineA,g_hdc,_nChar,GGO_BITMAP,addr @gm,0,0,offset dbMat
	.endif
	cmp eax,-1
	je _Old
	movzx eax,@gm.gmCellIncX
	mov ecx,_lpBitsPerRow
	mov [ecx],eax
	.if !_lpMem
		mov eax,1
		ret
	.endif
	
	add eax,31
	shr eax,5
	shl eax,2
	mov @nBytesPerRow,eax
	mul _nHeight
;	mul @gm.gmBlackBoxY
	mov esi,eax
	push eax
	mov eax,_lpMem
	mov ecx,F_AllocMem
	add ecx,nDist
	call ecx
	mov edx,_lpMem
	mov eax,[edx+0ch]
	cmp eax,[edx+10h]
	je _Old
	
	mov ecx,esi
	shr ecx,2
	mov edi,[edx+0ch]
	lea eax,[edi+esi]
	mov @pEnd,eax
	xor eax,eax
	rep stosd
	
	mov eax,@gm.gmBlackBoxX
	add eax,31
	shr eax,5
	shl eax,2
	mov @nBytesPerRowRaw,eax
	
	mov eax,@pFontInfo
	mov ecx,_nHeight
	mov edx,FontInfo.nAscent[eax][ecx*4]
	
	xor ebx,ebx
	lea esi,dwBuffer
	mov ecx,_lpMem
	mov edi,[ecx+0ch]
	sub edx,@gm.gmptGlyphOrigin.y
	jl _Old
	mov eax,_nHeight
	mov ecx,eax
	shr ecx,3
	sub eax,ecx
	sub eax,edx
;	dec eax
	mul @nBytesPerRow
	add edi,eax
	
	mov ecx,@gm.gmptGlyphOrigin.x
	cmp ecx,0
	jge @F
	xor ecx,ecx
	@@:
	shr ecx,3
	add edi,ecx
	
	mov eax,esi
	mov edx,edi
	.while ebx<@gm.gmBlackBoxY
		mov ecx,_lpMem
		.if edi<[ecx+0ch]
			mov eax,0
			mov [eax],ecx
		.endif
		mov ecx,@nBytesPerRowRaw
		shr ecx,2
		rep movsd
		.if edi>=@pEnd
			mov eax,0
			mov [eax],ecx
		.endif
		sub edx,@nBytesPerRow
		mov edi,edx
		add eax,@nBytesPerRowRaw
		mov esi,eax
		inc ebx
	.endw
	
	pop fs:[0]
	pop ecx
	
	mov eax,TRUE
	ret
_Old:
	pop fs:[0]
	pop ecx
	
	push _lpMem
	push _lpDescent
	push _lpBitsPerRow
	push _nChar
	push _nHeight
	push _dw1
	mov ecx,@nFontType
	mov eax,F_OldGetCharOutLine
	add eax,nDist
	call eax
	ret
_Err:
	xor eax,eax
	ret
_ErrSEH:
	pop fs:[0]
	pop eax
	invoke wsprintf,offset dwBuffer,$CTA0('Error Occurs. Pos: %08X\nWanna Exit?'),dwErrPos
	invoke MessageBox,0,offset dwBuffer,0,MB_YESNO or MB_DEFBUTTON2
	.if EAX==IDYES
		invoke ExitProcess,0
	.endif
	xor eax,eax
	ret
_Handler:
	mov eax,[esp+0ch]
	mov ecx,[eax+0b8h]
	mov [eax+0b8h],offset _ErrSEH
	mov dwErrPos,ecx
	xor eax,eax
	retn 0ch
GetCharOutLine endp

GetCharWidth1 proc _nHeight,_nChar,_pWidth
	LOCAL @nFontType
	mov ecx,esi
	invoke GetCharOutLine,edi,_nHeight,_nChar,_pWidth,0,0
	and eax,1
	jz _Old
	ret
_Old:
	push _pWidth
	push _nChar
	push _nHeight
	mov eax,F_OldGetCharWidth
	add eax,nDist
	call eax
	ret
GetCharWidth1 endp

end DllMain