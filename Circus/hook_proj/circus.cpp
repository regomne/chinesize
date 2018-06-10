#include "circus.h"
#include <string>
#include "PicSupport.h"
#include "FuncHelper.h"
#include "FileReader.h"

using namespace std;

#define READ_PIC_STACK_RET_OFFSET 0x1c
#define READ_PIC_RET_ADDR_1 0x404aab
#define READ_PIC_RET1_NAME_OFFSET 0x22c

#define READ_PIC_GLOBAL_PIC_OBJ 0xb21be8
#define READ_PIC_EXIT_POS 0x4031f8

struct PicObject {
    uint8_t* dib;
    uint8_t* dib_end;
    uint32_t stride;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t unk1;
    uint32_t unk2;
};

static inline uint32_t round_4(uint32_t val)
{
    return (val + 3) & ~3;
}

bool read_bmp_file1(NakedMemory& src, int* width, int* height, int* bit_depth, NakedMemory& dib) {
    auto file_header = (BITMAPFILEHEADER*)src.Get();
    auto info_header = (BITMAPINFOHEADER*)(file_header + 1);
    if (info_header->biBitCount != 24 && info_header->biBitCount != 32) {
        return false;
    }
    *width = info_header->biWidth;
    *height = info_header->biHeight;
    bool is_bottom = false;
    if (*height < 0) {
        *height = -*height;
        is_bottom = true;
    }
    *bit_depth = info_header->biBitCount;

    auto stride = round_4(*width * (*bit_depth / 8));
    if (file_header->bfOffBits + stride * *height > src.GetSize()) {
        return false;
    }
    NakedMemory dibdata(stride * *height);

    if (is_bottom) {
        auto p = (uint8_t*)dibdata.Get();
        auto po = (uint8_t*)src.Get() + file_header->bfOffBits;
        p += (*height - 1) * stride;
        for (int i = 0;i < *height;i++) {
            if (*bit_depth == 24) {
                memcpy(p, po, stride);
            }
            else {
                //BGRA -> GRAB
                for (int i = 0;i < *width;i++) {
                    p[i * 4 + 0] = po[i * 4 + 3];
                    p[i * 4 + 1] = po[i * 4 + 0];
                    p[i * 4 + 2] = po[i * 4 + 1];
                    p[i * 4 + 3] = po[i * 4 + 2];
                }
            }
            p -= stride;
            po += stride;
        }
    }
    else {
        auto p = (uint8_t*)dibdata.Get();
        auto po = (uint8_t*)src.Get() + file_header->bfOffBits;
        if (*bit_depth == 24) {
            memcpy(p, po, stride * *height);
        }
        else {
            for (int i = 0;i < *height;i++) {
                //BGRA -> GRAB
                for (int i = 0;i < *width;i++) {
                    p[i * 4 + 0] = po[i * 4 + 3];
                    p[i * 4 + 1] = po[i * 4 + 0];
                    p[i * 4 + 2] = po[i * 4 + 1];
                    p[i * 4 + 3] = po[i * 4 + 2];
                }
                p += stride;
                po += stride;
            }
        }
    }
    dib = std::move(dibdata);
    return true;
}

bool read_pic_in_circus(Registers* regs) {
    auto ret_addr = *(uint32_t*)(regs->esp + READ_PIC_STACK_RET_OFFSET);
    char* pic_name = nullptr;
    if (ret_addr == READ_PIC_RET_ADDR_1) {
        pic_name = *(char**)(regs->esp + READ_PIC_STACK_RET_OFFSET + READ_PIC_RET1_NAME_OFFSET);
    }
    auto pic_obj = (PicObject*)(READ_PIC_GLOBAL_PIC_OBJ + (regs->ebp << 7));
    if (!pic_name || pic_obj->depth != 0 && pic_obj->depth != 1) {
        return false;
    }
    
    auto pic_file_name = (string)pic_name + ".png";
    MyFileReader reader;
    auto png_src = reader.ReadToMem(pic_file_name.c_str());
    if (!png_src.Get()) {
        return false;
    }
    int width, height, bit_depth;
    NakedMemory dib;
    if (!ReadPngToBmp(png_src, &width, &height, &bit_depth, dib)) {
        LOGERROR("%s: png format error!", pic_file_name.c_str());
        return false;
    }
    //if (!read_bmp_file1(png_src, &width, &height, &bit_depth, dib)) {
    //    LOGERROR("%s bmp format error!", pic_file_name.c_str());
    //    return false;
    //}
    if (pic_obj->depth == 0 && bit_depth != 24 ||
        pic_obj->depth == 1 && bit_depth != 32) {
        LOGERROR("%s: pic bit depth error!", pic_file_name.c_str());
        return false;
    }
    if (pic_obj->width != width ||
        pic_obj->height != height ||
        round_4(width*(bit_depth / 8)) != pic_obj->stride) {
        LOGERROR("%s: pic width/height/stride error", pic_file_name.c_str());
        return false;
    }

    if (bit_depth == 24) {
        memcpy(pic_obj->dib, dib.Get(), pic_obj->stride * height);
    }
    else if (bit_depth == 32) {
        auto p = (uint8_t*)pic_obj->dib;
        auto po = (uint8_t*)dib.Get();
        for (int i = 0;i < height;i++) {
            for (int j = 0;j < width;j++) {
                p[j * 4 + 0] = 255 - po[j * 4 + 3];
                p[j * 4 + 1] = po[j * 4 + 0];
                p[j * 4 + 2] = po[j * 4 + 1];
                p[j * 4 + 3] = po[j * 4 + 2];
            }
            p += pic_obj->stride;
            po += pic_obj->stride;
        }
    }
    return true;
}

ptrdiff_t HOOKFUNC my_read_pic(Registers* regs, ptrdiff_t old_pos) {
    if (read_pic_in_circus(regs)) {
        return READ_PIC_EXIT_POS;
    }
    return old_pos;
}