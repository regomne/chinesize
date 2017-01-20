#include<windows.h>
#include <stdio.h>
#include <string>

#include "../PicSupport.h"
#include "../NakedMemory.h"
#include "../FileReader.h"

using namespace std;

static void myp(const wchar_t* format, ...)
{
    wchar_t buffer[0x1000];
    va_list ap;
    va_start(ap, format);
    auto char_cnt = vswprintf_s(buffer, format, ap);
    DWORD bytes;
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buffer, char_cnt, &bytes, 0);
    va_end(ap);
}

static inline uint32_t round_4(uint32_t val)
{
    return (val + 3) & ~3;
}

struct RctHeader {
    uint8_t magic[8];		/* "六丁TC00" and "六丁TC01" and "六丁TS00" and "六丁TS01" */
    uint32_t width;
    uint32_t height;
    uint32_t data_length;
};

bool get_rct_info(const wstring& rct_name, std::string& magic, std::string& fname)
{
    FILE* fp = nullptr;
    _wfopen_s(&fp, rct_name.c_str(), L"rb");
    if (!fp)
    {
        myp(L"无法打开：%s\n", rct_name.c_str());
        return false;
    }
    RctHeader hdr;
    fread(&hdr, 1, sizeof(hdr), fp);
    magic = std::string((char*)hdr.magic, 8);
    if (hdr.magic[7] == '1')
    {
        uint16_t name_size;
        fread(&name_size, 1, 2, fp);
        auto s = new char[name_size];
        fread(s, 1, name_size, fp);
        fname = std::string(s, name_size);
        delete[] s;
    }
    fclose(fp);
    return true;
}

static const int g_max_window_size = 0xffff;

bool pack_rct(const wstring& bmp_name, const wstring& old_name, const wstring& new_name)
{
    MyFileReader reader;
    auto bmp_mem = reader.ReadToMem(bmp_name.c_str());
    auto pb = (uint8_t*)bmp_mem.Get();
    auto bf = (BITMAPFILEHEADER*)pb;
    pb += sizeof(*bf);
    auto bi = (BITMAPINFOHEADER*)pb;
    pb += sizeof(*bi);
    
    if (bi->biBitCount != 24)
    {
        myp(L"%s: 必须是24位bmp！\n", bmp_name.c_str());
        return false;
    }

    auto height_sign = bi->biHeight < 0 ? 1 : -1;
    auto realheight = height_sign * bi->biHeight * -1;
    auto rowbytes = round_4(bi->biWidth * 3);

    NakedMemory new_dib((uint32_t)(bi->biWidth * realheight * 3 * 1.2 + 100));
    auto pn = (uint8_t*)new_dib.Get();
    int cur_row = height_sign == 1 ? 0 : realheight - 1;
    int cur_window = 0;
    uint32_t left_points = bi->biWidth * realheight;
    bool is_start = true;
    for (int i = 0;i < realheight;i++)
    {
        auto p = pb + cur_row * rowbytes;
        auto move_p = [&]() {
            *pn++ = *p++;
            *pn++ = *p++;
            *pn++ = *p++;
            left_points--;
        };
        for (int j = 0;j < bi->biWidth;j++)
        {
            if (is_start)
            {
                move_p();
                is_start = false;
            }
            else
            {
                if (cur_window == 0)
                {
                    if (left_points >= 0x80)
                    {
                        cur_window = left_points > g_max_window_size ? g_max_window_size : left_points;
                        *pn++ = 0x7f;
                        *pn++ = (cur_window - 0x80) & 0xff;
                        *pn++ = (cur_window - 0x80) >> 8;
                    }
                    else
                    {
                        cur_window = left_points;
                        *pn++ = cur_window - 1;
                    }
                }
                move_p();
                cur_window--;
            }
        }
        cur_row += height_sign;
    }
    new_dib.SetSize(pn - (uint8_t*)new_dib.Get());
    string magic, ref_name;
    if (!get_rct_info(old_name, magic, ref_name))
    {
        myp(L"%s: 获取rct信息失败！", old_name.c_str());
        return false;
    }
    
    FILE* fp = nullptr;
    _wfopen_s(&fp, new_name.c_str(), L"wb");
    if (!fp)
    {
        myp(L"无法打开: %s\n", new_name.c_str());
        return false;
    }

    RctHeader hdr;
    hdr.width = bi->biWidth;
    hdr.height = realheight;
    hdr.data_length = new_dib.GetSize();
    if (magic[7] == '0')
    {
        memcpy(hdr.magic, "\x98\x5a\x92\x9aTC00", 8);
        fwrite(&hdr, 1, sizeof(hdr), fp);
    }
    else
    {
        memcpy(hdr.magic, "\x98\x5a\x92\x9aTC01", 8);
        fwrite(&hdr, 1, sizeof(hdr), fp);
        auto len = ref_name.length();
        fwrite(&len, 1, 2, fp);
        fwrite(ref_name.c_str(), 1, len, fp);
    }
    fwrite(new_dib.Get(), 1, new_dib.GetSize(), fp);
    fclose(fp);
    return true;
}

bool fix_pallete(uint8_t* pal, uint32_t pal_cnt, uint8_t* dib, uint32_t dib_size)
{
    uint8_t idx[256] = { 0 };
    for (size_t i = 0;i < pal_cnt;i++)
    {
        auto b = pal[0];
        auto g = pal[1];
        auto r = pal[2];
        pal += 4;
        if (r != g || r != b)
        {
            return false;
        }
        idx[i] = r;
    }
    for (size_t i = 0;i < dib_size;i++)
    {
        dib[i] = idx[dib[i]];
    }
    return true;
}

