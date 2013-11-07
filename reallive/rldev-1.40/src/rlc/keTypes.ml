(*
    Rlc: assembler type definitions
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
open Optpp

(* Utility functions *)

let (//) a b = Filename.concat a b

let str_of_int32 v =
  let rv = String.create 4 in
  rv.[0] <- Char.unsafe_chr (Int32.to_int (Int32.logand v 0xffl));
  rv.[1] <- Char.unsafe_chr (Int32.to_int (Int32.logand (Int32.shift_right_logical v 8) 0xffl));
  rv.[2] <- Char.unsafe_chr (Int32.to_int (Int32.logand (Int32.shift_right_logical v 16) 0xffl));
  rv.[3] <- Char.unsafe_chr (Int32.to_int (Int32.logand (Int32.shift_right_logical v 24) 0xffl));
  rv

let str_of_int v =
  let rv = String.create 4 in
  rv.[0] <- Char.unsafe_chr (v land 0xff);
  rv.[1] <- Char.unsafe_chr ((v lsr 8) land 0xff);
  rv.[2] <- Char.unsafe_chr ((v lsr 16) land 0xff);
  rv.[3] <- Char.unsafe_chr ((v lsr 24) land 0xff);
  rv

let output_int oc v =
  output_byte oc (v land 0xff);
  output_byte oc ((v lsr 8) land 0xff);
  output_byte oc ((v lsr 16) land 0xff);
  output_byte oc ((v lsr 24) land 0xff)

let str16_of_int v =
  let rv = String.create 2 in
  rv.[0] <- Char.unsafe_chr (v land 0xff);
  rv.[1] <- Char.unsafe_chr ((v lsr 8) land 0xff);
  rv


(* Reporting functions *)

type location = { file: string; line: int }

let error where what =
  ksprintf cliError "Error (%s line %d): %s." where.file where.line (Text.sjs_to_err what)

let warning where what =
  ksprintf cliWarning "Warning (%s line %d): %s." where.file where.line (Text.sjs_to_err what)

let info where what =
  ksprintf cliWarning "%s line %d: %s" where.file where.line (Text.sjs_to_err what)

let nowhere = { file = "generated code"; line = ~-1 }


(* Target settings *)

type target_t = [ `Default | `RealLive | `Avg2000 | `Kinetic | `Compare of int * int * int * int -> bool ]

let target_t_of_string ?(err = sysError) =
  function
    | "reallive" | "2" -> `RealLive
    | "avg2000" | "1" -> `Avg2000
    | "kinetic" | "3" -> `Kinetic
    | s -> ksprintf err "unknown target `%s'" s

let global_target = ref `Default
and global_version = ref (0, 0, 0, 0)
and target_forced = ref false

let current_version () =
  if !global_version <> (0, 0, 0, 0) then !global_version
  else if !global_target = `Avg2000 then 1, 0, 0, 0
  else 1, 2, 7, 0

let current_version_string () =
  let i =
    match !global_target with
      | `Default | `RealLive -> "RealLive"
      | `Avg2000 -> "AVG2000"
      | `Kinetic -> "Kinetic"
      | _ -> assert false
  in
  match current_version () with
    | a, b, 0, 0 -> sprintf "%s %d.%d" i a b
    | a, b, c, 0 -> sprintf "%s %d.%d.%d" i a b c
    | a, b, c, d -> sprintf "%s %d.%d.%d.%d" i a b c d

let has_goto_if () = !global_target <> `Kinetic


(* RealLive API handling *)

let ident_of_opcode op_type op_module op_code op_overload =
  sprintf "__op_%d_%d_%d_%d" op_type op_module op_code op_overload

type func =
  { ident: string;
    flags: KfnTypes.flag list;
    op_type: int;
    op_module: int;
    op_code: int;
    prototypes: prototype array;
    targets: target_t list; }

and prototype = KfnTypes.parameter list option

let functions = Hashtbl.create 0
and ctrlcodes = Hashtbl.create 0
and gotofuncs = DynArray.create ()
let () =
  KfnTypes.handle_opcode :=
    (fun targets ident' ccstr flags op_type op_module op_function prototypes ->
      let ident = if ident' = "" then ident_of_opcode op_type op_module op_function 0 else ident' in
      let fd =
        { ident = ident;
          flags = flags;
          op_type = op_type;
          op_module = op_module;
          op_code = op_function;
          prototypes = Array.of_list (List.map (Option.map (List.filter (fun (_, l) -> not (List.mem KfnTypes.Fake l)))) prototypes);
          targets = List.map 
                      (function KfnTypes.Class s -> target_t_of_string s | KfnTypes.Compare c -> `Compare c) 
                      targets; } 
      in
      let tident = Text.ident ident in
      Hashtbl.add functions tident fd;
      if ccstr <> "" then Hashtbl.add ctrlcodes (Text.ident ccstr) { fd with ident = ccstr };
      if List.mem KfnTypes.IsGoto flags then DynArray.add gotofuncs (ident, tident))

let valid_opcode =
  function 
    | { targets = [] } 
       -> (* special case for unconstrained functions *)
          true 
    | { targets = ts } 
       -> (* identify current target interpreter *)
          let interpreter = if !global_target = `Default then `RealLive else !global_target
          and version = current_version () in
          (* split target constraints into permitted interpreter classes and version constraints *)
          let interpreters, versions = 
            List.fold_left 
              (fun (t, v) c -> match c with `Compare f -> t, f :: v | _ -> c :: t, v)
              ([], []) ts
          in
          (* check version matches all version constraints *)
          List.for_all (fun f -> f version) versions
          (* if an interpreter constraint exists, check the target fits it *)
          && (interpreters = [] || List.mem interpreter interpreters)

(* Given a list of options [opts] for the function [s], return one that is valid for
   the currently defined interpreter class and version. *)
let ver_fun s opts =
  try
    List.find valid_opcode opts
  with Not_found when s <> "" -> 
    ksprintf sysError "the function `%s' is not supported in %s" s (current_version_string ())

let rlfun s = ver_fun s (Hashtbl.find_all functions (Text.ident s))
let ccode s = ver_fun s (Hashtbl.find_all ctrlcodes (Text.ident s))

let function_type f =
  if List.mem KfnTypes.PushStore f.flags then 
    `Int
  else try
    match
      List.find
        (fun (_, pattrs) -> List.mem KfnTypes.Return pattrs)
        (Option.get 
          (List.find 
            ((<>) None)
            (Array.to_list f.prototypes)))
    with
      | KfnTypes.Int, _
          -> `Int
      | KfnTypes.Str, _
          -> `Str
      | _ -> ksprintf sysError "error in reallive.kfn: invalid return type for function `%s'" f.ident
  with Not_found ->
    `None


let modules = Hashtbl.create 0
let () =
  KfnTypes.handle_module :=
    (fun num name ->
      Hashtbl.add modules (Text.ident name) num)

let init () =
  (* Read function definition file *)
  let ic = open_in (Filename.concat (Config.prefix ()) "reallive.kfn") in
  if !App.verbose then sysInfo "Reading function definitions";
  KfnParser.parse KfnLexer.lex (Lexing.from_channel ic);
  close_in ic
