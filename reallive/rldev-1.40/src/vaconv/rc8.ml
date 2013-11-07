(*
   vaconv: Rokuchou 8-bit (Majiro) image format
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

let rokuchou8_header = "\x98Z\x92\x9a8_00"

let of_file fname =
  let arr = Binarray.read_input fname in
  (* Read header *)
  let width = Binarray.get_int arr 0x08
  and height = Binarray.get_int arr 0x0c 
  and data_len = Binarray.get_int arr 0x10 
  in
  (* Create source and destination arrays *)
  let iarr = Binarray.sub arr 0x314 data_len 
  and oarr = Binarray.create (width * height)
  in
  (* Decompress the data *)
  let rec blit ipos opos len =
    Binarray.blit (Binarray.sub iarr ipos len) (Binarray.sub oarr opos len);
    loop (ipos + len) (opos + len)
  and repeat bytes ipos opos len =
    for i = opos to opos + len - 1 do oarr.{i} <- oarr.{i - bytes} done;
    loop ipos (opos + len)
  and loop ipos opos =
    if ipos < data_len then
      let code = iarr.{ipos} and ipos = ipos + 1 in
      match char_of_int code with
        | '\x00'..'\x7e' -> blit ipos opos (code + 1)
        | '\x7f'         -> blit (ipos + 2) opos (128 + Binarray.get_i16 iarr ipos)
        | '\x80'..'\x86' -> repeat 1 ipos opos (code - 0x80 + 3)                      
        | '\x87'         -> repeat 1 (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\x88'..'\x8e' -> repeat 2 ipos opos (code - 0x88 + 3)
        | '\x8f'         -> repeat 2 (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\x90'..'\x96' -> repeat 3 ipos opos (code - 0x90 + 3)
        | '\x97'         -> repeat 3 (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\x98'..'\x9e' -> repeat 4 ipos opos (code - 0x98 + 3)
        | '\x9f'         -> repeat 4 (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xa0'..'\xa6' -> repeat (width - 3) ipos opos (code - 0xa0 + 3)
        | '\xa7'         -> repeat (width - 3) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xa8'..'\xae' -> repeat (width - 2) ipos opos (code - 0xa8 + 3)
        | '\xaf'         -> repeat (width - 2) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xb0'..'\xb6' -> repeat (width - 1) ipos opos (code - 0xb0 + 3)
        | '\xb7'         -> repeat (width - 1) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xb8'..'\xbe' -> repeat width ipos opos (code - 0xb8 + 3)
        | '\xbf'         -> repeat width (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xc0'..'\xc6' -> repeat (width + 1) ipos opos (code - 0xc0 + 3)
        | '\xc7'         -> repeat (width + 1) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xc8'..'\xce' -> repeat (width + 2) ipos opos (code - 0xc8 + 3)
        | '\xcf'         -> repeat (width + 2) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xd0'..'\xd6' -> repeat (width + 3) ipos opos (code - 0xd0 + 3)
        | '\xd7'         -> repeat (width + 3) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xd8'..'\xde' -> repeat (width * 2 - 2) ipos opos (code - 0xd8 + 3)
        | '\xdf'         -> repeat (width * 2 - 2) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xe0'..'\xe6' -> repeat (width * 2 - 1) ipos opos (code - 0xe0 + 3)
        | '\xe7'         -> repeat (width * 2 - 1) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xe8'..'\xee' -> repeat (width * 2) ipos opos (code - 0xe8 + 3)
        | '\xef'         -> repeat (width * 2) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)  
        | '\xf0'..'\xf6' -> repeat (width * 2 + 1) ipos opos (code - 0xf0 + 3)
        | '\xf7'         -> repeat (width * 2 + 1) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
        | '\xf8'..'\xfe' -> repeat (width * 2 + 2) ipos opos (code - 0xf8 + 3)
        | '\xff'         -> repeat (width * 2 + 2) (ipos + 2) opos (10 + Binarray.get_i16 iarr ipos)
  in
  blit 0 0 1;
  (* Depalettise the data *)
  let data = new_rgb width height 
  and paletted = reshape_2 (genarray_of_array1 oarr) height width
  in
  for j = 0 to height - 1 do
    for i = 0 to width - 1 do
      let pal = paletted.{j, i} * 3 in
      data.{j, i, 0} <- arr.{pal + 0x14};
      data.{j, i, 1} <- arr.{pal + 0x15};
      data.{j, i, 2} <- arr.{pal + 0x16};
    done
  done;
  (* Return an internal image representation *)
  { width = width;
    height = height;
    data = `RGB data;
    metadata = None; }

let to_file img fname format =
  if format <> "" then Optpp.sysWarning "subformats are not supported for Rokuchou output";
  let rgb = 
    match get_separate img with
      | rgb, None -> rgb
      | rgb, _ -> Optpp.sysWarning "The 8-bit Rokuchou format does not support masks, and no arrangements have been made to deal with this. Discarding alpha data for now"; rgb
  in
  let palette_map, palette_len = 
    RGBSet.fold (fun c (m, i) -> RGBMap.add c i m, i + 1) (get_unique_colours_rgb rgb) (RGBMap.empty, 0) 
  in
  assert (palette_len > 0 && palette_len <= 256);
  let bytes =
    let arr = Binarray.create (img.width * img.height) in
    let bytes = reshape_2 (genarray_of_array1 arr) img.height img.width in
    for i = 0 to img.width - 1 do
      for j = 0 to img.height - 1 do
        bytes.{j, i} <- RGBMap.find (rgb.{j, i, 0}, rgb.{j, i, 1}, rgb.{j, i, 2}) palette_map
      done
    done;
    arr
  in
  let hdr = Binarray.create 788 in
  Binarray.write hdr 0 rokuchou8_header;
  Binarray.put_int hdr 0x08 img.width;
  Binarray.put_int hdr 0x0c img.height;
  (*Binarray.put_int hdr 0x10 data_length;*)
  let pal = reshape_2 (genarray_of_array1 (Binarray.sub hdr 0x14 768)) 256 3 in
  RGBMap.iter (fun (r, g, b) idx -> pal.{idx, 0} <- r; pal.{idx, 1} <- g; pal.{idx, 2} <- b) palette_map;
  let oc = open_out_bin fname in
  Binarray.output hdr oc;
  let origin = pos_out oc in
  (* We don't currently attempt to compress data.
     Majiro can use PNGs anyway, which have better compression, so converting
     INTO the Rokucho formats isn't critical. *)
  output_byte oc bytes.{0};
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
      for i = pos to pos + chunk - 1 do
        output_byte oc bytes.{i}
      done;
      loop (pos + chunk) (rem - chunk)
  in
  loop 1 (Binarray.dim bytes - 1);
  let len = pos_out oc - origin in
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
  s = rokuchou8_header
  
let _ =
  formats#register
    { name = "RC8";
      extensions = ["RC8"];
      of_file = of_file;
      to_file = to_file;
      check_header = check_header;
      use_meta = false;
      default_conv_ext = "PNG"; }
