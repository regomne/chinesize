#include "PicSupport.h"
#include <windows.h>
#include <memory.h>
#include <type_traits>

#include <png.h>
#include "NakedMemory.h"
#include "FuncHelper.h"

struct MemStream
{
public:
    MemStream(void* buff1, uint32_t size1)
    {
        start = (uint8_t*)buff1;
        cur = start;
        size = size1;
    }

    uint8_t* start;
    uint8_t* cur;
    uint32_t size;
};

static inline uint32_t round_4(uint32_t val)
{
    return (val + 3) & ~3;
}

void PngRead(png_struct* png, png_byte* buff, png_size_t len)
{
    auto stream = (MemStream*)png_get_io_ptr(png);
    if (stream->cur - stream->start + len > stream->size)
    {
        png_error(png, "exceed limit in MemStream");
    }

    memcpy(buff, stream->cur, len);
    stream->cur += len;
}

bool ReadPngToBmp(NakedMemory& src, int* width, int* height, int* bit_count, NakedMemory& dib, int opt)
{
    png_struct* png_ptr;
    png_info* info_ptr;
    int color_type, temp;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if (png_ptr == NULL)
    {
        LOGERROR("create png read struct fail.");
        return false;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        LOGERROR("create png info struct fail.");
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        /* If we get here, we had a problem reading the file */
        LOGERROR("png reading file fail");
        return false;
    }

    MemStream stream(src.Get(), src.GetSize());
    png_set_read_fn(png_ptr, &stream, PngRead);

    png_read_info(png_ptr, info_ptr);

    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)width, (png_uint_32*)height, &bit_depth, &color_type,
        &temp, NULL, NULL);

    if (!(bit_depth == 8 && (color_type == PNG_COLOR_TYPE_RGBA || color_type == PNG_COLOR_TYPE_RGB)))
    {
        LOGERROR("png file info error: bit_depth:%d, color_type:%d", bit_depth, color_type);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    *bit_count = color_type == PNG_COLOR_TYPE_RGB ? 24 : 32;

    png_set_bgr(png_ptr);

    int bmp_row_bytes;
    if (opt & PngOptionRowNonAlign )
    {
        bmp_row_bytes = *width * (*bit_count / 8);
    }
    else
    {
        bmp_row_bytes = round_4(*width * (*bit_count / 8));
    }
    auto rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes > bmp_row_bytes)
    {
        LOGERROR("png fatal error: rowbytes:%d, bmp_rowbytes:%d", rowbytes, bmp_row_bytes);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }
    NakedMemory dibdata(*height * bmp_row_bytes);
    
    auto rowPointers = new uint8_t*[*height];
    uint8_t* p;
    int diff;
    if (opt & PngOptionNonReverse)
    {
        p = (uint8_t*)dibdata.Get();
        diff = bmp_row_bytes;
    }
    else
    {
        p = (uint8_t*)dibdata.Get() + (*height - 1) * bmp_row_bytes;
        diff = -bmp_row_bytes;
    }
    for (int i = 0;i < *height;i++)
    {
        rowPointers[i] = p;
        p += diff;
    }

    png_read_image(png_ptr, rowPointers);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    delete[] rowPointers;

    dib = std::move(dibdata);
    return true;
}

NakedMemory Dib32To24(int width, int height, NakedMemory& old, NakedMemory& alpha)
{
    if (width * 4 * height != old.GetSize())
    {
        LOGERROR("Dib size not correct!");
        return NakedMemory();
    }
    
    auto nrowbytes = round_4(width * 3);
    auto arowbytes = round_4(width);
    auto orowbytes = width * 4;
    NakedMemory newd(nrowbytes * height);
    NakedMemory newa(arowbytes * height);
    auto nrows = (uint8_t*)newd.Get();
    auto orows = (uint8_t*)old.Get();
    auto arows = (uint8_t*)newa.Get();
    for (int i = 0;i < height;i++)
    {
        auto pn = nrows;
        auto po = orows;
        auto pa = arows;
        for (int j = 0;j < width;j++)
        {
            pn[0] = po[0];
            pn[1] = po[1];
            pn[2] = po[2];
            *pa++ = po[3];
            pn += 3;
            po += 4;
        }
        nrows += nrowbytes;
        orows += orowbytes;
        arows += arowbytes;
    }
    alpha = std::move(newa);
    return std::move(newd);
}

