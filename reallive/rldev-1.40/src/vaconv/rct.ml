(*
   vaconv: Rokuchou True-Colour (Majiro) image format
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

open Printf
open Bigarray
open Image

let rokuchoutc_header = "\x98Z\x92\x9aTC00"

let of_file fname =
  let arr = Binarray.read_input fname in
  (* Read header *)
  let width = Binarray.get_int arr 0x08
  and height = Binarray.get_int arr 0x0c 
  and data_len = Binarray.get_int arr 0x10 
  in
  (* Create source and destination arrays *)
  let iarr = Binarray.sub arr 0x14 data_len 
  and oarr = Binarray.create (width * height * 3)
  in
  (* Decompress the data *)
  let rec blit ipos opos len =
    let len = len * 3 in
    Binarray.blit (Binarray.sub iarr ipos len) (Binarray.sub oarr opos len);
    loop (ipos + len) (opos + len)
  and repeat bytes ipos opos len =
    let bytes = bytes * 3 and len = len * 3 in
    for i = opos to opos + len - 1 do oarr.{i} <- oarr.{i - bytes} done;
    loop ipos (opos + len)
  and loop ipos opos =
    if ipos < data_len then
      let code = iarr.{ipos} and ipos = ipos + 1 in
      match char_of_int code with
        | '\x00'..'\x7e' -> blit ipos opos (code + 1)
        | '\x7f'         -> blit (ipos + 2) opos (128 + Binarray.get_i16 iarr ipos)
        | '\x80'..'\x82' -> repeat 1 ipos opos (code - 0x80 + 1)
        | '\x83'         -> repeat 1 (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x84'..'\x86' -> repeat 2 ipos opos (code - 0x84 + 1)
        | '\x87'         -> repeat 2 (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x88'..'\x8a' -> repeat 3 ipos opos (code - 0x88 + 1)
        | '\x8b'         -> repeat 3 (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x8c'..'\x8e' -> repeat 4 ipos opos (code - 0x8c + 1)
        | '\x8f'         -> repeat 4 (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x90'..'\x92' -> repeat 5 ipos opos (code - 0x90 + 1)
        | '\x93'         -> repeat 5 (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x94'..'\x96' -> repeat 6 ipos opos (code - 0x94 + 1)
        | '\x97'         -> repeat 6 (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x98'..'\x9a' -> repeat (width - 3) ipos opos (code - 0x98 + 1)
        | '\x9b'         -> repeat (width - 3) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\x9c'..'\x9e' -> repeat (width - 2) ipos opos (code - 0x9c + 1)
        | '\x9f'         -> repeat (width - 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xa0'..'\xa2' -> repeat (width - 1) ipos opos (code - 0xa0 + 1)
        | '\xa3'         -> repeat (width - 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xa4'..'\xa6' -> repeat  width      ipos opos (code - 0xa4 + 1)
        | '\xa7'         -> repeat  width      (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xa8'..'\xaa' -> repeat (width + 1) ipos opos (code - 0xa8 + 1)
        | '\xab'         -> repeat (width + 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xac'..'\xae' -> repeat (width + 2) ipos opos (code - 0xac + 1)
        | '\xaf'         -> repeat (width + 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xb0'..'\xb2' -> repeat (width + 3) ipos opos (code - 0xb0 + 1)
        | '\xb3'         -> repeat (width + 3) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xb4'..'\xb6' -> repeat (width * 2 - 3) ipos opos (code - 0xb4 + 1)
        | '\xb7'         -> repeat (width * 2 - 3) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xb8'..'\xba' -> repeat (width * 2 - 2) ipos opos (code - 0xb8 + 1)
        | '\xbb'         -> repeat (width * 2 - 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xbc'..'\xbe' -> repeat (width * 2 - 1) ipos opos (code - 0xbc + 1)
        | '\xbf'         -> repeat (width * 2 - 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xc0'..'\xc2' -> repeat (width * 2)     ipos opos (code - 0xc0 + 1)
        | '\xc3'         -> repeat (width * 2)     (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xc4'..'\xc6' -> repeat (width * 2 + 1) ipos opos (code - 0xc4 + 1)
        | '\xc7'         -> repeat (width * 2 + 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xc8'..'\xca' -> repeat (width * 2 + 2) ipos opos (code - 0xc8 + 1)
        | '\xcb'         -> repeat (width * 2 + 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xcc'..'\xce' -> repeat (width * 2 + 3) ipos opos (code - 0xcc + 1)
        | '\xcf'         -> repeat (width * 2 + 3) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xd0'..'\xd2' -> repeat (width * 3 - 3) ipos opos (code - 0xd0 + 1)
        | '\xd3'         -> repeat (width * 3 - 3) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xd4'..'\xd6' -> repeat (width * 3 - 2) ipos opos (code - 0xd4 + 1)
        | '\xd7'         -> repeat (width * 3 - 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xd8'..'\xda' -> repeat (width * 3 - 1) ipos opos (code - 0xd8 + 1)
        | '\xdb'         -> repeat (width * 3 - 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xdc'..'\xde' -> repeat (width * 3)     ipos opos (code - 0xdc + 1)
        | '\xdf'         -> repeat (width * 3)     (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xe0'..'\xe2' -> repeat (width * 3 + 1) ipos opos (code - 0xe0 + 1)
        | '\xe3'         -> repeat (width * 3 + 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xe4'..'\xe6' -> repeat (width * 3 + 2) ipos opos (code - 0xe4 + 1)
        | '\xe7'         -> repeat (width * 3 + 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xe8'..'\xea' -> repeat (width * 3 + 3) ipos opos (code - 0xe8 + 1)
        | '\xeb'         -> repeat (width * 3 + 3) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xec'..'\xee' -> repeat (width * 4 - 2) ipos opos (code - 0xec + 1)
        | '\xef'         -> repeat (width * 4 - 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)  
        | '\xf0'..'\xf2' -> repeat (width * 4 - 1) ipos opos (code - 0xf0 + 1)
        | '\xf3'         -> repeat (width * 4 - 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)  
        | '\xf4'..'\xf6' -> repeat (width * 4)     ipos opos (code - 0xf4 + 1)
        | '\xf7'         -> repeat (width * 4)     (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)  
        | '\xf8'..'\xfa' -> repeat (width * 4 + 1) ipos opos (code - 0xf8 + 1)
        | '\xfb'         -> repeat (width * 4 + 1) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)
        | '\xfc'..'\xfe' -> repeat (width * 4 + 2) ipos opos (code - 0xfc + 1)
        | '\xff'         -> repeat (width * 4 + 2) (ipos + 2) opos (4 + Binarray.get_i16 iarr ipos)  
  in
  blit 0 0 1;
  (* Return an internal image representation *)
  { width = width;
    height = height;
    data = `RGB (reshape_3 (genarray_of_array1 oarr) height width 3);
    metadata = None; }

let to_file img fname format =
  if format <> "" then Optpp.sysWarning "subformats are not supported for Rokuchou output";
  let rgb = 
    match get_separate img with
      | rgb, None -> rgb
      | rgb, _ -> Optpp.sysWarning "The true-colour Rokuchou format does not support masks, and no arrangements have been made to deal with this. Discarding alpha data for now"; rgb
  in
  let oc = open_out_bin fname in
  output_string oc rokuchoutc_header;
  output_byte oc (img.width land 0xff);
  output_byte oc ((img.width lsr 8) land 0xff);
  output_byte oc ((img.width lsr 16) land 0xff);
  output_byte oc ((img.width lsr 24) land 0xff);
  output_byte oc (img.height land 0xff);
  output_byte oc ((img.height lsr 8) land 0xff);
  output_byte oc ((img.height lsr 16) land 0xff);
  output_byte oc ((img.height lsr 24) land 0xff);
  output_string oc "    ";
  let bytes = linear_rgb rgb in
  (* We don't currently attempt to compress data.
     Majiro can use PNGs anyway, which have better compression, so converting
     INTO the Rokucho formats isn't critical. *)
  let rec loop pos rem =
    if rem != 0 then
      let chunk = min rem 0xffff in
      if chunk >= 128 then
        let clen = chunk - 128 in
        output_byte oc 0x7f;
        output_byte oc (clen land 0xff);
        output_byte oc ((clen lsr 8) land 0xff)
      else
        output_byte oc (chunk - 1);
      for i = pos to pos + (chunk * 3) - 1 do
        output_byte oc bytes.{i}
      done;
      loop (pos + chunk * 3) (rem - chunk)
  in
  output_byte oc bytes.{0};
  output_byte oc bytes.{1};
  output_byte oc bytes.{2};
  loop 3 (Binarray.dim bytes / 3 - 3);
  let len = pos_out oc - 0x14 in
  seek_out oc 0x10;
  output_byte oc (len land 0xff);
  output_byte oc ((len lsr 8) land 0xff);
  output_byte oc ((len lsr 16) land 0xff);
  output_byte oc ((len lsr 24) land 0xff);
  close_out oc;
  (* Return metadata *)
  Option.map string_of_metadata img.metadata
  
let check_header ic =
  let s = String.create 8 in
  ignore (input ic s 0 8);
  s = rokuchoutc_header
  
let _ =
  formats#register
    { name = "RCT";
      extensions = ["RCT"];
      of_file = of_file;
      to_file = to_file;
      check_header = check_header;
      use_meta = false;
      default_conv_ext = "PNG"; }
