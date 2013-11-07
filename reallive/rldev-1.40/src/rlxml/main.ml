(*
   RlXml: convert RealLive auxiliary file formats <> XML
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
open ExtList

(* Option definitions *)
(* Short options used: ov *)

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
    Opt { short = "-o"; long = "output"; argname = "NAME";
          descr = "if only one file is being converted, sets the output filename, otherwise names the directory to place output in";
          withoutarg = None;
          witharg = set_string App.outdir "output name" };
  ]

(* Main program *)

let main =
  let files = ref [] in
  try
    (* Parse command-line options. *)
    getopt options (function "" -> () | x -> files := !files @ [x]) (fun () -> failwith "help") ~app:App.app;
    if !files = [] then usageError "no input files";

    (* Create an output directory if necessary. *)
    if !App.outdir <> "" && List.length !files > 1 then (
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

    match !files with
      | [single] -> Convert.convert single ~single:true
      | multiple -> List.iter Convert.convert multiple

  with
    | Failure "help"    -> display_help App.app options
    | Failure "version" -> display_version App.app
    | Failure e         -> sysError e
