.486
.model flat,stdcall
option casemap:none


include extract.inc
include names.inc
include prescan.asm

NAMEFUNC		EQU		0A38H
PUSHOFF			EQU		12H

;NAMEFUNC		EQU		3951
;PUSHOFF			EQU		24

.code

start:
;｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ｡ｭ
invoke GetModuleHandle,0
mov hInstance,eax
invoke GetProcessHeap
mov hHeap,eax
call MyMain
invoke ExitProcess,NULL


MyMain proc
	LOCAL @hFile,@hTxt
	LOCAL @nFuncs
	LOCAL @lpBuff
	LOCAL @lpTxt
	LOCAL @szStr1[1024]:byte
	LOCAL @lpAinInfo
	invoke CreateFile,$CTA0("r22.ain"),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	mov @hFile,eax
	
	invoke GetFileSize,@hFile,0
	mov ebx,eax
	invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
	mov @lpBuff,eax
	invoke ReadFile,@hFile,@lpBuff,ebx,offset dwTemp,0
	invoke CloseHandle,@hFile
	
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,sizeof _AinSegs
	mov @lpAinInfo,eax
	
	invoke _PreScan,@lpBuff,ebx,@lpAinInfo
	.if eax
		invoke MessageBox,0,$CTA0("ﾔ､ﾉｨﾃ靆ｧｰﾜ｣｡"),0,MB_ICONERROR
		invoke ExitProcess,0
	.endif
	
	invoke VirtualAlloc,0,1000000,MEM_COMMIT,PAGE_READWRITE
	mov @lpTxt,eax
	
	invoke CreateDirectory,$CTA0("Rance2"),0
	invoke SetCurrentDirectory,$CTA0("Rance2")
	.if !eax
		invoke MessageBox,0,$CTA0("ﾎﾞｷｨｷﾃﾎﾊﾄｿﾂｼ｣｡"),0,MB_ICONERROR
		invoke ExitProcess,0
	.endif
	
	invoke _ProcessCode,@lpAinInfo,@lpTxt
	
;	invoke _ProcessFunc,@lpAinInfo,@lpTxt

;	invoke _ProcessSwitch,@lpAinInfo,@lpTxt

	mov ecx,@lpAinInfo
	invoke _OutText,addr _AinSegs.Strings[ecx],@lpTxt
	
	ret
MyMain endp

_OutText proc _lpSegment,_lpTxt
	LOCAL @szStr1[1024]:byte
	LOCAL @hFile
	invoke CreateFile,$CTA0("Strings.txt"),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	mov @hFile,eax
	mov ecx,_lpSegment
	mov esi,_Segment.lpRestruct[ecx]
	mov ecx,_Segment.lpAddr[ecx]
	mov ebx,[ecx]
	mov edi,_lpTxt
	mov word ptr [edi],0feffh
	add edi,2
	or ebx,ebx
	je _endloop1
	_loop1:
		lodsd
		lea ecx,@szStr1
		invoke MultiByteToWideChar,932,0,eax,-1,ecx,512
		lea ecx,@szStr1
		.repeat
			mov ax,[ecx]
			add ecx,2
			.if ax==0ah
				mov word ptr [ecx-2],'$'
			.endif
		.until !ax
		invoke wsprintfW,edi,$CTW0("%s\n"),addr @szStr1
		lea edi,[edi+eax*2]
		dec ebx
		jnz _loop1
	_endloop1:
	sub edi,_lpTxt
	invoke WriteFile,@hFile,_lpTxt,edi,offset dwTemp,0
	invoke CloseHandle,@hFile
	ret
_OutText endp

