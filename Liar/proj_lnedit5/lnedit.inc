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
      include \masm32\include\advapi32.inc
      include \masm32\macros\macros.asm

      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
      includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
       includelib \masm32\lib\advapi32.lib
     includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib

	include rsrc.inc

_DlgMainProc proto :dword,:dword,:dword,:dword
_NewListProc proto :dword,:dword,:dword,:dword
_NewEditProc proto :dword,:dword,:dword,:dword

_AddToList proto :dword
_SaveOrNot proto
_StringBrief proto :dword
_StringBack proto :dword
_SetText proto :dword
_ModifyOriText proto :dword,:dword
_memcpy proto
_UpdateWindow proto :dword
_GotoDlgProc proto :dword,:dword,:dword,:dword
_OptionDlgProc proto :dword,:dword,:dword,:dword
_DisplayMsg proto :dword
_ReplaceText proto 
_ReplaceDlgProc proto :dword,:dword,:dword,:dword
_RpcText proto :dword,:dword,:dword,:dword,:dword
_IsFormatted proto :dword
_StringToUnicode proto

_Encrypt proto :dword,:dword
_ExportTxt proto :dword,:dword
_ImportTxt proto :dword

_AddHandler proto :dword,:dword,:dword,:dword
_ProcHandler proto :dword,:dword,:dword,:dword

ID_TIMER equ 1

.data?
	hInstance dd ?
	hIcon dd ?
	hWinMain dd ?
	
	hRegKey dd ?
	
	hListO dd ?
	hListN dd ?
	hEditO dd ?
	hEditN dd ?
	hStatusLine dd ?
	hBtnModify dd ?
	hBtnSave dd ?
	hBtnGoto dd ?
	hBtnFind dd ?
	hBtnImp dd ?
	hBtnExp dd ?
;	hStatusByteO dd ?
;	hStatusByteN dd ?
	hBackBmp dd ?
	
	hBrushTr dd ?
	
	hFile dd ?
	hFile2 dd ?
	
	lpOldListProc dd ?
	lpOldEditProc dd ?
	
	lpFileBuff dd ?
	lpFileBuff2 dd ?
	
	lpTemp dd ?
	lpLineRecords2 dd ?
	lpBackBmp dd ?
	lpIndex dd ?
	lpIndex2 dd ?
	lpText2 dd ?
	
	szFileName db 512 dup(?)
	szStringO db 512 dup (?)
	szStringN db 512 dup (?)
	szTemp db 512 dup (?)
	szToFind db 64 dup (?)
	szToReplace db 64 dup (?)
	szReplaced db 32 dup (?)
	
	nTotalLine dd ?
	nRealLine dd ?
	nIndex dd ?
	nFileSizeNew dd ?
	
	bIsModified dd ?
	bScrolling dd ?
	bIsOpened dd ?
	
	dwTemp dd ?
	szNewName db 128 dup (?)
.data
	nVersion dd 1103
	nDword dd 4
	szRegKey db 'Software\AmaF\ME for Black',0
	szRegValueVersion db 'Version',0
	szNotNewest db '您所使用的编辑器不是最新版本！',0

	szDisplayName db '漆黑的夏尔诺斯 脚本编辑器 v1.1',0
	szGang db ' - ',0
	szTxt1 db 'txt',0
	szError db '错误',0
	
	szCaptionFind db '替换字符串',0
	szNewSc db 'NewSc\',0
	szSaveOrNot db '文件已修改，是否保存？',0
	szFailOpen db '无法打开文件！',0
	szFormat2 db '脚本文件(*.gsc)',0,'*.gsc',0,'所有文件',0,'*.*',0,0
	szFormat3 db '文本文件(*.txt)',0,'*.txt',0,0
	szCountingLines db '正在计算总行数...',0
	szAddingString db '正在获取文本...',0
	szConvertingString db '正在转换文本...',0
	szLineOver db '超过最大行数！',0
	szSaveSuccess db '保存文件成功！',0
	szSaveTxtSuccess db '导出TXT成功！',0
	szSaveTxtFail db '导出TXT失败！',0
	szImpTxtSuccess db '导入TXT成功！',0
	szImpTxtFail db '导入TXT失败！请重新启动程序！',0
	szTextNotFound db '字符串未找到！',0
	szRpcFormat db '共完成了%d处替换',0
	szNotTxt db '脚本格式不正确，继续打开吗？',0
	szInstruction db '回车',09h,09h,'保存本行修改',0dh,0ah,
				'PageDown',09h,'下一行(不保存)',0dh,0ah,
				'PageUp',09h,09h,'上一行(不保存)',0dh,0ah,
				'Ctrl+S',09h,09h,'保存所有修改到文件',0dh,0ah,
				'Ctrl+G',09h,09h,'跳转到某行',0dh,0ah,
				'Ctrl+F',09h,09h,'替换文本',0dh,0ah,0