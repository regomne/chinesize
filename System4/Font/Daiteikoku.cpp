#pragma comment(linker,"/ENTRY:DllMain")
#pragma comment(linker,"/SECTION:.text,ERW /MERGE:.rdata=.text /MERGE:.data=.text")
#pragma comment(linker,"/SECTION:.Amano,ERW /MERGE:.text=.Amano")

#include "Daiteikoku.h"
#include "Mem.cpp"

// DWORD g_Count;
PVOID g_pfChipSpriteEngAllocMemory;
WCHAR g_szFaceName[LF_FACESIZE] = L"SIMHEI";

/************************************************************************
  font height
  30 38 40 48 ... 190
************************************************************************/

ASM VOID FASTCALL ChipSpriteEngAllocMemory(PVOID pThis, SIZE_T Size)
{
    UNREFERENCED_PARAMETER(pThis);
    UNREFERENCED_PARAMETER(Size);
    INLINE_ASM
    {
        mov  eax, ecx;
        push edx;
        call g_pfChipSpriteEngAllocMemory;
        ret;
    }
}

WCHAR MBCharToUnicode(UINT MBChar)
{
    WCHAR WideChar;
    DWORD CodePage;

    CodePage = CP_GB2312;
    MBChar &= 0xFFFF;
    if (MBChar & 0xFF00)
    {
        if (IsShiftJISChar(MBChar))
            CodePage = CP_SHIFTJIS;

        MBChar = SWAPCHAR(MBChar);
    }

    MultiByteToWideChar(
        CodePage,
        0,
        (LPSTR)&MBChar,
        2,
        (LPWSTR)&WideChar,
        sizeof(WideChar) / sizeof(WCHAR));

    return WideChar;
}

BOOL IsCharSpec(UINT uChar)
{
    if (TEST_BITS(uChar, 0xFF00) == 0)
    {

        if (IN_RANGE('0', uChar, '9'))
            return FALSE;
/*
        uChar = CHAR_UPPER(uChar);
        if (IN_RANGE('A', uChar, 'Z'))
            return FALSE;
*/
        return TRUE;
    }

    switch (uChar)
    {
        case (WCHAR)'。':  // ｡｣
        case (WCHAR)'，':  // ｣ｬ
        case (WCHAR)'、':  // ｡｢
//        case (WCHAR)'（':  //｣ｨ
//        case (WCHAR)'）':  // ｣ｩ
        case (WCHAR)'…':  // ｡ｭ
        case (WCHAR)'「':  // ｡ｸ
        case (WCHAR)'」':  // ｡ｹ
        case (WCHAR)'　':  // '｡｡'
            return TRUE;
    }

    return FALSE;
}

ASM
BOOL
FASTCALL
OldGetCharOutline(
    LONG            Unknown,
    PVOID,
    PVOID           pThis,
    ULONG           Height,
    WCHAR           uChar,
    PULONG          pBitsPerRow,
    PULONG          pWidth,
    ALICE_MEMORY   *pMem
)
{
    UNREFERENCED_PARAMETER(Unknown);
    UNREFERENCED_PARAMETER(pThis);
    UNREFERENCED_PARAMETER(Height);
    UNREFERENCED_PARAMETER(uChar);
    UNREFERENCED_PARAMETER(pBitsPerRow);
    UNREFERENCED_PARAMETER(pWidth);
    UNREFERENCED_PARAMETER(pMem);
    ASM_DUMMY_AUTO();
}

