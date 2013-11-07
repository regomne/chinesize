(*
    Rlc: `select' function typechecking and compilation
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
open KfnTypes
open KeTypes
open KeAst
open Codegen


let handle_parameter insert_pos s l e =
  match type_of_normalised_expr e with
    | `Invalid -> Function.undeclared_func_in_expr e
    | `Int | `Str -> ksprintf (Output.add_code nowhere) "###PRINT(%s)" (code_of_expr e)
    | `Literal
       -> let text = Function.get_literal_from_expr e
          and invalid l c = ksprintf (error l) "`\\%s{}' invalid in %s() calls" c s
          and b = Buffer.create 0 and double_quote = Function.double_quote_var ~insert_pos in
          let quote, unquote, not_quoted =
            let quoted = ref false in
            (fun () -> if not !quoted then Buffer.add_char b '\"'; quoted := true),
            (fun () -> if !quoted then Buffer.add_char b '\"'; quoted := false),
            (fun () -> not !quoted)
          in
          (* We iterate through the string in much the same way as for handle_literal_in_strc. *)
          DynArray.iter
            (function `EOS -> assert false
              | `RCur _     -> quote (); Buffer.add_string b "}"
              | `LLentic _  -> Buffer.add_string b "\x81\x79"
              | `RLentic _  -> Buffer.add_string b "\x81\x7a"
              | `Asterisk _ -> Buffer.add_string b "\x81\x96"
              | `Percent _  -> Buffer.add_string b "\x81\x93"
              | `Hyphen _   -> Buffer.add_string b "-"
              | `Space (_, i) -> quote (); Buffer.add_string b (String.make i ' ')
              | `Text (l, _, s)
                 ->(if not_quoted () then try
                      Text.iter (fun c -> if not (StrTokens.unquoted_char c) then raise Exit) s
                    with Exit -> quote ());
                   (try
                      Buffer.add_string b (Text.to_sjs s)
                    with Text.Bad_char c ->
                      ksprintf (error l) "cannot represent U+%04x in RealLive bytecode" c)
             (* Invalid codes *)
              | `Speaker l -> invalid l "{}' / `\\name"
              | `Gloss (l, `Gloss, _, _) -> invalid l "g"
              | `Gloss (l, `Ruby, _, _) -> invalid l "ruby"
              | `Add (l, _) -> invalid l "a"
              | `Delete l -> invalid l "d"
              | `Rewrite (l, _) -> invalid l "f"
              | `Code (l, t, _, _) when not (StrTokens.is_output_code t) -> invalid l (Text.to_sjs t)
             (* Special treatment *)
              | `Name (l, lg, i, w)
                 -> let lg = if lg = `Local then "\x81\x93" else "\x81\x96" in
                    let i =
                      try
                        match !Global.expr__normalise_and_get_const i ~expect:`Int ~abort_on_fail:false with `Integer i -> Int32.to_int i | _ -> assert false
                      with Exit ->
                        error l "name index must be constant in static text"
                    in
                    Buffer.add_string b (StrTokens.make_name lg i);
                    Option.may
                      (fun w ->
                        let w =
                          try
                            match !Global.expr__normalise_and_get_const w ~expect:`Int ~abort_on_fail:false with `Integer i -> Int32.to_int i | _ -> assert false
                          with Exit ->
                            error l "name char index must be constant in static text"
                        in
                        bprintf b "\x82%c" (char_of_int (w + 0x4f)))
                      w
              | `DQuote _ -> unquote (); bprintf b "###PRINT(%s)" (code_of_expr (double_quote ()))
              | `Code (_, t, None, [`Simple (_, p)])
                 -> unquote ();
                    bprintf b "###PRINT(%s)" (code_of_expr p)
              | `Code (l, t, Some e, [`Simple (_, p)])
                 -> if t <> Text.of_sjs "i" then error l "length specifier invalid in \\s{} code";
                    unquote ();
                    let s = Memory.get_temp_str () in
                    Output.insert_code insert_pos
                      (FuncAsm.compile_function_str nowhere (rlfun "itoa")
                        [`Integer (code_of_expr p); `Integer (code_of_expr e)] ~returnval:(code_of_expr s));
                    bprintf b "###PRINT(%s)" (code_of_expr s)
              | `Code (l, t, _, _)
                 -> ksprintf (error l) "expected one parameter for \\%s{}" (Text.to_sjs t))
            text;
          unquote ();
          Output.add_code nowhere (Buffer.contents b)

let get_op l s t =
  match Text.to_sjs t with
    | "colour"         -> '0'
    | "title" | "grey" -> '1'
    | "hide"           -> '2'
    | "blank"          -> '3'
    | "cursor"         -> '4'
    | _ -> ksprintf (error l) "unknown effect `%s' in select condition" s

let compile (loc, dest, s, opcode, window, params) =
  if params = [] then ksprintf (warning loc) "`%s' called with no options" s;
  let handle = handle_parameter (Output.length ()) s in
  Output.add_kidoku loc;
  Output.add_code loc (code_of_opcode 0 2 opcode (List.length params) 0);
  Option.may 
    (fun e -> 
      if List.mem opcode [3; 13] (* removed 2 and 12 *)
      then ksprintf (error loc) "select window specifiers are not valid in `%s' calls" s
      else ksprintf (Output.add_code nowhere) "(%s)" (code_of_expr e))
    window;
  Output.add_code nowhere "{";
  Output.add_line loc ~force:true;
  (* TODO: test this! *)
  List.iter
    (function
      | `Always (l, e)
      | `Special (l, [], e)
         -> handle l e;
            Output.add_line l ~force:true
      | `Special (l, cl, e)
         -> Output.add_code nowhere "(";
            let use_line = ref true in
            List.iter 
              (function
                | `Flag (l, s, t)
                   -> ksprintf (Output.add_code nowhere) "%c" (get_op l s t);
                      use_line := (try string_of_strtokens (Global.expr__normalise_and_get_str e ~abort_on_fail:false) <> "" with _ -> true)
                | `NonCond (l, s, t, e) 
                   -> ksprintf (Output.add_code nowhere) "%c%s" (get_op l s t) (code_of_expr e)
                | `Cond (l, s, t, p, c) 
                   -> ksprintf (Output.add_code nowhere) "(%s)%c" (code_of_expr c) (get_op l s t);
                      Option.may (fun e -> Output.add_code nowhere (code_of_expr e)) p)
              cl;
            Output.add_code nowhere ")";
            if !use_line then handle l e;
            Output.add_line l ~force:true)
    params;
  Output.add_code nowhere "}";
  match dest with
    | `Store _ -> ()
    | _ -> ksprintf (Output.add_code loc) "%s\\\x1e$\xc8" (code_of_var dest)

let compile_vwf (loc, dest, s, opcode, window, params) =
  if not (List.mem opcode [0; 1; 10; 11]) then 
    compile (loc, dest, s, opcode, window, params)
  else 
    let get_var e =
      let v = Memory.get_temp_str () ~useloc:nowhere in
      Function.compile
        (nowhere, None, "strcpy", Text.ident "strcpy", List.map (fun x -> `Simple (nowhere, x)) [v; e], None);
      v
    in
    Memory.open_scope ();
    let vars, vparams =
      List.split
        (List.map
          (function
            | `Always (l, e)      -> let v = get_var e in v, `Always (l, v)
            | `Special (l, cl, e) -> let v = get_var e in v, `Special (l, cl, v))
          params)
    in
    let rec loop s =
      function
        | [] -> ()
        | l -> Meta.call s (Option.default (Meta.int ~-1) window :: List.take 3 l);
               loop "__vwf_SelectAdd" (List.drop 3 l)
    in
    loop "__vwf_SelectInit" vars;
    compile (loc, dest, s, opcode, window, vparams);
    Meta.call "__vwf_SelectCleanup" [(dest :> expression)];
    Memory.close_scope ()    
