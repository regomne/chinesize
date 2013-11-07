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
	
	include rsrc.inc

_ykc_header struct
	magic			db	8	dup(?)
	header_len		dd	?
	zerodata			dd	?
	index_offset		dd	?
	index_len		dd	?
_ykc_header ends

_entry struct
	str_offset			dd	?
	str_len			dd	?
	file_offset		dd	?
	file_len			dd	?
	zerodata			dd	?
_entry ends   

_DlgMainProc proto :dword,:dword,:dword,:dword
_Pack proto :dword,:dword
_Encode proto :dword,:dword

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	
	szFileName	db		512		dup(?)
	szDirName	db		512		dup(?)
	
	dwTemp	dd	?
	
.data
	szClassName db "ExtractClass",0
	szDisplayName db "ExtractProgram",0
	
	
	
	