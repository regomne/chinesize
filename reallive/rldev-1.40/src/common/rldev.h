/*
   RLdev: general C definitions
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

#ifndef RLDEV_H
#define RLDEV_H

#include <caml/memory.h>
#include <caml/bigarray.h>
#include <caml/fail.h>
#include <caml/callback.h>

typedef unsigned char uchar;

#define Binarray_val (uchar*) Data_bigarray_val
#define raise_malformed caml_raise_constant (*caml_named_value("Malformed"))

#endif
