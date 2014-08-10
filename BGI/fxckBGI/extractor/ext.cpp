#include <Windows.h>
#include <Commctrl.h>
#include "resource.h"
#include "..\shell\fxckBGI.h"
#include "extractor.h"
#include "misc.h"
#include <vector>
#include <string>

using namespace std;

#define FIELD_NAME 1
#define FIELD_OFFSET1 2
#define FIELD_SIZE 3

BOOL CallDecompressFunction(LPVOID dest, DWORD* dstSize, LPVOID src, DWORD srcSize)
{
    DWORD rslt;
    if (g_DecompType == DECOMPTYPE_CDECL)
    {
        rslt = g_DecompressFile(dest, dstSize, src, srcSize, 0, 0);
    }
    else if (g_DecompType == DECOMPTYPE_FASTCALL)
    {
        __asm
        {
            push 0;
            push 0;
            push srcSize;
            push dstSize;
            push dest;
            mov ecx, src;
            call g_DecompressFile;
            add esp,14h
            mov rslt, eax;
        }

    }
    return rslt;
}

DWORD GetEntryItem(void* ent, DWORD version, DWORD item)
{
    DWORD val;
    if (version == 1)
    {
        auto entry = (BGI_ARC_ENTRY*)ent;
        switch (item)
        {
        case FIELD_NAME:
            val = (DWORD)entry->FileName;
            break;
        case FIELD_OFFSET1:
            val = entry->Offset;
            break;
        case FIELD_SIZE:
            val = entry->Size;
            break;
        }
    }
    else if (version == 2)
    {
        auto entry = (BGI_ARC_ENTRY2*)ent;
        switch (item)
        {
        case FIELD_NAME:
            val = (DWORD)entry->FileName;
            break;
        case FIELD_OFFSET1:
            val = entry->Offset;
            break;
        case FIELD_SIZE:
            val = entry->Size;
            break;
        }
    }
    return val;
}


