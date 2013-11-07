(*
    Rlc: tokenised string type utilities
    Copyright (C) 2006 Haeleth

   The type itself was originally defined here, but had to be moved to KeAst to
   permit mutual recursion with KeAst.expression and KeAst.parameter.

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


(* to_string: convert tokens to constant string *)

let make_name s i =
  if i < 26
  then sprintf "%s\x82%c" s (char_of_int (i + 0x60))
  else sprintf "%s\x82%c\x82%c" s (char_of_int (i / 26 + 0x5f)) (char_of_int (i mod 26 + 0x60))

let is_output_code t =
  t = Text.of_sjs "i" || t = Text.of_sjs "s"

let is_object_code =
  let opts = List.map Text.of_sjs ["r"; "n"; "c"; "size"; "pos"; "posx"; "posy"] in
  fun t -> List.mem t opts

let get_object_code loc fname id params =
  let s = Text.to_sjs id in
  let p = 
    try
      List.map
        (function
          | `Simple (_, e) -> Global.expr__normalise_and_get_int e ~abort_on_fail:false
          | _ -> raise Exit)
        params
    with Exit ->
      ksprintf (error loc) "parameters to \\%s must be constants in %s" s fname
  in
  match s, p with
    | ("r" | "n"), [] -> "#D"
    | "size", [] -> "#S##" | "size", [size] -> sprintf "#S%ld##" size
    | "c", []    -> "#C##" | "c", [colour]  -> sprintf "#C%ld##" colour
    | "pos", [x]
    | "posx", [x]   -> sprintf "#X%ld##" x
    | "posy", [y]   -> sprintf "#Y%ld##" y
    | "pos", [x; y] -> sprintf "#X%ld#Y%ld##" x y
    | _, [] -> ksprintf (error loc) "not enough parameters to \\%s" s
    | _, _ -> ksprintf (error loc) "too many parameters to \\%s" s    
  

let unquoted_char c = 
     (c >= int_of_char 'A' && c <= int_of_char 'Z')
  || (c == int_of_char '_' || c == int_of_char '?')
  || (c >= int_of_char '0' && c <= int_of_char '9')
  || (String.length (TextTransforms.to_bytecode (Text.of_char c)) = 2)

let invalid l s = ksprintf (error l) "`\\%s{}' invalid in constant strings" s

let rec handle_output_code enc rv loc code width params =
  (* Convert \i{const} or \s{const} to a string *)
  let e =
    match params with
      | [`Simple (_, e)] -> e
      | [] -> ksprintf (error loc) "expected a parameter to \\%s{}" (Text.to_sjs code)
      | [(`Complex _ |`Special _)] -> ksprintf (error loc) "parameter to \\%s{} must be a simple expression" (Text.to_sjs code)
      | _ -> ksprintf (error loc) "too many parameters to \\%s{}" (Text.to_sjs code)
  and expect =
    if code = Text.of_sjs "i"
    then `Int
    else `Str
  in
  let c = !Global.expr__normalise_and_get_const e ~expect in
  let fw =
    match width with
      | None -> 0
      | Some w when expect = `Str -> warning loc "width specifier invalid in \\s{}"; 0
      | Some w -> match !Global.expr__normalise_and_get_const w ~expect:`Int with `Integer i -> Int32.to_int i | _ -> assert false
  in
  match expect, c with
    | `Int, `Integer i -> Buffer.add_string rv (Global.int32_to_string_padded fw i)
    | `Str, `String s -> ignore (to_string ~rv ~enc s)
    | _ -> assert false

and to_string ?(rv = Buffer.create 0) ?(enc = "CP932") ?(quote = false) (tokens : strtokens) =
  assert (quote = false || enc = "CP932");
  let needs_quotes = ref false in
  let encode = if quote then TextTransforms.to_bytecode else Text.to_string enc in
  let encode_char c = encode (Text.of_char c) in
  DynArray.iter
    (function
      | `EOS -> assert false
      | `DQuote l  -> if quote then error l "internal error: `\"' in StrTokens.to_string" else Buffer.add_char rv '\"'
      | `RCur _    -> needs_quotes := true; Buffer.add_char rv '}'
      | `LLentic _ -> Buffer.add_string rv (encode_char 0x3010)
      | `RLentic _ -> Buffer.add_string rv (encode_char 0x3011)
      | `Asterisk _-> Buffer.add_string rv (encode_char 0xff0a)
      | `Percent _ -> Buffer.add_string rv (encode_char 0xff05)
      | `Hyphen _  -> needs_quotes := true; Buffer.add_string rv "-"
      | `Speaker l -> invalid l "{}' / `\\name"
      | `Gloss (l, `Gloss, _, _) -> invalid l "g"
      | `Gloss (l, `Ruby, _, _) -> invalid l "ruby"
      | `Add (l, _) -> invalid l "a"
      | `Delete l -> invalid l "d"
      | `Rewrite (l, _) -> invalid l "f"
      | `Name (l, lg, i, w)
         -> let lg = if lg = `Local then "\x81\x93" else "\x81\x96" in
            let i =
              try
                match !Global.expr__normalise_and_get_const i ~expect:`Int ~abort_on_fail:false with `Integer i -> Int32.to_int i | _ -> assert false
              with Exit ->
                error l "name index must be constant in static text"
            in
            (* Will always be unencoded; this may be desirable for name references? *)
            Buffer.add_string rv (Text.sjs_to_enc enc (make_name lg i));
            Option.may
              (fun w ->
                let w =
                  try
                    match !Global.expr__normalise_and_get_const w ~expect:`Int ~abort_on_fail:false with `Integer i -> Int32.to_int i | _ -> assert false
                  with Exit ->
                    error l "name char index must be constant in static text"
                in
                bprintf rv "\x82%c" (char_of_int (w + 0x4f)))
              w
      | `Space (_, i) -> needs_quotes := true; Buffer.add_string rv (String.make i ' ')
      | `Code (l, t, width, params) when is_output_code t -> needs_quotes := true; handle_output_code enc rv l t width params
      | `Code (l, t, _, _) -> invalid l (Text.to_sjs t)
      | `Text (l, _, t)
         -> (try
               needs_quotes := !needs_quotes || (quote &&
                 (try Text.iter (fun c -> if not (unquoted_char c) then raise Exit) t; false with Exit -> true));
               Buffer.add_string rv (encode t)
             with Text.Bad_char c ->
               ksprintf (warning l) "cannot represent U+%04x in RealLive bytecode with %s" c (TextTransforms.describe ());
               Buffer.add_string rv " " (* sane default for emergencies *)))
    tokens;
  if quote && !needs_quotes
  then sprintf "\"%s\"" (Buffer.contents rv)
  else Buffer.contents rv
