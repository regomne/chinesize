#include <windows.h>
#include <stdio.h>
#include <string>

#include "..\mjo.h"
#include "..\NakedMemory.h"
#include "..\FileReader.h"
#include "..\PicSupport.h"

#include "rct_key.cpp"

struct RctHeader {
    uint8_t magic[8];		/* "鎗供TC00" and "鎗供TC01" and "鎗供TS00" and "鎗供TS01" */
    uint32_t width;
    uint32_t height;
    uint32_t data_length;
};

using namespace std;

void xor_dec(void* buff, uint32_t size)
{
    for (size_t i = 0;i < size;i++)
    {
        ((uint8_t*)buff)[i] ^= g_rct_key[i & 0x3ff];
    }
}

static DWORD rct_decompress(BYTE *uncompr, DWORD uncomprLen,
    BYTE *compr, DWORD comprLen, DWORD width)
{
    DWORD act_uncomprLen = 0;
    DWORD curByte = 0;
    DWORD pos[32];

    pos[0] = -3;
    pos[1] = -6;
    pos[2] = -9;
    pos[3] = -12;
    pos[4] = -15;
    pos[5] = -18;
    pos[6] = (3 - width) * 3;
    pos[7] = (2 - width) * 3;
    pos[8] = (1 - width) * 3;
    pos[9] = (0 - width) * 3;
    pos[10] = (-1 - width) * 3;
    pos[11] = (-2 - width) * 3;
    pos[12] = (-3 - width) * 3;
    pos[13] = 9 - ((width * 3) << 1);
    pos[14] = 6 - ((width * 3) << 1);
    pos[15] = 3 - ((width * 3) << 1);
    pos[16] = 0 - ((width * 3) << 1);
    pos[17] = -3 - ((width * 3) << 1);
    pos[18] = -6 - ((width * 3) << 1);
    pos[19] = -9 - ((width * 3) << 1);
    pos[20] = 9 - width * 9;
    pos[21] = 6 - width * 9;
    pos[22] = 3 - width * 9;
    pos[23] = 0 - width * 9;
    pos[24] = -3 - width * 9;
    pos[25] = -6 - width * 9;
    pos[26] = -9 - width * 9;
    pos[27] = 6 - ((width * 3) << 2);
    pos[28] = 3 - ((width * 3) << 2);
    pos[29] = 0 - ((width * 3) << 2);
    pos[30] = -3 - ((width * 3) << 2);
    pos[31] = -6 - ((width * 3) << 2);

    uncompr[act_uncomprLen++] = compr[curByte++];
    uncompr[act_uncomprLen++] = compr[curByte++];
    uncompr[act_uncomprLen++] = compr[curByte++];

    while (1) {
        BYTE flag;
        DWORD copy_bytes, copy_pos;

        if (curByte >= comprLen)
            break;

        flag = compr[curByte++];

        if (!(flag & 0x80)) {
            if (flag != 0x7f)
                copy_bytes = flag * 3 + 3;
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 0x80;
                copy_bytes *= 3;
            }

            if (curByte + copy_bytes - 1 >= comprLen)
                break;
            if (act_uncomprLen + copy_bytes - 1 >= uncomprLen)
                break;

            memcpy(&uncompr[act_uncomprLen], &compr[curByte], copy_bytes);
            act_uncomprLen += copy_bytes;
            curByte += copy_bytes;
        }
        else {
            copy_bytes = flag & 3;
            copy_pos = (flag >> 2) & 0x1f;

            if (copy_bytes != 3) {
                copy_bytes = copy_bytes * 3 + 3;
            }
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 4;
                copy_bytes *= 3;
            }

            for (unsigned int i = 0; i < copy_bytes; i++) {
                if (act_uncomprLen >= uncomprLen)
                    goto out;
                uncompr[act_uncomprLen] = uncompr[act_uncomprLen + pos[copy_pos]];
                act_uncomprLen++;
            }
        }
    }
out:
    //	if (curByte != comprLen)
    //		fprintf(stderr, "compr miss-match %d VS %d\n", curByte, comprLen);

    return act_uncomprLen;
}

