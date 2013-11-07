/*
   Rlc: GAMEEXE.INI parser
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
  open Ini
  open Printf
  let parse_error err_message =
    kprintf
      Optpp.sysError
      "parsing GAMEEXE.INI: %s at line %d"
      err_message
      (Parsing.symbol_start_pos()).Lexing.pos_lnum
%}

%token EOF
%token Cm Co Dot Eq Hash Hy Lp Rp
%token<bool> UN
%token<string> IDENT STRING DOTIDENT
%token<int32> INT
%token<int> DOTINT

%token<string> SHAKE
%token CDTRACK DSTRACK NAMAE

%start inifile
%type<unit> inifile

%%

inifile: definitions EOF {}

definitions:    Hash definition {}
  | definitions Hash definition {}

definition:
  | IDENT Eq parameters                        { set $1 $3 }
  | IDENT DOTINT Co INT Eq parameters          { for i = $2 to Int32.to_int $4 do set (sprintf "%s.%03d" $1 i) $6 done }
  | IDENT DOTINT Eq parameters                 { set (sprintf "%s.%03d" $1 $2) $4 }
  | IDENT DOTIDENT Eq parameters               { set (sprintf "%s.%s" $1 $2) $4 }
  | IDENT DOTINT DOTINT Eq parameters          { set (sprintf "%s.%03d.%03d" $1 $2 $3) $5 }
  | IDENT DOTINT DOTIDENT Eq parameters        { set (sprintf "%s.%03d.%s" $1 $2 $3) $5 }
  | IDENT DOTINT DOTINT DOTIDENT Eq parameters { set (sprintf "%s.%03d.%03d.%s" $1 $2 $3 $4) $6 }
  | IDENT DOTINT DOTIDENT DOTINT Eq parameters { set (sprintf "%s.%03d.%s.%03d" $1 $2 $3 $4) $6 }
  | IDENT DOTINT Dot range DOTIDENT Eq parameters
      { List.iter (fun i -> set (sprintf "%s.%03d.%03ld.%s" $1 $2 i $5) $7) $4 }
  | special {}

special:
  | SHAKE DOTINT Eq ranges
      { set (sprintf "%s.%03d" $1 $2) $4 }
  | DSTRACK Eq inthy inthy INT Eq STRING Eq STRING
      { (* ignore for now *) }
  | CDTRACK Eq INT Co INT Co INT Co inthy INT Co INT Co INT Co inthy INT Co INT Co INT Co INT Eq STRING
      { (* ignore for now *) }
  | NAMAE Eq STRING Eq STRING Eq Lp INT Cm INT Cm INT Rp
      { (* ignore for now *) }

inthy:
  | INT {}
  | INT Hy {}

ranges: /* empty */    { [] }
  | range ranges       { `Range $1 :: $2 }

range: Lp range_elt Rp { $2 }
range_elt:
  | INT                { $1 :: [] }
  | INT Cm range_elt   { $1 :: $3 }

parameters: /* empty */  { [] }
  | value                { $1 :: [] }
  | value Co parameters  { $1 :: $3 } /* treat colons like commas for now */
  | value Eq parameters  { $1 :: $3 } /* treat equals like commas for now */
  | value Cm parameters  { $1 :: $3 }

value:
  | INT                  { `Integer $1 }
  | STRING               { `String  $1 }
  | UN                   { `Enabled $1 }
