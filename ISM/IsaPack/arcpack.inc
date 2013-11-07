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


_WinMain proto
_DlgMainProc proto :dword,:dword,:dword,:dword
_ArcUpk proto :dword
_ArcPack proto :dword
_MakeDir proto :dword

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	hHeap			dd		?
	
	hPGB			dd		?
	
	dwTemp			dd		?
	
	szArcName		db		1024 dup (?)
	szDirName		db		1024 dup (?)
	
.data
	szMagic			db		'ISM ARCHIVED',0,0,0,0
	szFormat2		db		'Isa文件(*.isa)',0,'*.isa',0,'所有文件(*.*)',0,'*.*',0,0

