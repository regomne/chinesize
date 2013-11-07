(*
   Kprl: RealLive archiver and disassembler
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

open Optpp
open Printf
open Disassembler
open ExtList

(* Auxiliary functions *)

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

let action: Archiver.action ref =
  ref (false, fun _ -> usageError "you must specify one of the actions -l, -b, -x, -d, -c, -k")

let set_action =
  let action_set = ref 0 in
  fun x () ->
    incr action_set;
    match !action_set with
      | 1 -> action := x
      | 2 -> sysWarning "Only the first specified action will be used"
      | _ -> ()

let set_target s =
  match String.lowercase s with
    | "reallive" | "2" -> Disassembler.options.forced_target <- `RealLive
    | "avg2000" | "avg2k" | "1" -> Disassembler.options.forced_target <- `Avg2000
    | "kinetic" | "3" -> Disassembler.options.forced_target <- `Kinetic
    | _ -> ksprintf (usageError ~app:App.app) "unknown target `%s'" s

let set_target_version s =
  App.auto_target := false;
  try
    let a = Array.make 4 0
    and l = List.map int_of_string (ExtString.String.nsplit s ".") in
    List.iteri (Array.set a) l;
    App.target_version := a.(0), a.(1), a.(2), a.(3)
  with
    | Failure "int_of_string"
    | Invalid_argument "index out of bounds"
       -> if Sys.file_exists s then
            App.target_interpreter := s
          else
            usageError "target version must be specified as either an interpreter filename or up to four decimal integers separated by points"

(* Option definitions *)
(* Short options used: abcdfgklnNorsStuvx *)

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
          witharg =  None };
    Break;

    Opt { short = "-a"; long = "add"; argname = "";
          descr = "add to or update files in archive";
          withoutarg = Some (set_action Archiver.add);
          witharg = None };
    Opt { short = "-k"; long = "delete"; argname = "";
          descr = "remove files from archive";
          withoutarg = Some (set_action Archiver.remove);
          witharg = None };
    Opt { short = "-l"; long = "list"; argname = "";
          descr = "list archive contents";
          withoutarg = Some (set_action Archiver.list);
          witharg = None };
    Opt { short = "-d"; long = "disassemble"; argname = "";
          descr = "disassemble RealLive or AVG2000 bytecode";
          withoutarg = Some (set_action Archiver.disassemble);
          witharg = None };
    Break;

    Opt { short = "-x"; long = "extract"; argname = "";
          descr = "extract and decompress files";
          withoutarg = Some (set_action Archiver.extract);
          witharg = None };
    Opt { short = "-b"; long = "break"; argname = "";
          descr = "extract files without decompressing them";
          withoutarg = Some (set_action Archiver.break);
          witharg = None };
    Opt { short = "-c"; long = "compress"; argname = "";
          descr = "compress files without archiving them";
          withoutarg = Some (set_action Archiver.pack);
          witharg = None };
    Break;

    Message "Options when listing:";
    Break;
    Opt { short = "-N"; long = "names"; argname = "";
          descr = "list dramatis personae for each scenario instead of sizes";
          withoutarg = set_flag App.names_opt true;
          witharg = None };
    Break;

    Message "Options when extracting or disassembling:";
    Break;
    Opt { short = "-o"; long = "outdir"; argname = "DIR";
          descr = "place files in DIR";
          withoutarg = None;
          witharg = set_string App.outdir "output directory" };
    Break;

    Message "Options when disassembling only:";
    Break;
    Opt { short = "-e"; long = "encoding"; argname = "ENC";
          descr = sprintf "output text encoding (default: %s)" Config.default_encoding;
          withoutarg = None;
          witharg = Some (fun e -> App.enc := String.uppercase e) };
    Opt { short = ""; long = "bom"; argname = "";
          descr = "when output encoding is UTF-8, include BOM (byte-order mark)";
          withoutarg = set_flag App.bom true;
          witharg = None; };
    Opt { short = "-s"; long = "single-file"; argname = "";
          descr = "don't put text into a separate resource file";
          withoutarg = Some (fun () -> if Disassembler.options.separate_all then failwith "you cannot use -s and -S together";
                                       Disassembler.options.separate_strings <- false);
          witharg = None; };
    Opt { short = "-S"; long = "separate-all"; argname = "";
          descr = "put absolutely all Japanese text in the resource file";
          withoutarg = Some (fun () -> if not Disassembler.options.separate_strings then failwith "you cannot use -s and -S together";
                                       Disassembler.options.separate_all <- true);
          witharg = None; };
    Opt { short = "-u"; long = "unreferenced"; argname = "";
          descr = "suppress unreferenced code";
          withoutarg = Some (fun () -> Disassembler.options.suppress_uncalled <- true);
          witharg = None; };
    Opt { short = "-n"; long = "annotate"; argname = "";
          descr = "annotate output with offsets and parameter names";
          withoutarg = Some (fun () -> Disassembler.options.annotate <- true);
          witharg = None; };
    Opt { short = "-r"; long = "no-codes"; argname = "";
          descr = "don't generate control codes in strings";
          withoutarg = Some (fun () -> Disassembler.options.control_codes <- false);
          witharg = None; };
    Opt { short = "-g"; long = "debug-info"; argname = "";
          descr = "read debugging information";
          withoutarg = Some (fun () -> Disassembler.options.read_debug_symbols <- true);
          witharg = None; };
    Opt { short = "-t"; long = "target"; argname = "TGT";
          descr = "specify source as RealLive, AVG2000, or Kinetic";
          withoutarg = None;
          witharg = Some set_target };
    Opt { short = "-f"; long = "target-version"; argname = "VER";
          descr = "specify interpreter version, as either a version number or the filename of the interpreter (default: try to auto-detect)";
          withoutarg = None;
          witharg = Some set_target_version };
    Opt { short = ""; long = "force-transform"; argname = "ENC";
          descr = "";
          withoutarg = None;
          witharg = Some (fun s -> App.force_meta := Some (TextTransforms.enc_of_string s)) };
  ]

