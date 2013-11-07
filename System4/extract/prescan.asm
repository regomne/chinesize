.code

_PreScan4 proc uses esi edi ebx _lpFile,_nSize,_lpAinSegs
	LOCAL @pEnd
	LOCAL @nNums
	mov esi,_lpFile
	mov ebx,_lpAinSegs
	assume ebx:ptr _AinSegs
	mov eax,esi
	add eax,_nSize
	mov @pEnd,eax
	.while esi<@pEnd
		lodsd
		.if eax=='SREV'
			mov [ebx].Version.lpAddr,esi
			mov [ebx].Version.nSize,4
			add esi,4
		.elseif eax=='EDOC'
			lodsd
			mov [ebx].Code.lpAddr,esi
			mov [ebx].Code.nSize,eax
			add esi,eax
		.elseif eax=='CNUF'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Function.lpRestruct,eax
			mov @nNums,eax
			mov [ebx].Function.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			xor al,al
			or ecx,-1
			mov edi,esi
			.while ebx
				mov edx,@nNums
				mov [edx],edi
				add @nNums,4
				add edi,4
				repne scasb
				add edi,24
				mov edx,[edi-8]
				.while edx
					repne scasb
					add edi,12
					dec edx
				.endw
				dec ebx
			.endw
			pop ebx			
			mov ecx,edi
			sub ecx,esi
			mov esi,edi
			add ecx,4
			mov [ebx].Function.nSize,ecx
		.ELSEIF EAX=='BOLG'
			mov [ebx].GlobalVar.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			mov edi,esi
			xor al,al
			or ecx,-1
			.while ebx
				repne scasb
				add edi,12
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			add ecx,4
			mov [ebx].GlobalVar.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='TESG'
			mov [ebx].GlobalSet.lpAddr,esi
			lodsd
			shl eax,2
			lea eax,[eax+eax*2]
			add esi,eax
			add eax,4
			mov [ebx].GlobalSet.nSize,eax
		.ELSEIF EAX=='TRTS'
			mov [ebx].Structs.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			mov edi,esi
			or ecx,-1
			xor eax,eax
			.while ebx
				repne scasb
				add edi,12
				mov edx,[edi-4]
				.while edx
					dec edx
					repne scasb
					add edi,12
				.endw
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			mov esi,edi
			add ecx,4
			mov [ebx].Structs.nSize,ecx
		.ELSEIF EAX=='0GSM'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Message0.lpRestruct,eax
			mov [ebx].Message0.lpAddr,esi
			lodsd
			mov ecx,[ebx].Message0.lpRestruct
			push ebx
			mov ebx,ecx
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				mov dword ptr [ebx],edi
				repne scasb
				add ebx,4
				dec edx
			.endw
			pop ebx
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].Message0.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='NIAM'
			mov [ebx].Main.lpAddr,esi
			mov [ebx].Main.nSize,4
			add esi,4
		.ELSEIF EAX=='FGSM'
			mov [ebx].MessageFunc.lpAddr,esi
			mov [ebx].MessageFunc.nSize,4
			add esi,4
		.ELSEIF EAX=='0LLH'
			mov [ebx].HLL.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			mov edi,esi
			xor eax,eax
			.while ebx
				repne scasb
				mov edx,[edi]
				add edi,4
				.while edx
					repne scasb
					add edi,8
					push edx
					mov edx,[edi-4]
					.while edx
						repne scasb
						add edi,4
						dec edx
					.endw
					pop edx
					dec edx
				.endw
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			mov esi,edi
			add ecx,4
			mov [ebx].HLL.nSize,ecx
		.ELSEIF EAX=='0IWS'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].SwitchData.lpRestruct,eax
			mov @nNums,eax
			mov [ebx].SwitchData.lpAddr,esi
			lodsd
			mov edi,eax
			.while edi
				mov ecx,@nNums
				mov [ecx],esi
				add @nNums,4
				add esi,12
				mov edx,[esi-4]
				shl edx,3
				add esi,edx
				dec edi
			.endw
			mov ecx,esi
			sub ecx,[ebx].SwitchData.lpAddr
			mov [ebx].SwitchData.nSize,ecx
		.ELSEIF EAX=='REVG'
			mov [ebx].GameVersion.lpAddr,esi
			mov [ebx].GameVersion.nSize,4
			add esi,4
		.ELSEIF EAX=='LBLS'
			mov [ebx].SLBL.lpAddr,esi
			lodsd
			mov edx,eax
			xor eax,eax
			or ecx,-1
			mov edi,esi
			.while edx
				repne scasb
				add edi,4
				dec edx
			.endw
			mov ecx,edi
			sub ecx,esi
			add ecx,4
			mov [ebx].SLBL.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='0RTS'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Strings.lpRestruct,eax
			mov [ebx].Strings.lpAddr,esi
			lodsd
			mov ecx,[ebx].Strings.lpRestruct
			push ebx
			mov ebx,ecx
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				mov dword ptr [ebx],edi
				repne scasb
				add ebx,4
				dec edx
			.endw
			pop ebx
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].Strings.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='MANF'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].FileName.lpRestruct,eax
			mov [ebx].FileName.lpAddr,esi
			lodsd
			mov ecx,[ebx].FileName.lpRestruct
			push ebx
			mov ebx,ecx
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				mov dword ptr [ebx],edi
				repne scasb
				add ebx,4
				dec edx
			.endw
			pop ebx
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].FileName.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='PMJO'
			mov [ebx].Onjump.lpAddr,esi
			mov [ebx].Onjump.nSize,4
			add esi,4
		.ELSEIF EAX=='TCNF'
			lodsd
			mov [ebx].FuctionType.lpAddr,esi
			mov [ebx].FuctionType.nSize,eax
			add esi,eax
		.ELSEIF EAX=='CYEK'
			mov [ebx].KeyC.lpAddr,esi
			mov [ebx].KeyC.nSize,4
			add esi,4
		.else
			mov eax,1
			ret
		.endif
	.endw
	assume edi:nothing
	xor eax,eax
	ret