static DWORD rc8_decompress(BYTE *uncompr, DWORD uncomprLen,
    BYTE *compr, DWORD comprLen, DWORD width)
{
    DWORD act_uncomprLen = 0;
    DWORD curByte = 0;
    DWORD pos[16];

    pos[0] = -1;
    pos[1] = -2;
    pos[2] = -3;
    pos[3] = -4;
    pos[4] = 3 - width;
    pos[5] = 2 - width;
    pos[6] = 1 - width;
    pos[7] = 0 - width;
    pos[8] = -1 - width;
    pos[9] = -2 - width;
    pos[10] = -3 - width;
    pos[11] = 2 - (width * 2);
    pos[12] = 1 - (width * 2);
    pos[13] = (0 - width) << 1;
    pos[14] = -1 - (width * 2);
    pos[15] = (-1 - width) * 2;

    uncompr[act_uncomprLen++] = compr[curByte++];

    while (1) {
        BYTE flag;
        DWORD copy_bytes, copy_pos;

        if (curByte >= comprLen)
            break;

        flag = compr[curByte++];

        if (!(flag & 0x80)) {
            if (flag != 0x7f)
                copy_bytes = flag + 1;
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 0x80;
            }

            if (curByte + copy_bytes - 1 >= comprLen)
                break;
            if (act_uncomprLen + copy_bytes - 1 >= uncomprLen)
                break;

            memcpy(&uncompr[act_uncomprLen], &compr[curByte], copy_bytes);
            act_uncomprLen += copy_bytes;
            curByte += copy_bytes;
        }
        else {
            copy_bytes = flag & 7;
            copy_pos = (flag >> 3) & 0xf;

            if (copy_bytes != 7)
                copy_bytes += 3;
            else {
                if (curByte + 1 >= comprLen)
                    break;

                copy_bytes = compr[curByte++];
                copy_bytes |= compr[curByte++] << 8;
                copy_bytes += 0xa;
            }

            for (unsigned int i = 0; i < copy_bytes; i++) {
                if (act_uncomprLen >= uncomprLen)
                    break;
                uncompr[act_uncomprLen] = uncompr[act_uncomprLen + pos[copy_pos]];
                act_uncomprLen++;
            }
        }
    }

    //	if (curByte != comprLen)
    //		fprintf(stderr, "compr miss-match %d VS %d\n", curByte, comprLen);

    return act_uncomprLen;
}

bool un_rct(const RctHeader* hdr, NakedMemory& mem, const wchar_t* bmp_name, bool need_dec)
{
    auto cur = (uint8_t*)mem.Get() + sizeof(RctHeader);

    if (hdr->data_length + sizeof(*hdr) != mem.GetSize())
    {
        printf("size error!\n");
        return false;
    }

    if (need_dec)
    {
        xor_dec(cur, hdr->data_length);
    }
    auto unc_len = hdr->width * hdr->height * 3;
    NakedMemory newbuff(unc_len);
    auto act_len = rct_decompress((uint8_t*)newbuff.Get(), unc_len, cur, hdr->data_length, hdr->width);
    if (act_len != unc_len)
    {
        printf("decompress error!\n");
        return false;
    }
    auto ndib = dib_row_fix(hdr->width, hdr->height, 24, newbuff);
    auto bmp = build_bmp_file(hdr->width, -hdr->height, 24, NakedMemory(), ndib);
    auto fp = _wfopen(bmp_name, L"wb");
    fwrite(bmp.Get(), 1, bmp.GetSize(), fp);
    fclose(fp);
    return true;
}

bool un_rct1(const RctHeader* hdr, NakedMemory& mem, const wchar_t* bmp_name, bool need_dec)
{
    auto cur = (uint8_t*)mem.Get() + sizeof(RctHeader);
    cur += *(uint16_t*)cur + 2;

    if (hdr->data_length + cur != (uint8_t*)mem.Get() + mem.GetSize())
    {
        printf("size error!\n");
        return false;
    }

    if (need_dec)
    {
        xor_dec(cur, hdr->data_length);
    }
    auto unc_len = hdr->width * hdr->height * 3;
    NakedMemory newbuff(unc_len);
    auto act_len = rct_decompress((uint8_t*)newbuff.Get(), unc_len, cur, hdr->data_length, hdr->width);
    if (act_len != unc_len)
    {
        printf("decompress error!\n");
        return false;
    }
    auto ndib = dib_row_fix(hdr->width, hdr->height, 24, newbuff);
    auto bmp = build_bmp_file(hdr->width, -hdr->height, 24, NakedMemory(), ndib);
    auto fp = _wfopen(bmp_name, L"wb");
    fwrite(bmp.Get(), 1, bmp.GetSize(), fp);
    fclose(fp);
    return true;
}

