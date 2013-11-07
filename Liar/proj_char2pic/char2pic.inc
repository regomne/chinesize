      include \masm32\include\windows.inc
      include \masm32\include\masm32.inc
      include \masm32\include\gdi32.inc
      include \masm32\include\user32.inc
      include \masm32\include\kernel32.inc
      include \masm32\include\Comctl32.inc
      include \masm32\include\comdlg32.inc
      include \masm32\include\shell32.inc
      include \masm32\include\oleaut32.inc
      include \masm32\include\shlwapi.inc
       include \masm32\include\advapi32.inc
     include \masm32\include\msvcrt.inc
      include \masm32\macros\macros.asm
      include \masm32\macros\strings.mac

      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
        includelib \masm32\lib\advapi32.lib
     includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
      includelib \masm32\lib\shlwapi.lib
      includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib

;include rsrc.inc
Main proto :dword
_WinMain proto
_WndMainProc proto :dword,:dword,:dword,:dword
_Str2PicW proto :dword,:dword,:dword,:dword,:dword,:dword
_SaveBmp proto :dword,:dword
_MakeDIB proto :dword,:dword,:dword
_MakeLink proto :dword,:dword,:dword,:dword
_GetLinkW proto :dword,:dword

PIC_WIDTH		EQU		378
PIC_HEIGHT		EQU		330
TITLECHAR_WIDTH	EQU		24
TITLECHAR_HEIGHT	EQU		32
TITLE_X			EQU		0
TITLE_Y			EQU		10
PAGE_X			EQU		318

CHAR_WIDTH		EQU		18
CHAR_HEIGHT		EQU		29
CHAR_X			EQU		0
CHAR_Y			EQU		48

E_FORMAT		EQU		1
E_TOOLONG		EQU		2

_BitmapEntry struct
	Next			dd		?
	Hdr			BITMAPINFOHEADER	<?>
	lpDib			dd		?
_BitmapEntry ends

_PicLink struct
	ItemName		db		80	dup(?)
	PicIdx		dd		?
_PicLink ends


.data?
	hInstance dd ?
	hHeap	dd	?
	hWinMain dd ?
	hIcon dd ?
	
	hOutLink		dd		?
	Links		db		100*sizeof(_PicLink) dup (?)
	
	dwTemp		dd		?
.data
	defHdr		BITMAPINFOHEADER	<sizeof(BITMAPINFOHEADER),PIC_WIDTH,PIC_HEIGHT,1,32,0,0,0,0,0,0>
	szClassName db "Clock",0
	szDisplayName db "My Clock",0	
	szFont		db	'ËÎÌå',0
	szFontTitle	db	'ºÚÌå',0
	szBegin		db	0ffh,0feh