NakedMemory Dib24To32(int width, int height, NakedMemory& old)
{
    auto orowbytes = round_4(width * 3);
    if (orowbytes * height != old.GetSize())
    {
        LOGERROR("Dib size not correct!");
        return NakedMemory();
    }
    
    auto nrowbytes = width * 4;
    NakedMemory newd(nrowbytes * height);
    auto nrows = (uint8_t*)newd.Get();
    auto orows = (uint8_t*)old.Get();
    for (int i = 0;i < height;i++)
    {
        auto pn = nrows;
        auto po = orows;
        for (int j = 0;j < width;j++)
        {
            pn[0] = po[0];
            pn[1] = po[1];
            pn[2] = po[2];
            pn[4] = 0;
            pn += 4;
            po += 3;
        }
        nrows += nrowbytes;
        orows += orowbytes;
    }
    return std::move(newd);
}

//修正每行的4字节对齐
NakedMemory dib_row_fix(int width, int height, int bitc, NakedMemory& old)
{
    auto ori_row_bytes = width * (bitc / 8);
    if (ori_row_bytes % 4 == 0)
    {
        return std::move(old);
    }
    int row_bytes = round_4(ori_row_bytes);
    NakedMemory mem(row_bytes * height);
    
    auto pn = (uint8_t*)mem.Get();
    auto po = (uint8_t*)old.Get();
    for (int i = 0;i < height;i++)
    {
        memcpy(pn, po, ori_row_bytes);
        for (int i = ori_row_bytes;i < row_bytes;i++)
        {
            pn[i] = 0;
        }
        pn += row_bytes;
        po += ori_row_bytes;
    }
    return mem;
}

NakedMemory dib_row_fix_non_align(int width, int height, int bitc, NakedMemory& old)
{
    auto new_row_bytes = width * (bitc / 8);
    if (new_row_bytes % 4 == 0)
    {
        return std::move(old);
    }
    int old_row_bytes = round_4(new_row_bytes);
    NakedMemory mem(new_row_bytes * height);
    
    auto pn = (uint8_t*)mem.Get();
    auto po = (uint8_t*)old.Get();
    for (int i = 0;i < height;i++)
    {
        memcpy(pn, po, new_row_bytes);
        pn += new_row_bytes;
        po += old_row_bytes;
    }
    return mem;
}

NakedMemory build_bmp_file(int width, int height, int bitc, NakedMemory& pallete, NakedMemory& dib)
{
    auto pallete_size = bitc == 8 ? pallete.GetSize() : 0;
    auto pallete_cnt = pallete_size / 4;
    BITMAPFILEHEADER bf = { 0 };
    BITMAPINFOHEADER bi = { 0 };
    bf.bfOffBits = sizeof(bf) + sizeof(bi) + pallete_size;
    bf.bfType = 0x4d42;
    bf.bfSize = bf.bfOffBits + dib.GetSize();
    bi.biBitCount = bitc;
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biSize = sizeof(bi);
    bi.biPlanes = 1;
    bi.biClrUsed = pallete_cnt;
    
    NakedMemory mem(bf.bfSize);
    auto cur = (uint8_t*)mem.Get();
    memcpy(cur, &bf, sizeof(bf));
    cur += sizeof(bf);
    memcpy(cur, &bi, sizeof(bi));
    cur += sizeof(bi);
    if (bitc == 8)
    {
        memcpy(cur, pallete.Get(), pallete.GetSize());
        cur += pallete.GetSize();
    }
    memcpy(cur, dib.Get(), dib.GetSize());
    return std::move(mem);
}

class OctDeColor
{
private:
    struct octree
    {
        size_t index;

        size_t leaf;
        size_t level;

        size_t count;
        size_t colorR, colorG, colorB;

        octree *parent;
        octree *children[8];
    };

public:
    OctDeColor()
    {
        memset(&root_, 0, sizeof(root_));
        count_ = 0;
        root_.level = 8;
        memset(leafnodes_, 0, sizeof(leafnodes_));
    }

    ~OctDeColor()
    {
        clrcolor_(&root_);
    }

    size_t get_count() const
    {
        return count_;
    }

    void gen_plane(uint8_t* plane, uint32_t max_count)
    {
        auto cnt = count_ > max_count ? max_count : count_;
        for (size_t i = 0;i < cnt;i++)
        {
            octree *p = leafnodes_[i];
            plane[0] = (uint8_t)(p->colorR / p->count);
            plane[1] = (uint8_t)(p->colorG / p->count);
            plane[2] = (uint8_t)(p->colorB / p->count);
            p->index = i;
            plane += 4;
        }
    }