_PreScan4 endp

_PreScan6 proc uses esi edi ebx _lpFile,_nSize,_lpAinSegs
	LOCAL @pEnd
	LOCAL @nNums
	mov esi,_lpFile
	mov ebx,_lpAinSegs
	assume ebx:ptr _AinSegs
	mov eax,esi
	add eax,_nSize
	mov @pEnd,eax
	.while esi<@pEnd
		lodsd
		.if eax=='SREV'
			mov [ebx].Version.lpAddr,esi
			mov [ebx].Version.nSize,4
			add esi,4
		.elseif eax=='EDOC'
			lodsd
			mov [ebx].Code.lpAddr,esi
			mov [ebx].Code.nSize,eax
			add esi,eax
		.elseif eax=='CNUF'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Function.lpRestruct,eax
			mov @nNums,eax
			mov [ebx].Function.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			xor al,al
			or ecx,-1
			mov edi,esi
			.while ebx
				mov edx,@nNums
				mov [edx],edi
				add @nNums,4
				add edi,4
				repne scasb
				add edi,24
				mov edx,[edi-8]
				.while edx
					repne scasb
					add edi,12
					dec edx
				.endw
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			mov esi,edi
			add ecx,4
			mov [ebx].Function.nSize,ecx
		.ELSEIF EAX=='BOLG'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].GlobalVar.lpRestruct,eax
			mov edx,eax
			mov [ebx].GlobalVar.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			mov edi,esi
			xor al,al
			or ecx,-1
			.while ebx
				mov [edx],edi
				add edx,4
				repne scasb
				add edi,16
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			add ecx,4
			mov [ebx].GlobalVar.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='TESG'
			mov [ebx].GlobalSet.lpAddr,esi
			lodsd
			mov edx,eax
			mov edi,esi
			or ecx,-1
			xor al,al
			or edx,edx
			@@:
			je @F
				add edi,8
				.if dword ptr [edi-4]==0ch
					repne scasb
				.else
					add edi,4
				.endif
			dec edx
			jnz @B
			@@:
			mov ecx,edi
			sub ecx,esi
			add ecx,4
			mov esi,edi
			mov [ebx].GlobalSet.nSize,ecx
		.ELSEIF EAX=='TRTS'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Structs.lpRestruct,eax
			mov @nNums,eax
			mov [ebx].Structs.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			mov edi,esi
			or ecx,-1
			xor eax,eax
			.while ebx
				mov edx,@nNums
				mov [edx],edi
				add @nNums,4
				repne scasb
				add edi,12
				mov edx,[edi-4]
				.while edx
					dec edx
					repne scasb
					add edi,12
				.endw
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			mov esi,edi
			add ecx,4
			mov [ebx].Structs.nSize,ecx
		.ELSEIF EAX=='0GSM'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Message0.lpRestruct,eax
			mov [ebx].Message0.lpAddr,esi
			lodsd
			mov ecx,[ebx].Message0.lpRestruct
			push ebx
			mov ebx,ecx
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				mov dword ptr [ebx],edi
				repne scasb
				add ebx,4
				dec edx
			.endw
			pop ebx
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].Message0.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='NIAM'
			mov [ebx].Main.lpAddr,esi
			mov [ebx].Main.nSize,4
			add esi,4
		.ELSEIF EAX=='FGSM'
			mov [ebx].MessageFunc.lpAddr,esi
			mov [ebx].MessageFunc.nSize,4
			add esi,4
		.ELSEIF EAX=='0LLH'
			mov [ebx].HLL.lpAddr,esi
			lodsd
			push ebx
			mov ebx,eax
			mov edi,esi
			xor eax,eax
			.while ebx
				repne scasb
				mov edx,[edi]
				add edi,4
				.while edx
					repne scasb
					add edi,8
					push edx
					mov edx,[edi-4]
					.while edx
						repne scasb
						add edi,4
						dec edx
					.endw
					pop edx
					dec edx
				.endw
				dec ebx
			.endw
			pop ebx
			mov ecx,edi
			sub ecx,esi
			mov esi,edi
			add ecx,4
			mov [ebx].HLL.nSize,ecx
		.ELSEIF EAX=='0IWS'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].SwitchData.lpRestruct,eax
			mov @nNums,eax
			mov [ebx].SwitchData.lpAddr,esi
			lodsd
			mov edi,eax
			.while edi
				mov ecx,@nNums
				mov [ecx],esi
				add @nNums,4
				add esi,12
				mov edx,[esi-4]
				shl edx,3
				add esi,edx
				dec edi
			.endw
			mov ecx,esi
			sub ecx,[ebx].SwitchData.lpAddr
			mov [ebx].SwitchData.nSize,ecx
		.ELSEIF EAX=='REVG'
			mov [ebx].GameVersion.lpAddr,esi
			mov [ebx].GameVersion.nSize,4
			add esi,4
		.ELSEIF EAX=='LBLS'
			mov [ebx].SLBL.lpAddr,esi
			lodsd
			mov edx,eax
			xor eax,eax
			or ecx,-1
			mov edi,esi
			.while edx
				repne scasb
				add edi,4
				dec edx
			.endw
			mov ecx,edi
			sub ecx,esi
			add ecx,4
			mov [ebx].SLBL.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='0RTS'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].Strings.lpRestruct,eax
			mov [ebx].Strings.lpAddr,esi
			lodsd
			mov ecx,[ebx].Strings.lpRestruct
			push ebx
			mov ebx,ecx
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				mov dword ptr [ebx],edi
				repne scasb
				add ebx,4
				dec edx
			.endw
			pop ebx
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].Strings.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='MANF'
			mov ecx,[esi]
			shl ecx,2
			invoke HeapAlloc,hHeap,0,ecx
			mov [ebx].FileName.lpRestruct,eax
			mov [ebx].FileName.lpAddr,esi
			lodsd
			mov ecx,[ebx].FileName.lpRestruct
			push ebx
			mov ebx,ecx
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				mov dword ptr [ebx],edi
				repne scasb
				add ebx,4
				dec edx
			.endw
			pop ebx
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].FileName.nSize,ecx
			mov esi,edi
		.ELSEIF EAX=='PMJO'
			mov [ebx].Onjump.lpAddr,esi
			mov [ebx].Onjump.nSize,4
			add esi,4
		.ELSEIF EAX=='TCNF'
			mov [ebx].FuctionType.lpAddr,esi
			lodsd
			mov [ebx].FuctionType.nSize,eax
			lea esi,[esi+eax-4]
		.ELSEIF EAX=='CYEK'
			mov [ebx].KeyC.lpAddr,esi
			mov [ebx].KeyC.nSize,4
			add esi,4
		.elseif eax=='GJBO'
			mov [ebx].ObjG.lpAddr,esi
			lodsd
			mov edx,eax
			mov edi,esi
			xor eax,eax
			or ecx,-1
			.while edx
				repne scasb
				dec edx
			.endw
			lea ecx,[edi+4]
			sub ecx,esi
			mov [ebx].ObjG.nSize,ecx
			mov esi,edi
		.else
			mov eax,1
			ret
		.endif
	.endw
	assume edi:nothing
	xor eax,eax
	ret
_PreScan6 endp

_PreScan proc _lpFile,_nSize,_lpAinSegs
	mov eax,_lpFile
	.if dword ptr [eax]!='SREV'
		xor eax,eax
		ret
	.endif
	mov eax,[eax+4]
	push _lpAinSegs
	push _nSize
	push _lpFile
	.if eax==4 || eax==5
		call _PreScan4
	.elseif eax==6
		call _PreScan6
	.else
		xor eax,eax
	.endif
	ret
_PreScan endp