DWORD WINAPI ExtArc(const wstring& fname)
{

    HANDLE hf = CreateFile(fname.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hf == INVALID_HANDLE_VALUE)
    {
        throw wstring(L"无法打开文件");
    }

    BGI_ARC_HEADER hdr;
    BYTE* ent;
    DWORD temp;
    DWORD arcVersion;
    wchar_t logString[128];
    BYTE plte[0x400];
    auto p = (BYTE*)plte;
    for (int i = 0; i < 0x100; i++)
    {
        p[0] = p[1] = p[2] = i;
        p[3] = 0;
        p += 4;
    }

    ReadFile(hf, &hdr, sizeof(hdr), &temp, 0);
    if (!memcmp(hdr.Magic, "PackFile    ", 0xC))
    {
        arcVersion = 1;
    }
    else if (!memcmp(hdr.Magic, "BURIKO ARC20", 0xC))
    {
        arcVersion = 2;
    }
    else
    {
        CloseHandle(hf);
        throw wstring(L"无法识别的文件格式");
    }

    wstring foldername;
    if (g_extPath == L"")
        foldername = L"";
    else
        foldername = g_extPath + L'\\';
    int l, r;
    l = fname.rfind(L'\\');
    r = fname.rfind(L'.');
    if (r == -1 || l >= r)
        r = fname.length();
    foldername += fname.substr(l + 1, r - l - 1) + L"\\";
    HANDLE ret = MakeFile(foldername.c_str(), TRUE);
    if (ret == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hf);
        throw wstring(L"无法创建解包目录");
    }

    DWORD entryLen;
    if (arcVersion == 1)
        entryLen = sizeof(BGI_ARC_ENTRY);
    else if (arcVersion == 2)
        entryLen = sizeof(BGI_ARC_ENTRY2);
    BYTE* lpIdx = new BYTE[hdr.FileCount*entryLen];
    ReadFile(hf, lpIdx, hdr.FileCount*entryLen, &temp, 0);

    ent = lpIdx;
    int startPos = hdr.FileCount*entryLen + sizeof(hdr);

    swprintf(logString, 128, L"共包含%d个文件", hdr.FileCount);
    Log(logString);

    SendDlgItemMessage(g_hwndMain, IDC_PROGRESS, PBM_SETRANGE32, 0, hdr.FileCount);

    for (int i = 0; (ULONG)i < hdr.FileCount; i++, ent += entryLen)
    {
        WaitForSingleObject(g_paused, INFINITE);
        if (WaitForSingleObject(g_needStop, 0) == 0)
        {
            delete[] lpIdx;
            CloseHandle(hf);
            return 1;
        }
        SendDlgItemMessage(g_hwndMain, IDC_PROGRESS, PBM_SETPOS, i, 0);

        SetFilePointer(hf, GetEntryItem(ent, arcVersion, FIELD_OFFSET1) + startPos, 0, FILE_BEGIN);
        DWORD itemSize = GetEntryItem(ent, arcVersion, FIELD_SIZE);
        BYTE* lpOri = new BYTE[itemSize];
        BYTE* lpNew = 0;
        DWORD newSize;
        wchar_t* name = ToWCS((char*)GetEntryItem(ent, arcVersion, FIELD_NAME), 932);

        if (!name)
        {
            Log(L"文件名转换失败");
            delete[] lpOri;
            continue;
        }
        else
        {
            Log(foldername + name + L" 解压中");
        }
        ReadFile(hf, lpOri, itemSize, &temp, 0);

        BOOL isPic = FALSE;
        int oldBpp;

        if (!memcmp(lpOri, "CompressedBG___", 0x10))
        {
            isPic = TRUE;
            auto bgHdr = (BGI_COMPRESSED_BG_HEADER*)lpOri;
            oldBpp = bgHdr->BitsPerPixel;

            int dibSize = ((bgHdr->Width*(bgHdr->BitsPerPixel / 8) + 7)&~7)*bgHdr->Height;
            int allocSize = bgHdr->Width*bgHdr->Height * 4;
            lpNew = new BYTE[allocSize + sizeof(BGI_GRP_HEADER)];
            int rslt;
            try
            {
                rslt = CallDecompressFunction(lpNew, &newSize, lpOri, itemSize);
            }
            catch (...)
            {
                delete[] lpNew;
                delete[] lpOri;
                Log(L"----调用解压函数失败");
                continue;
            }
            if (rslt != 0)
            {
                delete[] lpNew;
                delete[] lpOri;
                Log(L"----解压函数返回错误");
                continue;
            }

        }
        else if (!memcmp(lpOri, "DSC FORMAT 1.00", 0x10))
        {
            auto hdr = (BGI_DSC_FORMAT_HEADER*)lpOri;
            lpNew = new BYTE[hdr->DecompressSize];
            DWORD rslt;

            try
            {
                rslt = CallDecompressFunction(lpNew, &newSize, lpOri, itemSize);
            }
            catch (...)
            {
                Log(L"----调用解压函数失败");
                delete[] lpNew;
                delete[] lpOri;
                continue;
            }
        }
        else if (!memcmp(lpOri, "SDC FORMAT 1.00", 0x10))
        {
            auto hdr = (BGI_SDC_FORMAT_HEADER*)lpOri;
            lpNew = new BYTE[hdr->DecompressedSize];
            DWORD rslt;

            try
            {
                rslt = CallDecompressFunction(lpNew, &newSize, lpOri, itemSize);
            }
            catch (...)
            {
                Log(L"----调用解压函数失败");
                delete[] lpNew;
                delete[] lpOri;
                continue;
            }
        }

        else
        {
            DWORD rslt;
            lpNew = new BYTE[itemSize];
            try
            {
                rslt = CallDecompressFunction(lpNew, &newSize, lpOri, itemSize);
            }
            catch (...)
            {
                Log(L"----调用解压函数失败");
                delete[] lpNew;
                delete[] lpOri;
                continue;
            }
        }

        wchar_t* extName = L"";

        auto grpHdr = (BGI_GRP_HEADER*)lpNew;
        auto sndHdr = (BGI_SOUND_HEADER*)lpNew;
        //if((grpHdr->BitsPerPixel==8) && grpHdr->Width%4!=0)
        //	__asm int 3
        if (isPic || ((grpHdr->BitsPerPixel == 8 || grpHdr->BitsPerPixel == 24 || grpHdr->BitsPerPixel == 32) &&
            grpHdr->Width*grpHdr->Height*(grpHdr->BitsPerPixel / 8) + sizeof(BGI_GRP_HEADER) == newSize))
        {
            auto tempBuff = lpNew;
            lpNew = new BYTE[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+newSize + 0x400];
            BYTE* dibData;
            if (grpHdr->BitsPerPixel == 8)
            {
                dibData = lpNew + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+0x400;
                memcpy(dibData, grpHdr + 1, newSize - sizeof(BGI_GRP_HEADER));
            }
            else if (grpHdr->BitsPerPixel == 24)
            {
                dibData = lpNew + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
                Copy24To24(dibData, (BYTE*)(grpHdr + 1), grpHdr->Width, grpHdr->Height);
            }
            else if (grpHdr->BitsPerPixel == 32 && isPic && oldBpp == 24)
            {
                dibData = lpNew + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
                Copy32To24(dibData, (BYTE*)(grpHdr + 1), grpHdr->Width, grpHdr->Height);
                grpHdr->BitsPerPixel = 24;
            }
            else
            {
                dibData = lpNew + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
                memcpy(dibData, grpHdr + 1, newSize - sizeof(BGI_GRP_HEADER));
            }

            if (IsDlgButtonChecked(g_hwndMain, IDC_ENCODE))
            {
                if (SendDlgItemMessage(g_hwndMain, IDC_PICFORMAT, CB_GETCURSEL, 0, 0) == PIC_TYPE_PNG)
                {
                    MemStream strm;
                    strm.start = strm.cur = new BYTE[newSize + 0x800];
                    strm.len = newSize;
                    if (EncodeBmpToPng(grpHdr->Width, -grpHdr->Height, grpHdr->BitsPerPixel, plte,
                        dibData, &strm))
                    {
                        delete[] tempBuff;
                        delete[] lpNew;
                        delete[] lpOri;
                        delete[] strm.start;
                        continue;
                    }
                    delete[] lpNew;
                    lpNew = strm.start;
                    newSize = strm.cur - strm.start;
                    extName = L".png";
                }
            }
            else
            {
                if (grpHdr->BitsPerPixel == 32 && IsDlgButtonChecked(g_hwndMain, IDC_ALPHABLEND))
                {
                    AlphaBlend1(lpNew + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),
                        grpHdr->Width, grpHdr->Height);
                }
                newSize = FillBmpHdr(lpNew, grpHdr->Width, grpHdr->Height, grpHdr->BitsPerPixel);
                extName = L".bmp";
            }
            delete[] tempBuff;
        }
        else if (sndHdr->Magic == '  wb')
        {
            auto tempBuff = lpNew;
            lpNew = new BYTE[sndHdr->AudioSteamSize];
            memcpy(lpNew, sndHdr + 1, sndHdr->AudioSteamSize);
            newSize = sndHdr->AudioSteamSize;
            if (*(DWORD*)lpNew == 'SggO')
                extName = L".ogg";
            delete[] tempBuff;
        }
        else if (*(WORD*)lpNew == 'MB')
        {
            extName = L".bmp";
        }
        HANDLE newf = CreateFile((foldername + name + extName).c_str(), GENERIC_WRITE, FILE_SHARE_READ,
            0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (newf == INVALID_HANDLE_VALUE)
        {
            Log(L"----创建文件失败");

        }
        else
        {
            DWORD temp;

            WriteFile(newf, lpNew, newSize, &temp, 0);
            CloseHandle(newf);
        }

        delete[] lpNew;
        delete[] lpOri;

    }


    CloseHandle(hf);

    delete[] lpIdx;

    return 0;
}