BOOL
FASTCALL
GetCharOutline(
    IN     LONG            Unknown,
           PVOID,
    IN     PVOID           pThis,
    IN     ULONG           Height,
    IN     WCHAR           uChar,
    OUT    PULONG          pBitsPerRow,
    IN OUT PULONG          pDescent,
    IN OUT ALICE_MEMORY   *pMem
)
{
    HDC   hDC;
    HFONT hFont;
    ULONG FontIndex, OutlineSize, BytesPerRow, BitsOfLeftSpace;
    GLYPHMETRICS GlyphMetrics;
    BYTE  Buffer[0x5000];
    PBYTE pbOutline, pbBuffer;
    FONT_OUTLINE_INFO *pOutlineInfo;

    static FONT_OUTLINE_INFO *s_pOutlineInfo;
    static MAT2  mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };

    pOutlineInfo = s_pOutlineInfo;
    if (s_pOutlineInfo == NULL)
    {
        HANDLE hHeap = CMem::GetGlobalHeap();
        if (hHeap == NULL)
        {
            hHeap = CMem::CreateGlobalHeap();
            if (hHeap == NULL)
                goto DEFAULT_PROC;
        }

        pOutlineInfo = (FONT_OUTLINE_INFO *)HeapAlloc(hHeap, 0, sizeof(*pOutlineInfo));
        if (pOutlineInfo == NULL)
            goto DEFAULT_PROC;

        s_pOutlineInfo = pOutlineInfo;

        pOutlineInfo->hDC = NULL;
        pOutlineInfo->LastFontIndex = -1;
        ZeroMemory(pOutlineInfo->hFont, sizeof(pOutlineInfo->hFont));
        FillMemory(pOutlineInfo->Descent, sizeof(pOutlineInfo->Descent), -1);
    }

    if (Unknown < 0 || IsCharSpec(uChar))
        goto DEFAULT_PROC;

    FontIndex = Height / FONT_STEP;
    if (FontIndex > countof(pOutlineInfo->hFont))
        goto DEFAULT_PROC;

    hDC = pOutlineInfo->hDC;
    if (hDC == NULL)
    {
        hDC = CreateCompatibleDC(NULL);
        if (hDC == NULL)
            goto DEFAULT_PROC;

        pOutlineInfo->hDC = hDC;
    }

    hFont = pOutlineInfo->hFont[FontIndex];
    if (hFont == NULL)
    {
        LOGFONTW lf;

        ZeroMemory(&lf, sizeof(lf));
        lf.lfHeight = Height;
        lf.lfWeight = FW_NORMAL;
        lf.lfCharSet = GB2312_CHARSET;
        lf.lfQuality = CLEARTYPE_QUALITY;
        lf.lfPitchAndFamily = FIXED_PITCH;
        CopyStruct(lf.lfFaceName, g_szFaceName, sizeof(g_szFaceName));

        hFont = CreateFontIndirectW(&lf);
        if (hFont == NULL)
            goto DEFAULT_PROC;

        pOutlineInfo->hFont[FontIndex] = hFont;
    }

    if (FontIndex != pOutlineInfo->LastFontIndex)
    {
        if (SelectObject(hDC, hFont) == HGDI_ERROR)
            goto DEFAULT_PROC;

        pOutlineInfo->LastFontIndex = FontIndex;
    }

    if (pDescent != NULL)
    {
        TEXTMETRICW tm;

        tm.tmDescent = pOutlineInfo->Descent[FontIndex];
        if (tm.tmDescent == -1)
        {
            if (GetTextMetricsW(hDC, &tm))
                pOutlineInfo->Descent[FontIndex] = tm.tmDescent;
            else
                ++tm.tmDescent;
        }

        *pDescent = tm.tmDescent;
        if (pBitsPerRow == NULL)
            return TRUE;
    }

    uChar = MBCharToUnicode(uChar);
    OutlineSize = GetGlyphOutlineW(
                    hDC,
                    uChar,
                    GGO_BITMAP,
                    &GlyphMetrics,
                    sizeof(Buffer),
                    pThis == NULL ? NULL : Buffer,
                    &mat);
    if (OutlineSize == GDI_ERROR)
        goto DEFAULT_PROC;

