/*
   vaconv: g00 bit-twiddling.
   Copyright (C) 2006 Haeleth.

   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.

   You should have received a copy of the GNU General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "lzcomp.h"
extern "C" {
#include "rldev.h"

/* Brute-force compressors: they produce better results than the
   algorithm in lzcomp.h, but are 20-30 times slower.
   Based on code by Ed Keyes, with a few minor optimisations. */
   
inline int ek_findbestmatchType0 (uchar *source, long sourcelen, uchar *currentpos, int *foundoffset)
{
  int bestlength = 0;
  int i, j;

  i = 1;
  while ((currentpos - i * 3 >= source) &&   // stop at beginning of data
         (i < 4096) &&   // limit to 12-bit offset
         (bestlength < 16)) {  // quit if we already find an optimal match
    j = 0;
    // only check if this position might hold a longer match
    if (currentpos[3 * bestlength] == (currentpos - 3 * i)[3 * bestlength] &&
        currentpos[3 * bestlength + 1] == (currentpos - 3 * i)[3 * bestlength + 1] &&
        currentpos[3 * bestlength + 2] == (currentpos - 3 * i)[3 * bestlength + 2]) {
      while ((currentpos + 3 * j < source + sourcelen) &&  // stop at end of data
             (currentpos[3 * j] == (currentpos - 3 * i)[3 * j]) &&  // do we match?
             (currentpos[3 * j + 1] == (currentpos - 3 * i)[3 * j + 1]) &&
             (currentpos[3 * j + 2] == (currentpos - 3 * i)[3 * j + 2]) &&
             (j < 16)) {   // limit to 4-bit size
        ++j;
        if (j > bestlength) {
          bestlength = j;
          *foundoffset = i;
        }
      }
    }
    ++i;
  }
  return bestlength;
}

long ek_LZSSCompressType0 (uchar *source, long sourcelen, uchar *dest)
{
  int size, offset, bitcount;
  long compressedsize;
  uchar *control;
  uchar *cursrc, *curdest;

  compressedsize = 0;
  cursrc = source;
  bitcount = 8;
  curdest = dest;
  while (cursrc < source + sourcelen) {
    if (bitcount >  7) {
      control = curdest++;
      *control = 0;
      bitcount = 0;
    }
    size = ek_findbestmatchType0 (source, sourcelen, cursrc, &offset);
    if (size > 0) {  // use an offset for 1-pixel matches or above
      // a zero flag bit in the control byte is already there
      *curdest++ = ((offset & 0x0f) << 4) | (size - 1);
      *curdest++ = (offset >> 4);
      cursrc += size * 3;
    }
    else {  // store raw pixel
      *control = (*control) | (0x01 << bitcount);  // flag for raw byte
      *curdest++ = *cursrc++;
      *curdest++ = *cursrc++;
      *curdest++ = *cursrc++;
    }
    ++bitcount;
  }
  return curdest - dest;
}

inline int ek_findbestmatch(uchar *source, long sourcelen, uchar *currentpos, int *foundoffset)
{
  int bestlength = 0;
  int i, j;

  i = 1;
  while ((currentpos - i >= source) &&   // stop at beginning of data
         (i < 4096) &&   // limit to 12-bit offset
         (bestlength < 17)) {  // quit if we already find an optimal match
    j = 0;
    // only check if this position might hold a longer match
    if (currentpos[bestlength] == (currentpos - i)[bestlength]) {
      while ((currentpos + j < source + sourcelen) &&  // stop at end of data
             (currentpos[j] == (currentpos - i)[j]) &&  // do we actually match?
             (j < 17)) {   // limit to 4-bit size
        ++j;
        if (j > bestlength) {
          bestlength = j;
          *foundoffset = i;
        }
      }
    }
    ++i;
  }
  return bestlength;
}


long ek_LZSSCompress(uchar *source, long sourcelen, uchar *dest)
{
  int size, offset, bitcount;
  long compressedsize;
  uchar *control;
  uchar *cursrc, *curdest;

  compressedsize = 0;
  cursrc = source;
  bitcount = 8;
  curdest = dest;
  while (cursrc < source + sourcelen) {
    if (bitcount > 7) {
      control = curdest++;
      *control = 0;
      bitcount = 0;
    }
    size = ek_findbestmatch (source, sourcelen, cursrc, &offset);
    if (size > 1) {  // use an offset for 2-byte matches or above
      // a zero flag bit in the control byte is already there
      *curdest++ = ((offset & 0x0f) << 4) | (size - 2);
      *curdest++ = (offset >> 4);
      cursrc += size;
    }
    else {  // store raw byte
      *control = (*control) | (0x01 << bitcount);  // flag for raw byte
      *curdest++ = *cursrc++;
    }
    ++bitcount;
  }
  return curdest - dest;
}

