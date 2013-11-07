(*
    Rlc: general-purpose string tokeniser
    Copyright (C) 2006 Haeleth

   Replaces three specialised lexers (KeULexer.lex_string, KeULexer.lex_resstr,
   and TextLexer.lex in RLAS 1.03) with one that adequately handles all cases.

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
(*ppopt ./pa_matches.cmo pa_macro.cmo *)

open Printf
open Ulexing
open KeTypes
open KeAst

(* Hacks to permit inter-module recursion. *)
type 'a entry_t = (Lexing.lexbuf -> KeAstParser.token) -> Lexing.lexbuf -> 'a
module KeULexer :
  sig
    val init :
      ('a entry_t -> location -> Text.t -> 'a) ->
      (location -> lexbuf -> KeAstParser.token * location) ->
      unit
    val call_parser_on_text : 'a entry_t -> location -> Text.t -> 'a
    val get_token : location -> lexbuf -> KeAstParser.token * location
  end = struct
    let cp = ref (fun _ _ _ -> failwith "StrLexer.KeULexer.call_parser_on_text: not initialised")
    let gt = ref (fun _ _ -> failwith "StrLexer.KeULexer.get_token: not initialised")
    let init f g = cp := Obj.magic f; gt := g
    let call_parser_on_text entrypoint loc text = Obj.magic (!cp) entrypoint loc text
    let get_token loc lexbuf = !gt loc lexbuf
  end


(* Hack to get around the inability to store parser tokens in a module referenced from the parser *)

let rewrites = DynArray.create ()


(* The actual lexer *)

type aux_t =
  { term: [ `Single | `Double | `ResStr ];
    file: string;
    mutable line: int; }

let loc aux = { KeTypes.file = aux.file; line = aux.line }
let aux loc term = { file = loc.KeTypes.file; line = loc.KeTypes.line; term = term }

let error aux = error (loc aux)

let unterminated ?(msg = "") aux = error aux ("unterminated string" ^ if msg <> "" then ": " ^ msg else "")

let regexp sp = [" \t" 0x3000]*

let rec skip_comment aux =
  lexer
    | "-}" -> ()
    | '\n' -> aux.line <- aux.line + 1; skip_comment aux lexbuf
    | '-'? [^ "-\n"]+ -> skip_comment aux lexbuf
    | eof -> error aux "unterminated comment"

let get_anon_resstr_key =
  let anon_resstrs = ref ~-1 in
  fun () ->
    incr anon_resstrs;
    Text.norm (ksprintf Text.of_sjs "__anon_resstr_%04d__" !anon_resstrs)

let get_resstr_key aux lexbuf =
  let rv = ref Text.empty in
  let lc = ref (loc aux) in
  let set_rv v =
    if Text.length !rv = 0
    then (lc := loc aux; rv := v)
    else rv := Text.append !rv v
  in
  let rec get_quoted_key qchar b =
    lexer
      | ["'\""]
         -> if lexeme_char lexbuf 0 <> qchar
            then (Text.Buf.add_int b (lexeme_char lexbuf 0); get_quoted_key qchar b lexbuf)
            else Text.Buf.contents b
      | '\\' '\r'? '\n' sp -> aux.line <- aux.line + 1; get_quoted_key qchar b lexbuf
      | '\\' _ -> Text.Buf.add_int b (lexeme_char lexbuf 1); get_quoted_key qchar b lexbuf
      | [^ "'\"\\\n"]+ -> Text.Buf.add_array b (lexeme lexbuf); get_quoted_key qchar b lexbuf
      | eof | '\n' -> unterminated aux
      | _ -> ksprintf (error aux) "invalid character 0x%02x in string literal" (lexeme_char lexbuf 0)
  and get_key =
    lexer
      | sp -> get_key lexbuf
      | '\r'? '\n' -> aux.line <- aux.line + 1; get_key lexbuf
      | ["'\""]
         -> set_rv (get_quoted_key (lexeme_char lexbuf 0) (Text.Buf.create 0) lexbuf);
            get_key lexbuf
      | ['0'-'9']+
         -> set_rv (Text.of_sjs (string_of_int (int_of_string (latin1_lexeme lexbuf))));
            get_key lexbuf
      | '$' ['0'-'9' 'A'-'Z' 'a'-'z']+
           -> set_rv (Text.of_sjs (string_of_int (int_of_string ("0x" ^ latin1_sub_lexeme lexbuf 1 (lexeme_length lexbuf - 1)))));
              get_key lexbuf
      | [^ " '\"\t\r\n>}"]+ -> set_rv (Text.of_arr (lexeme lexbuf)); get_key lexbuf
      | [">}"] sp -> !lc, !rv
      | eof -> raise End_of_file
      | _ -> ksprintf failwith "inexhaustive match at %s char %d" __FILE__ (fst __LOCATION__)
  in
  get_key lexbuf

let get_f_code aux floc lexbuf =
  let carr = DynArray.create () in
  let rec get_code loc bracelevel =
    let tkn, nloc as elt = KeULexer.get_token loc lexbuf in
    if tkn = KeAstParser.EOF || (tkn = KeAstParser.RCUR && bracelevel == 1) then (
      aux.line <- nloc.KeTypes.line;
      carr
    )
    else (
      DynArray.add carr elt; 
      get_code nloc
        (match tkn with 
           | KeAstParser.RCUR -> bracelevel - 1 
           | KeAstParser.LCUR -> bracelevel + 1
           | _ -> bracelevel)
    )
  in
  get_code floc 1

let rec get_token aux : lexbuf -> strtoken =
  let get_closed_tokens restrict ident context lexbuf =
    let tkns = DynArray.create () in
    let rec loop () =
      match get_token aux lexbuf with
        | `EOS -> unterminated aux ~msg:(sprintf "expected `}' in \\%s code" ident)
        | `RCur _ -> tkns
        | `Speaker _ when restrict -> ksprintf (error aux) "\\{} is illegal in %s" context
        | `Gloss (_, g, _, _) when restrict -> ksprintf (error aux) "\\%s{} is illegal in %s" (match g with `Gloss -> "g" | `Ruby -> "ruby") context
        | tkn -> DynArray.add tkns tkn; loop ()
    in
    loop ()
  in
  lexer
   (* Possible terminators *)
    | '\'' -> if aux.term = `Single then `EOS else `Text (loc aux, `Sbcs, Text.of_char 0x27)
    | '\"' -> if aux.term = `Double then `EOS else `DQuote (loc aux)
    | "<" -> if aux.term = `ResStr then `EOS else `Text (loc aux, `Sbcs, Text.of_char 0x3c)
    | eof -> if aux.term = `ResStr then `EOS else unterminated aux

   (* Line breaks *)
    | '\r'
       -> get_token aux lexbuf
    | sp '\r'? '\n' sp
       -> aux.line <- aux.line + 1;
          `Space (loc aux, 1)
    | '\\' sp '\r'? '\n' sp
       -> aux.line <- aux.line + 1;
          get_token aux lexbuf

   (* Comments *)
    | "{-" -> if aux.term = `ResStr then
                (skip_comment aux lexbuf; get_token aux lexbuf)
              else
                `Text (loc aux, `Sbcs, Text.of_arr (lexeme lexbuf))
    | "//" -> if aux.term = `ResStr then
                (lexer
                  | [^ '\n']* '\n' sp -> aux.line <- aux.line + 1; get_token aux lexbuf
                  | _ | eof -> `EOS)
                lexbuf
              else
                `Text (loc aux, `Sbcs, Text.of_arr (lexeme lexbuf))
    | ["{/"] -> `Text (loc aux, `Sbcs, Text.of_arr (lexeme lexbuf))

   (* Control codes *)
    | "}" -> `RCur (loc aux)

    | "\\{"
    | "\\name" sp "{"
       -> `Speaker (loc aux)

    | "\\g" (sp ["{"])?
       -> if lexeme_length lexbuf = 2 then error aux "expected `{' after \\g";
          let gloc = loc aux in
          let tkns = get_closed_tokens true "g" "a glossed term" lexbuf in
          let get_key =
            lexer
              | sp "=" sp "<" -> (try `ResStr (get_resstr_key aux lexbuf) with End_of_file -> unterminated aux)
              | sp "=" sp "{" -> let l = loc aux in
                                 let k = get_anon_resstr_key () and tkns = get_closed_tokens false "g" "" lexbuf in
                                 Hashtbl.replace Global.resources k (tkns, l);
                                 `ResStr (l, k)
              | _ -> error aux "expected '=' after \\g{}"
              | eof -> unterminated aux ~msg:"expected '=' after \\g{}"
          in
          `Gloss (gloc, `Gloss, tkns, get_key lexbuf)

    | "\\ruby" (sp "{")?
       -> if lexeme_length lexbuf = 5 then error aux "expected `{' after \\ruby";
          let rloc = loc aux in
          let tkns = get_closed_tokens true "ruby" "\\ruby{} base text" lexbuf in
          let get_key =
            lexer
              | sp "=" sp "<" -> (try `ResStr (get_resstr_key aux lexbuf) with End_of_file -> unterminated aux)
              | sp "<" -> warning (loc aux) "the format \\ruby{...}<id> is deprecated: use \\ruby{...}=<id> instead";
                          (try `ResStr (get_resstr_key aux lexbuf) with End_of_file -> unterminated aux)
              | sp "=" sp "{" -> let l = loc aux in `Closed (l, get_closed_tokens true "ruby" "\\ruby{} gloss text" lexbuf)
              | sp "{" -> let l = loc aux in
                          warning l "the format \\ruby{...}{...} is deprecated: use \\ruby{...}={...} instead";
                          `Closed (l, get_closed_tokens true "ruby" "\\ruby{} glosses" lexbuf)
              | _ -> error aux "expected '=' after \\ruby{}"
              | eof -> unterminated aux ~msg:"expected '=' after \\ruby{}"
          in
          `Gloss (rloc, `Ruby, tkns, get_key lexbuf)

    | "\\a" sp "{"
       -> let loc = loc aux in
          `Add (loc, try get_resstr_key aux lexbuf with End_of_file -> unterminated aux)
    | "\\a"
       -> `Add (loc aux, (loc aux, Text.empty))

    | "\\d" (sp "{" sp "}")?
       -> `Delete (loc aux)

    | "\\f" (sp "{")?
       -> let floc = loc aux in
          let code =
            if lexeme_length lexbuf > 2 then
              (* use specified code *)
              get_f_code aux floc lexbuf
            else 
              (* use default *)
              let str = 
                Global.expr__normalise_and_get_str
                  (`VarOrFn (floc, "__DefStrFunc__", Text.ident "__DefStrFunc__")) 
              in
              let aline = aux.line in
              let rv = 
                get_f_code aux 
                  (loc_of_expr (Memory.get_as_expression (Text.ident "__DefStrFunc__"))) 
                  (from_utf8_string (StrTokens.to_string str ~enc:"UTF8"))
              in
              aux.line <- aline; (* Prevent line number resetting bug *)
              rv
          in
          let key = DynArray.length rewrites in
          DynArray.add rewrites code;
          `Rewrite (floc, key)

    | '\\'['A'-'Z''a'-'z']
       -> `Code (loc aux, Text.of_char (lexeme_char lexbuf 1), None, [])

    | '\\'['A'-'Z''a'-'z''_']+ sp [":{"]
       -> let codeloc = loc aux in
          let code =
            let rhs = ref (lexeme_length lexbuf - 1) in
            while List.mem (lexeme_char lexbuf !rhs) [0x09; 0x20; 0x3000] do decr rhs done;
            Text.of_arr (sub_lexeme lexbuf 1 (!rhs - 1))
          and optarg =
            if latin1_lexeme_char lexbuf (lexeme_length lexbuf - 1) = '{' then None else
              let b = Text.Buf.create 0 in
              let rec get_opt pc =
                lexer
                  | '\r'? '\n' -> if aux.term <> `ResStr then unterminated aux; aux.line <- aux.line + 1; get_opt pc lexbuf
                  | '\\' '\r'? '\n' -> aux.line <- aux.line + 1; get_opt pc lexbuf
                  | '\r' -> get_opt pc lexbuf
                  | '\\' -> Text.Buf.add_char b '\\'; get_opt pc lexbuf
                  | "{" -> if pc > 0 then (Text.Buf.add_char b '{'; get_opt pc lexbuf) else Text.Buf.contents b
                  | "(" -> Text.Buf.add_char b '('; get_opt (pc + 1) lexbuf
                  | ")" -> Text.Buf.add_char b ')'; get_opt (if pc = 0 then 0 else pc - 1) lexbuf
                  | [^ "\\\r\n(){"]+ -> Text.Buf.add_array b (lexeme lexbuf); get_opt pc lexbuf
                  | eof -> if aux.term <> `ResStr then unterminated aux; Text.Buf.contents b
                  | _ -> ksprintf failwith "inexhaustive match at %s char %d" __FILE__ (fst __LOCATION__)
              in
              let text = get_opt 0 lexbuf in
              Some (KeULexer.call_parser_on_text KeAstParser.just_expression (loc aux) text)
          in
          let b = Text.Buf.create 0 in
          let rec get_arg inc bc =
            lexer
              | '\r'? '\n' -> if aux.term <> `ResStr then unterminated aux; aux.line <- aux.line + 1; Text.Buf.add_char b '\n'; get_arg inc bc lexbuf
              | '\\' '\r'? '\n' -> aux.line <- aux.line + 1; Text.Buf.add_char b '\n'; get_arg inc bc lexbuf
              | '\r' -> get_arg inc bc lexbuf
              | ["\\-"] -> Text.Buf.add_int b (lexeme_char lexbuf 0); get_arg inc bc lexbuf
              | "{" -> Text.Buf.add_char b '{'; get_arg inc (if inc then bc else bc + 1) lexbuf
              | [^ "\\\r\n{-}"]+ -> Text.Buf.add_array b (lexeme lexbuf); get_arg inc bc lexbuf
              | eof -> if aux.term <> `ResStr then unterminated aux; Text.Buf.contents b
              | "}"
                -> if inc then (Text.Buf.add_char b '}'; get_arg true bc lexbuf)
                   else if bc > 0 then (Text.Buf.add_char b '}'; get_arg inc (bc - 1) lexbuf)
                   else Text.Buf.contents b
              | "-}"
                -> if inc then (Text.Buf.add_array b (lexeme lexbuf); get_arg false bc lexbuf)
                   else if bc > 0 then (Text.Buf.add_array b (lexeme lexbuf); get_arg false (bc - 1) lexbuf)
                   else (Text.Buf.add_char b '-'; Text.Buf.contents b)
              | "{-" -> Text.Buf.add_array b (lexeme lexbuf); get_arg true bc lexbuf
              | _ -> ksprintf failwith "inexhaustive match at %s char %d" __FILE__ (fst __LOCATION__)
          in
          let iloc = loc aux in
          let text = get_arg false 0 lexbuf in
          let arglist = KeULexer.call_parser_on_text KeAstParser.just_param_list iloc text in
          if code = Text.ident "l" || code = Text.ident "m" then
            let return nidx cidx =
              let lg = if code = Text.ident "l" then `Local else `Global in
              let idx =
                match nidx with
                  | `VarOrFn (l, s, t) when Text.length t <= 2
                     -> let t = Text.to_arr t in
                        let getval i =
                          if i >= 0x61 && i <= 0x7a then i - 0x61
                          else if i >= 0x41 && i <= 0x5a then i - 0x41
                          else if i >= 0xff21 && i <= 0xff3a then i - 0xff21
                          else raise Exit
                        in
                        (try
                          let i = if Array.length t = 2 then (getval t.(0) + 1) * 26 + getval t.(1) else getval t.(0) in
                          `Int (l, Int32.of_int i)
                        with _ -> nidx)
                  | _ -> nidx
              in
              `Name (codeloc, lg, idx, cidx)
            in
            match arglist with
              | [] -> ksprintf (KeTypes.error iloc) "expected argument to control code \\%s{}" (Text.to_sjs code)
              | [`Simple (_, a)] -> return a None
              | [`Simple (_, a); `Simple (_, b)] -> return a (Some b)
              | [_] | [_; _] -> ksprintf (KeTypes.error iloc) "arguments to control code \\%s{} must be simple expressions" (Text.to_sjs code)
              | _ -> ksprintf (KeTypes.error iloc) "too many arguments to control code \\%s{}" (Text.to_sjs code)
          else
            `Code (codeloc, code, optarg, arglist)

   (* Spaces *)
    | ("\\_" | sp)+
       -> `Space
            (loc aux,
             Array.fold_left
              (fun acc -> function
                | 0x5c -> acc
                | 0x3000 | 0x09 -> acc + 2
                | _ -> acc + 1)
               0 (lexeme lexbuf))

   (* Escaped characters *)
    | "\\\"" -> `DQuote (loc aux)
    | "\\k" [^ "\r\n"] 
    | "\\"_  -> let c = lexeme_char lexbuf (lexeme_length lexbuf - 1) in
                `Text (loc aux, (if c > 0x300 then `Dbcs else `Sbcs), Text.of_char c)

   (* Special characters *)
    | 0x3010 (*y*) -> `LLentic  (loc aux)
    | 0x3011 (*z*) -> `RLentic  (loc aux)
    | 0xff0a (*–*) -> `Asterisk (loc aux)
    | 0xff05 (*“*) -> `Percent  (loc aux)
    | "-" -> `Hyphen (loc aux)
   
    | [0-8 (*\t \n*) 0xb-0xc (*\r*) 0xe-0x1f (* *) 0x21 (*'"'*) 0x23-0x26 (*'*)
       0x28-0x2c (*-*) 0x2e (*/*) 0x30-0x3b (*<*) 0x3d-0x5b (*\*) 0x5d-0x7c (*{*) 
       0x7e-0x2ff]+
       -> `Text (loc aux, `Sbcs, Text.of_arr (lexeme lexbuf))
    | [0x300-0x2fff (*wide space*) 0x3001-0x300f (*yz*) 0x3012-0xff04 (*“*)
       0xff06-0xff09 (*–*) 0xff0b-0xffff]+
       -> `Text (loc aux, `Dbcs, Text.of_arr (lexeme lexbuf))

    | _ -> ksprintf failwith "inexhaustive match at %s char %d" __FILE__ (fst __LOCATION__)


let get_string aux lexbuf =
  let rv = DynArray.create () in
  let rec loop () =
    let tkn = get_token aux lexbuf in
    if tkn = `EOS then rv else loop (DynArray.add rv tkn)
  in
  loop ()

let get_string_tokens term { KeTypes.file = file; line = line } lexbuf =
  let aux = { term = term; file = file; line = line } in
  let rv = get_string aux lexbuf in
  rv, loc aux


let rec lex_resfile_header aux =
  lexer
   (* Results: whether the file contains any resource strings *)
    | "<" -> true
    | eof -> false
   (* Spacing *)
    | [" \t"]+ -> lex_resfile_header aux lexbuf
    | '\r'? '\n'  -> aux.line <- aux.line + 1; lex_resfile_header aux lexbuf
    | "//" -> (lexer [^ '\n']* '\n' | _ | eof -> ()) lexbuf;
              aux.line <- aux.line + 1;
              lex_resfile_header aux lexbuf
    | "{-" -> skip_comment aux lexbuf;
              lex_resfile_header aux lexbuf
   (* Directives *)
    | "#character" sp
       -> let tkn, loc = KeULexer.get_token (loc aux) lexbuf in
          let s =
            match tkn with
              | KeAstParser.STRING s -> s
              | KeAstParser.IDENT (s, t)
                  -> (match !Global.expr__normalise_and_get_const (`VarOrFn (loc, s, t)) ~expect:`Str with
                        | `String s -> s
                        | _ -> assert false)
              | KeAstParser.DRES _
                  -> KeTypes.error loc "#res<> is not valid in resource files themselves"
              | _ -> KeTypes.error loc "expected string literal"
          in
          DynArray.add Global.dramatis_personae (StrTokens.to_string s);
          aux.line <- loc.KeTypes.line;
          lex_resfile_header aux lexbuf
   (* Erroneous states *)
    | "#" ['A'-'Z' 'a'-'z' '0'-'9' "_$"]+
       -> ksprintf (error aux) "invalid directive `%s' in resource file header" (latin1_lexeme lexbuf)
    | _
       -> ksprintf (error aux) "invalid character 0x%02x in resource file header" (lexeme_char lexbuf 0)


let lex_resstr aux lexbuf =
  let startpos, key = get_resstr_key aux lexbuf in
  let str = get_string aux lexbuf in
  if DynArray.length str > 0 && (DynArray.last str matches `Space (_, 1)) then DynArray.delete_last str;
  startpos, key, str

(* Process an Rlc-format resource string: resolve anonymous references and add the string
   to the global map. *)
let rec handle_resstr aux lexbuf (startpos, ikey, istr) =
  (* Normalise the key, creating one if this is an anonymous string.  Erroneous instances
     of anonymous strings have already been filtered out in lex_resfile. *)
  let key =
    if ikey = Text.empty
    then get_anon_resstr_key ()
    else Text.norm ikey
  in
  (* Warn about duplicate keys. *)
  if Hashtbl.mem Global.resources key then (
    let eloc = snd (Hashtbl.find Global.resources key) in
    ksprintf (KeTypes.warning startpos)
      "duplicate resource string key <%s> hides earlier definition at %s line %d"
      (Text.to_err key) eloc.KeTypes.file eloc.KeTypes.line
  );
  (* Scan the string, checking for anonymous references; for each one we find, we call
     handle_resstr recursively on the resource string in the file, which produces the
     documented behaviour with respect to nested anonymous references. *)
  (*TODO: check whether writing this with iteri and mutation would be noticably more efficient. *)
  let str =
    let getkey t =
      if t <> Text.empty
      then Text.norm t
      else let resstr = lex_resstr aux lexbuf in handle_resstr aux lexbuf resstr
    in
    DynArray.map
      (function
        | `Gloss (l, g, e, `ResStr (ll, t)) -> `Gloss (l, g, e, `ResStr (ll, getkey t))
        | `Add (l, (ll, t)) -> `Add (l, (ll, getkey t))
        | `Rewrite (_, key) as e
           -> let first = ref true in
              let code = DynArray.unsafe_get rewrites key in
              DynArray.iteri
                (fun i -> function
                  | KeAstParser.SPECIAL `S, _ when !first 
                      -> first := false
                  | KeAstParser.SPECIAL `S, l 
                      -> DynArray.unsafe_set code i (KeAstParser.DRES (getkey Text.empty), l)
                  | _ -> ())
                code;
              e
        | e -> e)
      istr
  in
  (* Finally, update the global map and return the normalised key. *)
  Hashtbl.replace Global.resources key (str, startpos);
  key

(* Read the contents of an Rlc format resource file. *)
let lex_resfile fname lexbuf =
  let aux = { term = `ResStr; file = fname; line = 1 } in
  (* lex_resfile_header returns true if it found the start of a string, false otherwise. *)
  if lex_resfile_header aux lexbuf then try
    while true do
      (* Retrieve a main resource string; check its key is not empty (we permit anonymous strings
         only where they are referenced by other resource strings). *)
      let kpos, key, _ as resstr = lex_resstr aux lexbuf in
      if key = Text.empty then KeTypes.error kpos "unmatched anonymous resource string";
      (* Pass to handle_resstr to resolve anonymous references and add to the global resource map.
         handle_resstr returns the key (for use in recursive calls), so we need to ignore that. *)
      ignore (handle_resstr aux lexbuf resstr)
    done;
    assert false
  (* End_of_file is raised by get_resstr_key if it did not find the start of another string. *)
  with End_of_file -> ()
