(*
    Rlc: text output compilation
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
(*ppopt pa_macro.cmo ./pa_matches.cmo *)

open Printf
open KeTypes
open KeAst
open Codegen

open Meta

(*** FOR TEXTOUT LIBRARY ***)

(* Keep up to date with the versions in lib/textout.kh *)
let id_stext = 0l  (* text followed by single space : offset, length                             *)
and id_ntext = 1l  (* text not followed by space    : offset, length                             *)
and id_dtext = 2l  (* DBCS characters               : offset, length                             *)
and id_dquot = 3l  (* double quote mark             : _, _                                       *)
and id_space = 4l  (* whitespace                    : length, _                                  *)
and id_namev = 5l  (* name variable                 : index, loc/glb, whether char index in next *)
and id_fsize = 6l  (* font size                     : size, has_size                             *)
and id_vrble = 7l  (* variable (now only strings)   : index, space                               *)
and id_ccode = 8l  (* control code                  : args (code-defined), code id               *)
and id_wname = 9l  (* name block    : (0 = enter, 1 = leave), (1 = normal, 0 = followed byÅuÅwÅi *)
and id_ruby  = 10l (* furigana / gloss              : depends, 0-2 = \ruby{} / 3-4 = \g{}        *)
and id_emoji = 11l (* äGï∂éö                        : index, whether to use colour               *)
and id_move  = 12l (* \mv*{}, \pos*{}: which provided (1 = x, 2 = y, 3 = both), (0 = mv/1 = pos) *)
(* 13l..15l are reserved. *)

let make_token_nconst id arg1 arg2 =
  assert (id < 16l);
  `Op (nowhere,
       `Parens (nowhere, `Op (nowhere, arg1, `Shl, int 12)),
       `Or,
       `Op (nowhere,
            `Int (nowhere, id),
            `Or,
            `Parens (nowhere, `Op (nowhere, arg2, `Shl, int 4))))

let make_token id arg1 arg2 =
  assert (arg1 >= 0 && arg1 < 1_048_575 && arg2 >= 0 && arg2 < 256);
  make_token_nconst id (int arg1) (int arg2)

let finalise () =
  if Memory.defined (Text.ident "__DynamicLineationUsed__") then
    Meta.parse_elt (`VarOrFn (nowhere, "__dynamic_textout_print", Text.of_sjs "__dynamic_textout_print"))

exception AddText of string


let rec get_code_tokens (loc, t, lenopt, params) =
  let noarg = int 0
  and noopt () = if lenopt <> None then ksprintf (error loc) "the control code \\%s cannot have a length specifier" (Text.to_err t)
  and noparams () = if params <> [] then ksprintf (error loc) "the control code \\%s does not take any parameters" (Text.to_err t)
  and oneparam s =
    match params with
      | [`Simple (_, p)] -> p
      | [_] -> ksprintf (error loc) "the parameter to \\%s{} must be a simple expression" s
      | _ -> ksprintf (error loc) "the control code \\%s{} must have one and only one parameter" s
  and add_text s = raise (AddText s)
  in
  match Text.to_sjs t with
    | "r" -> noopt (); noparams (); [make_token id_ccode 0 0x0d]
    | "n" -> noopt (); noparams (); [make_token id_ccode 0 0x0a]
    | "p" -> noopt (); noparams (); [make_token id_ccode 0 0x16]
    | "i"
       -> let param = oneparam "i" in
          (match lenopt, param with
            | None, `Int (_, i) -> add_text (Int32.to_string i)
            | Some (`Int (_, w)), `Int (_, i) -> add_text (Global.int32_to_string_padded (Int32.to_int w) i)
            | None, iexpr -> [make_token id_ccode 0 0x69; iexpr]
            | Some wexpr, iexpr -> [make_token_nconst id_ccode wexpr (int 0x69); iexpr])
    | "s"
       -> noopt ();
          let param = oneparam "s" in
          (match param with
            | `SVar (_, space, idx) -> [make_token_nconst id_vrble idx (int space)]
            | `Str _ -> assert false (* should never occur in normalised strings *)
            | _ -> error loc "the parameter to \\s{} must be a string expression")
    | "c"
       -> noopt ();
          let fg, bg, argc =
            match params with
              | [] -> noarg, noarg, 0
              | [`Simple (_, fg)] -> fg, noarg, 1
              | [`Simple (_, fg); `Simple (_, bg)] -> fg, bg, 2
              | [_] | [_; _] -> error loc "the parameters to \\c{} must be integer expressions"
              | _ -> error loc "the control code \\c{} must have between zero and two parameters"
          in
          if not (Memory.defined (Text.ident "__NoDebug__")) then
           (if argc > 0 then
              parse_elt (`If (nowhere,
                `AndOr (nowhere, `LogOp (nowhere, fg, `Ltn, noarg), `LOr, `LogOp (nowhere, fg, `Gte, int 255)),
                KeULexer.call_parser_on_string KeAstParser.program nowhere "rtlError ('called \\\\c{} with invalid foreground colour')",
                None));
            if argc > 1 then
              parse_elt (`If (nowhere,
                `AndOr (nowhere, `LogOp (nowhere, bg, `Ltn, noarg), `LOr, `LogOp (nowhere, bg, `Gte, int 255)),
                KeULexer.call_parser_on_string KeAstParser.program nowhere "rtlError ('called \\\\c{} with invalid background colour')",
                None)));
          let args =
            `Op (nowhere, `Parens (nowhere, `Op (nowhere, fg, `Shl, int 2)),  `Or,
            `Op (nowhere, `Parens (nowhere, `Op (nowhere, bg, `Shl, int 10)), `Or,
            int argc))
          in
          [make_token_nconst id_ccode args (int 0x63)]
    | "size"
       -> noopt ();
          let has_sz, sz =
            match params with
              | [] -> 0, noarg
              | [`Simple (_, size)] -> 1, size
              | _::[] -> error loc "if a parameter is given to \\size{}, it must be an integer expression"
              | _ -> error loc "the control code \\size{} takes a maximum of one parameter"
          in
          [make_token_nconst id_fsize sz (int has_sz)]
    | "wait"
       -> noopt ();
          let param = oneparam "wait" in
          if not (Memory.defined (Text.ident "__NoDebug__")) then
            parse_elt (`If (nowhere,
              `AndOr (nowhere, `LogOp (nowhere, param, `Ltn, noarg), `LOr, `LogOp (nowhere, param, `Gte, int 1_048_575)),
              KeULexer.call_parser_on_string KeAstParser.program nowhere "rtlError ('called \\\\wait{} with invalid duration')",
              None));
          [make_token_nconst id_ccode param (int 0x77)]
    | "e" | "em" as s
       -> noopt ();
          let idx, size =
            match params with
              | [] -> ksprintf (error loc) "the control code \\%s{} requires at least one parameter" s
              | [`Simple (_, idx)] -> idx, None
              | [`Simple (_, idx); `Simple (_, size)] -> idx, Some size
              | [_] | [_; _] -> ksprintf (error loc) "the parameters to \\%s{} must be integer expressions" s
              | _ -> ksprintf (error loc) "too many parameters for \\%s{}" s
          in
          let ecode = make_token_nconst id_emoji idx (int (if s = "e" then 1 else 0)) in
          (match size with
            | None -> [ecode]
            | Some sz -> [make_token_nconst id_fsize sz (int 1); ecode; make_token id_fsize 0 0])
    | "mv" | "mvx" | "mvy" | "pos" | "posx" | "posy" as s
       -> noopt ();
          let which, params =
            match s, params with
              | _, [] -> ksprintf (error loc) "not enough parameters for \\%s{}" s
              | ("mv" | "pos"), [`Simple (_, x); `Simple (_, y)] -> 0b11, [x; y]
              | ("mvy" | "posy"), [`Simple (_, y)] -> 0b10, [y]
              | _, [`Simple (_, x)] -> 0b01, [x]
              | _, [_] | ("mv" | "pos"), [_; _] -> ksprintf (error loc) "the parameters to \\%s{} must be integer expressions" s
              | _ -> ksprintf (error loc) "too many parameters for \\%s{}" s
          in
          make_token id_move which (if s.[0] = 'm' then 0 else 1) :: params
    | other
       -> ksprintf (error loc) "the control code `\\%s{}' seems not to exist" other


let compile addstrs loc text =
  if not (Memory.defined (Text.ident "__DynamicLineationUsed__")) then
    Memory.define ~scoped:false (Text.ident "__DynamicLineationUsed__") (`Integer 1l);
  Output.add_kidoku loc;
  call "strout" [`VarOrFn (nowhere, "__dto_token", Text.ident "__dto_token")]; (* hack to make saves work: __dto_token MUST be initialised empty and cleared after each call. *)
  let textbuf = Text.Buf.create 0
  and temptxt = Text.Buf.create 0
  and toknbuf = DynArray.create ()
  and currsbdb = ref None in
  let rec add_token id arg1 arg2 =
    DynArray.add toknbuf (make_token id arg1 arg2)
  and add_token_nconst id arg1 arg2 =
    DynArray.add toknbuf (make_token_nconst id arg1 arg2)
  and sflush with_space =
    if Text.Buf.length temptxt > 0 then
      (* Reuse repeated tokens. *)
      let curr = Text.to_arr (Text.Buf.contents textbuf)
      and tnew = Text.to_arr (Text.Buf.contents temptxt) in
      let max = Array.length curr - Array.length tnew
      and tnlen = Array.length tnew
      and found = ref ~-1 in
      begin try
        for i = 0 to max do
          if Array.unsafe_get curr i == Array.unsafe_get tnew 0
          && Array.unsafe_get curr (i + tnlen - 1) == Array.unsafe_get tnew (tnlen - 1) then
            try
              for j = 1 to tnlen - 2 do
                if Array.unsafe_get curr (i + j) != Array.unsafe_get tnew j then raise Not_found
              done;
              found := i; raise Exit
            with Not_found -> ()
        done
        (* We could also look for the case where the current text ends with a prefix of
           the new token.  But we don't. *)
      with Exit -> () end;
      let add = if !found = ~-1 then (found := Array.length curr; true) else false in
      let ttype =
        match with_space, !currsbdb with
          | _, Some `Dbcs -> id_dtext
          | true, _ -> id_stext
          | false, _ -> id_ntext
      in
      add_token ttype !found tnlen;
      if add then Text.Buf.add_string textbuf (Text.Buf.contents temptxt);
      currsbdb := None;
      Text.Buf.clear temptxt;
      (ttype = id_stext)
    else
      false
  and flush () = ignore (sflush false)
  and add_text sbdb t =
    Option.may (fun state -> if state <> sbdb then flush ()) !currsbdb;
    currsbdb := Some sbdb;
    Text.Buf.add_string temptxt t
  and add_nontext id arg1 arg2 =
    flush ();
    add_token id arg1 arg2
  and in_name = ref false
  in
  let rec parse idx =
    function
      | `EOS | `Delete _ | `Rewrite _ -> assert false
      | `DQuote _ -> add_nontext id_dquot 0 0
      | `LLentic _  -> add_text `Dbcs (Text.of_arr [| 0x3010 |])
      | `RLentic _  -> add_text `Dbcs (Text.of_arr [| 0x3011 |])
      | `Asterisk _ -> add_text `Dbcs (Text.of_arr [| 0xff0a |])
      | `Percent _  -> add_text `Dbcs (Text.of_arr [| 0xff05 |])
      | `Hyphen _   -> add_text `Sbcs (Text.of_arr [| 0x002d |]); flush ()
      | `Speaker l
         -> if !in_name then error l "\\{} may not be nested"; in_name := true; add_nontext id_wname 0 0
      | `RCur _
         -> if !in_name then
              let following =
                if idx < DynArray.length text - 1 then
                  match DynArray.unsafe_get text (idx + 1) with
                    | `Space (l, i)
                       -> DynArray.unsafe_set text (idx + 1) (`Space (l, i - 1));
                          if idx < DynArray.length text - 2
                            then match DynArray.unsafe_get text (idx + 2) with
                              | `Text (_, `Dbcs, t)
                                when Text.length t > 0 && List.mem (Text.to_arr t).(0) [0x300c; 0x300e; 0xff08]
                                -> 0 | _ -> 1
                          else 1
                    | `Text (_, `Dbcs, t)
                      when Text.length t > 0 && List.mem (Text.to_arr t).(0) [0x300c; 0x300e; 0xff08] -> 0
                    | _ -> 1
                else 1
              in
              in_name := false;
              add_nontext id_wname 1 following
            else
              add_text `Sbcs (Text.of_arr [| 0x7d |])
      | `Code code
         -> begin try
              flush ();
              List.iter (DynArray.add toknbuf) (get_code_tokens code)
            with AddText s ->
              add_text `Sbcs (Text.of_sjs s)
            end
      | `Text (_, sbdb, t) -> add_text sbdb t
      | `Space (_, 0) -> ()
      | `Space (_, i) -> let i = if sflush true then i - 1 else i in if i > 0 then add_nontext id_space i 0
      | `Name (_, lorg, idx, None)
         -> flush ();
            add_token_nconst id_namev idx (int (if lorg = `Local then 0b01 else 0b00))
      | `Name (l, lorg, idx, Some cindex)
         -> flush ();
            add_token_nconst id_namev idx (int (if lorg = `Local then 0b11 else 0b10));
            DynArray.add toknbuf cindex
      | `Gloss (_, `Ruby, base, gloss)
         -> let gloss = match gloss with `Closed (_, g) -> g | `ResStr _ -> (* converted to `Closed in Expr.traverse_str_tokens *) assert false in
            add_nontext id_ruby 0 0;
            DynArray.iteri parse base;
            add_nontext id_ruby 0 1;
            DynArray.iteri parse gloss;
            add_nontext id_ruby 0 2
      | `Gloss (l, `Gloss, base, _)
         -> warning loc "\\g{} not implemented in Textout-formatted text";
            DynArray.iteri parse base
      | `Add elt -> Queue.add elt addstrs
  in
  DynArray.iteri parse text;
  if !in_name then error loc "expected `}' to close name block";
  flush ();
  Memory.open_scope ();
  let s = Text.Buf.contents textbuf in
  call "strcpy" [`VarOrFn (nowhere, "__dto_text", Text.ident "__dto_text"); `Str (nowhere, Global.dynArray (`Text (nowhere, `Sbcs, s)))];
  Variables.allocate
    (nowhere, `Int 32, [], 
      [nowhere, "__dto_tokens_alloc__", Text.ident "__dto_tokens_alloc__", `Auto,
       `Array (int (DynArray.length toknbuf) :: DynArray.to_list toknbuf), None]);
  let token_origin =
    match Memory.get_deref_as_expression (Text.ident "__dto_tokens_alloc__") (int 0) with
      | `IVar (_, _, e) -> e
      | _ -> assert false
  in
  assign (`VarOrFn (nowhere, "__dto_arrayptr", Text.ident "__dto_arrayptr")) `Set token_origin;
  gosub (nowhere, "__dynamic_textout_print", Text.ident "__dynamic_textout_print");
  call "strclear" [`VarOrFn (nowhere, "__dto_token", Text.ident "__dto_token")];
  Memory.close_scope ()



(*** FOR REGULAR GAMES ***)

let compile_stub addstrs loc text =
  let b = Buffer.create 0 in
  let quoted = ref false in
  let ignore_one_space = ref false in
  let set_quotes q =
    if !quoted <> q then (
      quoted := q;
      Buffer.add_char b '\"'
    ) in
  let flush () =
    set_quotes false;
    Output.add_code loc (Buffer.contents b);
    Buffer.clear b
  in
  Output.add_kidoku loc;
  set_quotes true;
  let rec parse elt =
    if !ignore_one_space && not (elt matches `Space _) then ignore_one_space := false;
    match elt with
      | `EOS | `Delete _ | `Rewrite _ -> assert false
      | `DQuote _ -> set_quotes true; Buffer.add_string b "\\\""
      | `RCur _ -> set_quotes false;
                   Buffer.add_string b "\x81\x7a";
                   ignore_one_space := true
      | `LLentic _ -> set_quotes true; Buffer.add_string b "\x81\x79"
      | `RLentic _ -> Buffer.add_string b "\x81\x7a"
      | `Asterisk _ -> set_quotes true; Buffer.add_string b "\x81\x96"
      | `Percent _ -> set_quotes true; Buffer.add_string b "\x81\x93"
      | `Speaker _ -> set_quotes false; Buffer.add_string b "\x81\x79"
      | `Hyphen _  -> set_quotes true; Buffer.add_string b "-"
      | `Code (loc, id, _, params) when id = Text.ident "e" || id = Text.ident "em"
         -> let idx, size =
              match params with
                | [`Simple (_, idx)] -> idx, None
                | [`Simple (_, idx); `Simple (_, size)] -> idx, Some size
                | _ -> ksprintf (error loc) "incorrect parameters to code \\%s{}" (Text.to_sjs id)
            in
            let i =
              try
                match !Global.expr__normalise_and_get_const idx ~expect:`Int ~abort_on_fail:false with `Integer i -> Int32.to_int i | _ -> assert false
              with _ ->
                error loc "emoji index must be constant in static text"
            in
            set_quotes false;
            Option.may (fun sz -> flush (); call "FontSize" [sz]) size;
            bprintf b "\x81\x94\x82%c\x82%c\x82%c"
              (if Text.length id = 1 then '\x60' else '\x61')
              (char_of_int (0x4f + if i > 10 then i / 10 else 0))
              (char_of_int (0x4f + i mod 10));
            Option.may (fun _ -> flush (); call "FontSize" []) size
      | `Code (loc, id, _, params)
         -> flush ();
            Function.compile (loc, None, Text.to_sjs id, id, params, None) ~is_code:true
      | `Text (_, _, t) -> set_quotes true; Buffer.add_string b (TextTransforms.to_bytecode t)
      | `Space (_, i) -> let j = if i > 0 && !ignore_one_space then (ignore_one_space := false; i - 1) else i in
                         Buffer.add_string b (String.make j ' ')
      | `Name (l, lg, i, w)
         -> set_quotes false;
            let lg = if lg = `Local then "\x81\x93" else "\x81\x96" in
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
      | `Gloss (l, `Gloss, base, _) 
         -> warning l "\\g{} is not implemented in unformatted text";
            DynArray.iter parse base
      | `Gloss (_, `Ruby, base, gloss)
         -> let gloss = match gloss with `Closed (_, g) -> g | `ResStr _ -> (* converted to `Closed in Expr.traverse_str_tokens *) assert false in
            flush ();
            call "__doruby" [];
            DynArray.iter parse base;
            flush ();
            call "__doruby" [`Str (nowhere, gloss)]
      | `Add elt -> Queue.add elt addstrs
  in
  DynArray.iter parse text;
  flush ()
  

let handle_rewrite pause_or_page loc text =
  (* Extract the rewrite code *)
  let ntext = DynArray.make (DynArray.length text - 1) 
  and func = ref ~-1
  in
  DynArray.iter
    (function
      | `Rewrite (_, k) when !func = ~-1 -> func := k
      | `Rewrite (l, _) -> error l "a string may only have one \\f{} code"
      | elt -> DynArray.add ntext elt)
    text;
  assert (!func <> ~-1);
  (* Update $s and $pause tokens *)
  let func = DynArray.get StrLexer.rewrites !func in
  let nfunc = Queue.create () in
  let add t l = Queue.add (t, l) nfunc in
  DynArray.iter
    (function
      | KeAstParser.SPECIAL `S, l -> add (KeAstParser.STRING ntext) l
      | KeAstParser.SPECIAL `Pause, l 
         -> (match pause_or_page with
              | `No -> ()
              | `Pause _ -> add (KeAstParser.IDENT ("pause", Text.ident "pause")) l
              | `Page _ -> add (KeAstParser.IDENT ("page", Text.ident "page")) l)
      | elt -> Queue.add elt nfunc)
    func;
  Queue.add (KeAstParser.EOF, nowhere) nfunc;
  (* Parse and compile the code *)
  Meta.parse (KeULexer.call_parser KeAstParser.just_statements (fun _ () -> Queue.pop nfunc) ())


let rec do_compile ?(eaddstrs = Queue.create ()) compilefun (loc, text, pause_or_page) =
  let text =
    match text with
      | `Str (_, text) -> text
      | `SVar _ -> Global.dynArray (`Code (loc, Text.of_arr [| 0x73 |], None, [`Simple (loc, text)]))
      | _ -> failwith "expected textout to be normalised to `Str _"
   in
  let addstrs = Queue.create () in
  let this_pause_or_page = ref pause_or_page in
  (* If there's a \d, delete the string. *)
  if try ignore (Enum.find (function `Delete _ -> true | _  -> false) (DynArray.enum text)); true with Not_found -> false then
    this_pause_or_page := `No
  (* If there's an \f, perform the transformation requested. *)
  else if try ignore (Enum.find (function `Rewrite _ -> true | _  -> false) (DynArray.enum text)); true with Not_found -> false then
   (this_pause_or_page := `No; 
    handle_rewrite pause_or_page loc text)
  (* Otherwise compile the string normally. *)
  else compilefun addstrs loc text;
  (* In all cases, finalise. *)
  Queue.transfer eaddstrs addstrs;
  (match !this_pause_or_page with
    | `Pause l -> call "pause" []
    | `Page l -> call (if Queue.is_empty addstrs then "page" else "pause") []
    | `No -> ());
  if not (Queue.is_empty addstrs) then
    let _, (nextloc, nexttxt) = Queue.take addstrs in
    let text, loc = Global.get_resource nextloc (Text.to_err nexttxt, nexttxt) in
    let stext = (* FIXME: refactor this (partly a C&P from CompilerFrame.handle_textout) *)
      match Expr.normalise (`Return (loc, false, `Str (loc, text))) with
        | `Single (`Return (_, _, e)) -> e
        | `Nothing 
        | `Single _ -> assert false
        | `Multiple elts
           -> assert (DynArray.length elts > 0);
              let last = DynArray.last elts in
              DynArray.delete_last elts;
              Meta.parse elts;
              Memory.close_scope ();
              match last with `Return (_, _, e) -> e | _ -> assert false
    in
    do_compile compilefun (loc, stext, pause_or_page) ~eaddstrs:addstrs 



let compile_stub text = do_compile compile_stub text
and compile_vwf text = do_compile RlBabel.compile text
and compile text = do_compile compile text
