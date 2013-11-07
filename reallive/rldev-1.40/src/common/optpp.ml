(*
   Copyright (C) 2006 Haeleth; based on code copyright (C) 2000 Alain Frisch

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

   This file is a derivative of the getopt library by Alain Frisch, which is
   distributed under the GNU LGPL.  At the time of writing it was available from
   http://www.eleves.ens.fr:8080/home/frisch.
*)

(* option definition structures *)

type opt_srcp =
  | Opt of opt_srct
  | Message of string
  | Break

and opt_srct =
  { short : string;
    long : string;
    argname : string;
    descr : string;
    withoutarg : noarg_fun;
    witharg : arg_fun; }

and noarg_fun = (unit -> unit) option
and arg_fun = (string -> unit) option


type app_info =
  { name: string;
    exe_name: string;
    version: float;
    description: string;
    year: int;
    author: string;
    usage: string; }

let default_app_info =
  { name = "program";
    exe_name = Sys.executable_name;
    version = 1.00;
    description = "a program";
    year = 2006;
    author = "Haeleth";
    usage = "<options> <files>" }

open Printf

(* Pretty-printing *)

let base_indent = ref 2

let screen_width () = try int_of_string (Sys.getenv "COLUMNS") with _ -> 80

let openblk indent prespace =
  Format.set_margin (screen_width ());
  Format.printf "%s@[<b%d>%s" (String.make prespace '\n') indent (String.make indent ' ')

