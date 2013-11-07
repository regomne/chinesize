(*
    Rlc: compiler directive implementation
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
open Codegen

let resource loc expr =
  let fname = StrTokens.to_string (str_of_normalised_expr expr) ~enc:Config.default_encoding in
  let ic = try open_in fname with Sys_error e -> error loc e in
  if !App.verbose then ksprintf Optpp.sysInfo "Loading resources from `%s'" fname;
  try
    StrLexer.lex_resfile fname (KeULexer.lex_channel ic);
    close_in ic
  with e ->
    close_in ic;
    raise e

let getconst e =
  match !Global.expr__normalise_and_get_const e with
    | `Integer i -> `Integer i
    | `String s -> `String (Text.of_string "UTF8" (StrTokens.to_string s ~enc:"UTF8")) (*TODO: could be more efficient... *)

let set (loc, s, t, set, e) =
  try
    match Memory.get t with
      | (`Macro _ | `Integer _ | `String _), scoped
         -> Memory.mutate loc s t ((if set then getconst e else `Macro e), scoped)
      | _ -> Memory.memerr "not a constant"
  with Memory.MemoryError err ->
    ksprintf (error loc) "cannot mutate `%s': %s" s err

let ini_set (loc, s, t, expr) =
  Ini.set (Text.to_sjs t)
    [match const_of_normalised_expr expr with
      | `String s -> assert false
      | `Integer i -> `Integer i]

let generic =
  let as_string ?(enc = Config.default_encoding) expr =
    match const_of_normalised_expr expr with
      | `String s -> StrTokens.to_string s ~enc
      | `Integer i -> Int32.to_string i
  in
  fun loc expr ->
    function
      | "warn"-> warning (loc_of_expr expr) (as_string expr)
      | "error"-> error (loc_of_expr expr) (as_string expr)
      | "print" -> let loc = loc_of_expr expr in ksprintf Optpp.cliWarning "%s line %d: %s" loc.file loc.line (as_string expr)
      | "resource" -> resource loc expr
      | "val_0x2c" -> Global.val_0x2c := Int32.to_int (int_of_normalised_expr expr)
      | "character" -> DynArray.add Global.dramatis_personae (StrTokens.to_string (str_of_normalised_expr expr)) (* TODO: candidate for text transformation? *)
      | "entrypoint" -> let idx = Int32.to_int (int_of_normalised_expr expr) in
                        if idx < 0 || idx >= 100 
                        then ksprintf (error loc) "invalid entrypoint #Z%02d: valid values are 0..99" idx
                        else Output.add_entry idx
      | "kidoku_type" -> ini_set (loc, "KIDOKU_TYPE", Text.norm (Text.of_sjs "kidoku_type"), expr)
      | "file"
         -> if !App.outfile = "" then App.outfile := as_string expr ~enc:Config.default_encoding
      | _ -> assert false

let define dir loc s t def scoped = Memory.define t def ~scoped

let compile =
  function
    | `Directive (l, id, _, e) -> generic l e id
    | `DTarget (l, s) 
       -> if !target_forced
          then ksprintf (warning l) "target specified on command-line: ignoring #target directive"
          else global_target := target_t_of_string s ~err:(error l)
    | `Define (loc, s, t, scoped, expr) -> define "define" loc s t (`Macro expr) scoped
    | `DConst (loc, s, t, `Const, expr) -> define "const" loc s t (getconst expr) true
    | `DConst _ -> assert false (* removed in normalisation *)
    | `DInline (loc, s, t, scoped, ids, smts) -> define "inline" loc s t (`Inline (ids, smts)) scoped
    | `DUndef (_, l) -> List.iter (fun (loc, s, t) -> Memory.undefine loc s t) l
    | `DSet d -> set d
    | `DVersion (loc, a, b, c, d)
       -> let l = [a; b; c; d] in
          let l = List.map Global.expr__normalise_and_get_int l in
          let l = List.map Int32.to_int l in
          KeTypes.global_version := (match l with [a; b; c; d] -> a, b, c, d | _ -> assert false)