(* Main program *)

let main =
  let files = ref [] in
  try
    (* Parse command-line options. *)
    getopt options (function "" -> () | x -> files := !files @ [x]) (fun () -> failwith "help") ~app:App.app;
    if !files = [] then usageError "no input files";

    (* Detect the target version if possible. *)
    if !App.auto_target || !App.target_interpreter <> "" then
     (let fname =
        if !App.target_interpreter <> "" then 
          !App.target_interpreter 
        else
          let find d =
            Filename.concat d
              (Enum.find
                (fun s -> List.mem (String.lowercase s) ["reallive.exe"; "kinetic.exe"; "avg2000.exe"])
                (enum_of_array (Sys.readdir d)))
          in
          let fname =
            let dir = Filename.dirname (List.hd !files) in
            try
              find dir
            with Not_found ->
              try
                find (Filename.concat dir Filename.parent_dir_name)
              with Not_found ->
                ""
          in
          App.target_interpreter := fname;
          fname
      in
      if fname <> "" then App.target_version := seek_version fname);

    (* Create an output directory if necessary. *)
    let create_outdir, action = !action in
    if create_outdir && !App.outdir <> "" then (
      let rec mkpath dir =
        if try let r = Unix.stat dir in r.Unix.st_kind <> Unix.S_DIR with _ -> true
        then begin
          mkpath (Filename.dirname dir);
          Unix.mkdir dir 0o755
        end
      in try mkpath !App.outdir
      with Unix.Unix_error (e, _, _) ->
        ksprintf sysError "cannot create directory `%s': %s" !App.outdir
          (match e with
            | Unix.EACCES -> "permission denied"
            | Unix.EROFS  -> "device is read-only"
            | Unix.ENOSPC -> "no space left on device"
            | _ -> String.uncapitalize (Unix.error_message e))
    );

    (* Run the selected command. *)
    action !files

  with
    | Failure "help"    -> display_help App.app options
    | Failure "version" -> display_version App.app
    | Failure e         -> sysError e
