.386
.model flat,stdcall
option casemap:none

include editor.inc
include _BrowseFolder.asm
include misc.asm
include mainwnd.asm
include lineopr.asm

.code

assume fs:nothing
;
DllMain proc _hInstance,_dwReason,_dwReserved
	.if _dwReason==DLL_PROCESS_ATTACH
		push _hInstance
		pop hInstance
		
		invoke GetModuleHandle,offset szKernel32
		invoke GetProcAddress,eax,offset szCreateFileA
		mov lpCreateFileA,eax

		invoke CreateThread,0,0,offset _CreateMainWindow,hInstance,0,0
		.if !eax
			ret
		.endif
		invoke CloseHandle,eax
		invoke CreateThread,0,0,offset _CreateListWindow,hInstance,0,0
		.if !eax
			ret
		.endif
		invoke CloseHandle,eax
		
		invoke VirtualAlloc,0,100*20,MEM_COMMIT,PAGE_READWRITE
		.if !eax
			ret
		.endif
		mov lpGscQueryTable,eax
		
	.ENDIF
	mov eax,TRUE
	ret
DllMain endp

;
GetGsc proc
	pushad
	cmp bFatalErr,0
	jne _GetGscLabel2
	mov esi,[esp+24h]
	.repeat
		cmp dword ptr [esi],'csg.'
		je @F
		inc esi
	.until !byte ptr [esi]
	jmp _GetGscLabel2
@@:
	cmp byte ptr [szOriGscName],0
	je @F
	mov edi,lpGscQueryTable
	.while dword ptr [edi]
		invoke lstrcmp,offset szOriGscName,edi
		.if !eax
			mov eax,nCurOffset
			mov [edi+16],eax
			jmp @F
		.endif
		add edi,sizeof _GscRecord
	.endw
	mov bFatalErr,1
	invoke PostMessage,hWinMain,WM_PREGSCERR,11,0
	jmp _GetGscLabel2
@@:
	.if byte ptr [esi-4]<='1'
		mov bIsGscSkipped,1
		jmp _GetGscLabel2
	.endif
	mov bIsGscSkipped,0
	mov esi,[esp+24h]
	mov edi,lpGscQueryTable
;	assume edi:ptr _GscRecord
	.while dword ptr [edi]
		invoke lstrcmp,esi,edi
		.if !eax
			mov eax,[edi+16]
			mov nCurOffset,eax
			jmp _GetGscLabel1
		.endif
		add edi,sizeof _GscRecord
	.endw
	mov nCurOffset,0
	invoke lstrcpy,edi,[esp+24h]
;	assume edi:nothing
	
_GetGscLabel1:
	.if hGsc
		invoke _SaveGsc
		.if !eax
			invoke PostMessage,hWinMain,WM_SAVEERR,0,0
			jmp _GetGscLabel2
		.endif
		invoke CloseHandle,hGsc
		invoke SetWindowText,hWinMain,offset szDisplayName
	.endif
	invoke lstrcpy,offset szOriGscName,[esp+24h]
	invoke lstrcpy,offset szNewGscName,offset szNewGscFolder
	invoke _ConnectGscPathA,offset szNewGscName,offset szOriGscName
	invoke CreateFile,offset szNewGscName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
		invoke CopyFile,offset szOriGscName,offset szNewGscName,TRUE
		 .if eax==-1
		 @@:
			invoke PostMessage,hWinMain,WM_PREGSCERR,0,0
			jmp _GetGscLabel2
		 .endif
		invoke CreateFile,offset szNewGscName,GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
		cmp eax,-1
		je @B
	.endif
	mov hGsc,eax
	invoke _GetFileNameInPathA,offset szOriGscName
	sub esp,128
	mov ebx,esp
	invoke lstrcpy,ebx,eax
	invoke lstrcat,ebx,offset szGang
	invoke lstrcat,ebx,offset szDisplayName
	invoke SetWindowText,hWinMain,ebx
	add esp,128
