.486
.model flat,stdcall
option casemap:none

include D:\masm32\macros\Strings.mac

;PUBLIC TheData
PUBLIC CodeStart
PUBLIC CodeEnd
PUBLIC SetSEH

MyData struct
	f_HookApi		dd	?
	f_LdrLoadDll	dd	?
	p_charTbl		dd	?
	p_FontResc		dd	?
	n_FontSize		dd	?
	f_VirtualProtect	dd	?
	f_TextOutA		dd	?
	f_TextOutW		dd	?
	f_MessageBoxA	dd	?
	f_SendMessageA	dd	?
	f_CreateFontI	dd	?
	f_AddFont		dd	?
	f_CreateFile	dd	?
	f_FindFirstFile	dd	?
	f_GetFileAttr	dd	?
	f_lstrcmpi		dd	?
	b_fontLoaded	dd	?
MyData ends

.code

CodeStart LABEL DWORD

TheData		MyData		<0>

align 2
aNewXp3Name	dw		'n','s','c','n','.','x','p','3',0
align 2
aOldXp3Name dw		'p','a','t','c','h','.','x','p','3',0
align 2
aConfirm	db		'确认',0
align 2
aConfirmText	db	'确定要退出游戏吗？',0


MyGetProcAddress proc uses esi edi ebx _hModule:dword,_pName:dword
	xor eax,eax
	.if _hModule==0 || _pName==0
		ret
	.endif
	mov esi,_hModule
	.if word ptr [esi]!=5a4dh
		ret
	.endif
	add esi,dword ptr [esi+3ch]
	.if word ptr [esi]!=4550h
		ret
	.endif
	lea edi,[esi+78h]
	.if !dword ptr [edi] || !dword ptr [edi+4]
		ret
	.endif
	mov esi,_hModule
	add esi,dword ptr [edi]
	
	.if _pName<0ffffh
		mov eax,_pName
		sub eax,dword ptr [esi+10h]
		.if eax>=dword ptr [esi+14h]
			xor eax,eax
			ret
		.endif
		mov edi,[esi+1ch]
		add edi,_hModule
		mov eax,[edi+eax*4]
		add eax,_hModule
		ret
	.else
		mov ebx,esi
		mov edi,_pName
		xor eax,eax
		or ecx,-1
		repne scasb
		not ecx
		sub esp,4
		mov [esp],ecx
		mov edx,[ebx+18h]
		xor eax,eax
		push ebp
		mov ebp,[esp+1ch]
		.while eax<edx
			mov ecx,[ebx+20h]
			add ecx,ebp
			mov edi,[ecx+eax*4]
			add edi,ebp
			mov esi,[esp+20h]
			mov ecx,[esp+4]
			repe cmpsb
			.if ZERO?
				mov esi,dword ptr [ebx+24h]
				add esi,ebp
				movzx eax,word ptr [esi+eax*2]
				mov esi,dword ptr [ebx+1ch]
				add esi,ebp
				mov eax,[esi+eax*4]
				add eax,ebp
				pop ebp
				add esp,4
				ret
			.endif
			inc eax
		.endw
		pop ebp
		add esp,4
	.endif
	xor eax,eax
	ret
MyGetProcAddress endp

NewTextOutA proc
	push ebp
	push esi
	push ebx
	push edi
	call offsethelper
	offsethelper:
	pop ebp
	lea esi,[ebp+((offset TheData)-offsethelper)]

	cmp dword ptr [esp+24h],2
	jne _Old
	mov edx,[esp+20h]
	xor eax,eax
	mov ah,[edx]
	mov al,[edx+1]

	mov ecx,MyData.p_charTbl[esi]
	movzx ecx,word ptr [ecx+eax*2]
	test ecx,ecx
	jz _Old
	mov [edx],cx

	mov ebx,edx
	mov edi,eax

	mov eax,1
	mov ecx,[esp+20h]
	mov edx,[esp+1ch]
	push eax
	push ecx
	push edx
	mov eax,[esp+24h]
	mov ecx,[esp+20h]
	push eax
	push ecx
	mov eax,MyData.f_TextOutW[esi]
	add eax,2
	call eax

	mov ax,di
	mov [ebx],ah
	mov [ebx+1],al
	pop edi
	pop ebx
	pop esi
	pop ebp
	retn 14h

_Old:
	mov eax,MyData.f_TextOutA[esi]
	pop edi
	pop ebx
	pop esi
	pop ebp
	add eax,2
	jmp eax
NewTextOutA endp

NewCreateFontI proc
	call hp
	hp:
	pop ecx
	mov eax,[esp]
	.if 1;eax<7f6000h && eax>400000h
		mov eax,[esp+4]
		mov byte ptr [eax+17h],86h
		mov dword ptr [eax+1ch],'eheh'
		mov dword ptr [eax+20h],'eh'
		;mov dword ptr [eax+1ch],'hmiS'
		;mov dword ptr [eax+20h],'ie'
	.endif
	lea eax,[ecx+((offset TheData)-hp)]
	mov ecx,MyData.f_CreateFontI[eax]
	add ecx,2
	jmp ecx
NewCreateFontI endp


NewSendMessageA proc
	call hp
	hp:
	pop ecx
	lea edx,[ecx+((offset TheData)-hp)]
	
	cmp dword ptr [esp+8],0ch
	jne _Old
	mov eax,[esp+10h]
	test eax,eax
	jz _Old
	cmp dword ptr [eax],9591978fh
	jne _Old
	cmp dword ptr [eax+4],0ac96528eh
	jne _Old
	
	mov dword ptr [eax],0b0d7aec5h
	mov dword ptr [eax+4],0f6c2bdc9h
	mov byte ptr [eax+8],0