#if 0
    ULONG DwordOfLeftSpace, BytesPerRowRaw, Mask;
    static BYTE Bits[FONT_COUNT] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x16,
        0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
        0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x2B, 0x2C
    };

    BitsOfLeftSpace = Bits[FontIndex];
    BitsOfLeftSpace = 4;
    DwordOfLeftSpace = BitsOfLeftSpace / 8 / 4;
    if (pBitsPerRow != NULL)
        *pBitsPerRow = GlyphMetrics.gmBlackBoxX + BitsOfLeftSpace % bitsof(DWORD) + DwordOfLeftSpace * 4 * 8;

    if (pDescent != NULL)
    {
        TEXTMETRICW tm;

        tm.tmDescent = s_Descent[FontIndex];
        if (tm.tmDescent == -1)
        {
            if (GetTextMetricsW(hDC, &tm))
                s_Descent[FontIndex] = tm.tmDescent;
            else
                ++tm.tmDescent;
        }

        *pDescent = tm.tmDescent;
    }

    if (pThis == NULL)
        return TRUE;

    BytesPerRow = GlyphMetrics.gmBlackBoxX + BitsOfLeftSpace;
    BytesPerRow = ROUND_UP(BytesPerRow, bitsof(DWORD)) / 8;
    BitsOfLeftSpace %= bitsof(DWORD);

    ChipSpriteEngAllocMemory(pMem, BytesPerRow * Height);
    if (pMem->pvBufferEnd == pMem->pvBuffer)
        goto DEFAULT_PROC;

    ZeroMemory(pMem->pvBuffer, BytesPerRow * Height);

    BytesPerRowRaw = ROUND_UP(GlyphMetrics.gmBlackBoxX, bitsof(DWORD)) / 8;

    pbBuffer   = Buffer;
    pbOutline  = (PBYTE)pMem->pvBuffer;
    pbOutline += (GlyphMetrics.gmBlackBoxY - 1) * BytesPerRow;
    pbOutline += ((Height - GlyphMetrics.gmBlackBoxY) / 2 - 1) * BytesPerRow;
    pbOutline += DwordOfLeftSpace * 4;
    Mask = _rotl(1, BitsOfLeftSpace) - 1;

    for (ULONG i = GlyphMetrics.gmBlackBoxY; i; --i)
    {
        PBYTE pbOutline2, pbBuffer2;
        DWORD BitsHigh, BitsLow;

        BitsHigh = 0;
        BitsLow = 0;
        pbBuffer2 = pbBuffer;
        pbOutline2 = pbOutline;
        for (ULONG Count = BytesPerRowRaw / 4; Count; --Count)
        {
            DWORD v = *(PDWORD)pbBuffer2;

            BitsHigh = _rotl(v, BitsOfLeftSpace) & Mask;
            v = (v << BitsOfLeftSpace) | BitsLow;
            BitsLow = BitsHigh;
            *(PDWORD)pbOutline2 = v;
            pbOutline2 += 4;
            pbBuffer2  += 4;
        }
//        *(PDWORD)pbOutline2 = BitsLow;

        pbOutline -= BytesPerRow;
        pbBuffer  += BytesPerRowRaw;
    }

    WCHAR buf[0x500];
    wsprintfW(
        buf - 1 + GetTextFaceW(hDC, countof(buf),
        buf),
        L" Char = %c Index = %02u Bits = %02X",
        uChar,
        FontIndex + 1,
        BitsOfLeftSpace);
    SetWindowTextW(GetActiveWindow(), buf);

#else
    ULONG BytesPerRowRaw;
    static BYTE Bits[FONT_COUNT] = 
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x16,
        0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
        0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x2B, 0x2C
    };

    BitsOfLeftSpace = Bits[FontIndex];

    if (pBitsPerRow != NULL)
        *pBitsPerRow = GlyphMetrics.gmBlackBoxX + BitsOfLeftSpace;


    if (pThis == NULL)
        return TRUE;

    BytesPerRow = GlyphMetrics.gmBlackBoxX + BitsOfLeftSpace;
    BytesPerRow = ROUND_UP(BytesPerRow, bitsof(DWORD)) / 8;

    ChipSpriteEngAllocMemory(pMem, BytesPerRow * Height);
    if (pMem->pvBufferEnd == pMem->pvBuffer)
        goto DEFAULT_PROC;

    ZeroMemory(pMem->pvBuffer, BytesPerRow * Height);

    BytesPerRowRaw = ROUND_UP(GlyphMetrics.gmBlackBoxX, bitsof(DWORD)) / 8;

    pbBuffer   = Buffer;
    pbOutline  = (PBYTE)pMem->pvBuffer;
    pbOutline += (GlyphMetrics.gmBlackBoxY - 1) * BytesPerRow;
    pbOutline += ((Height - GlyphMetrics.gmBlackBoxY) / 2 - 1) * BytesPerRow;

    for (ULONG i = GlyphMetrics.gmBlackBoxY; i; --i)
    {
        __movsd(pbOutline, pbBuffer, BytesPerRowRaw / 4);
        pbOutline -= BytesPerRow;
        pbBuffer  += BytesPerRowRaw;
    }
#endif

    return TRUE;

