#include "exhibit.h"
#include <vector>
#include "FileReader.h"
#include "FuncHelper.h"
#include "TxtSupport.h"
#include "MyPack.h"


using namespace std;

MyPack* g_MyPack;

extern vector<string>* g_lines;
extern string g_cur_txt;
extern uint32_t g_line_idx;
uint32_t HOOKFUNC MyLoadRld(void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2)
{
    if (!g_MyPack) {
        g_MyPack = new MyPack(L"cnpack");
        if (!g_MyPack->Init()) {
            //todo
        }
    }
    auto fname = string((char*)arg1);
    size_t pos;
    if (fname.find("rld\\") != 0 ||
        (pos = fname.find((".rld"))) == string::npos)
    {
        return ThiscallFunction2(old_proc, thisp, arg1, arg2);
    }
    auto txtName = string("txt\\") + fname.substr(4, pos - 4) + ".txt";
    g_cur_txt = txtName;
    auto mem = g_MyPack->ReadAFile(txtName);
    if (!mem.Get()) {
        //Log("Can't read %s", txtName.c_str());
        return ThiscallFunction2(old_proc, thisp, arg1, arg2);
    }
    auto ansi = CvtToAnsi(mem, 936);
    g_lines = new vector<string>(SplitTxtA(ansi));
    g_line_idx = 0;

    auto ret = ThiscallFunction2(old_proc, thisp, arg1, arg2);

    delete g_lines;
    g_lines = nullptr;
    g_line_idx = 0;
    g_cur_txt = "";
    return ret;
}


bool g_enter_proc = false;
uint32_t g_cur_cmd = -1;
uint32_t g_proc_cmd_return_offset;
uint32_t HOOKFUNC MyProcCmds(Registers* regs, void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    static auto mod_addr = (intptr_t)GetModuleHandleA("resident.dll");
    if (*(uint32_t*)regs->esp == mod_addr + g_proc_cmd_return_offset)
    {
        g_enter_proc = true;
        g_cur_cmd = arg2;
    }
    auto ret = ThiscallFunction3(old_proc, thisp, arg1, arg2, arg3);
    g_enter_proc = false;
    g_cur_cmd = -1;
    return ret;
}

bool IsHalf(const string & s)
{
    for (auto c : s)
    {
        if ((unsigned char)c >= 0x80)
        {
            return false;
        }
    }
    return true;
}

void HOOKFUNC MyAddString(Registers* regs)
{
    if (g_enter_proc && g_lines)
    {
        auto old_str = string((const char*)regs->ecx);
        auto str_idx = regs->edi;
        auto cmd = g_cur_cmd & 0xffff;
        if (cmd == 28)
        {
            if (old_str.length() != 0 &&
                old_str != "*" &&
                old_str != "$noname$" &&
                std::count(old_str.begin(), old_str.end(), ',') < 2)
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
        else if (cmd == 21)
        {
            if (old_str.length() != 0 &&
                old_str != "*" &&
                old_str != "$noname$" &&
                std::count(old_str.begin(), old_str.end(), ',') < 2)
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
        else if (cmd == 48)
        {
            if (str_idx == 0)
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }

        }
        else if (cmd == 191)
        {
            if (str_idx == 0 && !IsHalf(old_str))
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
        else if (cmd == 203)
        {
            if (str_idx == 0 && !IsHalf(old_str))
            {
                auto news = ReadOneLineFromTxt();
                if (!news)
                {
                    LOGERROR("%s txt read error", g_cur_txt.c_str());
                    return;
                }
                regs->ecx = (uint32_t)news;
            }
        }
    }
}

//
//#include "rld_key.cpp"
//void HOOKFUNC MyDecRld(void* old_proc, void* thisp, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
//{
//    if (arg4 == 0x0AE85A916)
//    {
//        ThiscallFunction4(old_proc, thisp, arg1, arg2, arg3, arg4);
//    }
//    else
//    {
//        uint8_t* xor_key = (uint8_t*)&arg4;
//        auto dec_len = arg3;
//        if (dec_len > 0x3ff0 * 4)
//        {
//            dec_len = 0x3ff0 * 4;
//        }
//        else
//        {
//            dec_len &= 0xfffffffc;
//        }
//        auto buff = (uint8_t*)arg2;
//        for (size_t i = 0; i < dec_len; i++)
//        {
//            buff[i] ^= g_rld_key[i & 0x3ff] ^ xor_key[i & 3];
//        }
//    }
//}
