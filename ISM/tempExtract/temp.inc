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
      include \masm32\include\version.inc
      include \masm32\include\shlwapi.inc
      include \masm32\macros\macros.asm
      include \masm32\macros\strings.mac
	include \masm32\include\ole32.inc
	include \masm32\include\msimg32.inc

      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
      includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
      includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib
      includelib \masm32\lib\version.lib
      includelib \masm32\lib\shlwapi.lib
	includelib \masm32\lib\ole32.lib
	includelib \masm32\lib\msimg32.lib

_IsmHdr struc
	szMagic		db		12 dup (?)
	uEntries		dw		?
	uMode		dw		?
_IsmHdr ends

start proto
	
.data?
	hHeap		dd		?
	dwTemp		dd		?

.data