_Old:
	mov eax,MyData.f_SendMessageA[edx]
	add eax,2
	jmp eax
NewSendMessageA endp

NewMessageBoxA proc
	call hp
	hp:
	pop ecx
	lea edx,[ecx+((offset TheData)-hp)]
	
	mov eax,[esp+8]
	cmp dword ptr [eax],5b815183h
	jne _Old
	
	lea eax,[ecx+((offset aConfirmText)-hp)]
	mov [esp+8],eax
	lea ecx,[ecx+((offset aConfirm)-hp)]
	mov dword ptr [esp+0ch],ecx	
_Old:
	mov eax,MyData.f_MessageBoxA[edx]
	add eax,2
	jmp eax
NewMessageBoxA endp

assume fs:nothing
SetSEH LABEL dword
	;db 0ebh,0feh
	pushad
	call OffsetHelper2
	OffsetHelper2:
	pop ebp
		
	lea esi,[ebp+((offset TheData)-OffsetHelper2)]
	assume esi:ptr MyData
	call @F
	PHandleGdi:
	dd 0
	@@:
	jmp @F
	align 4
	aGdi32:
	dw 'g','d','i','3','2','.','d','l','l'
	dw 0,0,0
	@@:
	lea eax,[ebp+(aGdi32-OffsetHelper2)]
	mov [ebp+(@f-OffsetHelper2-4)],eax
	call @F
	dw 18
	dw 24
	dd 0
	@@:
	push 0
	push 0
	call [esi].f_LdrLoadDll
	
	lea esi,[ebp+((offset TheData)-OffsetHelper2)]
	assume esi:ptr MyData
	call @F
	PHandleUser32:
	dd 0
	@@:
	jmp @F
	align 4
	aUser32:
	dw 'u','s','e','r','3','2','.','d','l','l'
	dw 0,0
	@@:
	lea eax,[ebp+(aUser32-OffsetHelper2)]
	mov [ebp+(@f-OffsetHelper2-4)],eax
	call @F
	dw 20
	dw 24
	dd 0
	@@:
	push 0
	push 0
	call [esi].f_LdrLoadDll

	call @F
	PHandleKernel:
	dd	0
	@@:
	jmp @F
	align 4
	aKernel32:
	dw 'k','e','r','n','e','l','3','2','.','d','l','l'
	dw 0,0
	@@:
	lea eax,[ebp+(aKernel32-OffsetHelper2)]
	mov [ebp+(@f-OffsetHelper2-4)],eax
	call @F
	dw 24
	dw 28
	dd 0
	@@:
	push 0
	push 0
	call [esi].f_LdrLoadDll


	call @F
	db 'TextOutA',0
	@@:
	push [ebp+(PHandleGdi-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_TextOutA,eax
	call @F
	db 'TextOutW',0
	@@:
	push [ebp+(PHandleGdi-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_TextOutW,eax
	call @F
	db 'SendMessageA',0
	@@:
	push [ebp+(PHandleUser32-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_SendMessageA,eax
	call @F
	db 'MessageBoxA',0
	@@:
	push [ebp+(PHandleUser32-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_MessageBoxA,eax
	call @F
	db 'CreateFontIndirectA',0
	@@:
	push [ebp+(PHandleGdi-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_CreateFontI,eax
	call @F
	db 'AddFontMemResourceEx',0
	@@:
	push [ebp+(PHandleGdi-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_AddFont,eax
	call @F
	db 'VirtualProtect',0
	@@:
	push [ebp+(PHandleKernel-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_VirtualProtect,eax
	call @F
	db 'CreateFileW',0
	@@:
	push [ebp+(PHandleKernel-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_CreateFile,eax
	call @F
	db 'GetFileAttributesW',0
	@@:
	push [ebp+(PHandleKernel-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_GetFileAttr,eax
	call @F
	db 'lstrcmpiW',0
	@@:
	push [ebp+(PHandleKernel-OffsetHelper2)]
	call MyGetProcAddress
	mov [esi].f_lstrcmpi,eax

	call @F
	dd 0
	@@:
	push 40h
	push 2
	mov ebx,[esi].f_CreateFontI
	push ebx
	call [esi].f_VirtualProtect
	lea eax,[ebp+(NewCreateFontI-OffsetHelper2)]
	push eax
	push ebx
	call [esi].f_HookApi

	call @F
	dd 0
	@@:
	push 40h
	push 2
	mov ebx,[esi].f_TextOutA
	push ebx
	call [esi].f_VirtualProtect
	lea eax,[ebp+(NewTextOutA-OffsetHelper2)]
	push eax
	push ebx
	call [esi].f_HookApi
	
	call @F
	dd 0
	@@:
	push 40h
	push 2
	mov ebx,[esi].f_SendMessageA
	push ebx
	call [esi].f_VirtualProtect
	lea eax,[ebp+(NewSendMessageA-OffsetHelper2)]
	push eax
	push ebx
	call [esi].f_HookApi

	call @F
	dd 0
	@@:
	push 40h
	push 2
	mov ebx,[esi].f_MessageBoxA
	push ebx
	call [esi].f_VirtualProtect
	lea eax,[ebp+(NewMessageBoxA-OffsetHelper2)]
	push eax
	push ebx
	call [esi].f_HookApi

	call @F
	dd 1
	@@:
	push 0
	push [esi].n_FontSize
	push [esi].p_FontResc
	call [esi].f_AddFont
	assume esi:nothing
	popad

	jmp ecx



CodeEnd label DWORD

end