bool un_rc8(const RctHeader* hdr, NakedMemory& mem, const wchar_t* bmp_name)
{
    auto cur = (uint8_t*)mem.Get() + sizeof(RctHeader);

    if (hdr->data_length + sizeof(*hdr) + 0x300 != mem.GetSize())
    {
        printf("size error!\n");
        return false;
    }

    auto pal_ptr = cur;
    cur += 0x300;
    //xor_dec(cur, hdr->data_length);
    auto unc_len = hdr->width * hdr->height;
    NakedMemory newbuff(unc_len);
    auto act_len = rc8_decompress((uint8_t*)newbuff.Get(), unc_len, cur, hdr->data_length, hdr->width);
    if (act_len != unc_len)
    {
        printf("decompress error!\n");
        return false;
    }

    NakedMemory pallete(0x400);
    auto p = (uint8_t*)pallete.Get();
    for (int i = 0;i < 0x100;i++)
    {
        p[0] = pal_ptr[0];
        p[1] = pal_ptr[1];
        p[2] = pal_ptr[2];
        p[3] = 0;
        p += 4;
        pal_ptr += 3;
    }
    auto ndib = dib_row_fix(hdr->width, hdr->height, 8, newbuff);
    auto bmp = build_bmp_file(hdr->width, -hdr->height, 8, pallete, ndib);
    auto fp = _wfopen(bmp_name, L"wb");
    fwrite(bmp.Get(), 1, bmp.GetSize(), fp);
    fclose(fp);
    return true;

}

void test_unrct(const wchar_t* rctname, const wchar_t* bmpname)
{
    MyFileReader reader;
    auto mem = reader.ReadToMem(rctname);
    if (!mem.Get())
    {
        return;
    }
    auto hdr = (RctHeader*)mem.Get();
    bool ret = false;
    if (memcmp(hdr->magic, "\x98\x5a\x92\x9a\x38_00", 8) == 0)
    {
        ret = un_rc8(hdr, mem, bmpname);
    }
    else if (memcmp(hdr->magic, "\x98\x5a\x92\x9aTS00", 8) == 0)
    {
        ret = un_rct(hdr, mem, bmpname, true);
    }
    else if (memcmp(hdr->magic, "\x98\x5a\x92\x9aTS01", 8) == 0)
    {
        ret = un_rct1(hdr, mem, bmpname, true);
    }
    else if (memcmp(hdr->magic, "\x98\x5a\x92\x9aTC00", 8) == 0)
    {
        ret= un_rct(hdr, mem, bmpname, false);
    }
    else if (memcmp(hdr->magic, "\x98\x5a\x92\x9aTC01", 8) == 0)
    {
        ret = un_rct1(hdr, mem, bmpname, false);
    }
    else
    {
        wprintf(L"unk format\n", rctname);
    }

    if (!ret)
    {
        wprintf(L"unc error, fname: %s\n", rctname);
    }
}

void un_dir(const wchar_t* dirname, const wchar_t* outdir_name)
{
    WIN32_FIND_DATA wfd;
    wstring dir(dirname);
    dir += L'\\';
    wstring outdir(outdir_name);
    outdir += L'\\';
    auto hf = FindFirstFile((dir + L"*.*").c_str(), &wfd);
    if (hf != INVALID_HANDLE_VALUE)
    {
        do
        {
            auto rct_name = dir + wfd.cFileName;
            auto p = wcsstr(wfd.cFileName, L".");
            wstring bmp_name;
            if (!p)
            {
                bmp_name = outdir + wfd.cFileName + L".bmp";
            }
            else
            {
                bmp_name = outdir + wstring(wfd.cFileName, p - wfd.cFileName) + L".bmp";
            }
            test_unrct(rct_name.c_str(), bmp_name.c_str());
        } while (FindNextFile(hf, &wfd));
        FindClose(hf);
    }
}

int mjo_upk()
{
    auto cmd = GetCommandLineW();
    int arg_cnt;
    auto argv = CommandLineToArgvW(cmd, &arg_cnt);
    if (arg_cnt != 4)
    {
        printf("error arg cnt\n");
        return 0;
    }
    //TestParse(argv[1], argv[2]);
    //TestPack(argv[1], argv[2], argv[3]);
    return 0;
}

#include "packbmp.h"
int main()
{
    auto cmd = GetCommandLineW();
    int argc;
    auto argv = CommandLineToArgvW(cmd, &argc);
    //un_dir(L"d:\\galgame\\レミニセンス\\testrct",
    //    L"d:\\galgame\\レミニセンス\\testbmp");
    if (argc != 4)
    {
        wprintf(L"usage: %s <bmp folder> <original rct folder> <new bmp folder>\n", argv[0]);
    }
    pack_dir(
        argv[1],
        argv[2],
        argv[3]);
}
