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
      include \masm32\macros\strings.mac

      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
      includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
      includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib

assert macro expression
	if DEBUG
		.if !(expression)
			int 3
		.endif
	endif
endm

_JumpEntry struct
	Flink			dd		?
	nOffset		dd		?
_JumpEntry ends

.data?
	hInstance dd ?
	hProcessHeap	dd		?
	
	lpTxt		dd		?
	lpNewImage	dd		?
	
	pBJumpTable	dd		?
	
	dbBlockAddr	dd		6	dup(?)
	dbBlockSize	dd		6	dup(?)
	
	hCsxFile		dd		?
	hTxtFile		dd		?
	hNewCsxFile	dd		?
	
	dwTemp		dd		?
	
.data
	dbMess		db	08, 05, 01, 00, 00, 00, 04, 00,  00, 00, 4Dh, 00, 65h, 00, 73h, 0,73h,0,1
	

