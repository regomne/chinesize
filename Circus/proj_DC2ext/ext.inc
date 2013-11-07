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

      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
      includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
      includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib


MyMain proto

.data?
	hFindFile dd ?
	hFile dd ?
	hFiletxt dd ?
	
	dwTemp dd ?
	
	lpBuffer dd ?
	lpTxtBuffer dd ?
	pTxt dd ?
	lpUTFBuffer dd ?
	
	dwIndexes dd ?
	
	pIndex dd ?
	lpText dd ?
	
	
.data
	szMes db '*.mes',0
	szdirMes db 'cn',0
