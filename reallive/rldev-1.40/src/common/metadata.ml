(*
   RLdev: metadata storage in bytecode headers
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
(*syntax camlp4o *)
(*ppopt pa_macro.cmo *)

INCLUDE "../VERSION"

open Printf
open Binarray

(*
   Metadata format is:

    int metadata_len
    int id_len
    char[id_len + 1] compiler_identifier, null-terminated
    int (compiler_version * 100)
    int target_version
    byte text_transform : 0 = none, 1 = Chinese, 2 = Western, 3 = Korean
    
*)

type t = 
  { compiler_name : string;
    compiler_version : int;
    target_version : int * int * int * int;
    text_transform : [ `None | `Chinese | `Western | `Korean ]; }

let empty =
  { compiler_name = "";
    compiler_version = 0;
    target_version = 0, 0, 0, 0;
    text_transform = `None; }

let read arr idx =
  let metadata_len = get_int arr idx
  and id_len = get_int arr (idx + 4) + 1 in
  if metadata_len < id_len + 17 then 
    (prerr_endline "Warning: RLdev metadata malformed";
     empty)
  else
    let idx2 = idx + 8 + id_len in
    { compiler_name = read_sz arr ~idx:(idx + 8) ~len:id_len;
      compiler_version = get_int arr idx2;
      target_version = arr.{idx2 + 4}, arr.{idx2 + 5}, arr.{idx2 + 6}, arr.{idx2 + 7};
      text_transform = (match arr.{idx2 + 8} with
                          | 0 -> `None
                          | 1 -> `Chinese
                          | 2 -> `Western
                          | 3 -> `Korean
                          | _ -> prerr_endline "Warning: RLdev metadata malformed"; `None); }

let (!!) i =
  let s = String.create 4 in
  String.unsafe_set s 0 (char_of_int (i land 0xff));
  String.unsafe_set s 1 (char_of_int ((i lsr 8) land 0xff));
  String.unsafe_set s 2 (char_of_int ((i lsr 16) land 0xff));
  String.unsafe_set s 3 (char_of_int ((i lsr 24) land 0xff));
  s

let to_string ?(ident = "RLdev") encoding (a,b,c,d) =
  let ver = VERSION *. 100. in
  ksprintf (fun s -> !!(String.length s + 4) ^ s)
    "%s%s\x00\
     %s\
     %c%c%c%c\
     %c"
    !!(String.length ident)
    ident
    !!(int_of_float ver)
    (char_of_int a) (char_of_int b) (char_of_int c) (char_of_int d)
    (match encoding with
       | `None    -> '\x00'
       | `Chinese -> '\x01'
       | `Western -> '\x02'
       | `Korean  -> '\x03')
