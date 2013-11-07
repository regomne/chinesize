.code
_CreateDIBitmap proc _hwnd,_lpFileData
	local @lpBitmapInfo,@lpBitmapBits
	local @dwWidth,@dwHeight
	local @hdc,@hBitmap
	pushad
	mov @hBitmap,0
	mov esi,_lpFileData
	mov eax,BITMAPFILEHEADER.bfOffBits[esi]
	add eax,esi
	mov @lpBitmapBits,eax
	add esi,sizeof BITMAPFILEHEADER
	mov @lpBitmapInfo,esi
	.if BITMAPINFO.bmiHeader.biSize[esi]==sizeof BITMAPCOREHEADER
		movzx eax,BITMAPCOREHEADER.bcWidth[esi]
		movzx ebx,BITMAPCOREHEADER.bcHeight[esi]
	.else
		mov eax,BITMAPINFOHEADER.biWidth[esi]
		mov ebx,BITMAPINFOHEADER.biHeight[esi]
	.endif
	mov @dwWidth,eax
	mov @dwHeight,ebx
	invoke GetDC,_hwnd
	push eax
	invoke CreateCompatibleDC,eax
	mov @hdc,eax
	pop eax
	push eax
	invoke CreateCompatibleBitmap,eax,@dwWidth,@dwHeight
	mov @hBitmap,eax
	invoke SelectObject,@hdc,@hBitmap
	pop eax
	invoke ReleaseDC,_hwnd,eax
	invoke SetDIBitsToDevice,@hdc,0,0,@dwWidth,@dwHeight,0,0,0,@dwHeight,@lpBitmapBits,@lpBitmapInfo,DIB_RGB_COLORS
	.if eax==0
		invoke DeleteObject,@hBitmap
		mov @hBitmap,0
	.endif
	invoke DeleteDC,@hdc
	popad
	mov eax,@hBitmap
	ret
_CreateDIBitmap endp