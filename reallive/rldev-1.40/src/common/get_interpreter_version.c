/*
   kprl: interface to Win32 version query functions (and substitute for other platforms)
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

#include "get_interpreter_version.h"

/* If we have the Windows API to use, life is relatively simple. */

#if defined(WINNT) || defined(__WINNT) || defined(__WINNT__) ||\
    defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) ||\
    defined(__CYGWIN__)

#include <windows.h>

value rldev_get_interpreter_version (value filename)
{
	CAMLparam1 (filename);
	CAMLlocal1 (result);
	DWORD unused, bytes;
#ifdef __CYGWIN__
	char fname[MAX_PATH];
	cygwin_conv_to_win32_path (String_val(filename), fname);  
#else
#	define fname String_val(filename)
#endif
	result = caml_alloc_tuple (4);
	Store_field (result, 0, Val_long(0));
	Store_field (result, 1, Val_long(0));
	Store_field (result, 2, Val_long(0));
	Store_field (result, 3, Val_long(0));
	if (bytes = GetFileVersionInfoSize (fname, &unused)) {
		void *data = malloc (bytes);
		if (GetFileVersionInfo (fname, 0, bytes, data)) {
			VS_FIXEDFILEINFO *info;
			if (VerQueryValue (data, TEXT("\\"), (LPVOID*)&info, (PUINT)&bytes)) {
				Store_field (result, 0, Val_long(info->dwFileVersionMS >> 16));
				Store_field (result, 1, Val_long(info->dwFileVersionMS & 0xffff));
				Store_field (result, 2, Val_long(info->dwFileVersionLS >> 16));
				Store_field (result, 3, Val_long(info->dwFileVersionLS & 0xffff));
			}
		}
		free (data);
	}
	CAMLreturn (result);
}

#else

/* On other platforms, we have to read the executable file itself. */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

inline uint32_t get_i16 (FILE* f)
{
	uint32_t c = getc (f);
	return c | (getc (f) << 8);
}

inline uint32_t get_int (FILE* f)
{
	uint32_t c = getc (f);
	c |= getc (f) << 8;
	c |= getc (f) << 16;
	return c | getc (f) << 24;
}

int get_pe_versioninfo (const char *filename, long a[4])
{
	FILE *f = fopen (filename, "rb");
	if (f != NULL) {
		/* Read PE header */
		long offs;
		fseek (f, 60, SEEK_SET);
		offs = get_int (f);
		/* Check for PE header */
		if (fseek (f, offs, SEEK_SET) == 0 && get_int (f) == 0x4550) {
			int OptHeaderLen;
			char snam[8];
			fseek (f, 16, SEEK_CUR);
			OptHeaderLen = get_i16 (f);
			fseek (f, 2 + OptHeaderLen, SEEK_CUR);
			/* Start of data directory: look for .rsrc section */
			do { 
				fread (snam, 1, 8, f);
				if (strncmp (snam, ".rsrc", 8) == 0) break;
				fseek (f, 32, SEEK_CUR);
			} while (snam[0]);
			/* If we found a .rsrc section, read it. */
			if (snam[0]) {
				fseek (f, 12, SEEK_CUR);
				offs = get_int (f);
				/* Try to find the resource directory it points to. */
				if (fseek (f, offs, SEEK_SET) == 0) {
					/* We could probably navigate the resource tree neatly to find
					   the VS_FIXEDFILEINFO structure, but frankly we're so close
					   now that we may as well just brute-force things. */
					uint32_t check = 0;
					while (!feof (f) && check != 0xFEEF04BD) check = get_int (f);
					if (check == 0xFEEF04BD && fseek (f, 4, SEEK_CUR) == 0) {
						a[1] = get_i16 (f);
						a[0] = get_i16 (f);
						a[3] = get_i16 (f);
						a[2] = get_i16 (f);
						fclose (f);
						return 1;
					}
				}
			}
		}
		fclose (f);
	}
	return 0;
}

value rldev_get_interpreter_version (value filename)
{
	CAMLparam1 (filename);
	CAMLlocal1 (result);
	result = caml_alloc_tuple (4);
	Store_field (result, 0, Val_long(0));
	Store_field (result, 1, Val_long(0));
	Store_field (result, 2, Val_long(0));
	Store_field (result, 3, Val_long(0));
	{
		long a[4];
		if (get_pe_versioninfo (String_val(filename), a)) {
			Store_field (result, 0, Val_long(a[0]));
			Store_field (result, 1, Val_long(a[1]));
			Store_field (result, 2, Val_long(a[2]));
			Store_field (result, 3, Val_long(a[3]));
		}
	}
	CAMLreturn (result);
}

#endif
