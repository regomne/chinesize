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
      
      includelib E:\Program\zlib125-dll\lib\zdll.lib

MyMain proto
_memcpy2 proto :dword,:dword,:dword
_MakeMsg0 proto :dword,:dword,:dword
compress proto C :dword,:dword,:dword,:dword

_Segment struct
	lpAddr		dd		?
	nSize		dd		?
	lpRestruct	dd		?
_Segment ends

_AinSegs struct
	Version		_Segment		<>
	Code		_Segment		<>
	Function		_Segment		<>
	GlobalVar	_Segment		<>
	GlobalSet	_Segment		<>
	Structs		_Segment		<>
	Message0	_Segment		<>
	Main		_Segment		<>
	MessageFunc	_Segment		<>
	HLL			_Segment		<>
	SwitchData	_Segment		<>
	GameVersion	_Segment		<>
	SLBL			_Segment		<>
	Strings		_Segment		<>
	FileName		_Segment		<>
	Onjump		_Segment		<>
	FuctionType	_Segment		<>
	KeyC		_Segment		<>
	ObjG		_Segment		<>
_AinSegs ends

.data?
	hInstance		dd		?
	hHeap			dd		?

	dwTemp		dd		?
	
P_MES		EQU		-1
P_CALL		EQU		-2
P_FUNC		EQU		-3
P_ENDF		EQU		-4
P_EOF		EQU		-5
P_SPUSH		EQU		-6
.data
	dbHdr		dd		324941h,0,0,0
.const
				;	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
	dbCodeTable	db	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
				db	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
				db	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4,	4,	4,	0	;2
				db	-2,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
				db	4,	-6,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	4
				db	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	8,	0,	4,	4,	4,	4	;5
				db	4,	-3,	-5,	4,	0,	0,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0
				db	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-4,	0
				db	0,	8,	4,	0,	0,	0,	0,	0,	0,	0,	4,	4,	8,	0,	0,	0	;8
				db	0,	0,	0,	0,	0,	0,	0,	0,	0,	-10,	-10,	-10,	-10,	-10,	-10,	-10
	
	
