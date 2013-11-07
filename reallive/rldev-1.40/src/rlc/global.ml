(*
    Rlc: globally accessible data
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
open KeTypes
open KeAst

(* Workaround for a bug in 3.09.0 and 3.09.1, where 'printf "%0*ld" width value' does not pad the result *)

let int32_to_string_padded width value =
  let s = Int32.to_string value in
  let d = width - String.length s in
  if d > 0 then String.make d '0' ^ s else s

(* Damn limited apis *)

let dynArray e =
  let rv = DynArray.make 1 in
  DynArray.add rv e;
  rv

(* Damn manual recursion *)

let intrinsic__is_builtin : (Text.t -> bool) ref = ref (fun _ -> assert false)
let compilerFrame__parse : (statement DynArray.t -> unit) ref = ref (fun _ -> assert false)

let expr__normalise_and_get_const
  : (?abort_on_fail:bool -> ?expect:[ `Int | `None | `Str ] -> expression -> [ `Integer of int32 | `String of strtokens ]) ref
  = ref (fun ?(abort_on_fail = true) ?(expect = `None) e -> assert false)

let expr__normalise_and_get_int ?(abort_on_fail = true) e =
  match !expr__normalise_and_get_const e ~expect:`Int ~abort_on_fail with
    | `Integer i -> i
    | _ -> assert false

let expr__normalise_and_get_str ?(abort_on_fail = true) e =
  match !expr__normalise_and_get_const e ~expect:`Str ~abort_on_fail with
    | `String s -> s
    | _ -> assert false

let expr__disambiguate
  : ([ `Deref of location * string * Text.t * expression
     | `Func of location * string * Text.t * parameter list * label option
     | `VarOrFn of location * string * Text.t ] -> expression) ref
  = ref (fun _ -> assert false)

(* Header data *)

let dramatis_personae: string DynArray.t = DynArray.create ()
let val_0x2c = ref 0

(* Rlc-style resources *)

let resources : (Text.t, strtokens * location) Hashtbl.t = Hashtbl.create 0

let get_resource loc (key, keyt) =
  try
    Hashtbl.find resources (Text.norm keyt)
  with Not_found ->
    ksprintf (error loc) "undefined resource string `%s'" key

(* Miscellaneous *)

let unique =
  let src = ref Int64.min_int in
  fun () ->
    let rv = !src in
    src := Int64.succ rv;
    rv

let gloss_count = ref 0
