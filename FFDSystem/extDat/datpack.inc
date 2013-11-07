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
      
      include rsrc.inc

_DatEntry struct
	name1			db		64 dup(?)
	offset1			dd		?
	unclen			dd		?
	complen			dd		?
	mode			dd		?
_DatEntry ends

_WinMain proto
_DlgMainProc proto :dword,:dword,:dword,:dword
_ArcUpk proto :dword
_ArcPack proto :dword,:dword
_MakeDir proto :dword

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	hHeap			dd		?
	
	dwTemp			dd		?
	
	szArcName		db		1024 dup (?)
	szDirName		db		1024 dup (?)
	
.data
	sMagic			db		'RepiPack'
	sKeyName		dd		0837FC07Ah, 098FCDBA2h
	sKeyIdx			dd		0F517AA26h, 098FCDBA2h

	szFormat2		db		'Dat文件(*.dat)',0,'*.dat',0,'所有文件(*.*)',0,'*.*',0,0

