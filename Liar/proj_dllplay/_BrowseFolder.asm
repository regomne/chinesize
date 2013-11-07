;IUnknown 接口定义
include \masm32\include\ole32.inc
includelib \masm32\lib\ole32.lib
externdef IID_IUnknown:IID
LPUNKNOWN typedef DWORD
LPPUNKNOWN typedef ptr LPUNKNOWN

IUnknown_QueryInterfaceProto typedef proto :dword,:dword,:dword
IUnknown_AddRefProto typedef proto :dword
IUnknown_ReleaseProto typedef proto :dword
IUnknown_QueryInterface typedef ptr IUnknown_QueryInterfaceProto
IUnknown_AddRef typedef ptr IUnknown_AddRefProto
IUnknown_Release typedef ptr IUnknown_ReleaseProto

IUnknown struct dword
	QueryInterface IUnknown_QueryInterface ?
	AddRef IUnknown_AddRef ?
	Release IUnknown_Release ?
IUnknown ends

;IMalloc 接口定义
externdef IID_IMalloc:IID
LPMALLOC typedef dword
LPPMALLOC typedef ptr LPMALLOC

IMalloc_AllocProto typedef proto :dword,:dword
IMalloc_ReallocProto typedef proto :dword,:dword,:dword
IMalloc_FreeProto typedef proto :dword,:dword
IMalloc_GetSizeProto typedef proto :dword,:dword
IMalloc_DidAllocProto typedef proto :dword,:dword
IMalloc_HeapMinimizeProto typedef proto :dword,:dword

IMalloc_Alloc typedef ptr IMalloc_AllocProto
IMalloc_Realloc typedef ptr IMalloc_ReallocProto
IMalloc_Free typedef ptr IMalloc_FreeProto
IMalloc_GetSize typedef ptr IMalloc_GetSizeProto
IMalloc_DidAlloc typedef ptr IMalloc_DidAllocProto
IMalloc_HeapMinimize typedef ptr IMalloc_AllocProto
IMalloc_Alloc typedef ptr IMalloc_AllocProto

IMalloc struct dword
	QueryInterface IUnknown_QueryInterface ?
	AddRef IUnknown_AddRef ?
	Release IUnknown_Release ?
	Alloc IMalloc_Alloc ?
	Realloc IMalloc_Realloc ?
	Free IMalloc_Free ?
	GetSize IMalloc_GetSize ?
	DidAlloc IMalloc_DidAlloc ?
	HeapMinimize IMalloc_HeapMinimize ?
IMalloc ends

.data?
	_BrowseFolderTmp dd ?
.code

_BrowseFolderCallBack proc hwnd,uMsg,lParam,lpData
	local @szBuffer[260]:byte
	mov eax,uMsg
	.if eax==BFFM_INITIALIZED
		invoke SendMessage,hwnd,BFFM_SETSELECTION,TRUE,_BrowseFolderTmp
	.elseif eax==BFFM_SELCHANGED
		invoke SHGetPathFromIDList,lParam,addr @szBuffer
		invoke SendMessage,hwnd,BFFM_SETSTATUSTEXT,0,addr @szBuffer
	.endif
	xor eax,eax
	ret
_BrowseFolderCallBack endp
;
_BrowseFolder proc _hwnd,_lpszBuffer,_lpszInfo
	local @stBrowseInfo:BROWSEINFO
	local @stMalloc
	local @pidlParent,@dwReturn
	
	pushad
	invoke CoInitialize,NULL
	invoke SHGetMalloc,addr @stMalloc
	.if eax==E_FAIL
		mov @dwReturn,FALSE
		jmp @F
	.endif
	invoke RtlZeroMemory,addr @stBrowseInfo,sizeof @stBrowseInfo
	push _hwnd
	pop @stBrowseInfo.hwndOwner
	push _lpszBuffer
	pop _BrowseFolderTmp
	mov @stBrowseInfo.lpfn,offset _BrowseFolderCallBack
	push _lpszInfo
	pop @stBrowseInfo.lpszTitle
	mov @stBrowseInfo.ulFlags,BIF_RETURNONLYFSDIRS OR BIF_STATUSTEXT
	invoke SHBrowseForFolder,addr @stBrowseInfo
	mov @pidlParent,eax
	.if eax!=NULL
		invoke SHGetPathFromIDList,eax,_lpszBuffer
		mov eax,TRUE
	.else
		mov eax,FALSE
	.endif
	
	mov @dwReturn,eax
	mov eax,@stMalloc
	mov eax,[eax]
	invoke (IMalloc ptr [eax]).Free,@stMalloc,@pidlParent
	mov eax,@stMalloc
	mov eax,[eax]
	invoke (IMalloc ptr [eax]).Release,@stMalloc
	
	@@:
	invoke CoUninitialize
	popad
	mov eax,@dwReturn
	ret
_BrowseFolder endp