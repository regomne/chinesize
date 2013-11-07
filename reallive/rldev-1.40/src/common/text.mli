(*
    RLdev: unicode handling interface
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

type t
exception Bad_char of int

module Buf:
  sig
    type buf
    val create : int -> buf
    val add_int : buf -> int -> unit
    val add_char : buf -> char -> unit
    val add_string : buf -> t -> unit
    val add_array : buf -> int array -> unit
    val length : buf -> int
    val contents : buf -> t
    val clear : buf -> unit
  end

val append : t -> t -> t
val length : t -> int
val iter : (int -> unit) -> t -> unit
val map : (int -> int) -> t -> t
val empty : t

val to_arr : t -> int array
val of_arr : int array -> t
val of_char : int -> t

val to_string : string -> t -> string (* Convert to given encoding *)
val of_string : string -> string -> t (* Convert from given encoding *)
val to_err : t -> string (* Convert to native encoding - used for file names as well as errors *)
val of_err : string -> t (* Convert from native encoding - used for file names etc *)

val to_sjs : t -> string
val of_sjs : string -> t

val sjs_to_enc : string -> string -> string
val norm : t -> t
val ustream : string -> in_channel -> int Stream.t
val sjs_to_err : string -> string

val ident : string -> t (* Convert from CP932, memoise result *)