_ProcessCode proc uses esi edi ebx _lpAinInfo,_lpTxt
	LOCAL @szStr1[1024]:byte
	LOCAL @lpCurTxt
	LOCAL @pCodeEnd
	LOCAL @lpSwitches
	
	invoke HeapAlloc,hHeap,HEAP_ZERO_MEMORY,8000
	mov @lpSwitches,eax
	
	mov ecx,_lpTxt
	lea ebx,[ecx+2]
	mov word ptr [ecx],0feffh
	
	mov ecx,_lpAinInfo
	mov esi,_AinSegs.Code.lpAddr[ecx]
	mov eax,esi
	add eax,_AinSegs.Code.nSize[ecx]
	mov edi,_AinSegs.Message0.lpRestruct[ecx]
	mov @pCodeEnd,eax
	.if word ptr [esi]==62h && dword ptr [esi+2]==0
		add esi,6
	.endif
	
	.repeat
		xor eax,eax
		lodsw
		.if eax>0f0h
			int 3
		.endif
		lea ecx,dbCodeTable
		mov al,[ecx+eax]
		cmp al,0
		jl @F
			add esi,eax
			.continue
		@@:
		.if al==P_MES
			lodsd
			lea ecx,@szStr1
			invoke MultiByteToWideChar,932,0,[edi+eax*4],-1,ecx,512
			invoke wsprintfW,ebx,$CTW0("%s\n"),addr @szStr1
			lea ebx,[ebx+eax*2]
			.continue
		.elseif al==P_CALL
			lodsd
			.if eax==NAMEFUNC && word ptr [esi-PUSHOFF]==0
				mov eax,[esi-PUSHOFF+2]
				.continue .if !eax
				invoke _FindSwitchStringR2,_lpAinInfo,eax,@lpSwitches
				.if eax
					lea ecx,@szStr1
					invoke MultiByteToWideChar,932,0,eax,-1,ecx,512
					lea ecx,@szStr1
					.repeat
						mov ax,[ecx]
						add ecx,2
						.if ax==0ah
							mov word ptr [ecx-2],'$'
						.endif
					.until !ax
					invoke wsprintfW,ebx,$CTW0("N: %s\n"),addr @szStr1
				.else
					invoke wsprintfW,ebx,$CTW0("fr%04d\n"),dword ptr [esi-22]
				.endif
				lea ebx,[ebx+eax*2]
			.endif
			.continue
		.elseif al==P_FUNC
			lea ecx,[esi-2]
			invoke _FindFuncName,_lpAinInfo,ecx
			.if eax
				lea ecx,@szStr1
				invoke MultiByteToWideChar,932,0,eax,-1,ecx,512
			.else
				mov word ptr [@szStr1],0
			.endif
			lodsd
			lea ecx,@szStr1
			invoke wsprintfW,ebx,$CTW0("Func_%04d:%s\n"),eax,ecx
			lea ebx,[ebx+eax*2]
			.continue
		.elseif al==P_ENDF
			lodsd
			invoke wsprintfW,ebx,$CTW0("EndFunc_%04d\n\n"),eax
			lea ebx,[ebx+eax*2]
			.continue
		.ELSEIF AL==P_SPUSH
			mov ecx,_lpAinInfo
			mov edx,_AinSegs.Strings.lpRestruct[ecx]
			lodsd
;			lea ecx,@szStr1
;			invoke MultiByteToWideChar,932,0,[edx+eax*4],-1,ecx,512
;			invoke wsprintfW,ebx,$CTW0("S: %s\n"),addr @szStr1
;			lea ebx,[ebx+eax*2]
			.continue
		.elseif al==P_EOF
			invoke wsprintfW,ebx,$CTW0("EndFile_%04d\n"),dword ptr [esi]
			lea ebx,[ebx+eax*2]
			mov ecx,_lpAinInfo
			mov edx,_AinSegs.FileName.lpRestruct[ecx]
			lodsd
			invoke _MakeFile,[edx+eax*4]
			.if !eax
				int 3
			.endif
			sub ebx,_lpTxt
			push eax
			invoke WriteFile,eax,_lpTxt,ebx,offset dwTemp,0
			call CloseHandle
			mov ebx,_lpTxt
			add ebx,2
		.else
			int 3
		.endif
	.until esi>=@pCodeEnd
	ret
_ProcessCode endp

_FindSwitchStringR6 proc uses esi _lpAinInfo,_nIdx,_lpSwitches
	mov ecx,_lpSwitches
	mov eax,_nIdx
	.if eax>=2000
		int 3
	.endif
	mov eax,[ecx+eax*4]
	.if !eax
		mov ecx,_lpAinInfo
		mov edx,_AinSegs.SwitchData.lpRestruct[ecx]
		mov esi,[edx+12*4]
		add esi,8
		lodsd
		.if eax!=444
			int 3
		.endif
		mov ecx,eax
		mov edx,_nIdx
		@@:
			lodsd
			.if eax==edx
				lodsd
				mov ecx,_lpSwitches
				jmp @F
			.endif
			add esi,4
		loop @B
		xor eax,eax
		ret
	.endif
