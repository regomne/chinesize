/*
   vaconv: PDT bit-twiddling.
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

/* Compress RGB data in PDT10 format */
value va_compress_pdt10 (value arr)
{
  AVG32Comp::Compress<AVG32Comp::CInfoPDT> cmp;
  char *data = (char*) Data_bigarray_val(arr);
  cmp.WriteData (data, Bigarray_val(arr)->dim[0] * Bigarray_val(arr)->dim[1] * Bigarray_val(arr)->dim[2]);
  cmp.WriteDataEnd();
  cmp.Deflate();
  cmp.Flush();
  memmove (data, cmp.Data(), cmp.Length());
  return Val_long(cmp.Length());
}

/* Compress alpha data */
value va_compress_pdtmask (value arr)
{
  AVG32Comp::Compress<AVG32Comp::CInfoMask> cmp;
  char *data = (char*) Data_bigarray_val(arr);
  cmp.WriteData (data, Bigarray_val(arr)->dim[0] * Bigarray_val(arr)->dim[1]);
  cmp.WriteDataEnd();
  cmp.Deflate();
  cmp.Flush();
  memmove (data, cmp.Data(), cmp.Length());
  return Val_long(cmp.Length());
}

/* Decompress PDT10 format to RGB data */
value va_decompress_pdt10 (value src_in, value dst_in, value mask_in)
{
  CAMLparam3(src_in, dst_in, mask_in);
  uchar *src = Binarray_val(src_in) + 32;
  uchar *buf = Binarray_val(dst_in);
  uchar *srcend = Binarray_val(src_in) + (Int_val(mask_in) ? Int_val(mask_in) : Bigarray_val(src_in)->dim[0]);
  uchar *bufend = buf + Bigarray_val(dst_in)->dim[0] * Bigarray_val(dst_in)->dim[1] * Bigarray_val(dst_in)->dim[2];
  uchar flag;
  int bit = 0;
  while ((src < srcend) && (buf < bufend)) {
    if (bit == 0) {
      bit = 8;
      flag = *src++;
    }
    if (flag & 0x80) {
      *buf++ = *src++;
      *buf++ = *src++;
      *buf++ = *src++;
    }
    else {
      int i, count;
      uchar *rp;
      count = *src++;
      count += (*src++) << 8;
      rp = buf - ((count >> 4) + 1) * 3;
      count = ((count & 0x0f) + 1) * 3;
      if (rp < Binarray_val(dst_in) || rp >= buf) raise_malformed;
      for (i = 0; i < count && buf < bufend; i++) *buf++ = *rp++;
    }
    bit--;
    flag <<= 1;
  }
  CAMLreturn(Val_unit);
}

/* Decompress PDT11 format to RGB data */
value va_decompress_pdt11 (value src_in, value dst_in, value mask_in)
{
  CAMLparam3(src_in, dst_in, mask_in);
  uchar *srcbuf = Binarray_val(src_in);
  uchar *src = srcbuf + 0x460;
  uchar *buf = Binarray_val(dst_in);
  uchar *srcend = Binarray_val(src_in) + (Int_val(mask_in) ? Int_val(mask_in) : Bigarray_val(src_in)->dim[0]);
  uchar *bufend = buf + Bigarray_val(dst_in)->dim[0] * Bigarray_val(dst_in)->dim[1] * Bigarray_val(dst_in)->dim[2];
  uchar flag;
  int i, index[16], bit = 0;
  for (i = 0; i < 16; i++)
    index[i] = (srcbuf[i * 4 + 0x420])
             | (srcbuf[i * 4 + 0x421] << 8)
             | (srcbuf[i * 4 + 0x422] << 16)
             | (srcbuf[i * 4 + 0x423] << 24);

  while ((src < srcend) && (buf < bufend)) {
    if (bit == 0) {
      bit = 8;
      flag = *src++;
    }
    if (flag & 0x80) {
      int n = (*src++) * 4 + 0x20;
      *buf++ = srcbuf[n];
      *buf++ = srcbuf[n + 1];
      *buf++ = srcbuf[n + 2];
    }
    else {
      uchar *rp;
      int num = *src++;
      int count = (((num >> 4) & 0x0f) + 2) * 3;
      if (buf + count >= bufend) count = bufend - buf;
      rp = buf - index[num & 0x0f] * 3;
      if (rp < Binarray_val(dst_in) || rp >= buf) raise_malformed;
      for (i = 0; i < count && buf < bufend; i++) *buf++ = *rp++;
    }
    bit--;
    flag <<= 1;
  }
  CAMLreturn(Val_unit);
}

/*  Decompress alpha data */
value va_decompress_pdtmask (value src_in, value dst_in, value mask_in)
{
  CAMLparam3(src_in, dst_in, mask_in);
  uchar *src = Binarray_val(src_in) + Int_val(mask_in);
  uchar *buf = Binarray_val(dst_in);
  uchar *srcend = Binarray_val(src_in) + Bigarray_val(src_in)->dim[0];
  uchar *bufend = buf + Bigarray_val(dst_in)->dim[0] * Bigarray_val(dst_in)->dim[1];
  uchar flag;
  int bit = 0;
  while ((src < srcend) && (buf < bufend)) {
    if (bit == 0) {
      bit = 8;
      flag = *src++;
    }
    if (flag & 0x80)
      *buf++ = *src++;
    else {
      int i, count = (*src++) + 2;
      uchar *rp = buf - ((*src++) + 1);
      if (rp < Binarray_val(dst_in) || rp >= buf) raise_malformed;
      for (i = 0; i < count && buf < bufend; i++) *buf++ = *rp++;
    }
    bit--;
    flag <<= 1;
  }
  CAMLreturn(Val_unit);
}

} /* extern "C" */
