(*
   RLdev: function definition documentation mangler
   Copyright (C) 2006 Haeleth

   This is not meant to be a friendly or pretty program, but it does the job. 
   Such documentation as there is is in the comments at the start of each 
   .fundoc file.

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

{ (* ML *)

open Printf

module String =
  struct
    include String
    let of_char = make 1
    let replace_chars f s =
      let len = length s and tlen = ref 0 in
      let rec loop i acc =
        if i == len then acc else 
          let s = f (unsafe_get s i) in
          tlen := !tlen + length s;
          loop (i + 1) (s :: acc)
      in
      let strs = loop 0 [] in
      let sbuf = create !tlen in
      let pos = ref !tlen in
      let rec loop2 = function [] -> () | s :: acc ->
         let len = length s in
         pos := !pos - len;
         blit s 0 sbuf !pos len;
         loop2 acc
      in
      loop2 strs;
      sbuf
    let map f s =
      let len = length s in
      let sc = create len in
      for i = 0 to len - 1 do unsafe_set sc i (f (unsafe_get s i)) done;
      sc
  end

let esc'_' = String.replace_chars (function '_' -> "\\_" | c -> String.of_char c)
and dot'_' = String.map (function '_' -> ':' | c -> c)

let define dt ids =
  printf "\\prespace";
  List.iter
    (fun s -> 
      printf "\\maketarget[%s]{%s}{%s}\\%sidx{%s}" 
        (dot'_' s)
        (if dt = "fun" then "func" else dt)
        (esc'_' s)
        (if dt = "fun" then "fn" else dt)
        (esc'_' s))
    ids;
  printf "%%"
          
} (* LEX *)

let  sp = [' ' '\t' '\r' '\n']*

rule lex =
  parse
    | eof { raise End_of_file }
    | "%@" ([^ '\n']* as comment) '\n' { if comment <> "ignore" then printf "%%%s\n" comment }
    | "@{" { printf "\\begin{cmdesc}" }
    | "@}" ("\n\n"? as dlb) { printf "\\end{cmdesc}"; if dlb <> "" then printf "\n" }

    | "\\<" { printf "<" }
    | "@@" { printf "@" }

    | "<" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as var) ">"
      { printf "\\metavar{%s}" (esc'_' var) }

    | "@" (("fun" | "ivar") as dt) ([^ 'A'-'Z' 'a'-'z' '_' ' ' '\t'] as c)
      { define dt (lex_fun c [] (Buffer.create 0) lexbuf) }

    | "@ivars/" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as r) "/" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as s) "/"
      { let q = r ^ "." ^ s in 
        printf "\\maketarget[%s]{ivar}{%s}" (dot'_' q) (esc'_' q);
        printf "\\ivarsidx{%s}{%s}" (esc'_' r) (esc'_' s) }

    | "@" ('i'? as isi) "ref" ("np"? as np) "{" ([^ '}']+ as s) "}"
      { let rn = if isi = "" then if np = "np" then "fnrefnoparens" else "fnref" else "ivarref" in
        printf "\\%s[%s]{%s}" rn (dot'_' s) (esc'_' s) }

    | "@irefs{" ([^ '}']+ as r) "}" sp "{" ([^ '}']+ as s) "}"
      { let rs = r ^ "." ^ s in printf "\\ivarref[%s]{%s}" (dot'_' rs) (esc'_' rs) }

    | "@def" ([^ 'A'-'Z' 'a'-'z' '_' ' ' '\t'] as c)
      { let h, l, br = lex_def c (Buffer.create 0) (Buffer.create 0) false lexbuf in
        if h ^ "|" = l then
          printf "\\lstinline|%s|" h
        else
          printf "\\ifhevea\\lstinline|%s|\\else\\lstinline|%s\\fi" h l;
        if br then print_string "%\n" }

    | "@and" ([^ 'A'-'Z' 'a'-'z' '_' ' ' '\t' '@' '\r' '\n'] as c)
      { let h, l, br = lex_def c (Buffer.create 0) (Buffer.create 0) false lexbuf in
        if h ^ "|" = l then
          printf "\\\\\\noindent\\lstinline|%s|" h
        else
          printf "\\\\\\ifhevea\\lstinline|%s|\\else\\noindent\\lstinline|%s\\fi" h l;
        if br then print_string "%\n" }

    | "@ivar1" ('s'? as s) "/" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as v) "/" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as f) "/"
      { define "ivar" [v];
        print_string "\n";
        let q = if s = "" then "" else "\"" in
        lex (kprintf Lexing.from_string "@def/#%s = %s<%s>%s/" v q f q) }

    | "@" (['a' 'i'] as c) "varsd/" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as r) "/" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as s) "/" ([^ '/']+ as d) "/"
      { let q = r ^ "." ^ s in 
        printf "\\maketarget[%s]{ivar}{%s}" (dot'_' q) (esc'_' q);
        printf "\\ivarsidx{%s}{%s}" (esc'_' r) (esc'_' s);
        print_string "\n";
        lex (kprintf Lexing.from_string "@%s/#%s.<index>.%s = %s/" (if c = 'a' then "and" else "def") r s d) }

    | _ as c { print_char c }