@@:
	mov edx,_nIdx
	mov [ecx+edx*4],eax
	
	mov ecx,_lpAinInfo
	add eax,_AinSegs.Code.lpAddr[ecx]
	.if word ptr [eax]==41h
		mov edx,[eax+2]
		mov ecx,_AinSegs.Strings.lpRestruct[ecx]
		mov eax,[ecx+edx*4]
	.else
		mov ecx,_nIdx
		.if ecx==246
			mov eax,$CTA0("ぽっちゃり女子学生/エロピチャ・ニャンコ")
		.elseif ecx==219
			mov eax,$CTA0("征伐のミト/国王　ラグナロックアーク・スーパー・ガンジー")
		.elseif ecx==247
			mov eax,$CTA0("カクさん/カオル・クインシー・神楽")
		.elseif ecx==243
			mov eax,$CTA0("謎の好青年/光の将軍　アレックス・ヴァルス")
		.elseif ecx==233
			mov eax,$CTA0("謎の大男/パットン・ミスナルジ")
		.elseif ecx==273
			mov eax,$CTA0("謎の娘/カロリア・クリケット")
		.elseif ecx==201
			mov eax,$CTA0("刑務所長　エミ・アルフォーヌ/ラドン長官の娘　エミ・アルフォーヌ")
		.elseif ecx==249
			mov eax,$CTA0("スケさん/ウィチタ・スケート")
		.else
			xor eax,eax
		.endif
	.endif
	_end:
	ret
_FindSwitchStringR6 endp

OPTION LANGUAGE:C
Comparer proc
	mov eax,[esp+4]
	mov ecx,[esp+8]
	sub eax,[ecx]
	ret
Comparer endp

option language:stdcall
_FindSwitchStringR2 proc uses esi _lpAinInfo,_nIdx,_lpSwitches
	invoke bsearch,_nIdx,offset Names,93,sizeof NameStruct,offset Comparer
	test eax,eax
	jz _Ex
	lea eax,[eax+4]
_Ex:
	ret
_FindSwitchStringR2 endp

_FindFuncName proc uses esi _lpAinInfo,_lpFuncAddr
	mov ecx,_lpAinInfo
	mov edx,_lpFuncAddr
	sub edx,_AinSegs.Code.lpAddr[ecx]
	add edx,6
	mov esi,_AinSegs.Function.lpRestruct[ecx]
	mov eax,_AinSegs.Function.lpAddr[ecx]
	mov ecx,[eax]
	@@:
		lodsd
		.if [eax]==edx
			add eax,4
			ret
		.endif
	loop @B
	xor eax,eax
	ret
_FindFuncName endp

_MakeFile proc _lpFileName
	LOCAL @szStr1[1024]:byte
	invoke MultiByteToWideChar,932,0,_lpFileName,-1,addr @szStr1,512
	lea ecx,@szStr1
	lea ecx,[ecx+eax*2-2]
	.if dword ptr [ecx-4]==660061h && dword ptr [ecx-8]==6a002eh
		mov dword ptr [ecx-4],740078h
		mov dword ptr [ecx-8],74002eh
	.endif
	invoke CreateFileW,addr @szStr1,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	.if eax==INVALID_HANDLE_VALUE
		invoke GetLastError
		.if eax==ERROR_PATH_NOT_FOUND
			push esi
			push edi
			push ebx
			lea edi,@szStr1
			or ecx,-1
			xor ax,ax
			repne scasw
			sub edi,2
			not ecx
			dec ecx
			std
			mov ax,'\'
			repne scasw
			cld
			lea ebx,[edi+2]
			mov word ptr [ebx],0
			lea edi,@szStr1
			mov ecx,ebx
			sub ecx,edi
			shr ecx,1
			.while edi<ebx
				repne scasw
				.if ecx
					mov esi,ecx
					mov word ptr [edi-2],0
					invoke CreateDirectoryW,addr @szStr1,0
					mov word ptr [edi-2],'\'
					.if !eax
						invoke GetLastError
						.if eax!=ERROR_ALREADY_EXISTS
							mov word ptr [ebx],'\'
							pop ebx
							pop edi
							pop esi
							xor eax,eax
							ret
						.endif
					.endif
					mov ecx,esi
					mov ax,'\'
				.else
					mov edi,ebx
					invoke CreateDirectoryW,addr @szStr1,0
					mov word ptr [ebx],'\'
					invoke CreateFileW,addr @szStr1,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
					.if eax==INVALID_HANDLE_VALUE
						xor eax,eax
					.endif
				.endif
			.endw
			pop ebx
			pop edi
			pop esi
		.else
			xor eax,eax
		.endif
	.endif
	ret