DEFAULT_PROC:
    if (pThis == NULL)
        return FALSE;

    return OldGetCharOutline(Unknown, 0, pThis, Height, uChar, pBitsPerRow, pDescent, pMem);
}

ASM bool STDCALL OldGetCharBitsOfRow(ULONG Height, WCHAR uChar, PVOID pThis)
{
    UNREFERENCED_PARAMETER(Height);
    UNREFERENCED_PARAMETER(uChar);
    UNREFERENCED_PARAMETER(pThis);
    ASM_DUMMY_AUTO();
}

BOOL STDCALL GetCharBitsPerRow(ULONG Height, WCHAR uChar, PULONG pBitsPerRow)
{
    LONG Unknown;
    PVOID pFontInfo;

    INLINE_ASM
    {
        mov Unknown, esi;
        mov pFontInfo, eax;
    }

    if (Unknown < 0 || IsCharSpec(uChar))
        goto CALL_OLD_PROC;

    if (GetCharOutline(Unknown, 0, NULL, Height, uChar, pBitsPerRow, NULL, NULL))
        return TRUE;

CALL_OLD_PROC:
    INLINE_ASM
    {
        movzx eax, uChar;
        push  pBitsPerRow;
        push  eax;
        push  Height;
        mov   eax, pFontInfo;
        mov   esi, Unknown;
        call  OldGetCharBitsOfRow;
        and   eax, 1;
    }
}

ASM BOOL STDCALL OldGetCharDescent(ULONG Height, PULONG pDescent)
{
    UNREFERENCED_PARAMETER(Height);
    UNREFERENCED_PARAMETER(pDescent);
    ASM_DUMMY_AUTO();
}

BOOL STDCALL GetCharDescent(ULONG Height, PULONG pDescent)
{
    PVOID pFontInfo;
    LONG  Unknown;

    INLINE_ASM
    {
        mov  Unknown, esi;
        mov  pFontInfo, edi;
    }

    if (GetCharOutline(Unknown, 0, NULL, Height, 0, NULL, pDescent, NULL))
        return TRUE;

    INLINE_ASM
    {
        push pDescent;
        push Height;
        mov  esi, Unknown;
        mov  edi, pFontInfo;
        call OldGetCharDescent;
        and  eax, 1;
    }
}

BOOL FASTCALL GetIniString(ALICE_INI_INFO *pIniInfo, PVOID, STL_STRINGA *pString)
{
    BOOL Result;
    LONG Type;
    WCHAR szPath[MAX_PATH];
    bool (FASTCALL *pfGetIniString)(PVOID pIniInfo, PVOID, STL_STRINGA *pString);
    enum { INI_TYPE_FACE, INI_TYPE_FONT };

    static CHAR szFaceName[] = "FaceName";
    static CHAR szFontName[] = "FontName";

    *(PVOID *)&pfGetIniString = (PVOID)0x42CA40;

    LOOP_ALWAYS
    {
        Result = pfGetIniString(pIniInfo, 0, pString);
        if (!Result)
            return Result;

        if (pString->Length == sizeof(szFaceName) - 1 &&
            !StrICompareA(pString->GetBuffer(), szFaceName))
        {
            Type = INI_TYPE_FACE;
        }
        else if (pString->Length == sizeof(szFontName) - 1 &&
            !StrICompareA(pString->GetBuffer(), szFontName))
        {
            Type = INI_TYPE_FONT;
        }
        else
        {
            return TRUE;
        }

        LOOP_ONCE
        {
            if (!pfGetIniString(pIniInfo, 0, pString) || *pString->GetBuffer() != '=')
                break;

            if (!pfGetIniString(pIniInfo, 0, pString) || pString->Length == 0)
                break;

            switch (Type)
            {
                case INI_TYPE_FACE:
                    MultiByteToWideChar(
                        CP_ACP,
                        0,
                        pString->GetBuffer(),
                        -1,
                        g_szFaceName,
                        countof(g_szFaceName));
                    break;

                case INI_TYPE_FONT:
                    MultiByteToWideChar(
                        CP_ACP,
                        0,
                        pString->GetBuffer(),
                        -1,
                        szPath,
                        countof(szPath));

                    if (AddFontResourceExW(szPath, FR_PRIVATE, NULL) == 0)
                        break;

                    GetFontNameFromFile(szPath, g_szFaceName, countof(g_szFaceName));
                    break;
            }
        }
    }
}

