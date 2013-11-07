/*
   vaconv: general image bit-twiddling.
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

#include "rldev.h"

/* Separate RGBA data into RGB and mask */
value va_separate_rgba (value rgba, value rgb, value a)
{
  uchar *src = Binarray_val(rgba);
  uchar *bf1 = Binarray_val(rgb);
  uchar *bf2 = Binarray_val(a);
  uchar *end = src + Bigarray_val(rgba)->dim[0] * Bigarray_val(rgba)->dim[1] * 4;
  uchar mask_type = src[3] ? 0xff : 0x00;
  int has_mask = 0;
  while (src < end) {
    *bf1++ = *src++;
    *bf1++ = *src++;
    *bf1++ = *src++;
    *bf2 = *src++;
    if (!has_mask && *bf2 != mask_type) has_mask = 1;
    bf2++;
  }
  return Val_bool(has_mask);
}

/* Combine RGB and mask into RGBA data */
value va_combine_rgb_a (value rgb, value a, value rgba)
{
  uchar *src1 = Binarray_val(rgb);
  uchar *src2 = Binarray_val(a);
  uchar *buf = Binarray_val(rgba);
  uchar *end2 = src2 + Bigarray_val(a)->dim[0] * Bigarray_val(a)->dim[1];
  while (src2 < end2) {
    *buf++ = *src1++;
    *buf++ = *src1++;
    *buf++ = *src1++;
    *buf++ = *src2++;
  }
  return Val_unit;
}

/* Combine RGB and null mask into RGBA data */
value va_expand_rgb (value rgb, value rgba)
{
  uchar *src = Binarray_val(rgb);
  uchar *buf = Binarray_val(rgba);
  uchar *end = src + Bigarray_val(rgb)->dim[0] * Bigarray_val(rgb)->dim[1] * 3;
  while (src < end) {
    *buf++ = *src++;
    *buf++ = *src++;
    *buf++ = *src++;
    *buf++ = 0xff;
  }
  return Val_unit;
}