let split_string s =
  let buf = Buffer.create (String.length s)
  and rv = DynArray.create () in
  let flush () =
    if Buffer.length buf > 0 then (
      DynArray.add rv (`Text (Buffer.contents buf));
      Buffer.clear buf
    )
  in
  let rec loop i =
    if i < String.length s then
      match s.[i] with
        | ' ' | '\t'
          -> flush ();
             DynArray.add rv `Space;
             loop (i + 1)
        | '\n'
          -> flush ();
             DynArray.add rv `Eol;
             loop (i + 1)
        | c
          -> Buffer.add_char buf c;
             loop (i + 1)
    else
      flush ()
  in
  loop 0;
  rv

let prettily s =
  DynArray.iter
    (function
      | `Text s -> Format.print_string s
      | `Space -> Format.print_space ()
      | `Eol -> Format.force_newline ())
    (split_string s)

let prettily_blk_ln indent s =
  openblk indent 0;
  prettily s;
  Format.close_box ();
  Format.print_newline ()


(* Output functions (defaults are CLI) *)

let cliInfo = prettily_blk_ln !base_indent

let cliWarning s =
  Format.set_formatter_out_channel stderr;
  cliInfo s;
  flush stderr;
  Format.set_formatter_out_channel stdout

let cliError s =
  Format.set_formatter_out_channel stderr;
  cliInfo s;
  exit 2

let usageError ?(app = default_app_info) s =
  ksprintf cliError "Error: %s.\nFor basic usage information run `%s --help'" s app.exe_name

let sysInfo = cliWarning
let sysWarning s = ksprintf cliWarning "Warning: %s." s
let sysError s = ksprintf cliError "Error: %s." s


exception Error of string
let abort s = raise (Error s)

let noshort = '\000'
let nolong  = ""

type opt = char * string * ((unit -> unit) option) * ((string -> unit) option)

let index_option s c =
  try
    Some (String.index s c)
  with
    Not_found -> None

let extract_arg_handle opt =
  function
    | _, _, _, Some handle -> handle
    | _ -> ksprintf abort "Option %s does not accept argument" opt

let extract_handle opt =
  function
    | _, _, Some handle, _ -> handle
    | _ -> ksprintf abort "Option %s must have an argument" opt

let parse opts others args first last =
  let find_long opt =
    try
      List.find (fun (_, l, _, _) -> opt = l) opts
    with
      Not_found -> ksprintf abort "Unknown option --%s" opt
  and find_short opt =
    try
      List.find (fun (l, _, _, _) -> opt = l) opts
    with
      Not_found -> ksprintf abort "Unknown option -%c" opt
  in
  let rec skip no =
    if no <= last then (
      others args.(no);
      skip (no + 1)
    )
  in
  let rec aux no =
    if no <= last then
      let s = args.(no) in
      let l = String.length s in
      if l = 0 then (
        others s;
        aux (no + 1)
      )
      else
        if s.[0] = '-' then
          if l >= 2 && s.[1] = '-' then
            if l = 2 then
              skip (no + 1)
            else
              match index_option s '=' with
                | Some i (* long option with argument *)
                 -> let opt = String.sub s 2 (i - 2)
                    and arg = String.sub s (i + 1) (l - i - 1) in
                    (extract_arg_handle ("--" ^ opt) (find_long opt)) arg;
                    aux (no + 1)
                | None   (* long option with no argument *)
                 -> let opt = String.sub s 2 (l - 2) in
                    (extract_handle s (find_long opt)) ();
                    aux (no + 1)
          else
            if l = 1 then (
              others s;
              aux (no + 1)
            )
            else (* short option *)
              let opt = s.[1] in
              match find_short opt with
                | _, _, Some handle, None (* no argument allowed; next chars are options *)
                 -> handle ();
                    for i = 2 to l - 1 do
                      match find_short s.[i] with
                        | _, _, Some handle, None -> handle ()
                        | _ -> ksprintf abort "Only non-argument short-options can be concatenated (error with option %c in %s)"  s.[i] s
                    done;
                    aux (no + 1)
                | _, _, _,Some handle as o (* argument allowed or mandatory *)
                 -> if l > 2 then (
                      (* immediate argument *)
                      handle (String.sub s 2 (l - 2));
                      aux (no + 1)
                    )
                    else
                      if no + 1 <= last && args.(no + 1).[0] <> '-' then (
                        (* non-immediate argument *)
                        handle args.(no + 1);
                        aux (no + 2)
                      )
                      else
                        (* no argument *)
                        let handle = extract_handle s o in
                        handle ();
                        aux (no + 1)
                | _ -> failwith "Getopt.parse"
    else (
      others s;
      aux (no + 1)
    )
  in
  aux first

let parse_cmdline opts others =
  parse opts others Sys.argv 1 (Array.length Sys.argv - 1)


(* option structure conversion and display functions *)

let opt_src_to_opt osl =
  let as_char = function "" -> noshort | s -> s.[1] in
  let convert_optsrct os = (as_char os.short, os.long, os.withoutarg, os.witharg) in
  let rec doconv = function [] -> [] | head :: tail ->
    match head with
      | Message _
      | Break -> doconv tail
      | Opt o -> convert_optsrct o :: doconv tail
  in doconv osl

let display_options = function [] -> () | osl ->
  let shorts, shortlen =
    let shortlen = ref 0 in
    let short = function
      | Opt { descr = "" } -> "\001"
      | Opt { short = "" } -> " "
      | Opt { short = x; argname = "" } -> shortlen := max !shortlen (String.length x); x
      | Opt { short = x; argname = ar } -> let x = sprintf "%s %s" x ar in shortlen := max !shortlen (String.length x); x
      | Break -> " "
      | Message s -> "\000"
   in (List.map short osl), !shortlen
  and longs, longlen =
    let longlen = ref 0 in
    let long = function
      | Opt { long = "" } -> " "
      | Opt { long = x; argname = "" } -> let x = "--" ^ x in longlen := max !longlen (String.length x); x
      | Opt { long = x; argname = ar } -> let x = sprintf "--%s=%s" x ar in longlen := max !longlen (String.length x); x
      | Break | Message _ -> " "
    in (List.map long osl), !longlen
  and descrs =
    let descr = function
      | Opt { descr = "" } -> " "
      | Opt { descr = ds } -> ds
      | Break -> " "
      | Message s -> s
    in List.map descr osl
  in
  let rec print_options = function
    | [], _, _ -> ()
    | _, [], _ -> failwith "print_options"
    | _, _, [] -> failwith "print_options"
    | s::ss, l::ll, d::dd ->
      if s = "\000"
      then (
        Format.force_newline ();
        Format.open_box 0;
        Format.print_string d;
        Format.force_newline ();
        Format.close_box ()
      )
      else if s = "\001"
      then ()
      else (
        Format.print_tab ();
        Format.print_string s;
        Format.print_tab ();
        Format.print_string l;
        Format.print_tab ();
        Format.open_box 0;
        prettily d;
        Format.close_box ()
      );
      print_options (ss, ll, dd)
  in
  openblk 2 0;
  Format.open_tbox ();
  Format.set_tab ();
  Format.print_string (String.make (shortlen + 1) ' ');
  Format.set_tab ();
  Format.print_string (String.make (longlen + 2) ' ');
  Format.set_tab ();
  Format.print_string " ";
  print_options (shorts, longs, descrs);
  Format.close_tbox ();
  Format.close_box ();
  Format.print_flush ();
  print_newline ()

let display_version app =
  ksprintf cliInfo "%s %.2f: %s" app.name app.version app.description;
  ksprintf cliInfo "Copyright (C) %d %s" app.year app.author

let display_help app options =
  display_version app;
  if options <> [] then (
    Format.print_newline ();
    ksprintf cliInfo "Usage: %s %s" app.exe_name app.usage;
    display_options options
  );
  exit 0

let getopt ?(app = default_app_info) options add_file no_files =
  Format.set_margin (screen_width ());
  if Array.length Sys.argv <= 1
  then no_files ()
  else
    let options = opt_src_to_opt options in
    try
      parse_cmdline options add_file
    with
      | Error "Unknown option - " -> usageError ~app "unable to parse options (try adding `--' before filenames)"
      | Error s -> usageError ~app (String.uncapitalize s)

let set_flag flag value =
  Some (fun () -> flag := value)

let arg_to_flag op flag name =
  let flag_set = ref 0 in
  Some (fun x ->
    incr flag_set;
    match !flag_set with
      | 1 -> flag := op x
      | 2 -> ksprintf sysWarning "only the first specified %s will be used" name
      | _ -> ())

let set_string = arg_to_flag (fun x -> x)