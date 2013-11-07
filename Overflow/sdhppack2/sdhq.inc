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
;// gpk pidx format:
;typedef struct {
;	unsigned short unilen;			// unicode string length
;	unsigned short *unistring;		// unicode string exclude NULL
;	unsigned short sub_version;		// same as script.gpk.* suffix
;	unsigned short version;			// major version(always 1) 
;	unsigned short zero;			// always 0
;	unsigned int offset;			// pidx data file offset
;	unsigned int comprlen;			// compressed pidx data length
;	unsigned char dflt[4];			// magic "DFLT" or "    "
;	unsigned int uncomprlen;		// raw pidx data length(if magic isn't DFLT, then this filed always zero)
;	unsigned char comprheadlen;		// pidx data header length
;	unsigned char *comprheaddata;	// pidx data
;} 
;GPK_pidx_t;

_GpkTail struc
	szTailer2		db		12 dup (?)
	nIndexSize	dd		?
	szTailer1		db		16 dup (?)
_GpkTail ends

_WinMain proto
_DlgMainProc proto :dword,:dword,:dword,:dword
_GpkUnpack proto :dword
_GpkPack proto :dword,:dword
_MakeFile proto :dword

.data?
	hInstance dd ?
	hWinMain dd ?
	hIcon dd ?
	hHeap			dd		?
	
	dwTemp			dd		?
	
	szPackName		db		1024 dup (?)
	szDirName		db		1024 dup (?)
	
	bCompress		dd		?
	
	fCompress		dd		?
	fUncompress		dd		?
	
.const
	
	dbKey			db		82h, 0EEh, 1Dh, 0B3h, 57h, 0E9h, 2Ch, 0C2h, 2Fh, 54h, 7Bh, 10h, 4Ch, 9Ah, 75h, 49h

	szFormat2		db		'GPK文件(*.gpk)',0,'*.gpk',0,'所有文件(*.*)',0,'*.*',0,0
	szTailer2			db		"STKFile0PIDX"
	szTailer1			db		'STKFile0PACKFILE'

