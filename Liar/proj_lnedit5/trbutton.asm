
_TrButtonData struct
	DrawState dd ?
	IsEnabled dd ?
	IsDcStored dd ?
	StoredDC dd ?
	IsMoving dd ?
_TrButtonData ends

DST_MOVEIN EQU 1
DST_BUTTONDOWN EQU 2
DST_NOTHING EQU 3

.data?

	lpOldButtonProc dd ?
.data
	
	szButtonClass db 'button',0
	szNewButtonClass db 'button1',0


.code
 
_CreateTrButton proc _hInstance
	local @wce:WNDCLASSEX
	local @lpOldButtonProc

	invoke GetClassInfoEx,NULL,offset szButtonClass,addr @wce
	push @wce.lpfnWndProc
	pop lpOldButtonProc
	push offset _NewButtonProc
	pop @wce.lpfnWndProc
	push _hInstance
	pop @wce.hInstance
	mov @wce.lpszClassName,offset szNewButtonClass
	mov @wce.cbSize,sizeof @wce
	or @wce.style,CS_GLOBALCLASS
	mov @wce.cbWndExtra,8
	
	invoke RegisterClassEx,addr @wce

	ret
_CreateTrButton endp

;
_NewButtonProc proc uses ebx esi edi,hwnd,uMsg,wParam,lParam
	local @hbmp,@hcdc,@rect:RECT,@cx,@cy
	local @tm:TRACKMOUSEEVENT
	mov eax,uMsg
	.if eax==WM_ERASEBKGND
		invoke GetWindowLong,hwnd,4
		mov ebx,eax
		assume ebx:ptr _TrButtonData
		.if ![ebx].IsDcStored
			invoke GetClientRect,hwnd,addr @rect
			mov eax,@rect.bottom
			sub eax,@rect.top
			mov @cy,eax
			mov eax,@rect.right
			sub eax,@rect.left
			mov @cx,eax
			
			invoke CreateCompatibleDC,wParam
			mov @hcdc,eax
			invoke CreateCompatibleBitmap,wParam,@cx,@cy
			mov @hbmp,eax
			invoke SelectObject,@hcdc,@hbmp
			invoke BitBlt,@hcdc,0,0,@cx,@cy,wParam,0,0,SRCCOPY
			mov [ebx].IsDcStored,1
			mov eax,@hcdc
			mov [ebx].StoredDC,eax
		.endif
		assume ebx:nothing
		mov eax,TRUE
		ret
	.elseif eax==WM_MOUSEMOVE
		mov eax,wParam
		test eax,MK_LBUTTON
		je @F
			ret
		@@:
		invoke GetWindowLong,hwnd,4
		mov ebx,eax
		assume ebx:ptr _TrButtonData
		.if [ebx].IsEnabled && ![ebx].IsMoving
			mov [ebx].IsMoving,1
			mov [ebx].DrawState,DST_MOVEIN
			invoke InvalidateRect,hwnd,NULL,TRUE
			mov @tm.cbSize,sizeof @tm
			mov @tm.dwFlags,TME_LEAVE
			mov eax,hwnd
			mov @tm.hwndTrack,eax
			mov @tm.dwHoverTime,0
			invoke TrackMouseEvent,addr @tm
		.endif
	.elseif eax==WM_LBUTTONDOWN
		invoke GetWindowLong,hwnd,4
		mov ebx,eax
		mov [ebx].DrawState,DST_BUTTONDOWN
		invoke InvalidateRect,hwnd,NULL,TRUE
	.elseif eax==WM_LBUTTONUP
		invoke GetWindowLong,hwnd,4
		mov ebx,eax
		mov [ebx].DrawState,DST_NOTHING
		invoke InvalidateRect,hwnd,NULL,TRUE
	.elseif eax==WM_MOUSELEAVE
		invoke GetWindowLong,hwnd,4
		mov ebx,eax
		mov [ebx].DrawState,DST_NOTHING
		mov [ebx].IsMoving,0
		invoke InvalidateRect,hwnd,NULL,TRUE
		assume ebx:nothing
	.elseif eax==WM_CREATE
		invoke VirtualAlloc,NULL,sizeof _TrButtonData,MEM_COMMIT,PAGE_READWRITE
		.if !eax
			invoke SendMessage,hwnd,WM_DESTROY,0,0
			ret
		.endif
		invoke SetWindowLong,hwnd,4,eax
	.elseif eax==WM_DESTROY
		invoke GetWindowLong,hwnd,4
		invoke VirtualFree,eax,sizeof _TrButtonData,MEM_DECOMMIT
	.endif
	invoke CallWindowProc,lpOldButtonProc,hwnd,uMsg,wParam,lParam
	ret