/* Decompress RGB data in g00 format 0 */
value va_decompress_g00_0 (value src_in, value dst_in)
{
  CAMLparam2(src_in, dst_in);
  uchar *src = Binarray_val(src_in);
  uchar *srcend = src + Bigarray_val(src_in)->dim[0];
  uchar *buf = Binarray_val(dst_in);
  uchar *bufend = buf + Bigarray_val(dst_in)->dim[0] * Bigarray_val(dst_in)->dim[1] * Bigarray_val(dst_in)->dim[2];
  int bit = 1;
  uchar flag = *src++;
  while ((src < srcend) && (buf < bufend)) {
    if (bit == 256) {
      flag = *src++;
      bit = 1;
    }
    if (flag & bit) {
      *buf++ = *src++;
      *buf++ = *src++;
      *buf++ = *src++;
    }
    else {
      int i, count;
      uchar *rp;
      count = *src++;
      count += (*src++) << 8;
      rp = buf - (count >> 4) * 3;
      count = ((count & 0x0f) + 1) * 3;
      if (rp < Binarray_val(dst_in) || rp >= buf) raise_malformed;
      for (i = 0; i < count && buf < bufend; ++i) *buf++ = *rp++;
    }
    bit <<= 1;
  }
  CAMLreturn(Val_unit);
}


/* Compress data for g00 format 0 */
value va_compress_g00_0 (value arr, value brute_force)
{
  if Bool_val(brute_force) {
    long ilen = Bigarray_val(arr)->dim[0] * Bigarray_val(arr)->dim[1] * Bigarray_val(arr)->dim[2];
    uchar *data = (uchar*) malloc (ilen * 8 / 7);
    long length = ek_LZSSCompressType0 (Binarray_val(arr), ilen, data);
    memmove (Binarray_val(arr), data, length);
    return Val_long(length);
  }
  else {
    AVG32Comp::Compress<AVG32Comp::CInfoG00, AVG32Comp::Container::RLDataContainer> cmp;
    char *data = (char*) Data_bigarray_val(arr);
    cmp.WriteData (data, Bigarray_val(arr)->dim[0] * Bigarray_val(arr)->dim[1] * Bigarray_val(arr)->dim[2]);
    cmp.WriteDataEnd();
    cmp.Deflate();
    cmp.Flush();
    memmove (data, cmp.Data(), cmp.Length());
    return Val_long(cmp.Length());
  }
}

/* Decompress data for g00 format 1 */
value va_decompress_g00_1 (value src_in, value dst_in)
{
  CAMLparam2(src_in, dst_in);
  uchar *src = Binarray_val(src_in);
  uchar *srcend = src + Bigarray_val(src_in)->dim[0];
  uchar *buf = Binarray_val(dst_in);
  uchar *bufend = buf + Bigarray_val(dst_in)->dim[0];
  int bit = 1;
  uchar flag = *src++;
  while ((src < srcend) && (buf < bufend)) {
    if (bit == 256) {
      bit = 1;
      flag = *src++;
    }
    if (flag & bit) {
      *buf++ = *src++;
    }
    else {
      int i, count;
      uchar *rp;
      count = *src++;
      count += (*src++) << 8;
      rp = buf - (count >> 4);
      count = (count & 0x0f) + 2;
      if (rp < Binarray_val(dst_in) || rp >= buf) raise_malformed;
      for (i = 0; i < count && buf < bufend; ++i) *buf++ = *rp++;
    }
    bit <<= 1;
  }
  CAMLreturn(Val_unit);
}

/* Compress data for g00 format 1 */
value va_compress_g00_1 (value arr, value brute_force)
{
  if Bool_val(brute_force) {
    long ilen = Bigarray_val(arr)->dim[0];
    uchar *data = (uchar*) malloc (ilen * 8 / 7);
    long length = ek_LZSSCompress (Binarray_val(arr), ilen, data);
    memmove (Binarray_val(arr), data, length);
    return Val_long(length);    
  }
  else {
    AVG32Comp::Compress<AVG32Comp::CInfoRealLive, AVG32Comp::Container::RLDataContainer> cmp;
    char *data = (char*) Data_bigarray_val(arr);
    cmp.WriteData (data, Bigarray_val(arr)->dim[0]);
    cmp.WriteDataEnd();
    cmp.Deflate();
    cmp.Flush();
    memmove (data, cmp.Data(), cmp.Length());
    return Val_long(cmp.Length());
  }
}

} /* extern "C" */
