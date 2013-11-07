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

open Printf
open ExtList
open KeTypes

type parameter =
  [ `String of string
  | `Integer of string
  | `Unknown of string
  | `List of parameter list
  | `Special of char * KfnTypes.special_flag list * parameter list
  | `Literal of string ]


let rec parameters_to_string buffer prev =
  function
    | [] -> ()
    | p :: ps -> parameter_to_string buffer prev p; parameters_to_string buffer (Some p) ps

and parameter_to_string buffer prev =
  function
    | `String s
    | `Unknown s -> Buffer.add_string buffer s
    | `List l -> Buffer.add_char buffer '('; parameters_to_string buffer None l; Buffer.add_char buffer ')'
    | `Special (c, f, l) 
     -> bprintf buffer "a%c" c; 
        if List.mem KfnTypes.NoParens f 
        then parameters_to_string buffer None l
        else parameter_to_string buffer None (`List l)
    | `Integer s
     -> (* Precede unary operators with commas, if necessary. *)
        if s.[0] = '\\' then
          (match prev with
            | None | Some (`List _ | `Literal _) -> ()
            | Some (`Special (_, f, _)) when not (List.mem KfnTypes.NoParens f) -> ()
            | _ -> Buffer.add_char buffer ',');
        Buffer.add_string buffer s
    | `Literal s
     -> (* Precede string literals with commas, if necessary. *)
        if Buffer.length buffer > 0 then
          (match prev with
            | Some (`Literal _) -> Buffer.add_char buffer ','
            | Some (`Special (_, f, _)) when List.mem KfnTypes.NoParens f -> Buffer.add_char buffer ','
            | _ -> ());
        Buffer.add_string buffer (if s = "" then "\"\"" else s)


let get_prototype_lengths =
  (* Retrieve the number of parameters expected for each prototype in a function;
     -1 indicates that the prototype is missing or can take an arbitrary number. *)
  let protolen_cache = Hashtbl.create 0 in
  fun func ->
    try
      Hashtbl.find protolen_cache func
    with
      Not_found ->
        let found_arb = ref false in
        let rv =
          Array.map
            (function
              | None -> (* assert we have a maximum of one arbitrary length prototype *)
                        assert (let av = not !found_arb in found_arb := true; av);
                        -1, -1
              | Some prototype ->
                List.fold_left
                  (fun (alen, blen) (_, pflags) ->
                    if alen = -1 then -1, -1 else
                      if List.mem KfnTypes.Argc pflags then (
                        assert (let av = not !found_arb in found_arb := true; av);
                        -1, -1
                      )
                      else
                        if List.mem KfnTypes.Optional pflags then
                          alen, blen + 1
                        else
                          alen + 1, blen + 1)
                  (0, 0) prototype)
            func.prototypes
        in
        Hashtbl.add protolen_cache func rv;
       rv


let choose_overload loc func argc =
  (* Return the index of the prototype matching a given parameter count. *)
  let _, arb_idx, nonarbs =
    Array.fold_left
      (fun (i, ai, na) (min, max) ->
        if min = -1 then
          (i + 1), i, na
        else if min = max then
          (i + 1), ai, (i, min) :: na
        else
          let rec loop n acc =
            if n = max then
              (i, n) :: acc
            else
              loop (n + 1) ((i, n) :: acc)
          in
          (i + 1), ai, loop min na)
      (0, -1, [])
      (get_prototype_lengths func)
  in
  if Array.length func.prototypes <= 1
  then 0
  else try
    let idx, _ = List.find (fun (_, len) -> len = argc) nonarbs in
    idx
  with
    Not_found ->
      if arb_idx = -1 then
        ksprintf (error loc) "unable to find a prototype for `%s' that matches these parameters" func.ident
      else
        arb_idx


let compile_function loc ?overload ?returnval func parameters =
  let argc = List.length parameters + (if returnval = None || List.mem KfnTypes.PushStore func.flags then 0 else 1) in
  let overload', parameters', argc', append =
    let overload' =
      match overload with
        | Some value -> value
        | None -> choose_overload loc func argc
    in
    if func.prototypes = [||] || func.prototypes.(overload') = None then
      if returnval <> None && not (List.mem KfnTypes.PushStore func.flags) then
        error loc "the assignment syntax is only valid for functions with prototypes"
      else
        overload', parameters, argc, ""
    else
      let prototype = Option.get func.prototypes.(overload') in
      let _, rvpos, argcmod =
        List.fold_left
          (fun (i, rvpos, argcmod) (_, pflags) ->
            i + 1,
            (if List.mem KfnTypes.Return pflags then i else rvpos),
            (if List.mem KfnTypes.Uncount pflags then argcmod + 1 else argcmod))
          (0, -1, 0)
          prototype
      in
      let parameters', append =
        match rvpos, returnval with
          | -1, (None | Some "$\xc8"(* rval = store *)) -> parameters, ""
          | -1, Some rval
           -> if List.mem KfnTypes.PushStore func.flags
              then parameters, sprintf "%s\\\x1e$\xc8" rval (* simulate return values *)
              else ksprintf (error loc) "the function `%s' does not return a value" func.ident
          | _, None -> ksprintf (error loc) "return value of function `%s' cannot be ignored" func.ident
          | _, Some rval -> let pre, post = List.split_nth rvpos parameters in pre @ [`Unknown rval] @ post, ""
      in
      overload', parameters', argc - argcmod, append
  in
  let b = Buffer.create 0 in
  Buffer.add_string b (Codegen.code_of_opcode func.op_type func.op_module func.op_code argc' overload');
  if parameters' <> [] then (
    Buffer.add_string b "(";
    parameters_to_string b None parameters';
    Buffer.add_string b ")"
  );
  Buffer.contents b, if append = "" then None else Some append

let compile_function_str loc ?overload ?returnval func parameters =
  match compile_function loc ?overload ?returnval func parameters with
    | s, None -> s
    | s, Some t -> s ^ t
