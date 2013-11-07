.486
.model flat,stdcall
option casemap:none

ADDR_TEXTSEG	equ		401000h
LEN_TEXTSEG		equ		5a000h
ADDR_RDATA		EQU		45B000H
LEN_RDATA		EQU		22000H

ADDR_CWE		equ		4418e6h
ADDR_CF			equ		42c25dh
ADDR_EFE		EQU		441C7DH
ADDR_SCMP		EQU		44049BH

ADDR_INST_E		EQU		443B90H

ADDR_JMP1		EQU		436004h
ADDR_JMP2		EQU		43AD44H

include d3d9.inc


.code
assume fs:nothing

PatchCallImport macro _Addr,_Dest
	mov eax,_Addr+0
	add eax,nDist
	mov ecx,_Dest-5
	sub ecx,eax
	mov [eax+1],ecx
	mov byte ptr [eax],0e8h
	mov byte ptr [eax+5],90h
endm

PatchCall macro _Addr,_Dest
	mov eax,_Addr+0
	add eax,nDist
	mov ecx,_Dest-5
	sub ecx,eax
	mov [eax+1],ecx
	mov byte ptr [eax],0e8h
endm

DllMain proc _hInstance,_dwReason,_unused
	local @mbi:MEMORY_BASIC_INFORMATION
	LOCAL @hInfo,@hName,@hMail
	LOCAL @lpInfo,@lpName,@lpMail
	.if _dwReason==DLL_PROCESS_ATTACH
		invoke GetModuleHandleW,0
		mov hInstance,eax
		lea ecx,[eax-400000h]
		mov nDist,ecx
		
		mov eax,ADDR_TEXTSEG
		ADD EAX,nDist
		invoke VirtualProtect,eax,LEN_TEXTSEG,PAGE_EXECUTE_READWRITE,offset dwTemp
		mov eax,ADDR_RDATA
		ADD EAX,nDist
		invoke VirtualProtect,eax,LEN_RDATA,PAGE_READWRITE,offset dwTemp
		
		PatchCallImport ADDR_CWE,offset NewCWE
		PatchCallImport ADDR_CF,offset NewCF
		PatchCallImport ADDR_EFE,offset NewEFE
		PatchCallImport ADDR_SCMP,offset NewSCmp
		
		;hook读取字符串指令
		mov eax,ADDR_INST_E
		add eax,nDist
		mov ecx,eax
		add eax,5
		mov edx,offset Inst_E
		sub edx,eax
		mov byte ptr [ecx],0e9h
		mov dword ptr [ecx+1],edx
		
		;跳过字符串地址校验
		mov eax,ADDR_JMP1
		add eax,nDist
		mov byte ptr [eax],0ebh
		mov eax,ADDR_JMP2
		add eax,nDist
		mov byte ptr [eax],0ebh
		
		;处理GetGlyphOutline对于全角空格获取不能的问题
		mov eax,430dbfh
		add eax,nDist
		mov dword ptr [eax],9090c033h
		
		;改默认字体
		mov ecx,45b59ch
		add ecx,nDist
		invoke lstrcpy,ecx,offset szFontName
		mov ecx,441baah
		add ecx,nDist
		mov dword ptr [ecx],offset szCharSet
		
		invoke GetFileAttributesW,$CTW0('txt')
		.if eax!=-1
			invoke OpenIdxs
			mov bRead,1
		.else
			mov bRead,0
		.endif
	.endif
_Ex:
	MOV EAX,TRUE
	ret
DllMain endp

SearchIdx proc uses esi ebx key
	lea esi,frFiles+16
	mov ecx,1
	mov ebx,key
	.while ecx<23
		mov eax,[esi]
		mov edx,[eax]
		cmp ebx,edx
		jb @F
		add esi,16
		inc ecx
	.endw
	add esi,16
	@@:
	sub esi,16
	assume esi:ptr FileRec
	invoke bsearch,ebx,[esi].lpIdx,[esi].nNum,4,offset cmpIdx
	test eax,eax
	jz _Ex
	sub eax,[esi].lpIdx
	add eax,[esi].lpTxtTable
	mov eax,[eax]
	assume esi:nothing
_Ex:
	ret
SearchIdx endp

Inst_E proc uses esi edi ebx
	mov edi,ecx
	mov ebx,[edi+820h]
	lea edx,[ebx+1]
	mov [edi+820h],edx
	mov byte ptr [edi+ebx*8+8],4
	mov esi,[edi+81ch]
	mov ecx,[edi+4h]
	xor eax,eax
	mov al,byte ptr [ecx+esi]
	lea edx,[esi+1]
	mov [edi+ebx*8+12],edx
	lea eax,[eax+esi+1]
	mov [edi+81ch],eax
	
	.if bRead
		invoke SearchIdx,esi
		.if eax
			sub eax,dword ptr [edi+4]
			mov [edi+ebx*8+12],eax
		.endif
	.endif
	
	ret
Inst_E endp

NewCWE proc se,cla,wn,s,x,y,w,h,p,m,i,pa
	LOCAL @szStr[200]:byte
	LOCAL @szStr1[100]:byte
	invoke MultiByteToWideChar,932,0,wn,-1,addr @szStr,100
	invoke WideCharToMultiByte,0,0,addr @szStr,-1,addr @szStr1,100,0,0
	invoke CreateWindowEx,se,cla,addr @szStr1,s,x,y,w,h,p,m,i,pa
	ret
NewCWE endp

