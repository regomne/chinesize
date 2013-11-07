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
      
_Name struct
	lpszID		dd		?
	lpszName	dd		?
	lpszNumber	dd		?
_Name ends

_Mail struct
	lpszID		dd		?
	unk			dd		12	dup(?)
	lpszSub		dd		?
	lpszTo		dd		?
	lpszText		dd		?
_Mail ends

ReadInfo proto :dword,:dword
ReadName proto :dword,:dword
ReadMail proto :dword,:dword
ReadBbs proto :dword,:dword

.data?
	hInstance 	dd		 ?
	nDist		dd		?
	
	lpCgcnTbl	dd		?
	dwTemp		dd		?
	scStr			dd		?
.data
	scCgcn		dd		0,6363h,0,0,0,2,0fh
	
		