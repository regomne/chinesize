.486
.model flat,stdcall
option casemap:none


include exeext.inc

SONGS_START	EQU		0FBE80H
SONGS_END		EQU		1009A8H
SONGS_SIZE		EQU		SONGS_END-SONGS_START

STRLEN			EQU		100H
NUM_SONGS		EQU		SONGS_SIZE/STRLEN

STR_START		equ		34648h
STR_END			equ		06ee48h
STR_SIZE			EQU		STR_END-STR_START

NUM_STRS		EQU		STR_SIZE/STRLEN

.code


_ExtractStr proc uses edi esi ebx _lpBuff,_lpTxt,_nStr
	mov esi,_lpBuff
	mov edi,_lpTxt
	mov word ptr [edi],0feffh
	add edi,2
	xor ebx,ebx
	.while ebx<_nStr
		invoke MultiByteToWideChar,932,0,esi,-1,edi,10000
		lea edi,[edi+eax*2+2]
		mov dword ptr [edi-4],0a000dh
		add esi,STRLEN
		inc ebx
	.endw
	sub edi,_lpTxt
	mov eax,edi
	ret
_ExtractStr endp

Main3 proc
	LOCAL @hExeFile,@hTxt
	LOCAL @nTxtSize
	LOCAL @lpSongs,@lpSongs2,@lpTxt
	LOCAL @szTemp[STRLEN*2]:byte
	LOCAL @str1[64]:byte
	invoke CreateFile,$CTA0("sakura.exe"),GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hExeFile,eax
	
	invoke SetFilePointer,@hExeFile,STR_START,0,FILE_BEGIN
	invoke VirtualAlloc,0,STR_SIZE,MEM_COMMIT,PAGE_READWRITE
	mov @lpSongs,eax
	invoke ReadFile,@hExeFile,@lpSongs,STR_SIZE,offset dwTemp,0
	
	invoke VirtualAlloc,0,500000,MEM_COMMIT,PAGE_READWRITE
	mov @lpTxt,eax
	
	invoke _ExtractStr,@lpSongs,@lpTxt,NUM_STRS
	mov ebx,eax
	invoke CreateFile,$CTA0("abc.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	push eax
	invoke WriteFile,eax,@lpTxt,ebx,offset dwTemp,0
	call CloseHandle
	
	ret
Main3 endp

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
call Main3
invoke ExitProcess,NULL


_FindStar proc uses edi _lpBuff,_nSize
	mov edi,_lpBuff
	mov ecx,edi
	add ecx,_nSize
	mov byte ptr [ecx],0
	.while edi<ecx
		.if word ptr [edi]==0f4a1h
			lea eax,[edi+2]
			.repeat
				inc edi
			.until word ptr [edi]==0a0dh || !byte ptr [edi]
			.if word ptr [edi-2]==0f4a1h
				sub edi,2
			.endif
			mov byte ptr [edi],0
			ret
		.endif
		inc edi
	.endw
	xor eax,eax
	ret
_FindStar endp

MyMain proc
	LOCAL @hExeFile,@hTxt
	LOCAL @nTxtSize
	LOCAL @lpSongs,@lpSongs2,@lpTxt
	LOCAL @szTemp[STRLEN*2]:byte
	LOCAL @str1[64]:byte
	invoke CreateFile,$CTA0("sakura.exe"),GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hExeFile,eax
	
	invoke SetFilePointer,@hExeFile,STR_START,0,FILE_BEGIN
	invoke VirtualAlloc,0,STR_SIZE,MEM_COMMIT,PAGE_READWRITE
	mov @lpSongs,eax
	invoke ReadFile,@hExeFile,@lpSongs,STR_SIZE,offset dwTemp,0
	
	invoke VirtualAlloc,0,500000,MEM_COMMIT,PAGE_READWRITE
	mov @lpTxt,eax
	
	invoke VirtualAlloc,0,STR_SIZE,MEM_COMMIT,PAGE_READWRITE
	mov @lpSongs2,eax
	mov esi,@lpSongs
	mov edi,@lpSongs2
	mov ebx,NUM_STRS/2
	invoke SetCurrentDirectory,offset szTxt
	.repeat
		mov eax,esi
		mov ecx,STRLEN/4
		rep movsd
		lea ecx,@str1
		mov byte ptr [ecx],'x'
		inc ecx
		invoke lstrcpy,ecx,eax
		invoke lstrcat,addr @str1,$CTA0(".txt")
		invoke CreateFile,addr @str1,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		.if eax==-1
			invoke MultiByteToWideChar,932,0,esi,-1,addr @szTemp,STRLEN
			invoke WideCharToMultiByte,936,0,addr @szTemp,-1,edi,STRLEN,0,0
		.else
			mov @hTxt,eax
			invoke GetFileSize,eax,0
			mov @nTxtSize,eax
			invoke ReadFile,@hTxt,@lpTxt,eax,offset dwTemp,0
			invoke _FindStar,@lpTxt,@nTxtSize
			test byte ptr [eax],0
			invoke lstrcpy,edi,eax
			invoke CloseHandle,@hTxt
		.endif 
		add edi,STRLEN
		ADD ESI,STRLEN
		dec ebx
	.until !ebx
	
	invoke SetFilePointer,@hExeFile,STR_START,0,FILE_BEGIN
	invoke WriteFile,@hExeFile,@lpSongs2,STR_SIZE,offset dwTemp,0
	ret
MyMain endp

MyMain2 proc
	LOCAL @hExeFile,@hTxt
	LOCAL @nTxtSize
	LOCAL @lpSongs,@lpSongs2,@lpTxt
	LOCAL @szTemp[STRLEN*2]:byte
	LOCAL @str1[64]:byte
	invoke CreateFile,$CTA0("sakura.exe"),GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hExeFile,eax
	invoke CreateFile,$CTA0("select.txt"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hTxt,eax
	
	invoke SetFilePointer,@hExeFile,STR_START,0,FILE_BEGIN
	invoke VirtualAlloc,0,STR_SIZE,MEM_COMMIT,PAGE_READWRITE
	mov @lpSongs,eax
	invoke ReadFile,@hExeFile,@lpSongs,STR_SIZE,offset dwTemp,0
	
	invoke VirtualAlloc,0,500000,MEM_COMMIT,PAGE_READWRITE
	mov @lpTxt,eax
	invoke GetFileSize,@hTxt,0
	mov @nTxtSize,eax
	invoke ReadFile,@hTxt,@lpTxt,@nTxtSize,offset dwTemp,0
	
	invoke VirtualAlloc,0,STR_SIZE,MEM_COMMIT,PAGE_READWRITE
	mov @lpSongs2,eax
	mov esi,@lpSongs
	mov edi,@lpTxt
;	mov ebx,NUM_STRS2/2
	invoke SetCurrentDirectory,offset szTxt
	.repeat
		add esi,STRLEN
		mov eax,edi
		.while word ptr [edi]!=0a0dh
			inc edi
		.endw
		mov byte ptr [edi],0
		add edi,2
		invoke lstrcpy,esi,eax
		invoke lstrlen,esi
		lea ecx,[esi+eax]
		mov dword ptr [ecx],0
		mov dword ptr [ecx+4],0
		mov dword ptr [ecx+8],0
		add esi,STRLEN
		dec ebx
	.until !ebx

	
;	invoke SetFilePointer,@hExeFile,STR_START2,0,FILE_BEGIN
;	invoke WriteFile,@hExeFile,@lpSongs,STR_SIZE2,offset dwTemp,0
	ret
MyMain2 endp

end start