_NewButtonProc endp
;
_DrawButton proc _lpDrawItem
	local @rect:RECT,@cx,@cy
	local @hilight,@shadow
	local @hPenhi,@hPenshd
	local @szText[256]:byte
	local @size:POINT
	mov esi,_lpDrawItem
	assume esi:ptr DRAWITEMSTRUCT
	invoke GetWindowLong,[esi].hwndItem,4
	.if !eax
		ret
	.endif
	mov ebx,eax
	assume ebx:ptr _TrButtonData
	mov eax,[esi].itemState
	test eax,ODS_DISABLED
	.if ZERO?
		mov [ebx].IsEnabled,1
	.else
		mov [ebx].IsEnabled,0
		mov [ebx].DrawState,DST_NOTHING
	.endif
	invoke GetClientRect,[esi].hwndItem,addr @rect
	mov eax,@rect.right
	sub eax,@rect.left
	mov @cx,eax
	mov eax,@rect.bottom
	sub eax,@rect.top
	mov @cy,eax
	invoke BitBlt,[esi].hdc,0,0,@cx,@cy,[ebx].StoredDC,0,0,SRCCOPY

	invoke GetSysColor,COLOR_BTNHILIGHT
	mov @hilight,eax
	invoke GetSysColor,COLOR_BTNSHADOW
	mov @shadow,eax
	invoke CreatePen,PS_SOLID,0,@hilight
	mov @hPenhi,eax
	invoke CreatePen,PS_SOLID,0,@shadow
	mov @hPenshd,eax
	dec @rect.right
	dec @rect.bottom
	.if [ebx].DrawState==DST_MOVEIN
		invoke SelectObject,[esi].hdc,@hPenhi
		invoke MoveToEx,[esi].hdc,0,@rect.bottom,NULL
		invoke LineTo,[esi].hdc,0,0
		invoke LineTo,[esi].hdc,@rect.right,0
		invoke SelectObject,[esi].hdc,@hPenshd
		invoke LineTo,[esi].hdc,@rect.right,@rect.bottom
		invoke LineTo,[esi].hdc,0,@rect.bottom
	.elseif [ebx].DrawState==DST_BUTTONDOWN
		invoke SelectObject,[esi].hdc,@hPenshd
		invoke MoveToEx,[esi].hdc,0,@rect.bottom,NULL
		invoke LineTo,[esi].hdc,0,0
		invoke LineTo,[esi].hdc,@rect.right,0
		invoke SelectObject,[esi].hdc,@hPenhi
		invoke LineTo,[esi].hdc,@rect.right,@rect.bottom
		invoke LineTo,[esi].hdc,0,@rect.bottom
	.endif
	invoke DeleteObject,@hPenhi
	invoke DeleteObject,@hPenshd
	
	invoke GetWindowText,[esi].hwndItem,addr @szText,256
	push eax
	mov ecx,eax
	invoke GetTextExtentPoint32,[esi].hdc,addr @szText,ecx,addr @size
	mov eax,@cx
	sub eax,@size.x
	shr eax,1
	mov @cx,eax
	mov eax,@cy
	sub eax,@size.y
	shr eax,1
	mov @cy,eax
	pop edi
	.if [ebx].IsEnabled
		.if [ebx].DrawState==DST_BUTTONDOWN
			inc @cx
			inc @cy
		.endif
		invoke SetBkMode,[esi].hdc,TRANSPARENT
		invoke TextOut,[esi].hdc,@cx,@cy,addr @szText,edi
	.else
		invoke DrawState,[esi].hdc,NULL,NULL,addr @szText,edi,@cx,@cy,0,0,DST_TEXT OR DSS_DISABLED
	.endif
	
	assume esi:nothing,ebx:nothing
	ret
_DrawButton endp