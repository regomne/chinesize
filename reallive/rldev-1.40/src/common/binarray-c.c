/*
   Binarray: functions in C
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

   Based on code from the Cryptokit library by Xavier Leroy, copyright 2002
   INRIA.  The full Cryptokit library is distributed under the GNU LGPL with a
   linking exception.  At the time of writing the library was available for
   download from http://pauillac.inria.fr/~xleroy/software.html.
*/

#include <string.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/bigarray.h>


/* Return a string corresponding to a slice of a Binarray.t */

CAMLprim value string_of_binary_array (value src_arr, value src_idx, value src_len)
{
	CAMLparam3 (src_arr, src_idx, src_len);
	CAMLlocal1 (retval);
	int len = Long_val(src_len);
	retval = alloc_string (len);
	memmove (String_val(retval), (char *) Data_bigarray_val(src_arr) + (Long_val(src_idx)), len);
	CAMLreturn (retval);
}


/* Fill a buffer from a slice of a Binarary.t */
CAMLprim value string_from_binary_array (value src_arr, value buffer, value src_idx, value src_len, value dst_idx)
{
	CAMLparam5 (src_arr, buffer, src_idx, src_len, dst_idx);
	char *dst = (String_val(buffer)) + (Long_val(dst_idx));
	memmove (dst, (char *) Data_bigarray_val(src_arr) + (Long_val(src_idx)), Long_val(src_len));
	CAMLreturn (Val_unit);
}


/* Return a null-terminated string from a slice of a Binarray.t */

CAMLprim value cstring_of_binary_array (value src_arr, value src_idx, value src_len)
{
	CAMLparam3 (src_arr, src_idx, src_len);
	CAMLlocal1 (retval);
	int len = Long_val(src_len);
	char *retstr = (char *) calloc (len + 1, 1);
	if (retstr == NULL) failwith ("unable to allocate internal buffer");
	memcpy (retstr, (char *) Data_bigarray_val(src_arr) + (Long_val(src_idx)), len);
	retval = copy_string (retstr);
	free (retstr);
	CAMLreturn (retval);
}

/* As above, but unsafe; the advantage is that you don't need to know how long it might be. */
CAMLprim value unsafe_cstring_of_binary_array (value src_arr, value src_idx)
{
	return copy_string ((char *) Data_bigarray_val(src_arr) + (Int_val(src_idx)));
}

/* Write a string to a Binarray.t */

CAMLprim value string_to_binary_array (value dst_arr, value dst_idx, value src_str)
{
	CAMLparam3 (dst_arr, dst_idx, src_str);
	int len = string_length (src_str), idx = Long_val(dst_idx);
	if (idx + len > Bigarray_val(dst_arr)->dim[0]) invalid_argument ("Binarray.write");
	memcpy ((char *) Data_bigarray_val(dst_arr) + idx, String_val(src_str), len);
	CAMLreturn (Val_unit);
}


/* Write a null-terminated string to a slice of a Binarray.t */

CAMLprim value cstring_to_binary_array (value dst_arr, value dst_idx, value dst_len, value src_str)
{
	CAMLparam4 (dst_arr, dst_idx, dst_len, src_str);
	int len = string_length (src_str), idx = Long_val(dst_idx), dlen = Long_val(dst_len);
	if (idx + dlen > Bigarray_val(dst_arr)->dim[0] || len > dlen) invalid_argument ("Binarray.write_sz");
	memcpy ((char *) Data_bigarray_val(dst_arr) + idx, String_val(src_str), len);
	memset ((char *) Data_bigarray_val(dst_arr) + idx + len, 0, dlen - len);
	CAMLreturn (Val_unit);
}


/* MD5 digest of a Binarray.t */

typedef struct {
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
} MD5Context;

CAMLextern void MD5Init (MD5Context *context);
CAMLextern void MD5Update (MD5Context *context, unsigned char *buf, unsigned int len);
CAMLextern void MD5Final (unsigned char *digest, MD5Context *ctx);

CAMLprim value digest_array (value v_iarr)
{
	CAMLparam1(v_iarr);
	CAMLlocal1(result);
	MD5Context context;
	int len = Bigarray_val(v_iarr)->dim[0];
	unsigned char *buf = Data_bigarray_val(v_iarr);

	MD5Init (&context);
	while (len > 0) {
		int block = (len > 8192) ? 8192 : len;
		MD5Update (&context, buf, block);
		buf += block;
		len -= block;
	}

	result = alloc_string (16);
	MD5Final (&Byte_u(result, 0), &context);
	CAMLreturn(result);
}