;	.if hReadThread
;		invoke WaitForSingleObject,hReadThread,INFINITE
;	.endif
;	invoke CreateThread,0,0,offset _PreReadGsc,hGsc,0,0
	invoke _CorrectSize,hGsc
	.if !eax
		invoke PostMessage,hWinMain,WM_PREGSCERR,10,0
	.endif
	invoke _PreReadGsc,hGsc
	.if eax
		invoke PostMessage,hWinMain,WM_PREGSCERR,eax,0
		jmp _GetGscLabel2
	.endif
;	mov hReadThread,eax
	
_GetGscLabel2:
	popad
	jmp dword ptr [lpCreateFileA]
GetGsc endp

;

;读取脚本
;ThreadProc
_PreReadGsc proc uses edi esi ebx _hGsc
	push offset _Handler
	push fs:[0]
	mov fs:[0],esp
	invoke _FreeGscInfo
	lea edi,sGscInfo
	mov ecx,sizeof sGscInfo
	xor al,al
	rep stosb
	invoke SetFilePointer,_hGsc,0,0,FILE_BEGIN
	invoke ReadFile,_hGsc,offset sGscInfo.sHeader,1ch,offset dwTemp,0
	mov eax,sGscInfo.sHeader.nControlStreamSize
	shl eax,1
	invoke VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
	mov sGscInfo.lpControlStream,eax
	mov eax,sGscInfo.sHeader.nIndexSize
	shl eax,1
	invoke VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
	mov sGscInfo.lpIndex,eax
	mov eax,sGscInfo.sHeader.nTextSize
	shl eax,1
	invoke VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
	mov sGscInfo.lpText,eax
	mov eax,sGscInfo.sHeader.nExtraSize1
	add eax,sGscInfo.sHeader.nExtraSize2
	invoke VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
	mov sGscInfo.lpExtraData,eax
	mov eax,sGscInfo.sHeader.nControlStreamSize
	mov ebx,eax
	shr eax,3
	invoke VirtualAlloc,0,eax,MEM_COMMIT,PAGE_READWRITE
	mov sGscInfo.lpRelocTable,eax
	shr ebx,1
	invoke VirtualAlloc,0,ebx,MEM_COMMIT,PAGE_READWRITE
	mov sGscInfo.lpIndexCS,eax
	
	invoke SetFilePointer,_hGsc,sGscInfo.sHeader.nHeaderSize,0,FILE_BEGIN
	invoke ReadFile,_hGsc,sGscInfo.lpControlStream,sGscInfo.sHeader.nControlStreamSize,offset dwTemp,0
	invoke ReadFile,_hGsc,sGscInfo.lpIndex,sGscInfo.sHeader.nIndexSize,offset dwTemp,0
	invoke ReadFile,_hGsc,sGscInfo.lpText,sGscInfo.sHeader.nTextSize,offset dwTemp,0
	mov eax,sGscInfo.sHeader.nExtraSize1
	add eax,sGscInfo.sHeader.nExtraSize2
	invoke ReadFile,_hGsc,sGscInfo.lpExtraData,eax,offset dwTemp,0

	invoke _ProcControlStream
	cmp eax,-1
	je _ProcErr
	
	pop fs:[0]
	pop ecx
	xor eax,eax
;	invoke CloseHandle,hReadThread
;	mov hReadThread,0
	ret

_ProcErr:
	pop fs:[0]
	pop ecx
	invoke _FreeGscInfo
	mov eax,2
;	invoke CloseHandle,hReadThread
;	mov hReadThread,0
	ret
	
_NoMemory:
	pop fs:[0]
	pop ecx
	invoke _FreeGscInfo
	mov eax,1
;	invoke CloseHandle,hReadThread
;	mov hReadThread,0
	ret
_Handler:
	mov eax,[esp+0ch]
	mov [eax+0b0h],offset _NoMemory
	xor eax,eax
	ret