    int mapcolor(size_t r, size_t g, size_t b)
    {
        int l;
        size_t rbit, gbit, bbit, tidx = 0;

        octree *p = &root_;

        for (l = 7; !p->leaf; l--) {

            rbit = (r >> l) & 1;
            gbit = (g >> l) & 1;
            bbit = (b >> l) & 1;
            tidx = (rbit << 2) | (gbit << 1) | bbit;

            p = p->children[tidx];

            p->count++;
            p->colorR += r;
            p->colorG += g;
            p->colorB += b;
        }
        return p->index;
    }

    int addcolor(size_t r, size_t g, size_t b)
    {
        size_t l;
        size_t rbit, gbit, bbit, tidx = 0;

        octree *p = &root_;

        for (l = 7; !p->leaf; l--) {

            rbit = (r >> l) & 1;
            gbit = (g >> l) & 1;
            bbit = (b >> l) & 1;
            tidx = (rbit << 2) | (gbit << 1) | bbit;

            if (p->children[tidx] == NULL) {
                octree *tree = new octree();
                memset(tree, 0, sizeof(octree));
                tree->leaf = (l == 0);
                tree->level = l;
                if (tree->leaf) {
                    leafnodes_[count_] = tree;
                    count_++;
                }
                tree->parent = p;
                p->children[tidx] = tree;
            }

            p = p->children[tidx];
            p->count++;
            p->colorR += r;
            p->colorG += g;
            p->colorB += b;
        }

        while (count_ > 256) {
            octree *tree = leafnodes_[0]->parent;
            for (l = 1; l < count_; l++) {
                octree *node = leafnodes_[l]->parent;
                if (node == NULL)
                    printf("%p %d %p\n", leafnodes_[l], l, &root_);
                if (node->level > tree->level)
                    continue;
                if (tree->count > node->count)
                    tree = node;
            }
            size_t adjcnt = 0;
            for (l = 0; l < count_; l++) {
                octree *node = leafnodes_[l];
                if (node->parent == tree) {
                    delete node;
                    continue;
                }
                leafnodes_[adjcnt++] = leafnodes_[l];
            }
            tree->leaf = 1;
            leafnodes_[adjcnt++] = tree;
            for (l = 0; l < 8; l++)
                tree->children[l] = NULL;
            count_ = adjcnt;
        }

        return 0;
    }

private:
    int clrcolor_(octree *root)
    {
        for (int i = 0; i < 8; i++)
        {
            octree *node = root->children[i];
            if (node == NULL)
                continue;
            clrcolor_(node);
            delete node;
        }
        memset(root, 0, sizeof(octree));
        count_ = 0;
        return 0;
    }


private:
    octree root_;
    size_t count_;
    octree *leafnodes_[257];
};



bool decolor24(int w, int h, NakedMemory& rgb, NakedMemory& pal, NakedMemory& new_rgb)
{
    auto orowbytes = round_4(w * 3);
    if (orowbytes * h != rgb.GetSize())
    {
        return false;
    }
    auto doff = (uint8_t*)rgb.Get();

    OctDeColor oct;
    for (int i = 0; i < h; i++) {
        auto pd = doff;
        for (int j = 0; j < w; j++) {
            oct.addcolor(pd[0], pd[1], pd[2]);
            pd += 3;
        }
        doff += orowbytes;
    }

    NakedMemory pallete(0x400);
    auto plane = (uint8_t*)pallete.Get();
    memset(plane, 0, pallete.GetSize());
    oct.gen_plane(plane, 0x100);

    auto nrowbytes = round_4(w);
    NakedMemory outdib(nrowbytes * h);
    auto npd = (uint8_t*)outdib.Get();

    doff = (uint8_t*)rgb.Get();

    for (int i = 0; i < h; i++) {
        auto pd = doff;
        for (int j = 0; j < w; j++) {
            npd[j] = oct.mapcolor(pd[0], pd[1], pd[2]);
            pd += 3;
        }
        doff += orowbytes;
        npd += nrowbytes;
    }

    pal = std::move(pallete);
    new_rgb = std::move(outdib);
    return true;
}

bool read_bmp_file(NakedMemory& src, int* width, int* height, int* bit_depth, NakedMemory& dib) {
    auto file_header = (BITMAPFILEHEADER*)src.Get();
    auto info_header = (BITMAPINFOHEADER*)(file_header + 1);
    if (info_header->biBitCount != 24 || info_header->biBitCount != 32) {
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
            memcpy(p, po, stride);
            p -= stride;
            po += stride;
        }
    }
    else {
        auto p = (uint8_t*)dibdata.Get();
        auto po= (uint8_t*)src.Get() + file_header->bfOffBits;
        memcpy(p, po, stride * *height);
    }
    dib = std::move(dibdata);
    return true;
}