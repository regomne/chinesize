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

type t = (int, Bigarray.int8_unsigned_elt, Bigarray.c_layout) Bigarray.Array1.t

(* Functions basically the same as Bigarray.Array1 *)
val create : int -> t
val blit : t -> t -> unit
val sub : t -> int -> int -> t
val map_file : Unix.file_descr -> bool -> int -> t
val dim : t -> int
val fill : t -> int -> unit

(* Functions similar to the usual IO thingies *)
val read : t -> idx:int -> len:int -> string
val read_sz : t -> idx:int -> len:int -> string
val get_int : t -> idx:int -> int
val get_i16 : t -> idx:int -> int
val get_int32 : t -> idx:int -> int32
val put_int : t -> idx:int -> int -> unit
val put_i16 : t -> idx:int -> int -> unit
val put_int32 : t -> idx:int -> int32 -> unit
val write : t -> idx:int -> string -> unit            (* Writes the string data 'as is' *)
val write_sz : t -> idx:int -> len:int -> string -> unit  (* Writes the string data, pads to end of slice with zeroes *)

val unsafe_read_sz : t -> idx:int -> string
(* Reads a null-terminated string direct from memory.  May cause weird stuff if no null byte is found! *)

(* Other functions *)
val copy : t -> t
val digest : t -> Digest.t
val read_input : string -> t
val map_output : string -> int -> t * Unix.file_descr
val write_file : t -> string -> unit
val output : t -> out_channel -> unit