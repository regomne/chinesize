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

E_LINENOTMATCH		equ		1
E_MESERROR			EQU		2
E_TXTERROR			EQU		3
E_INVALIDCHAR		EQU		4
E_LINENOTFIT			equ		5

_DlgMainProc proto :dword,:dword,:dword,:dword
_AddEnterA proto :dword,:dword,:dword,:dword
_WriteErrA proto :dword,:dword,:dword,:dword

_CorrectText proto :dword,:dword

_memcpy proto

.data?
	hInstance dd ?
	hWinMain dd ?
	
	hFileTxt dd ?
	hFileMes dd ?
	hFindFile dd ?
	
	szFileNameTxt db 1024 dup (?)
	szFileNameMes db 1024 dup (?)
	
	szDirT db 512 dup (?)
	szDirM db 512 dup (?)
	
	szInform db 64 dup (?)
	
	dwTemp dd ?
.data
	szTxtFormat db '译文(*.txt)',0,'*.txt',0,0
	szMesFormat db '脚本(*.mes)',0,'*.mes',0,0
	
	szFormatInform db '文件%s封装错误，请检查，封装过程将跳过该文件并继续',0
	
	szWritingError db '脚本写入发生错误！',0
	szWroteSuccess db '脚本写入成功！',0
	szWrotePartSuccess db '某些文件未被成功写入，请查看error.txt文件！',0
	szOpenFailed db '文件打开失败！',0
	
	szTxtDir db '请选择译文文件目录',0
	szMesDir db '请选择脚本文件目录',0
	
	szTxt db '*.txt',0
	
	szDisplayName db 'DC2PC封装器',0
