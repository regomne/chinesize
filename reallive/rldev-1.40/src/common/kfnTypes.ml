(*
   RLdev: KFN type definitions
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

type parameter = param_type * param_flag list
and param_type = Any | Int | IntC | IntV | Str | StrC | StrV | ResStr
               | Special of (int * special_t * special_flag list) list | Complex of parameter list
and special_t = Named of string * parameter list | AsComplex of parameter list
and param_flag = Optional | Return | Uncount | Fake | TextObject | Tagged of string | Argc
and special_flag = NoParens
and flag = PushStore | IsJump | IsGoto | IsCond | IsTextout | NoBraces | IsLbr
and version_t = Class of string | Compare of (int * int * int * int -> bool)

let which_ident : (string * string -> string) ref = ref fst

let handle_module
  : (int -> string -> unit) ref
  = ref (fun num name -> failwith "not initialised")

let handle_opcode
  : (version_t list -> string -> string -> flag list -> int -> int -> int -> parameter list option list -> unit) ref
  = ref (fun verlimit ident ccstr flags op_type op_module op_function prototypes -> failwith "not initialised")
