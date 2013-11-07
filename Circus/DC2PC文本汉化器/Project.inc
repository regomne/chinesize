; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤?

;     include files
;     ~~~~~~~~~~~~~
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

;     libraries
;     ~~~~~~~~~
      includelib \masm32\lib\masm32.lib
      includelib \masm32\lib\gdi32.lib
      includelib \masm32\lib\user32.lib
      includelib \masm32\lib\kernel32.lib
      includelib \masm32\lib\Comctl32.lib
      includelib \masm32\lib\comdlg32.lib
      includelib \masm32\lib\shell32.lib
      includelib \masm32\lib\oleaut32.lib
      includelib \masm32\lib\msvcrt.lib

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤?

      DisplayMenu MACRO handl, IDnum
        invoke LoadMenu,hInstance,IDnum
        invoke SetMenu,handl,eax
      ENDM

      DisplayWindow MACRO handl, ShowStyle
        invoke ShowWindow,handl, ShowStyle
        invoke UpdateWindow,handl
      ENDM

        ;=================
        ; Local prototypes
        ;=================
        WinMain          PROTO :DWORD,:DWORD,:DWORD,:DWORD
        WndProc          PROTO :DWORD,:DWORD,:DWORD,:DWORD
        Paint_Proc       PROTO :DWORD
        RegisterWinClass PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
        MsgLoop          PROTO
        Main             PROTO
        Extract PROTO 
        Extractm proto
        StringCopy PROTO
        ReadCount proto
        ReadCountm proto
        CleanBuff proto
        Edit2Proc proto :dword,:dword,:dword,:dword
        MyWriteFile proto
        CreateConf proto
        MyGetPath proto
        MySetPath proto
        
    Conf struct 1
		head db 4 dup (?)
		workpath db 256 dup (?)
		prevfn db 256 dup (?)
		tempfn db 256 dup (?)
		ncnt db 4 dup (?)
		
	Conf ends


    .data
        szDisplayName db "³õÒôµº2 P.C. ÎÄ±¾ºº»¯Æ÷ v1.0 by AmaF",0
        szFailRead db "ÎÞ·¨´ò¿ªÎÄ¼þ£¡",0
        szJpfont db "MS Gothic",0
        szChnfont db "ËÎÌå",0
        szEditObject db "edit",0
        szStaticObject db "static",0
        szButtonObject db "button",0
        szPrev db "ÉÏÒ»Ìõ",0
        szNext db "ÏÂÒ»Ìõ",0
        szSave db "±£´æµ½ÄÚ´æ",0
        szFileType db "*.mes",0
        szPrevError db "ÒÑ¾­ÊÇµÚÒ»ÌõÁË£¡",0
        szNextError db "ÒÑ¾­ÊÇ×îºóÒ»ÌõÁË£¡",0
        szZero dw 0
        szInput db "ÇëÊäÈëºº»¯ÎÄ±¾£¡",0
        szInputName db "ÇëÊäÈëÈËÃû£¡",0
 ;       szTempFileName db "working.mes",0
        szConfFileName db "dc2pcm.cfg",0
        szNameTable db "dcnames.dat",0
        szFailWrite db "Ð´ÈëÎÄ¼þÊ§°Ü£¡",0
        szWarning db 'Äã¶Ô±¾ÎÄ¼þËù×öµÄËùÓÐ¸ü¸Ä½«¶ªÊ§£¡Òª¼ÌÐøÂð£¿',0
        szListbox db "listbox",0
        szCount db "%d",0
        szStr1 db "µÚ",0
        szStr2 db "ÌõÎÄ±¾",0
        szGoto db "×ªµ½",0
        szStr3 db "¹²ÓÐ%3dÌõÎÄ±¾",0
        rectDigit dd 280
						dd 350
						dd 390
						dd 390

    .data?
        hInstance dd ?
        CommandLine dd ?
        hIcon dd ?
        hCursor dd ?
        sWid dd ?
        sHgt dd ?
        hWnd dd ?
        hfChn dd ?
        hfJp dd ?
        lpszStr3 db 16 dup (?)
        hfSys dd ?
        bNameMatched dd ?
        hwndEdit1 dd ?
        hwndEdit2 dd ?
        hwndName1 dd ?
        hwndName2 dd ?
        hwndButtonPrev dd ?
        hwndButtonNext dd ?
        hwndButtonSave dd ?
        hwndListbox dd ?
        hwndCount dd ?
        hwndButtonGoto dd ?
        hwndNameFile dd ?
        fname dd ?
        hFile dd ?
 ;       hTempFile dd ?
        hConfFile dd ?
        pBytesRead dd ?
        fnbuffer db 32 dup (?)
        lpszCount dd ?
        nTotal dd ?
        dtBuff1 db 4096 dup (?)
        dtBuff2 db 100000 dup (?)
        nCount dd ?
        nOffset dd ?
        nOffsetm dd ?
        nLength1 dd ?
        nLength2 dd ?
        
        szTarget db 512 dup (?)
        szUncrypted db 512 dup (?)
        szNewText db 512 dup (?)
        szNewCryptText db 512 dup (?)
        szNewName db 32 dup (?)
        szTargetName db 32 dup (?)
        dtBuff1m db 4096 dup (?)
        dtBuff2m db 100000 dup (?)
        dtBuff2mm db 100000 dup (?)
        szPath db 256 dup (?)
        OldEdit2Proc dd ?
        names db 2048 dup (?)
        bNameModified dw ?
        bFileModified dw ?
		dtConfBuff Conf <>
