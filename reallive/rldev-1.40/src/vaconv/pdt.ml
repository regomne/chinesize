(*
   vaconv: PDT bitmap format
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

external decompress_pdt10 : Binarray.t -> rgb -> int -> unit = "va_decompress_pdt10"
external decompress_pdt11 : Binarray.t -> rgb -> int -> unit = "va_decompress_pdt11"
external decompress_mask : Binarray.t -> mask -> int -> unit = "va_decompress_pdtmask"

let of_file fname =
  let arr = Binarray.read_input fname in
  (* Read header *)
  let decompress_pdt =
    match Binarray.read_sz arr 0 8 with
      | "PDT10" -> decompress_pdt10
      | "PDT11" -> decompress_pdt11
      | other -> raise (Unsupported ("unknown PDT format " ^ other))
  in
  let width = Binarray.get_int arr 0x0c
  and height = Binarray.get_int arr 0x10
  and maskptr = Binarray.get_int arr 0x1c
  in
  (* Read data *)
  let rgb = new_rgb width height in
  decompress_pdt arr rgb maskptr;
  let mask =
    if maskptr == 0 then
      None
    else
      let mask = new_mask width height in
      decompress_mask arr mask maskptr;
      Some mask
  in
  (* Return internal representation *)
  { width = width;
    height = height;
    data =
      (match mask with
        | Some mask -> `RGBM (rgb, mask)
        | None -> `RGB rgb);
    metadata = None }


external compress_pdt10 : rgb -> int = "va_compress_pdt10"
external compress_mask : mask -> int = "va_compress_pdtmask"

let to_file img outname format =
  if format <> "" then Optpp.sysWarning "subformats are not (currently) supported for PDT output";
  (* Perform compression *)
  let rgb, mask = get_separate img in
  let rgb_len = compress_pdt10 rgb
  and mask_len =
    match mask with
      | Some mask -> compress_mask mask
      | None -> 0
  in
  (* Write header *)
  let len = rgb_len + mask_len + 0x20 in
  let arr = Binarray.create len in
  Binarray.write_sz arr 0 0x20 "PDT10";
  Binarray.put_int arr 0x08 len;
  Binarray.put_int arr 0x0c img.width;
  Binarray.put_int arr 0x10 img.height;
  (* Write data *)
  Binarray.blit (Binarray.sub (linear_rgb rgb) 0 rgb_len) (Binarray.sub arr 0x20 rgb_len);
  Option.may
    (fun m ->
      let mask_pos = rgb_len + 0x20 in
      Binarray.put_int arr 0x1c mask_pos;
      Binarray.blit (Binarray.sub (linear_mask m) 0 mask_len) (Binarray.sub arr mask_pos mask_len))
    mask;
  (* Output file *)
  Binarray.write_file arr outname;
  (* Return any metadata *)
  Option.map string_of_metadata img.metadata

let check_header ic =
  let s = String.create 8 in
  ignore (input ic s 0 8);
  s = "PDT10\000\000\000" || s = "PDT11\000\000\000"

let _ =
  formats#register
    { name = "PDT";
      extensions = ["PDT"];
      of_file = of_file;
      to_file = to_file;
      check_header = check_header;
      use_meta = false;
      default_conv_ext = "PNG"; }
