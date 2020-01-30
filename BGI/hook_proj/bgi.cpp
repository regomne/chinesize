#include "bgi.h"
#include <stdint.h>
#include "FuncHelper.h"
#include "FileReader.h"
#include "PicSupport.h"

uint32_t g_old_load_file = 0;
int call_old_load_file(uint8_t* dst, uint32_t* dst_len, uint8_t* src, uint32_t src_len, uint32_t arg1, uint32_t arg2)
{
    if (g_old_load_file == 0)
    {
        g_old_load_file = (uint32_t)GetModuleHandle(nullptr) + 0x6f070;
    }
    int ret_val = 0;
    __asm {
        push arg1;
        push arg2;
        push src_len;
        push src;
        mov edx, dst_len;
        mov ecx, dst;
        call g_old_load_file;
        add esp, 0x10;
        mov ret_val, eax;
    }
    return ret_val;
}

#pragma pack(1)
struct BmpHdr
{
    uint16_t width;
    uint16_t height;
    uint16_t bit;
};
#pragma pack()

int read_my_pic(char* fname, uint8_t* dst, uint32_t* dst_len, uint8_t* src, uint32_t src_len, uint32_t arg1, uint32_t arg2)
{
    static auto module_path = get_module_path(GetModuleHandle(nullptr));
    auto ret_val = call_old_load_file(dst, dst_len, src, src_len, arg1, arg2);
    if (ret_val != 0 || *dst_len < 0x10)
    {
        return ret_val;
    }
    auto full_name = module_path + L"pic\\" + decode_string(fname, 932) + L".png";
    if (!exists_file(full_name.c_str()))
    {
        return ret_val;
    }
    MyFileReader reader;
    auto buf = reader.ReadToMem(full_name.c_str());
    if (!buf)
    {
        return ret_val;
    }
    int width, height, bit_dep;
    NakedMemory bmp;
    if (!ReadPngToBmp(buf, &width, &height, &bit_dep, bmp, PngOptionNonReverse | PngOptionRowNonAlign))
    {
        return ret_val;
    }
    auto bmp_hdr = (BmpHdr*)dst;
    if (width != bmp_hdr->width || height != bmp_hdr->height)
    {
        return ret_val;
    }
    if (bit_dep != bmp_hdr->bit)
    {
        if (bit_dep == 32 && bmp_hdr->bit == 24)
        {
            memcpy(dst + 0x10, bmp.Get(), bmp.GetSize());
            bmp_hdr->bit = 32;
            *dst_len = bmp.GetSize() + 0x10;
            return ret_val;
        }
        else
        {
            return ret_val;
        }
    }
    if (bmp.GetSize() != *dst_len - 0x10)
    {
        LOGERROR("seems error");
        return ret_val;
    }
    memcpy(dst + 0x10, bmp.Get(), bmp.GetSize());
    return ret_val;
}

void HOOKFUNC on_load_rsc(Registers* regs)
{
    auto fname = *(char**)(regs->ebp + 8);
    auto dst = (uint8_t*)regs->ecx;
    auto dst_len = (uint32_t*)regs->edx;
    auto src = *(uint8_t**)(regs->esp);
    auto src_len = *(uint32_t*)(regs->esp + 4);
    auto arg1 = *(uint32_t*)(regs->esp + 8);
    auto arg2 = *(uint32_t*)(regs->esp + 12);
    regs->eax = read_my_pic(fname, dst, dst_len, src, src_len, arg1, arg2);
}
