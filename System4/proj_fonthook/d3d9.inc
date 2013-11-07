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
      include \masm32\include\shlwapi.inc
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
      includelib \masm32\lib\shlwapi.lib
      includelib \masm32\lib\msvcrt.lib
      

FontInfo struct
	szName	db		64	dup(?)
	hFont	dd		256	dup(?)
	nDescent	dd		256	dup(?)
	nAscent	dd		256	dup(?)
FontInfo ends

HookApi proto :dword,:dword,:dword
NewCrFile proto :dword,:dword,:dword,:dword,:dword,:dword,:dword
NewCrDir proto :dword,:dword
NewFind proto :dword,:dword
NewFindN proto :dword,:dword
NewCpFile proto :dword,:dword,:dword

.data?
	hInstance dd ?
	nDist		dd		?
	
	szGName		db		256	dup(?)
	
	g_hdc		dd		?
	g_LastFont	dd		?
	g_LastSize	dd		?
	
	g_FontInfo	FontInfo	FONT_TYPES	dup(<>)
	
	dwBuffer		db		8192	dup	(?)
	
	dwErrPos		dd		?
	
	dwTemp		dd		?
.data
	
	dbMat		dd		10000h,0,0,10000h
	TW0		'SimHei',		szFn0
	TW0		'SimSun',		szFn1
	TW0		'NSimSun',	szFn2
	
	dbNames		dd		offset szFn0,offset szFn1,offset szFn2
	
		