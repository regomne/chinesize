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
_MakeFile proto :dword
_CvtOrs proto :dword
_CvtTxt proto :dword
_WriteLog proto :dword,:dword
_FindFiles proto :dword,:dword
_ReplaceInMem proto :dword,:dword,:dword,:dword,:dword
_memcpy proto

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	hHeap			dd		?
	
	hLogFile			dd		?
	
	dwTemp			dd		?
	
	szOrsDir		db		1024 dup (?)
	szTxtDir		db		1024 dup (?)
	
	fCompress		dd		?
	fUncompress		dd		?
	
.const
	szNull		dw		0,0,0,0