bool pack_rc8(const wstring& bmp_name, const wstring& old_name, const wstring& new_name)
{
    MyFileReader reader;
    auto bmp_mem = reader.ReadToMem(bmp_name.c_str());
    auto pb = (uint8_t*)bmp_mem.Get();
    auto bf = (BITMAPFILEHEADER*)pb;
    pb += sizeof(*bf);
    auto bi = (BITMAPINFOHEADER*)pb;
    pb += sizeof(*bi);

    if (bi->biBitCount != 8)
    {
        myp(L"%s: 必须是8位bmp！\n", bmp_name.c_str());
        return false;
    }
    auto height_sign = bi->biHeight < 0 ? 1 : -1;
    auto realheight = height_sign * bi->biHeight * -1;
    auto rowbytes = round_4(bi->biWidth);
    auto pal = pb;
    auto pal_cnt = bi->biClrUsed ? bi->biClrUsed : 256;
    pb = (uint8_t*)bmp_mem.Get() + bf->bfOffBits;

    if (!fix_pallete(pal, pal_cnt, pb, rowbytes * realheight))
    {
        myp(L"%s: 调色板必须为单色！\n", bmp_name.c_str());
        return false;
    }

    NakedMemory new_dib((uint32_t)(bi->biWidth * realheight * 1.2 + 100));
    auto pn = (uint8_t*)new_dib.Get();
    int cur_row = height_sign == 1 ? 0 : realheight - 1;
    int cur_window = 0;
    uint32_t left_points = bi->biWidth * realheight;
    bool is_start = true;
    for (int i = 0;i < realheight;i++)
    {
        auto p = pb + cur_row * rowbytes;
        auto move_p = [&]() {
            *pn++ = *p++;
            left_points--;
        };
        for (int j = 0;j < bi->biWidth;j++)
        {
            if (is_start)
            {
                move_p();
                is_start = false;
            }
            else
            {
                if (cur_window == 0)
                {
                    if (left_points >= 0x80)
                    {
                        cur_window = left_points > g_max_window_size ? g_max_window_size : left_points;
                        *pn++ = 0x7f;
                        *pn++ = (cur_window - 0x80) & 0xff;
                        *pn++ = (cur_window - 0x80) >> 8;
                    }
                    else
                    {
                        cur_window = left_points;
                        *pn++ = cur_window - 1;
                    }
                }
                move_p();
                cur_window--;
            }
        }
        cur_row += height_sign;
    }
    new_dib.SetSize(pn - (uint8_t*)new_dib.Get());
    uint8_t new_pal[0x300];
    for (int i = 0;i < 0x100;i++)
    {
        new_pal[i * 3 + 0] = i;
        new_pal[i * 3 + 1] = i;
        new_pal[i * 3 + 2] = i;
    }

    FILE* fp = nullptr;
    _wfopen_s(&fp, new_name.c_str(), L"wb");
    if (!fp)
    {
        myp(L"无法打开: %s\n", new_name.c_str());
        return false;
    }

    RctHeader hdr;
    hdr.width = bi->biWidth;
    hdr.height = realheight;
    hdr.data_length = new_dib.GetSize();
    memcpy(hdr.magic, "\x98\x5a\x92\x9a\x38_00", 8);
    fwrite(&hdr, 1, sizeof(hdr), fp);
    fwrite(new_pal, 1, sizeof(new_pal), fp);
    fwrite(new_dib.Get(), 1, new_dib.GetSize(), fp);
    fclose(fp);
    return true;
}

wstring to932(const wstring& ori)
{
    auto ansi_len = WideCharToMultiByte(932, 0, ori.c_str(), ori.length(), 0, 0, 0, 0);
    auto ansi = new char[ansi_len];
    ansi_len = WideCharToMultiByte(932, 0, ori.c_str(), ori.length(), ansi, ansi_len, 0, 0);
    auto wide = new wchar_t[ansi_len + 1];
    auto wide_len = MultiByteToWideChar(936, 0, ansi, ansi_len, wide, ansi_len + 1);
    auto str = wstring(wide, wide_len);
    delete[] ansi;
    delete[] wide;
    return std::move(str);
}

void pack_dir(const wchar_t* dir_bmp, const wchar_t* dir_ori, const wchar_t* new_dir)
{
    WIN32_FIND_DATA wfd;
    wstring dir(dir_bmp);
    dir += L'\\';
    wstring olddir(dir_ori);
    olddir += L'\\';
    wstring outdir(new_dir);
    outdir += L'\\';

    auto hf = FindFirstFile((dir + L"*.bmp").c_str(), &wfd);
    if (hf != INVALID_HANDLE_VALUE)
    {
        do
        {
            auto bmp_name = dir + wfd.cFileName;
            auto p = wcsstr(wfd.cFileName, L".");
            auto base_name = wstring(wfd.cFileName, p - wfd.cFileName);
            if (p)
            {
                myp(L"packing: %s\n", bmp_name.c_str());
                wstring new_ext;
                auto ori_name1 = olddir + base_name + L".rct";
                auto ori_name2 = olddir + base_name + L".rc8";
                if (exists_file(ori_name1.c_str()))
                {
                    pack_rct(bmp_name, ori_name1, outdir + to932(base_name) + L".rct");
                }
                else if (exists_file(ori_name2.c_str()))
                {
                    pack_rc8(bmp_name, ori_name2, outdir + to932(base_name) + L".rc8");
                }
                else
                {
                    myp(L"%s: 未找到对应的rct或rc8文件\n", bmp_name);
                    continue;
                }
            }
        } while (FindNextFile(hf, &wfd));
        FindClose(hf);
    }
}