DWORD WINAPI ExtractThread(LPVOID param)
{
    auto arcList = (vector<wstring>*)param;
    wchar_t logString[128];
    BOOL breaked = FALSE;

    int len = GetWindowTextLength(GetDlgItem(g_hwndMain, IDC_PATHTOSAVE));
    wchar_t* tbuff = new wchar_t[len + 1];
    GetDlgItemText(g_hwndMain, IDC_PATHTOSAVE, tbuff, len + 1);
    g_extPath = tbuff;

    SendDlgItemMessage(g_hwndMain, IDC_LOGLIST, LB_RESETCONTENT, 0, 0);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_PAUSE), TRUE);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_STOP), TRUE);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_EXPORTLOG), FALSE);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_SELECTARC), FALSE);

    //劳资受不了了，还是用中文吧，尼妹
    Log(L"开始解包...");
    swprintf(logString, 128, L"共%d个文件", arcList->size());
    Log(logString);
    Log(L"-------------------------");
    swprintf(logString, 128, L"使用解包函数地址：%X, 类型：%d", g_DecompressFile, g_DecompType);
    Log(logString);
    for (int i = 0; (ULONG)i < arcList->size(); i++)
    {
        WaitForSingleObject(g_paused, INFINITE);
        if (WaitForSingleObject(g_needStop, 0) == 0)
        {
            breaked = TRUE;
            break;
        }
        swprintf(logString, 128, L"fxckBGI - working... %d/%d", i + 1, arcList->size());
        SetWindowText(g_hwndMain, logString);

        Log((*arcList)[i] + L" 提取中");

        int rslt;
        try
        {
            rslt = ExtArc((*arcList)[i]);
        }
        catch (wstring s)
        {
            Log(wstring(L"----") + s);
            continue;
        }
        catch (...)
        {
            Log(wstring(L"----未知异常"));
            continue;
        }
        if (rslt == 1)
        {
            breaked = TRUE;
            break;
        }
    }
    if (!breaked)
        Log(L"所有文件处理完毕");
    else
        Log(L"操作被中止");

    EnableWindow(GetDlgItem(g_hwndMain, IDC_EXPORTLOG), TRUE);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_SELECTARC), TRUE);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_PAUSE), FALSE);
    SetDlgItemText(g_hwndMain, IDC_PAUSE, L"Pause");
    SendDlgItemMessage(g_hwndMain, IDC_PROGRESS, PBM_SETPOS, 0, 0);
    EnableWindow(GetDlgItem(g_hwndMain, IDC_STOP), FALSE);
    SetWindowText(g_hwndMain, L"fxckBGI v" PRODUCT_VERSION);
    delete arcList;
    return 0;
}
