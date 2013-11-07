      include \masm32\include\windows.inc
      include \masm32\include\masm32.inc
      include \masm32\include\gdi32.inc
      include \masm32\include\user32.inc
      include \masm32\include\kernel32.inc
      include \masm32\include\Comctl32.inc
      include \masm32\include\comdlg32.inc
      include \masm32\include\shell32.inc
      include \masm32\include\oleaut32.inc
      include \masm32\include\msvcrt.inc
      include \masm32\macros\macros.asm

      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
      includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
      includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib

include rsrc.inc

_MyRLEData struct
	pSrc dd ?
	nSrcLen dd ?
	union
		pCodeTable dd ?
		pDestAlpha dd ?
	ends
	union
		nTableSize dd ?
		nAlphaSize dd ?
	ends
	pDest dd ?
	nDestSize dd ?
	nErrCode dd ?
_MyRLEData ends

_Bits struct
	curbits dd ?
	curbyte dd ?
	cache dd ?
	stream dd ?
	stream_length dd ?
_Bits ends

;_MyConvertData struct
;	pSrc dd ?
;	nSrcLen dd ?
;	pDestDIB dd ?
;	nDIBSize dd ?
;	pDestAlpha dd ?
;	nAlphaSize dd ?
;	nErrCode dd ?
;_MyConvertData ends

_DlgMainProc proto :dword,:dword,:dword,:dword
_FoldPic proto :dword,:dword
_RLECompress proto :ptr _MyRLEData,:dword,:dword
_RLECompress2 proto :ptr _MyRLEData,:dword,:dword
_BitsPutHigh proto :ptr _Bits,:dword,:dword
_PrepareFreqTable proto :dword,:dword,:dword,:dword
_PrepareFreqTable2 proto :dword,:dword,:dword,:dword
_ConvertDIB32to16 proto :ptr _MyRLEData,:dword,:dword
_memcpy proto

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	
	hFindFile dd ?
	hBmpFile dd ?
	lpLimBuff dd ?
	
	nLimSize dd ?
	
	dwTemp dd ?
	
	pMemory dd 16 dup (?)
	
	szFileNameOri db 256 dup (?)
	szDir1 db 256 dup (?)
	
.data
	szDisplayName db 'lim folder by AmaF',0
	szNoMemory db '无法申请到足够的内存！',0
	
	szFormat2 db '位图文件(*.bmp)',0,'*.bmp',0,0

	szBmp db '*.bmp',0	
