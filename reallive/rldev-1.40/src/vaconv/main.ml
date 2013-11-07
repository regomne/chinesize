(*
   vaconv: VisualArt's bitmap format converter
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

(* Option definitions *)

let set_format var name =
  Some (fun s ->
    match !var with
      | { Image.name = "" }
         -> let nv = 
              try
                Image.formats#find_extension s
              with Not_found ->
                ksprintf sysError "unknown format `%s': valid options are %s" s (String.concat ", " (Image.formats#list_extensions))
            in var := nv
      | _ -> ksprintf sysWarning "only the first specified %s will be used" name)

let set_g00 =
  Some (fun s ->
    let subfmt =
      match s with
        | "0" | "1" | "2" -> s
        | _ -> ksprintf sysError "unknown G00 format %s: valid options are 0, 1, and 2" s
    in
    let ofmt = !App.oformat in
    App.oformat := Image.formats#find_extension "G00";
    App.osubfmt := subfmt;
    match ofmt.Image.name with
      | "" | "G00" -> ()
      | _ -> sysWarning "G00 format option is overriding previously specified output format")

let options =
  (* Short options used: dfgimoqv *)
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
    Opt { short = "-d"; long = "outdir"; argname = "DIR";
          descr = "place output file(s) in DIR";
          withoutarg = Some (fun () -> App.outdir := "PRESERVE");
          witharg = set_string App.outdir "output directory" };
    Opt { short = "-o"; long = "output"; argname = "FILE";
          descr = "override the default output filename and extension";
          withoutarg = None;
          witharg = set_string App.outfile "output" };
    Opt { short = "-f"; long = "format"; argname = "FMT";
          descr = sprintf "select an output format (%s)" (String.concat ", " (Image.formats#list_extensions));
          withoutarg = None;
          witharg = set_format App.oformat "output format" };
    Opt { short = "-g"; long = "g00"; argname = "FMT";
          descr = "select a G00 format to use for output (0, 1, or 2; default is auto-detect)";
          withoutarg = None;
          witharg = set_g00 };
    Opt { short = "-i"; long = "input-format"; argname = "FMT";
          descr = "select an input format (default is auto-detect)";
          withoutarg = None;
          witharg = set_format App.iformat "input format" };
    Opt { short = "-m"; long = "metadata"; argname = "FILE";
          descr = "override the default metadata filename";
          withoutarg = None;
          witharg = set_string App.metafile "metadata filename" };
    Opt { short = "-q"; long = "no-metadata"; argname = "";
          descr = "disable metadata";
          withoutarg = set_flag App.use_meta false;
          witharg = None };
    Opt { short = "-b"; long = "best"; argname = "";
          descr = "enable maximum compression (no effect on some formats, can be extremely slow on others)";
          withoutarg = set_flag App.brutal true;
          witharg = None };
    Opt { short = ""; long = "masked-only"; argname = "";
          descr = "for format 2 G00s, allow cropping of transparent areas of the image regardless of their contents";
          withoutarg = set_flag App.masked_only true;
          witharg = None };
    Opt { short = ""; long = "dump-format-2"; argname = "";
          descr = "" (* dump raw version of format 2 G00s (debugging option) *);
          withoutarg = set_flag App.debug_dump_file true;
          witharg = None };
    Opt { short = ""; long = "include-parts"; argname = "";
          descr = "" (* include parts in metadata for format 2 G00s (debugging option) *);
          withoutarg = set_flag App.include_parts true;
          witharg = None };
  ]


(* Main program *)

let () =
  let files = ref [] in
  try
    getopt options (function "" -> () | x -> files := !files @ [x]) (fun () -> failwith "help") ~app:App.app;
    if !files = [] then usageError "no input files";
    if !App.outfile <> "" && List.tl !files <> [] then usageError "the output filename cannot be specified with multiple input files";
    if !App.metafile <> "" && List.tl !files <> [] then usageError "the metadata filename cannot be specified with multiple input files";
    List.iter Convert.convert_file !files
  with
    | Failure "help"    -> display_help App.app options
    | Failure "version" -> display_version App.app
    | Failure e         -> sysError e
