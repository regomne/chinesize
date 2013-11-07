(*
   Rlc: GAMEEXE.INI parser type definitions
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

type deftab = (string, definition) Hashtbl.t
and definition = value list
and value =
  [ `Defined
  | `Enabled of bool
  | `Integer of int32
  | `String of string
  | `Range of int32 list ]

let gameexe : deftab = Hashtbl.create 0

let find key = Hashtbl.find gameexe (String.lowercase key)

let set key =
  Hashtbl.replace gameexe (String.lowercase key)

let unset key =
  Hashtbl.remove gameexe (String.lowercase key)

let exists key =
  Hashtbl.mem gameexe (String.lowercase key)

let set_int key value =
  set key [`Integer (Int32.of_int value)]

let get key =
  try Some (find key) with Not_found -> None

let get_def key ~def =
  try find key with Not_found -> def

let get_int key ~def =
  match get key with
    | Some [`Integer i] -> Int32.to_int i
    | _ -> def

let get_pair key ~def =
  match get key with
    | Some [`Integer a; `Integer b] -> Int32.to_int a, Int32.to_int b
    | _ -> def

let init parsefun lexfun srcdir =
  let filename =
    if !App.gameexe <> "" then
      if Sys.file_exists !App.gameexe
      && List.mem (Unix.stat !App.gameexe).Unix.st_kind [Unix.S_REG; Unix.S_LNK]
      then !App.gameexe
      else Printf.ksprintf Optpp.sysError "`%s' is not a valid INI file" !App.gameexe
    else  (* check command-line *)
      try Sys.getenv "GAMEEXE" with Not_found ->  (* check environment *)
        try
          Filename.concat srcdir
            (List.find
              (fun f -> Sys.file_exists (Filename.concat srcdir f))
              ["GAMEEXE.INI"; "gameexe.ini";
               Filename.concat Filename.parent_dir_name "GAMEEXE.INI";
               Filename.concat Filename.parent_dir_name "gameexe.ini"])
        with Not_found -> ""
  in
  if filename <> "" then
    let ic = open_in filename in
    App.gameexe := filename;
    if !App.verbose then Printf.ksprintf Optpp.sysInfo "Reading INI: %s" filename;
    try
      parsefun lexfun (Lexing.from_channel ic);
      close_in ic
    with
      e -> close_in_noerr ic;
           raise e
  else
    Optpp.sysWarning "unable to locate `gameexe.ini': using default values"