NewCF proc h,w,e,o,we,i,u,so,cse,op,cp,q,pa,f
	.if bRead
		invoke CreateFont,h,w,e,o,we,i,u,so,86h,op,cp,q,pa,f
	.else
		invoke CreateFont,h,w,e,o,we,i,u,so,86h,op,cp,q,pa,f
	.endif
	ret
NewCF endp

NewEFE proc dc,lf,cb,pa,f
	mov eax,lf
	.if bRead
		mov byte ptr [eax+17h],86h
	.else
		mov byte ptr [eax+17h],80h
	.endif
	invoke EnumFontFamiliesEx,dc,lf,cb,pa,f
	ret
NewEFE endp

NewSCmp proc _str1,_str2
	invoke CompareString,411h,NORM_IGNORECASE,_str1,-1,_str2,-1
	sub eax,2
	ret
NewSCmp endp 

;
ReadIdx proc uses edi esi ebx _lpIdx,_lpTxt,_num
	LOCAL @hIdx,@hTxt,@nIdxLen,@nTxtLen
	
	lea edi,frFiles
	mov eax,_num
	shl eax,1
	lea edi,[edi+eax*8]
	assume edi:ptr FileRec
	invoke CreateFile,_lpIdx,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	cmp eax,-1
	je _Ex
	mov @hIdx,eax
	invoke CreateFile,_lpTxt,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
		invoke CloseHandle,@hIdx
		jmp _Ex
	.endif
	mov @hTxt,eax
	
	invoke GetFileSize,@hIdx,0
	mov @nIdxLen,eax
	invoke GetFileSize,@hTxt,0
	mov @nTxtLen,eax
	
	invoke GlobalAlloc,GPTR,@nIdxLen
	.if !eax
		invoke CloseHandle,@hIdx
		invoke CloseHandle,@hTxt
		jmp _Ex
	.endif
	mov [edi].lpIdx,eax
	invoke GlobalAlloc,GPTR,@nTxtLen
	.if !eax
		@@:
		invoke GlobalFree,[edi].lpIdx
		invoke CloseHandle,@hIdx
		invoke CloseHandle,@hTxt
		jmp _Ex
	.endif
	mov [edi].lpTxt,eax
	
	invoke ReadFile,@hIdx,[edi].lpIdx,@nIdxLen,offset dwTemp,0
	.if !eax
	_lbl1:
		invoke GlobalFree,[edi].lpTxt
		jmp @B
	.endif
	invoke ReadFile,@hTxt,[edi].lpTxt,@nTxtLen,offset dwTemp,0
	test eax,eax
	jz _lbl1
	mov ecx,[edi].lpTxt
	cmp word ptr [ecx],0feffh
	jne _lbl1
	
	invoke GlobalAlloc,GPTR,@nTxtLen
	test eax,eax
	jz _lbl1
	mov ebx,eax
	mov ecx,[edi].lpTxt
	add ecx,2
	mov eax,@nTxtLen
	sub eax,2
	shr eax,1
	invoke WideCharToMultiByte,936,0,ecx,eax,ebx,@nTxtLen,0,0
	.if !eax
		invoke GlobalFree,ebx
		jmp _lbl1
	.endif
	mov @nTxtLen,eax
	invoke GlobalFree,[edi].lpTxt
	mov [edi].lpTxt,ebx
	
	mov eax,@nIdxLen
	shr eax,2
	mov [edi].nNum,eax
	
	mov esi,[edi].lpTxt
	mov edx,esi
	add edx,@nTxtLen
	xor ebx,ebx
	.while esi<edx
		mov al,[esi]
		inc esi
		.if al==0dh && byte ptr [esi]==0ah
			inc esi
			mov word ptr [esi-2],0
			inc ebx
		.endif
	.endw
	.if word ptr [esi-2]!=0
		inc ebx
	.endif
	cmp ebx,[edi].nNum
	jne _lbl1
	shl ebx,2
	invoke GlobalAlloc,GPTR,ebx
	test eax,eax
	je _lbl1
	mov [edi].lpTxtTable,eax
	invoke CloseHandle,@hIdx
	invoke CloseHandle,@hTxt
	
	mov esi,[edi].lpTxt
	mov edx,esi
	add edx,@nTxtLen
	mov edi,[edi].lpTxtTable
	assume edi:nothing
	.while esi<edx
		mov eax,esi
		mov [edi],eax
		add edi,4
		.while word ptr [esi]
			.break .if esi>=edx
			inc esi
		.endw
		add esi,2
	.endw
_Ex:
	ret
ReadIdx endp

OpenIdxs proc uses edi esi ebx
	LOCAL @idxn[100]:byte
	LOCAL @txtn[100]:byte
	invoke ReadIdx,$CTA0("txt\\0000.idx"),$CTA0("txt\\0000.txt"),0
	
	mov ebx,4171
	lea esi,@idxn
	lea edi,@txtn
	.while ebx<=4191
		invoke wsprintf,esi,$CTA0("txt\\%04d.idx"),ebx
		invoke wsprintf,edi,$CTA0("txt\\%04d.txt"),ebx
		lea ecx,[ebx-4171+1]
		invoke ReadIdx,esi,edi,ecx
		inc ebx
	.endw
	mov nFiles,22
	ret
OpenIdxs endp

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

OPTION LANGUAGE:C
cmpIdx proc
	mov eax,[esp+4]
	mov ecx,[esp+8]
	sub eax,[ecx]
	ret
cmpIdx endp

end DllMain