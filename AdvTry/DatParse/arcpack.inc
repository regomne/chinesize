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
      
      
_DatHeader struct
	szMagic			db		12 dup(?)
	nHeaderLen		dd		?
	nIndexOffset		dd		?
	nIndexEntryLen	dd		?
	resvd			dd		2 dup(?)
	nIndexEntries		dd		?
_DatHeader ends

_DatEntry struc
	nZero			db		?
	szName			db		263 dup (?)
	nOffset			dd		?
	nLen				dd		?
	nReserved		dd		?
_DatEntry ends

_WinMain proto
_DlgMainProc proto :dword,:dword,:dword,:dword

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon	dd	?
	hHeap			dd		?
	
	dwTemp			dd		?
	szFileN			db		1024 dup (?)
.data
