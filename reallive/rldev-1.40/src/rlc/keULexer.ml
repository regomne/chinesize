(*
    Rlc: Kepago lexer
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

open Printf
open KeTypes
open KeAstParser
open Ulexing


(* Regexp definitions *)
let regexp digit = ['0'-'9']
let regexp hex = ['0'-'9' 'a'-'f' 'A'-'F']
let regexp decimal = digit (digit | '_')*
let regexp hex_num = '$' hex (hex | '_')*
let regexp oct_num = "$%" ['0'-'7' '_']*
let regexp bin_num = "$#" ["01_"]*
let regexp kpg_number = hex_num | decimal | oct_num | bin_num

let ml_num_of_kpg_num s =
  if String.length s > 1 then
    if s.[1] = '#' then (s.[0] <- '0'; s.[1] <- 'b') else if s.[1] = '%' then (s.[0] <- '0'; s.[1] <- 'o');
  if s.[0] = '$' then "0x" ^ String.sub s 1 (String.length s - 1) else s

let regexp sp = [" \t" 0x3000]*

let regexp id_start =
  ['A'-'Z' 'a'-'z' '?' '_' '#' '$'
   0x391-0x3a1 0x3a3-0x3a9 0x3b1-0x3c1 0x3c3-0x3c9
   0x401 0x410-0x44f 0x451
   0x2010 0x2015 0x2018-0x2019 0x201c-0x201d 0x2020-0x2021 0x2025-0x2026 0x2030 0x2032-0x2033 0x203b 0x2103 0x2116 0x2121 0x212b 0x2160-0x2169 0x2170-0x2179 0x2190-0x2193 0x21d2 0x21d4 0x2200 0x2202-0x2203 0x2207-0x2208 0x220b 0x2211 0x221a 0x221d-0x2220 0x2225 0x2227-0x222c 0x222e 0x2234-0x2235 0x223d 0x2252 0x2260-0x2261 0x2266-0x2267 0x226a-0x226b 0x2282-0x2283 0x2286-0x2287 0x22a5 0x22bf 0x2312 0x2460-0x2473 0x2500-0x2503 0x250c 0x250f-0x2510 0x2513-0x2514 0x2517-0x2518 0x251b-0x251d 0x2520 0x2523-0x2525 0x2528 0x252b-0x252c 0x252f-0x2530 0x2533-0x2534 0x2537-0x2538 0x253b-0x253c 0x253f 0x2542 0x254b 0x25a0-0x25a1 0x25b2-0x25b3 0x25bc-0x25bd 0x25c6-0x25c7 0x25cb 0x25ce-0x25cf 0x25ef 0x2605-0x2606 0x2640 0x2642 0x266a 0x266d 0x266f
   0x3000-0x3003 0x3005-0x3015 0x301d 0x301f
   0x3041-0x3093 0x309b-0x309e
   0x30a1-0x30f6 0x30fb-0x30fe
   0xb4 0xa8 0xb1 0xd7 0xf7 0xb0 0xa7 0xb6
   0x3231-0x3232 0x3239 0x32a4-0x32a8 0x3303 0x330d 0x3314 0x3318 0x3322-0x3323 0x3326-0x3327 0x332b 0x3336 0x333b 0x3349-0x334a 0x334d 0x3351 0x3357 0x337b-0x337e 0x338e-0x338f 0x339c-0x339e 0x33a1 0x33c4 0x33cd
   0x4e00-0x9fa0 0xf929 0xf9dc 0xfa0e-0xfa2d
   0xff01-0xff5e
   0xffe0-0xffe5]

let regexp id_cont = id_start | ['0'-'9']
let regexp ident = id_start id_cont*


(* Keywords *)
let keywords = ref (
  Array.fold_left (fun m (k, v) -> PMap.add (Text.ident k) v m) PMap.empty
    [|
    (* Directives *)
      "#file",        DWITHEXPR ("file",        `Str);
      "#resource",    DWITHEXPR ("resource",    `Str);
      "#entrypoint",  DWITHEXPR ("entrypoint",  `Int);
      "#character",   DWITHEXPR ("character",   `Str);
      "#val_0x2c",    DWITHEXPR ("val_0x2c",    `Int);
      "#kidoku_type", DWITHEXPR ("kidoku_type", `Int);
      "#print",       DWITHEXPR ("print",       `None);
      "#error",       DWITHEXPR ("error",       `None);
      "#warn",        DWITHEXPR ("warn",        `None);
      "#hiding",      DHIDING;
      "#define",      DDEFINE `Define;
      "#sdefine",     DDEFINE `DefineScoped;
      "#undef",       DUNDEF;
      "#redef",       DDEFINE `Redefine;
      "#const",       DDEFINE `Const;
      "#bind",        DDEFINE `Bind;
      "#ebind",       DDEFINE `EBind;
      "#set",         DSET;
      "#target",      DTARGET;
      "#version",     DVERSION;
      "#inline",      DINLINE false;
      "#sinline",     DINLINE true;
      "#load",        DLOAD;

    (* Keywords per se *)
      "eof", DEOF;
      "halt", DHALT;
      "op", OP;
      "return", RETURN;
      "_", USCORE;
      "int", INT 32;
      "str", STR;
      "bit", INT 1;
      "bit2", INT 2;
      "bit4", INT 4;
      "byte", INT 8;
      "#if", DIF;
      "#ifdef", DIFDEF true;
      "#ifndef", DIFDEF false;
      "#else", DELSE;
      "#elseif", DELSEIF;
      "#endif", DENDIF;
      "#for", DFOR;
      "if", IF;
      "else", ELSE;
      "while", WHILE;
      "repeat", REPEAT;
      "till", TILL;
      "for", FOR;
      "case", CASE;
      "of", OF;
      "other", OTHER;
      "ecase", ECASE;
      "break", BREAK;
      "continue", CONTINUE;
      "raw", RAW;
      "endraw", ENDRAW;
      
      "$s", SPECIAL `S;
      "$pause", SPECIAL `Pause;

    (* Variables *)
      "store", REG 0xc8;
      Config.svar_prefix ^ "K", SVAR 0x0a;  Config.ivar_prefix ^ "L", VAR 0x0b;
      Config.svar_prefix ^ "M", SVAR 0x0c;  Config.svar_prefix ^ "S", SVAR 0x12;
      Config.ivar_prefix ^ "A", VAR 0x00;   Config.ivar_prefix ^ "B", VAR 0x01;
      Config.ivar_prefix ^ "C", VAR 0x02;   Config.ivar_prefix ^ "D", VAR 0x03;
      Config.ivar_prefix ^ "E", VAR 0x04;   Config.ivar_prefix ^ "F", VAR 0x05;
      Config.ivar_prefix ^ "G", VAR 0x06;   Config.ivar_prefix ^ "Z", VAR 0x19;
     Config.ivar_prefix ^ "Ab", VAR 0x1a;  Config.ivar_prefix ^ "Bb", VAR 0x1b;
     Config.ivar_prefix ^ "Cb", VAR 0x1c;  Config.ivar_prefix ^ "Db", VAR 0x1d;
     Config.ivar_prefix ^ "Eb", VAR 0x1e;  Config.ivar_prefix ^ "Fb", VAR 0x1f;
     Config.ivar_prefix ^ "Gb", VAR 0x20;  Config.ivar_prefix ^ "Zb", VAR 0x33;
    Config.ivar_prefix ^ "A2b", VAR 0x34; Config.ivar_prefix ^ "B2b", VAR 0x35;
    Config.ivar_prefix ^ "C2b", VAR 0x36; Config.ivar_prefix ^ "D2b", VAR 0x37;
    Config.ivar_prefix ^ "E2b", VAR 0x38; Config.ivar_prefix ^ "F2b", VAR 0x39;
    Config.ivar_prefix ^ "G2b", VAR 0x3a; Config.ivar_prefix ^ "Z2b", VAR 0x4d;
    Config.ivar_prefix ^ "A4b", VAR 0x4e; Config.ivar_prefix ^ "B4b", VAR 0x4f;
    Config.ivar_prefix ^ "C4b", VAR 0x50; Config.ivar_prefix ^ "D4b", VAR 0x51;
    Config.ivar_prefix ^ "E4b", VAR 0x52; Config.ivar_prefix ^ "F4b", VAR 0x53;
    Config.ivar_prefix ^ "G4b", VAR 0x54; Config.ivar_prefix ^ "Z4b", VAR 0x67;
    Config.ivar_prefix ^ "A8b", VAR 0x68; Config.ivar_prefix ^ "B8b", VAR 0x69;
    Config.ivar_prefix ^ "C8b", VAR 0x6a; Config.ivar_prefix ^ "D8b", VAR 0x6b;
    Config.ivar_prefix ^ "E8b", VAR 0x6c; Config.ivar_prefix ^ "F8b", VAR 0x6d;
    Config.ivar_prefix ^ "G8b", VAR 0x6e; Config.ivar_prefix ^ "Z8b", VAR 0x81; 

    (* Hard functions *)
      "goto_on",      GOTOON   (`Goto);
      "goto_case",    GOTOCASE (`Goto);
      "gosub_on",     GOTOON   (`Gosub);
      "gosub_case",   GOTOCASE (`Gosub);
      "select_w",     SELECT ("select_w",   0);
      "select",       SELECT ("select",     1);
      "select_s2",    SELECT ("select_s2",  2);
      "select_s",     SELECT ("select_s",   3);
      "select_w2",    SELECT ("select_w2", 10);
    |]
  )

let token_of_identifier tkn =
  (* Add goto-type functions to keyword table dynamically *)
  if not (DynArray.empty gotofuncs) then (
    keywords := DynArray.fold_left (fun m (s, t) -> PMap.add t (GOTO (s, t)) m) !keywords gotofuncs;
    DynArray.clear gotofuncs
  );
  (* Search for token in keyword table *)
  try
    PMap.find (Text.norm tkn) !keywords
  with Not_found ->
    IDENT (Text.to_sjs tkn, Text.norm tkn)


let lcount = Array.fold_left (fun acc c -> if c = 10 then acc + 1 else acc) 0


let rec skip_comment start lstate =
  lexer
    | eof  -> error start "unterminated comment"
    | "-}" -> lstate
    | '\n' -> skip_comment start { lstate with line = lstate.line + 1 } lexbuf
    | '-'
    | [^ "-\n"]+ -> skip_comment start lstate lexbuf
    | _ -> ksprintf (error lstate) "found char 0x%02x" (lexeme_char lexbuf 0)


let rec skip_whitespace lstate =
  lexer
    | eof -> lstate
    | [" \t"]+ -> skip_whitespace lstate lexbuf
    | '\r'? '\n'  -> skip_whitespace { lstate with line = lstate.line + 1 } lexbuf
    | "//" -> (lexer [^ '\n']* ('\n' | eof) -> () | _ -> assert false) lexbuf;
              skip_whitespace { lstate with line = lstate.line + 1 } lexbuf
    | "{-" -> let nstate = skip_comment lstate lstate lexbuf in
              skip_whitespace nstate lexbuf
    | _ -> rollback lexbuf; lstate


let lex_resstr_key lstate lexbuf =
  let loc, key = StrLexer.get_resstr_key (StrLexer.aux lstate `ResStr) lexbuf in
  key, loc

let lex_string sterm _ =
  StrLexer.get_string_tokens (match sterm with "'" -> `Single | "\"" -> `Double | _ -> `ResStr)


let rec get_token lstate =
  let return t = t, lstate in
  lexer
    | eof -> return EOF

    (* Whitespace and comments *)
    | [" \t"]+ -> get_token lstate lexbuf
    | '\r'? '\n'  -> get_token { lstate with line = lstate.line + 1 } lexbuf
    | "//" -> (lexer [^ '\n']* ('\n' | eof) -> () | _ -> assert false) lexbuf;
              get_token { lstate with line = lstate.line + 1 } lexbuf
    | "{-" -> let nstate = skip_comment lstate lstate lexbuf in
              get_token nstate lexbuf

    (* Lexer directives *)
    | "#line" [" \t"]+ kpg_number
       -> rollback lexbuf;
          (lexer "#line" -> () | _ -> assert false) lexbuf;
          (match get_token lstate lexbuf with
            | INTEGER i, lstate' -> get_token { lstate' with line = Int32.to_int i - 1 } lexbuf
            | _ -> assert false)

    | "#res" [" \t\r\n"]* "<" [" \t\r\n"]*
       -> let lc = lcount (lexeme lexbuf) in
          let key, nlc = lex_resstr_key { lstate with line = lstate.line + lc } lexbuf in
          if key = Text.empty then error nlc "anonymous resource string references not permitted in #res references";
          DRES key, nlc

    (* Symbols *)
    | "(" -> return LPAR  | ")" -> return RPAR  | "[" -> return LSQU  | "]" -> return RSQU
    | "{" -> return LCUR  | "}" -> return RCUR
    | ":" -> return COLON | ";" -> return SEMI  | "," -> return COMMA | "." -> return POINT
    
    | "+=" -> return SADD | "-=" -> return SSUB | "*=" -> return SMUL | "/=" -> return SDIV
    | "%=" -> return SMOD | "&=" -> return SAND | "|=" -> return SOR  | "^=" -> return SXOR
    | "<<="-> return SSHL | ">>="-> return SSHR | "="  -> return SET  | "->" -> return ARROW
    | "+"  -> return ADD  | "-"  -> return SUB  | "*"  -> return MUL  | "/"  -> return DIV
    | "%"  -> return MOD  | "&"  -> return AND  | "|"  -> return OR   | "^"  -> return XOR
    | "<<" -> return SHL  | ">>" -> return SHR  | "==" -> return EQU  | "!=" -> return NEQ
    | "<=" -> return LTE  | ">=" -> return GTE  | "<"  -> return LTN  | ">"  -> return GTN
    | "&&" -> return LAND | "||" -> return LOR  | "!"  -> return NOT  | "~"  -> return TILDE

    (* Literals *)
    | "0x" hex (hex | '_')*
       -> warning lstate "Kepago uses $nnn rather than 0xnnn for hexadecimal constants";
          return (INTEGER (Int32.of_string (utf8_lexeme lexbuf)))

    | kpg_number
       -> let s = utf8_lexeme lexbuf in
          return (INTEGER (Int32.of_string (ml_num_of_kpg_num s)))

    | '\'' | '\"'
       -> let sterm = utf8_lexeme lexbuf in
          let s, nstate = lex_string sterm (Text.Buf.create 0) lstate lexbuf in
          STRING s, nstate

    (* Keywords and identifiers *)
    | "__file__" -> return (STRING (DynArray.of_list [`Text (lstate, `Sbcs, Text.of_err lstate.file)]))
    | "__line__" -> return (INTEGER (Int32.of_int lstate.line))
    | ident -> return (token_of_identifier (Text.of_arr (lexeme lexbuf)))

    (* Labels *)
    | '@' id_cont+
       -> let id = Text.of_arr (sub_lexeme lexbuf 1 (lexeme_length lexbuf - 1)) in
          return (LABEL (Text.to_sjs id, Text.norm id))

    | _ -> ksprintf (error lstate) "invalid character 0x%02x in source file" (lexeme_char lexbuf 0)


let call_parser ?(file = "no file") ?(line = 1) entrypoint lexfun lexbuf =
  let dummy = Lexing.from_function (fun _ n -> n) in
  dummy.Lexing.lex_start_p <- { dummy.Lexing.lex_start_p with Lexing.pos_fname = file; pos_lnum = line };
  let lex dummy =
    let pos = dummy.Lexing.lex_start_p in
    let rv, ns = lexfun { file = pos.Lexing.pos_fname; line = pos.Lexing.pos_lnum } lexbuf in
    dummy.Lexing.lex_start_p <- { pos with Lexing.pos_lnum = ns.line };
    rv
  in
  entrypoint lex dummy
(* Example: call_parser KeParser.statement get_token (lex_channel some_channel) ~file:"foo.ke" *)

let call_parser_on_text entrypoint loc s =
  call_parser ~file:loc.file ~line:loc.line
    entrypoint get_token (from_int_array (Text.to_arr s))

let call_parser_on_string entrypoint loc s =
  call_parser_on_text entrypoint loc (Text.of_sjs s)

let lex_channel ?(enc = !App.enc) ic =
  from_stream (Text.ustream enc ic)