_PreReadGsc endp

;释放GscInfo中申请的内存
_FreeGscInfo proc uses edi
	lea edi,sGscInfo.lpControlStream
	mov ebx,5
	@@:
	.if dword ptr [edi]
		invoke VirtualFree,[edi],0,MEM_RELEASE
	.endif
	add edi,4
	dec ebx
	jne @b
	ret
_FreeGscInfo endp

;预读一遍脚本中的控制流，保证没有异常，并把跳转的offset记录下来
_ProcControlStream proc uses esi edi ebx
	LOCAL @pIdx
	mov eax,sGscInfo.lpIndexCS
	mov @pIdx,eax
	mov esi,sGscInfo.lpControlStream
	xor ebx,ebx
	xor edx,edx
	.while edx<sGscInfo.sHeader.nControlStreamSize
		mov ax,[esi+edx]
		add edx,2
		mov cx,ax
		and ax,0f000h
		.if !ZERO?
			shr ax,12
			and eax,0fh
			lea edi,dtParamSize1
			movzx eax,byte ptr [edi+eax]
			.if eax==-1
				inc eax
				ret
			.endif
			mov ecx,@pIdx
			mov [ecx],edx
			sub dword ptr [ecx],2
			add @pIdx,4
			add edx,eax
		.else
			lea edi,dtParamSize2
			movzx ecx,cl
			movzx eax,byte ptr [edi+ecx]
			.if eax==-1
				inc eax
				ret
			.endif
			push eax
			mov eax,@pIdx
			mov [eax],edx
			sub dword ptr [eax],2
			add @pIdx,4
			pop eax
			add edx,eax
			
			.if (cl>=03 && cl<=05)
				lea eax,[edx-4]
				mov edi,sGscInfo.lpRelocTable
				mov [edi+ebx],eax
				add ebx,4
				.continue
			.elseif cl==0eh
				lea eax,[edx-52]
				mov edi,sGscInfo.lpRelocTable
				mov ecx,5
				@@:
					mov [edi+ebx],eax
					add eax,4
					add ebx,4
					.continue .if !dword ptr [esi+eax]
				loop @B
			.endif
		.endif
	.endw
	mov eax,1
	ret
_ProcControlStream endp

_SaveGsc proc uses ebx edi
	invoke SetFilePointer,hGsc,0,0,FILE_BEGIN
	invoke WriteFile,hGsc,offset sGscInfo.sHeader,sizeof _GscHeader,offset dwTemp,0
	mov ebx,eax
	invoke WriteFile,hGsc,sGscInfo.lpControlStream,sGscInfo.sHeader.nControlStreamSize,offset dwTemp,0
	and ebx,eax
	invoke WriteFile,hGsc,sGscInfo.lpIndex,sGscInfo.sHeader.nIndexSize,offset dwTemp,0
	and ebx,eax
	invoke WriteFile,hGsc,sGscInfo.lpText,sGscInfo.sHeader.nTextSize,offset dwTemp,0
	and ebx,eax
	mov eax,sGscInfo.sHeader.nExtraSize1
	add eax,sGscInfo.sHeader.nExtraSize2
	invoke WriteFile,hGsc,sGscInfo.lpExtraData,eax,offset dwTemp,0
	and ebx,eax
	invoke SetEndOfFile,hGsc
	
	.if !ebx
		xor eax,eax
		ret
	.endif
	
	mov edi,lpGscQueryTable
;	assume edi:ptr _GscRecord
	.while dword ptr [edi]
		invoke lstrcmp,offset szOriGscName,edi
		.if eax
			mov eax,nCurOffset
			mov [edi+16],eax
			.break
		.endif
		add edi,sizeof _GscRecord
	.endw
;	assume edi:nothing
	mov eax,1
	ret
_SaveGsc endp

