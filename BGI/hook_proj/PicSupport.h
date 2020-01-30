#pragma once

#include <stdint.h>

#include "NakedMemory.h"

enum PngOption {
    PngOptionRowAlign4 = 0,
    PngOptionReverse = 0,

    PngOptionRowNonAlign = 1,
    PngOptionNonReverse = 1<<1,
};

bool ReadPngToBmp(NakedMemory& src, int* width, int* height, int* bit_depth, NakedMemory& dib, int opt = 0);
bool read_bmp_file(NakedMemory& src, int* width, int* height, int* bit_depth, NakedMemory& dib);

NakedMemory Dib32To24(int width, int height, NakedMemory& old, NakedMemory& alpha);
NakedMemory Dib24To32(int width, int height, NakedMemory& old);
NakedMemory dib_row_fix(int width, int height, int bitc, NakedMemory& old);
NakedMemory dib_row_fix_non_align(int width, int height, int bitc, NakedMemory& old);
NakedMemory build_bmp_file(int width, int height, int bitc, NakedMemory& pallete, NakedMemory& dib);

bool decolor24(int w, int h, NakedMemory& rgb, NakedMemory& pal, NakedMemory& new_rgb);
