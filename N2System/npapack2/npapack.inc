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
      includelib \masm32\lib\shlwapi.lib
      includelib \masm32\lib\ntdll.lib
      
;      includelib E:\Program\zlib125-dll\lib\zdll.lib
	includelib zlib.lib
;      includelib E:\Program\zlib-1.2.5\contrib\vstudio\vc10\x86\ZlibDllRelease\zlibvc.lib
      includelib "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\uuid.lib"
      includelib "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\lib\libcmt.lib"
      includelib "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\lib\oldnames.lib"
      
      include rsrc.inc

_NpaHdr struc
	sMagic		db	3 dup(?)
	Version		dd	?
	hash0		dd	?
	hash1		dd	?
	isCompressed	db	?
	isCrypted		db	?
	nEntries		dd	?
	nDirEntries	dd	?
	nFileEntries	dd	?
	resv1		dd	?
	resv2		dd	?
	nIndexLen	dd	?
_NpaHdr ends

_NpaEntry struc
	szName			db	512	dup(?)
	nNameLen		dd	?
	nType			dd	?
	nDirNum			dd	?
	nDataOffset		dd	?
	nCompLen		dd	?
	nUncLen			dd	?
	bIsCrypted		dd	?
	bIsComp			dd	?
	nKey			dd	?
	nDecLen			dd	?
	nHash0			dd	?
	nHash1			dd	?
_NpaEntry ends

uncompress proto :dword,:dword,:dword,:dword
compress proto :dword,:dword,:dword,:dword

_WinMain proto
_DlgMainProc proto :dword,:dword,:dword,:dword
_NpaUpk proto :dword
_NpaPack proto :dword,:dword
_MakeFile proto :dword

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	hHeap			dd		?
	hPGB			dd		?
	
	dwTemp			dd		?
	
	dbEncTable		db		256 dup(?)
	
	szArcName		db		1024 dup (?)
	szDirName		db		1024 dup (?)
	
	
.data
;貌似是鬼哭街的
;	dbDecTable\
;db	021h, 0E1h, 0A0h, 08Dh, 086h, 0F1h, 0E3h, 0E9h, 000h, 0B6h, 02Dh, 0D7h, 06Ah, 043h, 093h, 0B0h, 0ECh, 00Ah, 0CFh, 037h, 0B8h, 03Eh, 09Eh, 0F9h, 09Ah, 04Dh, 0C8h, 07Dh, 05Ah, 077h, 03Ah, 03Dh
;db	012h, 027h, 017h, 0CAh, 089h, 08Fh, 06Ch, 09Ch, 05Fh, 00Fh, 040h, 064h, 0CDh, 020h, 03Fh, 0FDh, 0B9h, 073h, 0BEh, 030h, 0EBh, 0ADh, 0B3h, 010h, 0EEh, 038h, 07Ah, 0FAh, 0D5h, 04Bh, 05Dh, 0B5h
;db	0E6h, 091h, 0A5h, 0E7h, 0DAh, 0AAh, 075h, 00Dh, 03Ch, 09Fh, 0F8h, 07Eh, 0B2h, 0A4h, 062h, 0ABh, 0CBh, 003h, 06Eh, 018h, 0E5h, 046h, 0C1h, 04Eh, 0FFh, 063h, 001h, 097h, 0E2h, 01Ah, 076h, 065h
;db	049h, 0CCh, 01Ch, 0BBh, 009h, 054h, 058h, 0C9h, 087h, 005h, 0F2h, 014h, 052h, 0DCh, 023h, 0F0h, 0DFh, 07Fh, 0EDh, 004h, 0ACh, 016h, 0A8h, 00Eh, 01Fh, 0AEh, 0BAh, 066h, 06Fh, 05Eh, 0D2h, 096h
;db	0FEh, 061h, 069h, 0B7h, 055h, 0A7h, 045h, 01Dh, 0B1h, 080h, 02Ah, 0C4h, 070h, 06Dh, 0F5h, 09Dh, 0D4h, 0DEh, 079h, 074h, 007h, 0C2h, 02Bh, 0A9h, 0BCh, 025h, 013h, 05Bh, 015h, 022h, 006h, 08Ch
;db	082h, 0F3h, 029h, 094h, 0A1h, 0A6h, 07Bh, 09Bh, 0D8h, 0A3h, 098h, 01Eh, 011h, 01Bh, 035h, 059h, 002h, 0FBh, 099h, 0D6h, 0B4h, 072h, 0D3h, 04Ch, 090h, 0C0h, 0BFh, 02Ch, 0EFh, 08Bh, 088h, 0FCh
;db	053h, 051h, 085h, 0D1h, 0DBh, 056h, 04Ah, 0F4h, 019h, 092h, 033h, 042h, 0C3h, 036h, 026h, 060h, 02Eh, 0D0h, 0AFh, 083h, 0F7h, 048h, 078h, 04Fh, 095h, 008h, 0C6h, 032h, 0CEh, 03Bh, 034h, 0D9h
;db	057h, 05Ch, 0DDh, 0E8h, 031h, 0C7h, 07Ch, 0E4h, 067h, 071h, 050h, 00Bh, 081h, 044h, 00Ch, 0C5h, 024h, 08Ah, 041h, 08Eh, 0E0h, 039h, 02Fh, 0A2h, 068h, 06Bh, 0EAh, 0F6h, 084h, 0BDh, 028h, 047h
	
