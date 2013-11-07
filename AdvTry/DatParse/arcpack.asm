.486
.model flat,stdcall
option casemap:none


include arcpack.inc

CLP_INIT_OFF			EQU		11H
CLP_SEC_OFF			EQU		46H
szCPKey				db		'ClOVeRrE'
szCPCode			db		1Fh, 03h, 4Fh, 56h, 0B9h, 4Ch, 72h, 45h
szCPArchive			db		'ARCHIVE',0,0,0,0,0
szUBOM				db		0ffh,0feh

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
invoke LoadIcon,hInstance,500
mov hIcon,eax
invoke GetProcessHeap
mov hHeap,eax

invoke _WinMain

invoke ExitProcess,NULL

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­

_XorDecode proc uses esi ebx _buff,_len,_key,_offset
	mov esi,_buff
	mov ecx,_len
	mov edx,_offset
	mov ebx,_key
	@@:
		mov eax,edx
		and eax,7
		mov al,[ebx+eax]
		xor byte ptr [esi],al
		inc esi
		inc edx
	loop @B
	ret
_XorDecode endp

_WinMain proc
	LOCAL @hDat
	LOCAL @datHdr:_DatHeader
	LOCAL @lpIndex
	LOCAL @code1[8]:byte
	LOCAL @hReport
	invoke CreateFile,$CTA0("report.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov @hReport,eax
	invoke WriteFile,eax,offset szUBOM,2,offset dwTemp,0
	
	invoke CreateFile,$CTA0('K:\\Clover Point\\grp.dat'),GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hDat,eax
	invoke SetFilePointer,eax,CLP_INIT_OFF,0,FILE_BEGIN
	invoke ReadFile,@hDat,addr @code1,8,offset dwTemp,0
	invoke _XorDecode,addr @code1,8,offset szCPKey,0
	
	invoke SetFilePointer,@hDat,CLP_SEC_OFF,0,FILE_BEGIN
	invoke ReadFile,@hDat,addr @datHdr,sizeof _DatHeader,offset dwTemp,0
	invoke _XorDecode,addr @datHdr,sizeof _DatHeader,addr @code1,0
	
	mov ecx,@datHdr.nIndexOffset
	add ecx,CLP_SEC_OFF
	invoke SetFilePointer,@hDat,ecx,0,FILE_BEGIN
	mov eax,@datHdr.nIndexEntryLen
	mul @datHdr.nIndexEntries
	mov ebx,eax
	invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
	mov @lpIndex,eax
	invoke ReadFile,@hDat,eax,ebx,offset dwTemp,0
	
	invoke _XorDecode,@lpIndex,ebx,addr @code1,@datHdr.nIndexOffset
	
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,1024
	mov edi,eax
	xor ebx,ebx
	mov esi,@lpIndex
	assume esi:ptr _DatEntry
	.while ebx<@datHdr.nIndexEntries
		invoke MultiByteToWideChar,936,0,addr [esi].szName,-1,edi,512
		.if !eax
			int 3
		.endif
;		mov ecx,edi
;		.while word ptr [ecx]!=0
;			.if word ptr [ecx]==30fbh
;				mov word ptr [ecx],0ff0eh
;			.endif
;			add ecx,2
;		.endw
		invoke wsprintfW,offset szFileN,$CTW0("%s\n"),edi
		shl eax,1
		invoke WriteFile,@hReport,offset szFileN,eax,offset dwTemp,0
		invoke WideCharToMultiByte,936,0,edi,-1,addr [esi].szName,263,$CTA0("@"),offset dwTemp
		.if dwTemp
			int 3
		.endif
		
		add esi,sizeof _DatEntry
		inc ebx
	.endw
	
	mov eax,@datHdr.nIndexEntryLen
	mul @datHdr.nIndexEntries
	mov ebx,eax
	invoke _XorDecode,@lpIndex,ebx,addr @code1,@datHdr.nIndexOffset

	mov ecx,@datHdr.nIndexOffset
	add ecx,CLP_SEC_OFF
;	invoke SetFilePointer,@hDat,ecx,0,FILE_BEGIN
;	invoke WriteFile,@hDat,@lpIndex,ebx,offset dwTemp,0
	invoke CloseHandle,@hDat
	assume esi:nothing
	ret
_WinMain endp



end start