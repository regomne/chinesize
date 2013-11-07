(*
   Rlc: RealLive-compatible assembler
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
open Optpp

let set_target s =
  KeTypes.global_target := KeTypes.target_t_of_string (String.lowercase s) ~err:(usageError ~app:App.app);
  KeTypes.target_forced := true

let enum_of_array a = (* stolen from DynArray.enum, remove if it makes it into ExtLib *)
    let rec make start =
        let idxref = ref 0 in
        let next () =
            if !idxref >= Array.length a then
                raise Enum.No_more_elements
            else
                let retval = Array.unsafe_get a !idxref in
                incr idxref;
                retval
        and count () =
            if !idxref >= Array.length a then 0
            else Array.length a - !idxref
        and clone () =
            make !idxref
        in
        Enum.make ~next:next ~count:count ~clone:clone
    in
    make 0

external seek_version : string -> int * int * int * int = "rldev_get_interpreter_version"
let target_interpreter = ref ""
and auto_target = ref true

let set_target_version s =
  auto_target := false;
  try
    let a = Array.make 4 0
    and l = List.map int_of_string (ExtString.String.nsplit s ".") in
    List.iteri (Array.set a) l;
    KeTypes.global_version := a.(0), a.(1), a.(2), a.(3)
  with
    | Failure "int_of_string"
    | Invalid_argument "index out of bounds"
       -> if Sys.file_exists s then
            target_interpreter := s
          else
            usageError "target version must be specified as either an interpreter filename or up to four decimal integers separated by points"

let set_interpreter fname =
  KeTypes.global_version := seek_version fname;
  if !KeTypes.global_target = `Default 
  then match String.lowercase (Filename.basename fname) with
    | "reallive" | "reallive.exe" -> KeTypes.global_target := `RealLive
    | "kinetic"  | "kinetic.exe"  -> KeTypes.global_target := `Kinetic
    | "avg2000"  | "avg2000.exe"  -> KeTypes.global_target := `Avg2000
    | _ -> ()


(* Short forms used: defgioOtuvx *)

let options =
  [
    Opt { short = ""; long = "help"; argname = "";
          descr = "display this usage information";
          withoutarg = Some (fun () -> failwith "help");
          witharg = None };
    Opt { short = ""; long = "version"; argname = "";
          descr = "display " ^ App.app.name ^ " version information";
          withoutarg = Some (fun () -> failwith "version");
          witharg = None };
    Opt { short = "-v"; long = "verbose"; argname = "";
          descr = "describe what " ^ App.app.name ^ " is doing";
          withoutarg = set_flag App.verbose true;
          witharg = None };
    Break;
    Opt { short = "-o"; long = "output"; argname = "FILE";
          descr = "override output filename";
          withoutarg = None;
          witharg = set_string App.outfile "output filename" };
    Opt { short = "-d"; long = "outdir"; argname = "DIR";
          descr = "place output file in DIR";
          withoutarg = None;
          witharg = set_string App.outdir "output directory" };
    Opt { short = "-i"; long = "ini"; argname = "FILE";
          descr = "specify GAMEEXE.INI to use at compile-time (default: try to find automatically)";
          withoutarg = None;
          witharg = set_string App.gameexe "GAMEEXE.INI" };
    Opt { short = "-e"; long = "encoding"; argname = "ENC";
          descr = sprintf "input text encoding (default: %s)" Config.default_encoding;
          withoutarg = None;
          witharg = Some (fun e -> App.enc := String.uppercase e) };
    Opt { short = "-x"; long = "transform-output"; argname = "ENC";
          descr = "output encoding transformation (default: none)";
          withoutarg = None;
          witharg = Some (fun e -> TextTransforms.set_encoding e) };
    Opt { short = "";   long = "force-transform"; argname = "";
          descr = "don't abort when input can't be represented in output encoding";
          withoutarg = set_flag TextTransforms.force_encode true;
          witharg = None; };
    Opt { short = "-t"; long = "target"; argname = "TGT";
          descr = "specify target as RealLive, AVG2000, or Kinetic (default: try to autodetect, fall back on RealLive)";
          withoutarg = None;
          witharg = Some set_target };
    Opt { short = "-f"; long = "target-version"; argname = "VER";
          descr = "specify interpreter version, as either a version number or the filename of the interpreter (default: try to auto-detect)";
          withoutarg = None;
          witharg = Some set_target_version };
    Opt { short = "-u"; long = "uncompressed"; argname = "";
          descr = "don't compress and encrypt output";
          withoutarg = set_flag App.compress false;
          witharg = None };
    Opt { short = "-g"; long = "no-debug"; argname = "";
          descr = "strip debugging information";
          withoutarg = set_flag App.debug_info false;
          witharg = None };
    Opt { short = ""; long = "no-metadata"; argname = "";
          descr = "strip RLdev metadata (not recommended)";
          withoutarg = set_flag App.metadata false;
          witharg = None };
    Opt { short = ""; long = "no-assert"; argname = "";
          descr = "disable runtime assertions";
          withoutarg = set_flag App.assertions false;
          witharg = None };
    Opt { short = ""; long = "safe-arrays"; argname = "";
          descr = "enable runtime bounds-checking for arrays";
          withoutarg = set_flag App.array_bounds true;
          witharg = None };
    Opt { short = ""; long = "flag-labels"; argname = "";
          descr = "append labelled variable names to flag.ini";
          withoutarg = set_flag App.flag_labels true;
          witharg = None };
  (*Opt { short = "-O"; long = "optimisation"; argname = "LEV";
          descr = "set optimisation level (default 1, 0 to disable)";
          withoutarg = None;
          witharg = arg_to_flag
                      (fun s ->
                        try Int32.of_string s
                        with _ -> usageError ~app:App.app "argument to -O must be an integer")
                      App.opt_level "optimisation level"; };*)
  ]

let () =
  try
    let file = ref "" in
    getopt options ~app:App.app
      (function "" -> () | x -> if !file = "" then file := x  else failwith "help")
      (fun () -> failwith "help");
    if !file = "" then failwith "help";

    (* Detect the target version if possible. *)
    if !auto_target || !target_interpreter <> "" then
     (if !target_interpreter <> "" then
        set_interpreter !target_interpreter
      else
        let find d =
          Filename.concat d
            (Enum.find
              (fun s -> List.mem (String.lowercase s) ["reallive.exe"; "kinetic.exe"; "avg2000.exe"])
              (enum_of_array (Sys.readdir d)))
        in
        let dir = 
          let ge = !App.gameexe in
          if ge = ""
          then "."
          else if String.lowercase (Filename.basename ge) = "gameexe.ini"
          then Filename.dirname ge
          else (App.gameexe := Filename.concat ge "gameexe.ini"; ge)
        in
        try
          set_interpreter (find dir)
        with Not_found ->
          ());

    CompilerFrame.compile !file
  with
    | Failure "help"    -> display_help App.app options
    | Failure "version" -> display_version App.app
    | Failure e         -> sysError e
    | Text.Bad_char c   -> ksprintf sysError "unable to encode the character U+%04x. Check input and output encoding settings" c
