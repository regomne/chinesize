/*
   vaconv: libpng bindings
   Copyright (C) 2006 Haeleth

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

#include <png.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <caml/memory.h>
#include <caml/bigarray.h>

value va_get_png_size (value fname)
{
  CAMLparam1 (fname);
  CAMLlocal1 (rv);
  png_structp png_ptr;
  png_infop info_ptr;
  png_uint_32 width, height;
  int mask;

  FILE *fp = fopen (String_val(fname), "rb");
  if (fp == NULL)
    failwith ("unreported open failure");

  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose (fp);
    failwith ("generic png");
  }

  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) {
    fclose (fp);
    png_destroy_read_struct (&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    failwith ("out of memory");
  }

  if (setjmp (png_ptr->jmpbuf)) {
    fclose (fp);
    png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
    failwith ("generic png");
  }

  png_init_io (png_ptr, fp);
  png_read_info (png_ptr, info_ptr);
  width = png_get_image_width (png_ptr, info_ptr);
  height = png_get_image_height (png_ptr, info_ptr);
  mask = (png_get_color_type (png_ptr, info_ptr) & PNG_COLOR_MASK_ALPHA) || png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS);
  png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
  fclose (fp);

  rv = alloc_tuple (3);
  Store_field (rv, 0, Val_int(width));
  Store_field (rv, 1, Val_int(height));
  Store_field (rv, 2, Val_bool(mask));
  CAMLreturn (rv);
}

value va_read_png (value fname, value arr)
{
  CAMLparam2 (fname, arr);

  char *filename = String_val(fname);
  unsigned char *larr = Data_bigarray_val(arr);
  png_bytep *row_pointers;
  png_structp png_ptr;
  png_infop info_ptr;
  png_uint_32 width, height;
  int i, bit_depth, color_type, interlace_type;
  FILE *fp;
  size_t rowbytes;

  fp = fopen (filename, "rb");
  if (fp == NULL)
    failwith ("unreported open failure");

  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose (fp);
    failwith ("generic png");
  }

  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) {
    fclose (fp);
    png_destroy_read_struct (&png_ptr, NULL, NULL);
    failwith ("out of memory");
  }

  if (setjmp (png_ptr->jmpbuf)) {
    fclose (fp);
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    failwith ("generic png");
  }

  png_init_io (png_ptr, fp);
  png_read_info (png_ptr, info_ptr);
  png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb (png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb (png_ptr);

  if (bit_depth == 16)
    png_set_strip_16 (png_ptr);

  if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha (png_ptr);

  png_set_filler (png_ptr, 0xff, PNG_FILLER_AFTER);

  png_set_bgr(png_ptr);

  png_read_update_info (png_ptr, info_ptr);
  png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

  row_pointers = (png_bytep*) malloc (height * sizeof(png_bytep));

  for (i = 0; i < height; ++i)
    row_pointers[i] = larr + i * width * 4;

  png_read_image (png_ptr, row_pointers);
  png_read_end (png_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose (fp);
  free (row_pointers);
  CAMLreturn (Val_unit);
}

value va_write_png (value arr, value pix_len, value max_compression, value fname)
{
  CAMLparam4 (arr, pix_len, max_compression, fname);
  int pixlen = Int_val(pix_len);
  long width = Bigarray_val(arr)->dim[1], height = Bigarray_val(arr)->dim[0];
  char *filename = String_val(fname);

  int i;
  png_bytep *row_pointers;
  png_structp png_ptr;
  png_infop info_ptr;
  FILE *fp;

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    failwith ("generic png");

  info_ptr = png_create_info_struct  (png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct (&png_ptr, (png_infopp)NULL);
    failwith ("out of memory");
  }

  if (setjmp (png_ptr->jmpbuf)) {
    fclose (fp);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    failwith ("generic png");
  }

  fp = fopen (filename, "wb");
  if (fp == NULL)
    failwith ("unreported open failure");

  png_init_io (png_ptr, fp);

  if (Bool_val(max_compression))
    png_set_compression_level (png_ptr, Z_BEST_COMPRESSION);
  else
	png_set_compression_level (png_ptr, 2);

  png_set_IHDR (png_ptr, info_ptr, width, height, 8,
                (pixlen == 3) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  row_pointers = (png_bytep*) malloc (height * sizeof(png_bytep));

  for (i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep) Data_bigarray_val(arr) + i * width * pixlen;

  png_set_rows (png_ptr, info_ptr, row_pointers);
  png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);
  png_destroy_write_struct (&png_ptr, &info_ptr);
  free (row_pointers);
  fclose (fp);
  CAMLreturn (Val_unit);
}

#ifdef __cplusplus
}
#endif