;
CBProc proc
	mov eax,413020h
	call eax
	pushad
	cmp bFatalErr,0
	jne ScriptProcLabel1
	mov ebx,eax
	.if bIsGscSkipped
		invoke IsWindowEnabled,hEditMain
		.if eax
			invoke PostMessage,hWinMain,WM_NOTAVBGSC,0,0
		.endif
		jmp ScriptProcLabel1
	.endif
	mov edi,sGscInfo.lpControlStream
	add edi,nCurOffset
	mov eax,ebx
	.if ax!=word ptr [edi]
		invoke PostMessage,hWinMain,WM_STREAMERR,0,0
		jmp ScriptProcLabel1
	.endif

	mov bx,ax
	and ax,0f000h
	.if !ZERO?
		shr ax,12
		and eax,0fh
		lea edi,dtParamSize1
		movzx eax,byte ptr [edi+eax]
		.if eax==-1
			invoke PostMessage,hWinMain,WM_STREAMERR,1,0
			jmp ScriptProcLabel1
		.endif
		add eax,2
		add nCurOffset,eax
	.else
		.if bl==51h
			invoke PostMessage,hWinMain,WM_STREAMBYTE,51h,nCurOffset
		.elseif bl>=03 && bl<=05
			mov eax,[edi+2]
			mov nCurOffset,eax
			jmp ScriptProcLabel1
		.elseif bl==0eh
			invoke PostMessage,hWinMain,WM_STREAMBYTE,0eh,nCurOffset
			lea esi,[edi+8]
			lea edi,nSelOffset
			mov ecx,5
			rep movsd

		.endif
		
		lea edi,dtParamSize2
		movzx ebx,bl
		movzx eax,byte ptr [edi+ebx]
		.if eax==-1
			invoke PostMessage,hWinMain,WM_STREAMERR,1,0
			jmp ScriptProcLabel1
		.endif
		add eax,2
		add nCurOffset,eax
	.endif
ScriptProcLabel1:
	popad
	ret
CBProc endp

;
_CorrectSize proc uses edi esi _hGsc
	LOCAL @nSize:LARGE_INTEGER
	LOCAL @lpBuff,@dwTemp
	invoke GetFileSizeEx,_hGsc,addr @nSize
	invoke VirtualAlloc,0,dword ptr [@nSize],MEM_COMMIT,PAGE_READWRITE
	.if !eax
		ret
	.endif
	mov @lpBuff,eax
	invoke ReadFile,_hGsc,@lpBuff,dword ptr [@nSize],offset dwTemp,0
	.if !eax
		ret
	.endif
	mov edi,@lpBuff
	mov eax,[edi] 
	.if eax!=dword ptr [@nSize]
		mov @dwTemp,eax
		invoke SetFilePointer,_hGsc,0,0,FILE_BEGIN
		invoke WriteFile,_hGsc,addr @dwTemp,4,offset dwTemp,0
		.if !eax
			ret
		.endif
	.endif
	mov esi,edi
	add edi,dword ptr [esi+4]
	add edi,dword ptr [esi+8]
	add edi,dword ptr [esi+12]
	mov edx,edi
	mov eax,[edi-4]
	add edi,eax
	xor al,al
	or ecx,-1
	repne scasb
	sub edi,edx
	.if edi!=[esi+16]
		mov @dwTemp,edi
		invoke SetFilePointer,_hGsc,16,0,FILE_BEGIN
		invoke WriteFile,_hGsc,addr @dwTemp,4,offset dwTemp,0
		.if !eax
			ret
		.endif
	.endif
	invoke VirtualFree,@lpBuff,0,MEM_RELEASE
	mov eax,1
	ret
_CorrectSize endp

;
SelNotify proc
	.if !bIsGscSkipped
		push edi
		lea edi,nSelOffset
		mov eax,[edi+eax*4-4]
		mov nCurOffset,eax
		pop edi
	.endif
	mov eax,41a0a0h
	jmp eax
SelNotify endp

end DllMain