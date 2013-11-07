/*
   RLdev: function definition file parser
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
*/

%{

  open Parsing
  open KfnTypes
  open ExtList

  let modules = Hashtbl.create 0

  let process_fun_def versions (ident, ccode, fun_flags, t, m, op, overloads, prototypes) =
    let ident = !which_ident ident in
    if List.length prototypes <> overloads + 1 then (
      Printf.eprintf "Error: parsing reallive.kfn: incorrect overload count for %s\n" ident;
      exit 2
    );
    let ccname, ccflags = ccode in
    let ccstr =
      match ccname with
        | `Absent -> ""
        | `Unnamed -> ident
        | `Named s -> s
    in
    !handle_opcode versions ident ccstr (ccflags @ fun_flags) t m op prototypes

%}

%token EOF
%token MODULE FUN VER END                           /*  keywords   */
%token Lt Gt Eq Cm Lp Rp Lbr Rbr Qu St Pl Co Pt Ha /* punctuation */
%token INT INTC INTV STR STRC STRV RES SPECIAL    /*     types   */
%token<int> INTEGER
%token<string> IDENT STRING

%start parse
%type<unit> parse

%%

parse:
  | error
    { Printf.eprintf "Error: parsing reallive.kfn: syntax error at line %d\n" (symbol_start_pos ()).Lexing.pos_lnum;
      exit 2 }
  | EOF {}
  | module_def parse {}
  | one_fun_def parse {}
  | fun_defs parse {}

module_def:
  | MODULE INTEGER Eq IDENT
     { Hashtbl.add modules $4 $2;
       !handle_module $2 $4 }
  | MODULE INTEGER
     {}

fun_def:
  FUN ident ccode fun_flags Lt INTEGER Co module_id Co INTEGER Cm INTEGER Gt prototypes
    { $2, $3, $4, $6, $8, $10, $12, $14 }

one_fun_def: 
  fun_def 
    { process_fun_def [] $1 }

fun_defs:
  VER versions fun_list END
    { List.iter (process_fun_def $2) $3 }
    
fun_list: /*empty*/  { [] }
  | fun_def fun_list { $1 :: $2 }

versions:
  | version             { $1 :: [] }
  | version Cm versions { $1 :: $3 }

version:
  | IDENT             { Class (String.lowercase $1) }
  | Lt vstamp         { Compare (fun x -> x < $2) }
  | Gt vstamp         { Compare (fun x -> x > $2) }
  | Lt Eq vstamp      { Compare (fun x -> x <= $3) }
  | Gt Eq vstamp      { Compare (fun x -> x >= $3) }

vstamp:
  | INTEGER                                  { $1,  0,  0,  0 }
  | INTEGER Pt INTEGER                       { $1, $3,  0,  0 }
  | INTEGER Pt INTEGER Pt INTEGER            { $1, $3, $5,  0 }
  | INTEGER Pt INTEGER Pt INTEGER Pt INTEGER { $1, $3, $5, $7 }

module_id:
  | INTEGER           { $1 }
  | IDENT             { try
                          Hashtbl.find modules $1
                        with
                          Not_found ->
                            Printf.eprintf "Error: parsing reallive.kfn: undeclared module %s\n" $1;
                            exit 2 }

ccode:
  | /* empty */         { `Absent, [] }
  | Lbr Rbr             { `Unnamed, [] }
  | Lbr IDENT Rbr       { `Named $2, [] }
  | Lbr St IDENT Rbr    { `Named $3, [IsTextout] }
  | Lbr Eq IDENT Rbr    { `Named $3, [NoBraces] }
  | Lbr St Eq IDENT Rbr { `Named $4, [NoBraces; IsLbr] }

ident:
  | /* empty */ { "", "" }
  | END         { "end", "end" } /* Silly keyword-related problems */
  | IDENT       { $1, $1 }
  | IDENT IDENT { $1, $2 }

fun_flags:
  | /* empty */ { [] }
  | Lp flags Rp { $2 }

flags: /* empty */ { [] }
  | IDENT flags
    { let flag =
        match String.lowercase $1 with
          | "store" -> PushStore
          | "jump"  -> IsJump
          | "goto"  -> IsGoto
          | "if"    -> IsCond
          | s -> Printf.kprintf failwith "unknown flag %s in reallive.kfn" s
      in
      flag :: $2 }
  

prototypes: /* empty */  { [] }
  | prototype prototypes { $1 :: $2 }

prototype:
  | Qu            { None }    /* = undefined */
  | Lp parameters { Some $2 } /* = defined, but may be empty */

parameters:
  | Rp                      { [] }
  | parameter Rp            { $1 :: [] }
  | parameter Cm parameters { $1 :: $3 }

parameter:
  | preparm STRING  postparm { IntC, Tagged $2 :: $1 @ $3 }
  | preparm typedef postparm { $2, $1 @ $3 }

preparm:
  | /* empty */     { [] }
  | Ha preparm      { TextObject :: $2 }
  | Qu preparm      { Optional :: $2 }
  | Lt preparm      { Uncount :: $2 }
  | Gt preparm      { Return :: $2 }
  | Eq preparm      { Fake :: $2 }

postparm:
  | /* empty */     { [] }
  | Pl postparm     { Argc :: $2 }
  | STRING postparm { Tagged $1 :: $2 }

typedef:
  | INT  { Int  }
  | INTC { IntC }
  | INTV { IntV }
  | STR  { Str  }
  | STRC { StrC }
  | STRV { StrV }
  | RES  { ResStr }
  | SPECIAL Lp special Rp { Special $3 }
  | Lp complex Rp         { Complex $2 }

complex:
  | compdef            { $1 :: [] }
  | compdef Cm complex { $1 :: $3 }

compdef:
  | typedef            { $1, [] }
  | typedef STRING     { $1, [Tagged $2] }
  | STRING             { IntC, [Tagged $1] }

special:
  | specdef            { $1 :: [] }
  | specdef Cm special { $1 :: $3 }

specdef:
  | INTEGER Co specflags Lbr complex Rbr { $1, AsComplex $5, $3 }
  | INTEGER Co specflags IDENT Lp complex Rp { $1, Named ($4, $6), $3 }

specflags:
  | /* empty */        { [] }
  | Ha specflags       { NoParens :: $2 }