_MakeFile endp

_ProcessFunc proc _lpAinInfo,_lpTxt
	LOCAL @szStr1[1024]:byte
	LOCAL @nNum
	mov ebx,_lpTxt
	mov word ptr [ebx],0feffh
	add ebx,2
	
	mov ecx,_lpAinInfo
	mov esi,_AinSegs.Function.lpRestruct[ecx]
	mov edx,_AinSegs.Function.lpAddr[ecx]
	mov eax,[edx]
	mov @nNum,eax
	
	.while @nNum
		lodsd
		lea edi,[eax+4]
		mov ecx,[eax]
		mov edx,_lpAinInfo
		add ecx,_AinSegs.Code.lpAddr[edx]
		.if word ptr [ecx-6]==61h
			invoke wsprintfW,ebx,$CTW0("%d: "),dword ptr [ecx-4]
			lea ebx,[ebx+eax*2]
		.endif
		invoke MultiByteToWideChar,932,0,edi,-1,ebx,512
		lea ebx,[ebx+eax*2]
		mov word ptr [ebx-2],'('
		invoke lstrlenA,edi
		lea edi,[edi+eax+25]
		push esi
		mov esi,[edi-8]
		or esi,esi
		je _endloop1
		_loop1:
			invoke MultiByteToWideChar,932,0,edi,-1,ebx,512
			lea ebx,[ebx+eax*2]
			mov word ptr [ebx-2],','
			invoke lstrlenA,edi
			lea edi,[edi+eax+13]
			dec esi
			jnz _loop1
		sub ebx,2
		_endloop1:
		mov word ptr [ebx],')'
		mov dword ptr [ebx+2],0a000dh
		add ebx,6
		pop esi
		dec @nNum
	.endw
	
	invoke CreateFile,$CTA0("Func.txt"),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	push eax
	sub ebx,_lpTxt
	invoke WriteFile,eax,_lpTxt,ebx,offset dwTemp,0
	call CloseHandle
	
	ret
_ProcessFunc endp

_ProcessSwitch proc _lpAinInfo,_lpTxt
	LOCAL @nNum
	mov ebx,_lpTxt
	mov word ptr [ebx],0feffh
	add ebx,2
	
	mov ecx,_lpAinInfo
	mov esi,_AinSegs.SwitchData.lpAddr[ecx]
	lodsd
	mov @nNum,eax
	
	xor edi,edi
	.while edi<@nNum
		invoke wsprintfW,ebx,$CTW0("Switch %d: Type(%d) Branch(%d)\n\t"),edi,dword ptr [esi],dword ptr [esi+8]
		lea ebx,[ebx+eax*2]
		add esi,8
		lodsd
		push edi
		mov edi,eax
		or edi,edi
		je _endloop1
		_loop1:
			invoke wsprintfW,ebx,$CTW0("%d:%X,"),dword ptr [esi],dword ptr [esi+4]
			lea ebx,[ebx+eax*2]
			add esi,8
			dec edi
			jnz _loop1
		sub ebx,2
		_endloop1:
		mov dword ptr [ebx],0a000dh
		add ebx,4
		pop edi
		inc edi
	.endw
	
	invoke CreateFile,$CTA0("Switch.txt"),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0
	push eax
	sub ebx,_lpTxt
	invoke WriteFile,eax,_lpTxt,ebx,offset dwTemp,0
	call CloseHandle
	
	ret
_ProcessSwitch endp

end start