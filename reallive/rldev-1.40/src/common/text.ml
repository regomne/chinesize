(*
    RLdev: unicode handling
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
open Encoding

exception Bad_char of int

type t = int array

let append = Array.append
and length = Array.length
and empty = [||]
and iter = Array.iter
and map = Array.map

module Buf =
  struct
    type buf = int DynArray.t
    let create = DynArray.make
    and add_int = DynArray.add
    and add_char b c = DynArray.add b (int_of_char c)
    and add_string b = Array.iter (DynArray.add b)
    let add_array = add_string
    and length = DynArray.length
    and contents = DynArray.to_array
    and clear = DynArray.clear
  end

let to_arr x = x
and of_arr x = x

let of_char c = [| c |]

let to_sjs a =
  let b = Buffer.create 0 in
  Array.iter
    (fun c ->
      try
        Buffer.add_string b (IMap.find c Cp932.uni_to_db)
      with
        Not_found -> bprintf b "\\u{$%04x}" c)
    a;
  Buffer.contents b

let of_sjs s =
  let b = Buf.create 0 in
  let rec getc idx =
    if idx = String.length s then
      Buf.contents b
    else
      let c = s.[idx] in
      match c with
        | '\x81'..'\x9f' | '\xe0'..'\xef' | '\xf0'..'\xfc' when idx + 1 < String.length s
         -> Buf.add_int b (Cp932.db_to_uni.(int_of_char c - 0x81).(int_of_char s.[idx + 1] - 0x40));
            getc (idx + 2)
        | _
         -> Buf.add_int b (Cp932.sb_to_uni.(int_of_char c));
            getc (idx + 1)
  in
  getc 0

let to_utf8 a =
  let b = Buffer.create 0 in
  let mask = 0b111111 in
  Array.iter
    (fun i ->
      if i < 0 || i >= 0x4000000 then (
        Buffer.add_char b (Char.chr (0xfc + (i lsr 30)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 24) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 18) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 12) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 6) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor (i land mask)))
      ) else if i <= 0x7f then
        Buffer.add_char b (Char.unsafe_chr i)
      else if i <= 0x7ff then (
        Buffer.add_char b (Char.unsafe_chr (0xc0 lor (i lsr 6)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor (i land mask)))
      ) else if i <= 0xffff then (
        Buffer.add_char b (Char.unsafe_chr (0xe0 lor (i lsr 12)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 6) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor (i land mask)))
      ) else if i <= 0x1fffff then (
        Buffer.add_char b (Char.unsafe_chr (0xf0 + (i lsr 18)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 12) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 6) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor (i land mask)))
      ) else (
        Buffer.add_char b (Char.unsafe_chr (0xf8 + (i lsr 24)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 18) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 12) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor ((i lsr 6) land mask)));
        Buffer.add_char b (Char.unsafe_chr (0x80 lor (i land mask)))
      ))
    a;
  Buffer.contents b

let sjs_to_enc enc s =
  match enc_type enc with
    | `Sjs -> s
    | `Euc -> sjs_to_euc s
    | `Utf8 -> to_utf8 (of_sjs s)
    | `Other -> ksprintf sysError "unknown encoding `%s' (valid encodings are Shift_JIS, EUC-JP, UTF-8)" enc

let norm =
  Array.map
    (fun ch ->
      if ch >= int_of_char 'A' && ch <= int_of_char 'Z'
      then ch + 32
      else ch)

let getc_sjs chan =
  try
    let c1 = IO.read_byte chan in
    if (c1 >= 0x81 && c1 <= 0x9f) || (c1 >= 0xe0 && c1 <= 0xef) || (c1 >= 0xf0 && c1 <= 0xfc) then
      Cp932.db_to_uni.(c1 - 0x81).(IO.read_byte chan - 0x40)
    else
      Cp932.sb_to_uni.(c1)
  with
    Invalid_argument "index out of bounds" -> sysError "invalid character in input: check encoding settings"

let getc_euc chan =
  try
    let c1 = IO.read_byte chan in
    if c1 < 0x7f then
      c1
    else
      let c1 = c1 - 0x80
      and c2 = IO.read_byte chan - 0x80 in
      Cp932.db_to_uni
           .((c1 + 1) lsr 1 + if c1 < 95 then -17 else 47)
           .(c2 + if c1 mod 2 = 0 then 62 else if c2 > 95 then -32 else -33)
  with
    Invalid_argument "index out of bounds" -> sysError "invalid character in input: check encoding settings"

let getc_utf8 chan =
  let c1 = IO.read_byte chan in
  if c1 <= 0x7f then
    c1
  else let c2 = IO.read_byte chan in
  if c1 <= 0xdf then
    (c1 - 0xc0) lsl 6 lor (c2 land 0x7f)
  else let c3 = IO.read_byte chan in
  if c1 <= 0xef then
    ((c1 - 0xe0) lsl 6 lor (c2 land 0x7f)) lsl 6 lor (c3 land 0x7f)
  else let c4 = IO.read_byte chan in
  if c1 <= 0xf7 then
    (((c1 - 0xf0) lsl 6 lor (c2 land 0x7f)) lsl 6 lor (c3 land 0x7f)) lsl 6 lor (c4 land 0x7f)
  else let c5 = IO.read_byte chan in
  if c1 <= 0xfb then
    ((((c1 - 0xf8) lsl 6 lor (c2 land 0x7f)) lsl 6 lor (c3 land 0x7f)) lsl 6 lor (c4 land 0x7f)) lsl 6 lor (c5 land 0x7f)
  else let c6 = IO.read_byte chan in
  if c1 <= 0xfd then
    (((((c1 - 0xfc) lsl 6 lor (c2 land 0x7f)) lsl 6 lor (c3 land 0x7f)) lsl 6 lor (c4 land 0x7f)) lsl 6 lor (c5 land 0x7f)) lsl 6 lor (c6 land 0x7f)
  else
    sysError "invalid character in input: check encoding settings"

let get_getc enc =
  match enc_type enc with
    | `Euc -> getc_euc
    | `Sjs -> getc_sjs
    | `Utf8 -> getc_utf8
    | `Other -> ksprintf sysError "unknown encoding `%s' (valid encodings are Shift_JIS, EUC-JP, UTF-8)" enc

let ustream enc ic =
  let chan = IO.input_channel ic
  and f = get_getc enc in
  let strm = Stream.from (fun _ -> try Some (f chan) with IO.No_more_input -> None) in
  begin match Stream.peek strm with
    | Some (0xfffe | 0xfeff) -> Stream.junk strm (* Ignore BOM *)
    | _ -> ()
  end;
  strm

let to_string enc t = 
  match enc_type enc with
    | `Sjs -> to_sjs t
    | `Euc -> sjs_to_euc (to_sjs t)
    | `Utf8 -> to_utf8 t
    | `Other -> ksprintf sysError "unknown encoding `%s' (valid encodings are Shift_JIS, EUC-JP, UTF-8)" enc

let of_string enc s =
  let chan = IO.input_string s
  and f = get_getc enc
  and b = Buf.create 0 in
  try
    while true do Buf.add_int b (f chan) done;
    assert false
  with _ ->
    Buf.contents b

let to_err = to_string Config.default_encoding
and of_err = of_string Config.default_encoding

let sjs_to_err = sjs_to_enc Config.default_encoding

let ident =
  let memo = Hashtbl.create 0 in
  fun s ->
    try
      Hashtbl.find memo s
    with Not_found ->
      let rv = of_sjs (String.lowercase s) in
      Hashtbl.add memo s rv;
      rv