;アザナエル
;	dbDecTable\
;db	21h, 071h, 0F0h, 0D8h, 0D6h, 081h, 073h, 07Ch, 000h, 046h, 028h, 067h, 0BDh, 093h, 0E3h, 040h, 07Fh, 00Dh, 05Ah, 037h, 04Bh, 039h, 0E9h, 08Ch, 0EDh, 098h, 05Bh, 0C8h, 0ADh, 0C7h, 03Dh, 038h
;db	12h, 027h, 017h, 05Dh, 0DCh, 0DAh, 0BFh, 0EFh, 0AAh, 00Ah, 090h, 0B4h, 058h, 020h, 03Ah, 088h, 04Ch, 0C3h, 049h, 030h, 07Eh, 0F8h, 043h, 010h, 079h, 03Bh, 0CDh, 08Dh, 065h, 09Eh, 0A8h, 045h
;db	76h, 0E1h, 0F5h, 077h, 06Dh, 0FDh, 0C5h, 008h, 03Fh, 0EAh, 08Bh, 0C9h, 042h, 0F4h, 0B2h, 0FEh, 05Eh, 003h, 0B9h, 01Bh, 075h, 096h, 051h, 099h, 08Ah, 0B3h, 001h, 0E7h, 072h, 01Dh, 0C6h, 0B5h
;db	9Ch, 05Fh, 01Fh, 04Eh, 00Ch, 0A4h, 0ABh, 05Ch, 0D7h, 005h, 082h, 014h, 0A2h, 06Fh, 023h, 080h, 06Ah, 0CAh, 078h, 004h, 0FFh, 016h, 0FBh, 009h, 01Ah, 0F9h, 04Dh, 0B6h, 0BAh, 0A9h, 062h, 0E6h
;db	89h, 0B1h, 0BCh, 047h, 0A5h, 0F7h, 095h, 018h, 041h, 0D0h, 02Dh, 054h, 0C0h, 0B8h, 085h, 0E8h, 064h, 069h, 0CCh, 0C4h, 007h, 052h, 02Eh, 0FCh, 04Fh, 025h, 013h, 0AEh, 015h, 022h, 006h, 0DFh
;db	0D2h, 083h, 02Ch, 0E4h, 0F1h, 0F6h, 0CEh, 0EEh, 06Bh, 0F3h, 0EBh, 019h, 011h, 01Eh, 035h, 0ACh, 002h, 08Eh, 0ECh, 066h, 044h, 0C2h, 063h, 09Fh, 0E0h, 050h, 04Ah, 02Fh, 07Ah, 0DEh, 0DBh, 08Fh
;db	0A3h, 0A1h, 0D5h, 061h, 06Eh, 0A6h, 09Dh, 084h, 01Ch, 0E2h, 033h, 092h, 053h, 036h, 026h, 0B0h, 029h, 060h, 0FAh, 0D3h, 087h, 09Bh, 0CBh, 09Ah, 0E5h, 00Bh, 056h, 032h, 059h, 03Eh, 034h, 06Ch
;db	0A7h, 0AFh, 068h, 07Bh, 031h, 057h, 0CFh, 074h, 0B7h, 0C1h, 0A0h, 00Eh, 0D1h, 094h, 00Fh, 055h, 024h, 0DDh, 091h, 0D9h, 070h, 03Ch, 02Ah, 0F2h, 0BBh, 0BEh, 07Dh, 086h, 0D4h, 048h, 02Bh, 097h

