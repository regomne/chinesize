#include "TxtSupport.h"
#include "FuncHelper.h"

using namespace std;

vector<string> SplitTxtA(NakedMemory& txt)
{
    auto buff = (uint8_t*)txt.Get();
    uint32_t i = 0;
    uint32_t last = 0;
    auto ls = vector<string>();
    while (true)
    {
        while (i < txt.GetSize() && buff[i] != '\r') i++;
        ls.push_back(string((char*)buff + last, i - last));
        if (i + 2 > txt.GetSize()) break;
        i += 2;
        last = i;
    }
    return ls;
}

vector<wstring> SplitTxtW(NakedMemory& txt)
{
    auto buff = (char16_t*)txt.Get();
    auto buff_len = txt.GetSize() / 2;
    if (buff[0] == 0xfeff) {
        buff++;
        buff_len--;
    }
    uint32_t i = 0;
    uint32_t last = 0;
    auto ls = vector<wstring>();
    while (true)
    {
        while (i < buff_len && buff[i] != u'\r') i++;
        ls.push_back(wstring((wchar_t*)buff + last, i - last));
        if (i + 2 > buff_len) break;
        i += 2;
        last = i;
    }
    return ls;
}

NakedMemory CvtToAnsi(NakedMemory& str, int cp)
{
    auto buff = (uint8_t*)str.Get();
    if (*(WORD*)buff == 0xfeff)
    {
        auto newsize = WideCharToMultiByte(cp, 0, (wchar_t*)(buff + 2), (str.GetSize() - 2) / 2, 0, 0, 0, 0);
        NakedMemory ansi(newsize);
        WideCharToMultiByte(cp, 0, (wchar_t*)(buff + 2), (str.GetSize() - 2) / 2, (char*)ansi.Get(), ansi.GetSize(), 0, 0);
        return std::move(ansi);
    }
    else if (str.GetSize()>3 && buff[0] == 0xef && buff[1] == 0xbb && buff[2] == 0xbf)
    {
        if (cp == CP_UTF8)
        {
            NakedMemory ansi(str.GetSize() - 3);
            memcpy(ansi.Get(), buff + 3, ansi.GetSize());
            return std::move(ansi);
        }
        else
        {
            NakedMemory u16(str.GetSize() * 2);
            auto newsize1 = MultiByteToWideChar(CP_UTF8, 0, (char*)buff + 3, str.GetSize() - 3, (wchar_t*)u16.Get(), str.GetSize());

            auto newsize = WideCharToMultiByte(cp, 0, (wchar_t*)u16.Get(), newsize1, 0, 0, 0, 0);
            NakedMemory ansi(newsize);
            WideCharToMultiByte(cp, 0, (wchar_t*)u16.Get(), newsize1, (char*)ansi.Get(), ansi.GetSize(), 0, 0);
            return std::move(ansi);
        }
    }
    else
    {
        return str;
    }
}


vector<string>* g_lines = nullptr;
string g_cur_txt;
uint32_t g_line_idx = 0;

char* ReadOneLineFromTxt()
{
    static char line[1000];
    if (g_lines)
    {
        while (g_line_idx < g_lines->size())
        {
            auto& ls = (*g_lines)[g_line_idx];
            if (ls.length() > 0 && ls[0] != '$')
            {
                if (ls.length() > sizeof(line))
                {
                    LOGERROR("%s line %d too long", g_cur_txt.c_str(), g_line_idx + 1);
                    return nullptr;
                }
                ls.copy(line, ls.length());
                line[ls.length()] = '\0';
                g_line_idx++;
                return line;
            }
            g_line_idx++;
        }
    }
    return nullptr;
}
