(*
    Rlc: function assembler
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

type parameter =
    [ `Integer of string
    | `List of parameter list
    | `Literal of string
    | `Special of char * KfnTypes.special_flag list * parameter list
    | `String of string
    | `Unknown of string ]

val choose_overload
  : KeTypes.location -> KeTypes.func -> int 
  -> int

val compile_function 
  : KeTypes.location -> ?overload:int -> ?returnval:string -> KeTypes.func -> parameter list
  -> string * string option

val compile_function_str 
  : KeTypes.location -> ?overload:int -> ?returnval:string -> KeTypes.func -> parameter list
  -> string