;君と彼女と彼女の恋
dbDecTable\
db 057h, 0D7h, 066h, 010h, 014h, 0E7h, 0DFh, 0D8h, 036h, 074h, 050h, 095h, 0C9h, 0AFh, 02Fh, 076h, 0DDh, 039h, 082h, 0F5h, 07Ah, 0F1h, 021h, 0E8h, 029h, 0A0h, 08Ah, 000h, 0B9h, 005h, 0F9h, 0F0h
db 04Bh, 055h, 045h, 089h, 018h, 012h, 0CDh, 02Dh, 0B2h, 032h, 0A6h, 0C3h, 080h, 056h, 0F2h, 0E0h, 078h, 00Fh, 071h, 0F6h, 0DCh, 060h, 07Fh, 046h, 0D1h, 0FAh, 009h, 0E9h, 09Eh, 0ACh, 0B0h, 07Eh
db 0D4h, 027h, 06Eh, 0D5h, 099h, 069h, 00Eh, 030h, 0FDh, 022h, 0EAh, 001h, 07Bh, 063h, 0CBh, 06Ch, 08Ch, 03Fh, 0C1h, 04Ah, 0DEh, 0A4h, 087h, 0A1h, 0E2h, 0CFh, 037h, 025h, 0DBh, 049h, 004h, 0CEh
db 0A8h, 08Dh, 04Dh, 07Ch, 038h, 0B3h, 0BAh, 088h, 015h, 03Eh, 0EBh, 043h, 0BBh, 09Dh, 05Fh, 0E6h, 092h, 002h, 0D0h, 033h, 06Dh, 044h, 06Ah, 031h, 042h, 061h, 079h, 0C4h, 0C2h, 0B1h, 09Bh, 024h
db 0E1h, 0C7h, 0C8h, 075h, 0BEh, 065h, 0AEh, 040h, 077h, 016h, 059h, 083h, 006h, 0C0h, 0EEh, 020h, 093h, 091h, 008h, 003h, 035h, 08Bh, 05Ch, 068h, 07Dh, 05Eh, 04Fh, 0BCh, 04Eh, 05Bh, 034h, 01Dh
db 01Bh, 0EFh, 058h, 023h, 067h, 064h, 00Ch, 02Ch, 09Ah, 06Fh, 02Ah, 041h, 047h, 04Ch, 0FEh, 0B8h, 03Bh, 0ECh, 028h, 094h, 073h, 00Bh, 09Fh, 0ADh, 026h, 086h, 072h, 05Dh, 0D2h, 01Ch, 01Ah, 0EDh
db 0BFh, 0B7h, 01Eh, 097h, 09Ch, 0B4h, 0A9h, 0E3h, 048h, 02Bh, 0FFh, 0ABh, 08Fh, 0F4h, 054h, 0C6h, 051h, 096h, 062h, 01Fh, 0E5h, 0AAh, 00Ah, 0A2h, 02Eh, 03Ah, 084h, 0FBh, 081h, 0FCh, 0F3h, 098h
db 0B5h, 0BDh, 090h, 0DAh, 0F7h, 085h, 00Dh, 0D3h, 0C5h, 007h, 0B6h, 03Ch, 017h, 0A3h, 03Dh, 08Eh, 053h, 019h, 0A7h, 011h, 0D6h, 0F8h, 052h, 06Bh, 0CAh, 0CCh, 0D9h, 0E4h, 013h, 070h, 05Ah, 0A5h


	szFormat2		db		'Npa文件(*.npa)',0,'*.npa',0,'所有文件(*.*)',0,'*.*',0,0