and lex_fun c acc b =
  parse
    | ","
      { let s = Buffer.contents b in
        Buffer.clear b;
        lex_fun c (s :: acc) b lexbuf }
    | _ as lc
      { if lc = c then
          Buffer.contents b :: acc
        else
          (Buffer.add_char b lc; lex_fun c acc b lexbuf) }

and lex_def c hb lb endescape =
  parse
    | ' '+ as sp
      { if not endescape then (
          Buffer.add_string hb "^";
          Buffer.add_string lb "|"
        );
        bprintf hb "%s" (String.make (String.length sp) '~');
        Buffer.add_string lb sp;
        lex_def c hb lb true lexbuf }
    | "<" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as var) ">"
      { if not endescape then (
          Buffer.add_string hb "^";
          Buffer.add_string lb "|"
        );
        let amv = esc'_' var in
        bprintf hb "\\metavar{%s}" amv;
        bprintf lb "\\metavar{%s}" amv;
        lex_def c hb lb true lexbuf }
    | "[" (['A'-'Z' 'a'-'z' '0'-'9' '_']+ as var) "]"
      { if not endescape then (
          Buffer.add_string hb "^";
          Buffer.add_string lb "|"
        );
        let amv = esc'_' var in
        bprintf hb "\\metaopt{%s}" amv;
        bprintf lb "\\metaopt{%s}" amv;
        lex_def c hb lb true lexbuf }
    | "@" (['[' ']'] as ch)
      { if not endescape then (
          Buffer.add_string hb "^";
          Buffer.add_string lb "|"
        );
        bprintf hb "\\textit{%c}" ch;
        bprintf lb "\\textit{%c}" ch;
        lex_def c hb lb true lexbuf }
    | "~{"
      { if not endescape then (
          Buffer.add_string hb "^";
          Buffer.add_string lb "|"
        );
        lex_def c hb lb false lexbuf }
    | "~}"
      { lex_def c hb lb true lexbuf }
    | (_ as lc) ('\n'? as br)
      { if lc = c then (
          if endescape then Buffer.add_char hb '^' else Buffer.add_char lb '|';
          Buffer.contents hb, Buffer.contents lb, br = "\n"
        )
        else (
          if endescape then (
            Buffer.add_char hb '^';
            Buffer.add_string lb "\\lstinline|"
          );
          Buffer.add_char hb lc;
          Buffer.add_char lb lc;
          lex_def c hb lb false lexbuf
        ) }

{ (* ML *)

let () =
  printf "%%\n%% AUTOMATICALLY GENERATED FILE.\n%% Edit the .fundoc equivalent instead, not this!\n%%\n";
  let lb = Lexing.from_channel stdin in
  try
    while true do lex lb done
  with
    End_of_file -> ()

}