BOOL
WINAPI
MyReadIni(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
)
{
    ULONG Length;

    if (!ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped))
        return FALSE;

    if (*lpNumberOfBytesRead < 2 || *(PUSHORT)lpBuffer != BOM_UTF16_LE)
        return TRUE;

    Length = WideCharToMultiByte(
                CP_ACP,
                0,
                (LPWSTR)lpBuffer + 1,
                *lpNumberOfBytesRead / sizeof(WCHAR) - 1,
                (LPSTR)lpBuffer,
                nNumberOfBytesToRead - sizeof(WCHAR),
                NULL,
                NULL);

    ZeroMemory((PBYTE)lpBuffer + Length, nNumberOfBytesToRead - Length);

    return TRUE;
}

HRESULT WINAPI MySHGetFolderPathA(HWND hWndOwner, INT nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath)
{
    switch (nFolder & ~CSIDL_FLAG_MASK)
    {
        case CSIDL_PERSONAL:
        case CSIDL_APPDATA:
            break;

        default:
            return SHGetFolderPathA(hWndOwner, nFolder, hToken, dwFlags, pszPath);
    }

    return !GetExecuteDirectoryA(pszPath, MAX_PATH);
}

ASM bool FASTCALL OldLoadPlugins(PVOID pECX, PVOID pEDX, PVOID pEcxInStack)
{
    UNREFERENCED_PARAMETER(pECX);
    UNREFERENCED_PARAMETER(pEDX);
    UNREFERENCED_PARAMETER(pEcxInStack);

    ASM_DUMMY_AUTO();
}

BOOL FASTCALL LoadPlugins(PVOID Param1, PVOID Param2, PVOID Param3)
{
    BOOL     Result;
    HMODULE  hModule;

    // ensure edi is not modified
    Result = OldLoadPlugins(Param1, Param2, Param3);
    if (!Result)
        return Result;

    hModule = GetModuleHandleW(L"DLL\\Sys43VM.dll");
    if (hModule != NULL)
        FreeLibrary(hModule);

    hModule = LoadLibraryExW(L"DLL\\Sys43VM.dll", NULL, 0);
    if (hModule != NULL)
    {
        INTEL_STATIC MEMORY_PATCH p[] =
        {
            { 0xEB, 1, 0x064EF },
            { 0xEB, 1, 0x1C74E },
            { 0xEB, 1, 0x1F35B },
            { 0xEB, 1, 0x223B7 },
            { 0xEB, 1, 0x229C4 },
            { 0xEB, 1, 0x31DA8 },
            { 0xEB, 1, 0x35C72 },
            { 0xEB, 1, 0x35E09 },
            { 0xEB, 1, 0x36491 },
            { 0xEB, 1, 0x365D9 },
            { 0xEB, 1, 0x36A81 },
            { 0xEB, 1, 0x36BB9 },
            { 0xEB, 1, 0x36F5F },
        };

        PatchMemory(p, countof(p), 0, 0, hModule);
    }

    hModule = GetModuleHandleW(L"ChipmunkSpriteEngine.dll");
    if (hModule != NULL)
    {
        INTEL_STATIC MEMORY_PATCH p[] =
        {
            { 0xEB, 1, 0x02899 },
            { 0xEB, 1, 0xA3ED7 },
            { 0xEB, 1, 0xA81E6 },
        };

        INTEL_STATIC MEMORY_FUNCTION_PATCH f[] =
        {
            { JUMP, 0xEE00, GetCharOutline,     2, OldGetCharOutline },
            { JUMP, 0xF050, GetCharBitsPerRow,  0, OldGetCharBitsOfRow },
            { JUMP, 0xEFE4, GetCharDescent,     1, OldGetCharDescent },
        };

        PatchMemory(p, countof(p), f, countof(f), hModule);
        g_pfChipSpriteEngAllocMemory = (PVOID)(pfChipSpriteEngAllocMemory_RVA + (ULONG_PTR)hModule);
    }

    hModule = GetModuleHandleW(L"CrayfishLogViewer.dll");
    if (hModule != NULL)
    {
        INTEL_STATIC MEMORY_PATCH p[] =
        {
            { 0xEB, 1, 0x02833 },
            { 0xEB, 1, 0x04F3A },
        };
        PatchMemory(p, countof(p), 0, 0, hModule);
    }

    hModule = GetModuleHandleW(L"FileOperation.dll");
    if (hModule != NULL)
    {
        INTEL_STATIC MEMORY_PATCH p[] =
        {
            { 0xEB, 1, 0x0DFC4 },
            { 0xEB, 1, 0x0E199 },
        };
        PatchMemory(p, countof(p), 0, 0, hModule);
    }

    hModule = GetModuleHandleW(L"GoatGUIEngine.dll");
    if (hModule != NULL)
    {
        INTEL_STATIC MEMORY_PATCH p[] =
        {
            { 0xEB, 1, 0x1BBE8 },
            { 0xEB, 1, 0x1D5D0 },
            { 0xEB, 1, 0x1E98C },
            { 0xEB, 1, 0x1E9DF },
            { 0xEB, 1, 0x1EA18 },
            { 0xEB, 1, 0x1EB0C },
            { 0xEB, 1, 0x1EB71 },
            { 0xEB, 1, 0x20DCE },
        };
        PatchMemory(p, countof(p), 0, 0, hModule);
    }

    hModule = GetModuleHandleW(L"SystemService.dll");
    if (hModule != NULL)
    {
        INTEL_STATIC MEMORY_PATCH p[] =
        {
            { 0xEB, 1, 0x01F54 },
            { 0xEB, 1, 0x02037 },
        };
        PatchMemory(p, countof(p), 0, 0, hModule);
    }

    return TRUE;
}

