(*
   Kprl: SEEN.TXT archiving, encryption, and compression handling
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
open Binarray
open Bytecode


(* Stubs for routines in C. *)
external c_decompress: Binarray.t -> Binarray.t -> unit = "rl_prim_decompress"
external c_apply_mask: Binarray.t -> int -> unit = "rl_prim_apply_mask"
external c_compress: Binarray.t -> int = "rl_prim_compress"


(* Return an equivalent to a bytecode file with decompressed and decrypted
   data.  The input array is modified. *)
let decompress arr =
  let hdr = read_file_header arr in
  c_apply_mask arr hdr.data_offset;
  match hdr.compressed_size with
    | None -> arr
    | Some i -> let rv = create (hdr.data_offset + hdr.uncompressed_size) in
                blit (sub arr 0 hdr.data_offset) (sub rv 0 hdr.data_offset);
                c_decompress (sub arr hdr.data_offset i) (sub rv hdr.data_offset hdr.uncompressed_size);
                rv


(* Return an equivalent to an uncompressed bytecode file with compressed and
  encrypted data.  The input array is not modified. *)
let compress arr =
  let hdr = read_file_header arr in
  let data_offset = hdr.data_offset in
  if hdr.compressed_size = None then
    let rv = create (dim arr) in
    blit arr rv;
    put_int rv 4 hdr.compiler_version;
    c_apply_mask rv data_offset;
    rv
  else
    let buffer = create ((dim arr - data_offset) * 9 / 8 + 9) in
    let to_compress = sub buffer 8 (dim arr - data_offset) in
    blit (sub arr (data_offset - 8) (dim arr - data_offset + 8)) (sub buffer 0 (dim arr - data_offset + 8));
    let compressed_size = c_compress to_compress + 8 in
    let rv = create (data_offset + compressed_size) in
    blit (sub arr 0 data_offset) (sub rv 0 data_offset);
    put_int buffer ~idx:0 compressed_size;
    put_int buffer ~idx:4 (dim to_compress);
    put_int rv ~idx:0 0x1d0;
    put_int rv ~idx:4 hdr.compiler_version;
    put_int rv ~idx:0x28 compressed_size;
    blit (sub buffer 0 compressed_size) (sub rv data_offset compressed_size);
    c_apply_mask rv data_offset;
    rv
