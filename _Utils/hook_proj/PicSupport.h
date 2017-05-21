#pragma once

#include <stdint.h>

#include "NakedMemory.h"

bool ReadPngToBmp(NakedMemory& src, int* width, int* height, int* bit_depth, NakedMemory& dib);

NakedMemory Dib32To24(int width, int height, NakedMemory& old, NakedMemory& alpha);
NakedMemory Dib24To32(int width, int height, NakedMemory& old);
NakedMemory dib_row_fix(int width, int height, int bitc, NakedMemory& old);
NakedMemory build_bmp_file(int width, int height, int bitc, NakedMemory& pallete, NakedMemory& dib);

bool decolor24(int w, int h, NakedMemory& rgb, NakedMemory& pal, NakedMemory& new_rgb);