ASM BOOL WINAPI OldPeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    UNREFERENCED_PARAMETER(lpMsg);
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wMsgFilterMin);
    UNREFERENCED_PARAMETER(wMsgFilterMax);
    UNREFERENCED_PARAMETER(wRemoveMsg);
    ASM_DUMMY_AUTO();
}
#if 0
BOOL WINAPI MyPeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    BOOL Result = OldPeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    if (Result == FALSE)
    {
        DWORD TickCount;
        static DWORD LastTickCount;

        TickCount = GetTickCount();
        if (TickCount - LastTickCount < 10 && --g_Count == 0)
        {
            g_Count = 4;
            Sleep(1);
        }
        LastTickCount = TickCount;
    }

    return Result;
}
#endif
VOID Init()
{
    HMODULE hModule;
    static CHAR szConfig[] = "AliceStart_sc.ini";

    hModule = GetModuleHandleW(NULL);

    INTEL_STATIC MEMORY_PATCH p[] =
    {
        { 0xEB, 1, 0x25A4A },
        { 0xEB, 1, 0x275A6 },
        { 0xEB, 1, 0x27684 },
        { 0xEB, 1, 0x2CAAF },
        { 0xEB, 1, 0x2CBFB },
        { 0xEB, 1, 0x2CC4F },
        { 0xEB, 1, 0x2DB6C },
        { 0xEB, 1, 0x2DDD3 },
        { 0xEB, 1, 0x2E9CA },
        { 0xEB, 1, 0x2EB17 },
        { 0xEB, 1, 0x2EC0E },
        { 0xEB, 1, 0x2ECFE },
        { 0xEB, 1, 0x2EDF4 },
        { 0xEB, 1, 0x2EED7 },
        { 0xEB, 1, 0x2F066 },

        // config path
        { STRTOULONG(szConfig), 4, 0x235D9 },
        { sizeof(szConfig) - 1, 1, 0x235D7 },
    };

    INTEL_STATIC MEMORY_FUNCTION_PATCH f[] =
    {
        { CALL, 0x1DCA9, MyReadIni,   1 },
        { CALL, 0x26185, GetIniString,   0 },
        { JUMP, 0x2D440, LoadPlugins, 1, OldLoadPlugins },
//        { JUMP, (ULONG_PTR)PeekMessageA - (ULONG_PTR)hModule, MyPeekMessageA, 0, OldPeekMessageA },
    };

    PatchMemory(p, countof(p), f, countof(f), hModule);

//    g_Count = 4;

    SetExecuteDirectoryAsCurrentW();
}

BOOL WINAPI DllMain(HINSTANCE hInstance, ULONG Reason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (Reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hInstance);
            Init();
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}