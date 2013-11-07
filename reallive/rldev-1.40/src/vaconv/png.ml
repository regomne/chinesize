(*
   vaconv: PNG format
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
*)

open Image

external get_size : string -> int * int * bool = "va_get_png_size"
external read : string -> rgb -> unit = "va_read_png"
external write : rgb -> int -> bool -> string -> unit = "va_write_png"

let of_file fname =
  let width, height, mask = get_size fname in
  let rgba = new_rgba width height in
  read fname rgba;
  { width = width;
    height = height;
    data = `RGBA rgba;
    metadata = None; }

let to_file img fname format =
  if format <> "" then Optpp.sysWarning "subformats are not supported for PNG output";
  let pixels, bpp =
    match img.data with
      | `RGB rgb -> rgb, 3
      | `RGBA rgba -> rgba, 4
      | `RGBM _ -> get_as_rgba img, 4
  in
  write pixels bpp !App.brutal fname;
  Option.map string_of_metadata img.metadata
  
let check_header ic =
  let s = String.create 8 in
  ignore (input ic s 0 8);
  s = "\x89PNG\x0d\x0a\x1a\x0a"
  
let _ =
  formats#register
    { name = "PNG";
      extensions = ["PNG"];
      of_file = of_file;
      to_file = to_file;
      check_header = check_header;
      use_meta = false;
      default_conv_ext = "G00"; }
