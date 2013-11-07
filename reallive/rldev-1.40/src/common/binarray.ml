(*
   Binarray: bigarray wrapper for byte twiddling
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
(*syntax camlp4o *)
(*ppopt pa_macro.cmo *)

let buffer_size = 4_194_304

type t = (int, Bigarray.int8_unsigned_elt, Bigarray.c_layout) Bigarray.Array1.t

let create = Bigarray.Array1.create Bigarray.int8_unsigned Bigarray.c_layout
let blit = Bigarray.Array1.blit
let sub = Bigarray.Array1.sub
let map_file fd = Bigarray.Array1.map_file fd Bigarray.int8_unsigned Bigarray.c_layout
let dim = Bigarray.Array1.dim
let fill = Bigarray.Array1.fill

let copy arr =
  let rv = create (dim arr) in
  blit arr rv;
  rv

external read : t -> idx:int -> len:int -> string = "string_of_binary_array"
external unsafe_read_buf : t -> buffer:string -> idx:int -> len:int -> offs:int -> unit = "string_from_binary_array"
external read_sz : t -> idx:int -> len:int -> string = "cstring_of_binary_array"
external unsafe_read_sz : t -> idx:int -> string = "unsafe_cstring_of_binary_array"
external write : t -> idx:int -> string -> unit = "string_to_binary_array"
external write_sz : t -> idx:int -> len:int -> string -> unit = "cstring_to_binary_array"
external unsafe_char_of_int : int -> char = "%identity"

external digest : t -> Digest.t = "digest_array"

let put_int32 arr ~idx i =
  arr.{idx} <- Int32.to_int (Int32.logand i 0xff_l);
  arr.{idx + 1} <- Int32.to_int (Int32.logand (Int32.shift_right i 8) 0xff_l);
  arr.{idx + 2} <- Int32.to_int (Int32.logand (Int32.shift_right i 16) 0xff_l);
  arr.{idx + 3} <- Int32.to_int (Int32.logand (Int32.shift_right i 24) 0xff_l)

let put_i16 arr ~idx i =
  arr.{idx} <- i land 0xff;
  arr.{idx + 1} <- (i asr 8) land 0xff

let put_int arr ~idx i =
  put_i16 arr ~idx i;
  arr.{idx + 2} <- (i asr 16) land 0xff;
  arr.{idx + 3} <- (i asr 24) land 0xff

let get_int32 arr ~idx =
  Int32.add
    (Int32.of_int arr.{idx})
    (Int32.add
      (Int32.shift_left (Int32.of_int arr.{idx + 1}) 8)
      (Int32.add
        (Int32.shift_left (Int32.of_int arr.{idx + 2}) 16)
        (Int32.shift_left (Int32.of_int arr.{idx + 3}) 24)))

let get_i16 arr ~idx =
  arr.{idx} + (arr.{idx + 1} lsl 8)

let get_int arr ~idx =
  get_i16 arr ~idx + (arr.{idx + 2} lsl 16) + (arr.{idx + 3} lsl 24)

let read_input fname : t =
  if not (Sys.file_exists fname) then Printf.ksprintf failwith "file `%s' not found" fname;
  IFDEF WIN32 THEN
    let ic = open_in_bin fname in
    let rlen = in_channel_length ic in
    let rv = create rlen in
    let buffer = String.create (min rlen buffer_size) in
    let rec loop idx len =
      if len = 0 then (
        close_in ic;
        rv
      )
      else
        let read = input ic buffer 0 (min len buffer_size) in
        write rv idx (String.sub buffer 0 read);
        loop (idx + read) (len - read)
    in
    loop 0 (in_channel_length ic)
  ELSE
    let fdescr =
      try
        Unix.openfile fname [Unix.O_RDONLY] 0
      with
        Unix.Unix_error (e, _, _)
         -> Printf.ksprintf failwith "cannot read file `%s': %s" fname (String.uncapitalize (Unix.error_message e))
    in
    let arr = map_file fdescr false ~-1 in
    let rv = create (dim arr) in
    blit arr rv;
    rv
  END

let map_output fname len =
  try
    assert (len > 0);
    IFDEF WIN32 THEN Hackery.force_access fname ELSE () END;
    let f = Unix.openfile fname [Unix.O_RDWR; Unix.O_CREAT; Unix.O_TRUNC] 0o755 in
    map_file f true len, f
  with
    | Unix.Unix_error (e, _, _)
     -> Printf.ksprintf failwith "cannot write to file `%s': %s" fname (String.uncapitalize (Unix.error_message e))

let write_file arr fname =
  let oa, od = map_output fname (dim arr) in
  try
    blit arr oa;
    Unix.close od
  with
    e -> Unix.close od;
         raise e

let output arr oc =
  let buffer = String.create (min (dim arr) buffer_size) in
  let rec loop idx len =
    if len > 0 then
      let left = min len buffer_size in
      unsafe_read_buf arr ~buffer ~idx ~len:left ~offs:0;
      if left = buffer_size then
        output_string oc buffer
      else
        output_string oc (String.sub buffer 0 left);
      loop (idx + left) (len - left)
  in
  loop 0 (dim arr)
