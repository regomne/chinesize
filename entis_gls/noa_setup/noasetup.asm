.486
.model flat,stdcall
option casemap:none


include noasetup.inc

;Return Value
;ÎÄ¼þ·ÃÎÊ´íÎó=1

.code

start:
;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
invoke GetModuleHandle,NULL
mov hInstance,eax
call Main
invoke ExitProcess,eax

;¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­¡­
Main proc
	invoke CreateFileW,$CTW0("data1.noa"),GENERIC_READ or GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	.if eax==-1
_AccessErr:
		mov eax,1
		ret
	.endif
	mov hNoaFile,eax
	invoke ReadFile,hNoaFile,offset NoaHdr,sizeof NoaHdr,offset dwTemp,0
	or eax,eax
	je _AccessErr
	invoke ReadFile,hNoaFile,offset NoaDir,sizeof NoaDir,offset dwTemp,0
	or eax,eax
	je _AccessErr
	lea esi,Eris
	assume esi:ptr _EriPos
	invoke CreateFile,esi,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0
	cmp eax,-1
	je _AccessErr
	assume esi:nothing
	ret
Main endp

end start