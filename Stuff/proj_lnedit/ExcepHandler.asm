.data
	szErrMsg db '脚本读取发生错误！',0dh,0ah,'位置：%08X，代码：%08X，标志：%08X',0dh,0ah
			db '请查看文本列表最后一行正常文本的行数，并连同上述信息一起报告给作者！',0
	szFatalError db '发生致命错误！程序即将终止！',0dh,0ah
	szProcError db '错误位置：%08X，代码：%08X，标志：%08X',0

.code

_AddHandler proc _lpExceptionRecord,_lpSEH,_lpContext,_lpDispatcherContext
	local @szBuffer[256]:byte 
	pushad
	mov esi,_lpExceptionRecord
	mov edi,_lpContext
	assume esi:ptr EXCEPTION_RECORD,edi:ptr CONTEXT
	.if [esi].ExceptionCode!=EXCEPTION_ACCESS_VIOLATION && [esi].ExceptionCode!=EXCEPTION_IN_PAGE_ERROR
		mov eax,ExceptionContinueSearch
		ret
	.endif
	invoke wsprintf,addr @szBuffer,addr szErrMsg,[edi].regEip,[esi].ExceptionCode,[esi].ExceptionFlags
	invoke MessageBox,0,addr @szBuffer,0,MB_OK or MB_ICONERROR
	mov eax,_lpSEH
	push [eax+8]
	pop [edi].regEip
	push eax
	pop [edi].regEsp
	assume esi:nothing,edi:nothing
	popad
	mov eax,ExceptionContinueExecution
	ret
_AddHandler endp

_ProcHandler proc uses esi edi,_lpExceptionRecord,_lpSEH,_lpContext,_lpDispatcherContext
	local @szBuffer[256]:byte 
	mov esi,_lpExceptionRecord
	mov edi,_lpContext
	assume esi:ptr EXCEPTION_RECORD,edi:ptr CONTEXT
	.if [esi].ExceptionCode==0c0000027h
		invoke wsprintf,addr @szBuffer,addr szFatalError,[edi].regEip,[esi].ExceptionCode,[esi].ExceptionFlags
		invoke MessageBox,hWinMain,addr @szBuffer,0,MB_OK or MB_ICONERROR
		mov eax,ExceptionContinueSearch
	.else
		invoke wsprintf,addr @szBuffer,addr szProcError,[edi].regEip,[esi].ExceptionCode,[esi].ExceptionFlags
		invoke MessageBox,hWinMain,addr @szBuffer,0,MB_OK or MB_ICONERROR
		mov eax,_lpSEH
		push [eax+8]
		pop [edi].regEip
		push [eax+0ch]
		pop [edi].regEbp
		push [eax]
		pop [edi].regEsp
		mov eax,ExceptionContinueExecution
	.endif
	assume esi:nothing,edi:nothing
	ret
	
_ProcHandler endp