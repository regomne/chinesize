(*
   vaconv: internal bitmap format
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

open Bigarray

exception Unsupported of string
exception Convert_error of string
exception Malformed
exception Bad_metadata of string
let () = Callback.register_exception "Malformed" Malformed

module RGB =
  struct
    type t = int * int * int
    let compare (a1, a2, a3) (b1, b2, b3) =
      let s1 = a1 - b1 in
      if s1 != 0 then s1 else
        let s2 = a2 - b2 in
        if s2 != 0 then s2 else
          a3 - b3
  end
module RGBSet = Set.Make (RGB)
module RGBMap = Map.Make (RGB)

module RGBA =
  struct
    type t = int * int * int * int
    let compare (a1, a2, a3, a4) (b1, b2, b3, b4) =
      let s1 = a1 - b1 in
      if s1 != 0 then s1 else
        let s2 = a2 - b2 in
        if s2 != 0 then s2 else
          let s3 = a3 - b3 in
          if s3 != 0 then s3 else
            a4 - b4
  end
module RGBASet = Set.Make (RGBA)
module RGBAMap = Map.Make (RGBA)

let too_many_colours = Convert_error "cannot create palette for image with more than 256 unique colours"
let get_unique_colours_rgb rgb =
  assert (Array3.dim3 rgb = 3);
  let rv = ref (RGBSet.empty) in
  for i = 0 to Array3.dim1 rgb - 1 do
    for j = 0 to Array3.dim2 rgb - 1 do
      rv := RGBSet.add (rgb.{i, j, 0}, rgb.{i, j, 1}, rgb.{i, j, 2}) !rv;
    done;
    if RGBSet.cardinal !rv > 256 then raise too_many_colours
  done;
  !rv
and get_unique_colours_rgba rgba =
  assert (Array3.dim3 rgba = 4);
  let rv = ref (RGBASet.empty) in
  for i = 0 to Array3.dim1 rgba - 1 do
    for j = 0 to Array3.dim2 rgba - 1 do
      rv := RGBASet.add (rgba.{i, j, 0}, rgba.{i, j, 1}, rgba.{i, j, 2}, rgba.{i, j, 3}) !rv;
    done;
    if RGBASet.cardinal !rv > 256 then raise too_many_colours
  done;
  !rv


type rgb = (int, int8_unsigned_elt, c_layout) Array3.t
type mask = (int, int8_unsigned_elt, c_layout) Array2.t

type t =
  { width: int;
    height: int;
    data: [`RGB of rgb | `RGBA of rgb | `RGBM of rgb * mask];
    metadata: (string * Xml.xml) option; }

type format =
  { name: string;
    extensions: string list;
    check_header: in_channel -> bool;
    of_file: string -> t;
    to_file: t -> string -> string -> string option;
    use_meta: bool;
    default_conv_ext: string; }

let dummy_format = 
  { name = ""; 
    extensions = []; 
    check_header = (fun _ -> false);
    of_file = (fun _ -> failwith "dummy"); 
    to_file = (fun _ _ _ -> failwith "dummy"); 
    use_meta = false;
    default_conv_ext = ""; }

let formats =
  object (self)
    val mutable known = []
    method register fmt = 
      known <- fmt :: known
    method find_extension ext =
      let ext = String.uppercase ext in 
      List.find (fun { extensions = e } -> List.mem ext e) known
    method find_by_header ic = 
      List.find (fun { check_header = f } -> seek_in ic 0; f ic) known
    method list_extensions =
      let exts = List.fold_left (fun acc { extensions = e } -> List.rev_append acc e) [] known in
      List.map String.uppercase (List.sort String.compare exts)
    method default_conversion fmt =
      self#find_extension fmt.default_conv_ext
  end

let new_rgb x y = Array3.create int8_unsigned c_layout y x 3
let new_rgba x y = Array3.create int8_unsigned c_layout y x 4
let linear_rgb a = reshape_1 (genarray_of_array3 a) (Array3.dim1 a * Array3.dim2 a * Array3.dim3 a)
let new_mask x y = Array2.create int8_unsigned c_layout y x
let linear_mask a = reshape_1 (genarray_of_array2 a) (Array2.dim1 a * Array2.dim2 a)

external rgba_of_rgb : rgb -> rgb -> unit = "va_expand_rgb"
external rgba_of_rgb_a : rgb -> mask -> rgb -> unit = "va_combine_rgb_a"
external rgb_a_of_rgba : rgb -> rgb -> mask -> bool = "va_separate_rgba"

let get_separate img =
  match img.data with
    | `RGB rgb
        -> assert (Array3.dim3 rgb = 3);
           rgb, None
    | `RGBM (rgb, m)
        -> assert (Array3.dim3 rgb = 3);
           rgb, Some m
    | `RGBA rgba
        -> assert (Array3.dim3 rgba = 4);
           let rgb = new_rgb img.width img.height
           and m = new_mask img.width img.height in
           if rgb_a_of_rgba rgba rgb m then
             rgb, Some m
           else
             rgb, None

let get_as_rgba img =
  match img.data with
    | `RGB rgb
        -> assert (Array3.dim3 rgb = 3);
           let rgba = new_rgba img.width img.height in
           rgba_of_rgb rgb rgba;
           rgba
    | `RGBM (rgb, m)
        -> assert (Array3.dim3 rgb = 3);
           let rgba = new_rgba img.width img.height in
           rgba_of_rgb_a rgb m rgba;
           rgba
    | `RGBA rgba
        -> assert (Array3.dim3 rgba = 4);
           rgba

let string_of_metadata (dtd, xml) =
  Printf.sprintf
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE %s SYSTEM \"%s\">\n%s"
    (Xml.tag xml)
    dtd
    (Xml.to_string_fmt xml)
