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

_NoaHdr struct
	magic			db		8 dup (?)
	file_id			dd		?
	reserved			dd		?
	format_desc		db		40 dup(?)
	len1_lo			dd		?
	len1_hi			dd		?
_NoaHdr ends

_NoaDirRecord struct
	code			db		8 dup(?)
	len1_lo			dd		?
	len1_hi			dd		?
_NoaDirRecord ends

_NoaEntry struct
	len1_lo			dd		?
	len1_hi			dd		?
	attr				dd		?
	encode			dd		?
	offset_lo			dd		?
	offset_hi			dd		?
	time1			dd		?
	time2			dd		?
_NoaEntry ends

_EriPos struct
	name1			db		64 dup(?)
	pos				dd		?
_EriPos ends

Main proto

.data?
	hInstance dd ?
	hNoaFile			dd		?
	
	NoaHdr			_NoaHdr			<>
	NoaDir			_NoaDirRecord	<>
	nEntries			dd		?
	
	dwTemp			dd		?
	
.data
	Eris				_EriPos	<'countdown10.eri',10edh>
	
