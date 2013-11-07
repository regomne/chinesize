/*
   Rlc: Kepago parser
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

  There should be 5 shift/reduce conflicts in this grammar: on ELSE in IF 
  statements, and on COMMA in identlist, setlist, plainidts, and vardecls.
  These have been confirmed to produce the desired parse; to resolve them 
  properly would be tedious and of little practical use.  (Any other conflicts 
  should, however, be treated as bugs.)
*/

%{

open Printf
open KeTypes
open KeAst
open ExtList

let _loc () =
  let pos = symbol_start_pos () in
  { file = pos.Lexing.pos_fname;
    line = pos.Lexing.pos_lnum }

let _rhs n =
  let pos = rhs_start_pos n in
  { file = pos.Lexing.pos_fname;
    line = pos.Lexing.pos_lnum }

%}

%token EOF
%token LPAR RPAR /* () */
%token LSQU RSQU /* [] */
%token LCUR RCUR /* {} */
%token SADD SSUB SMUL SDIV SMOD SAND SOR SXOR SSHL SSHR SET
%token ADD SUB MUL DIV MOD AND OR XOR SHL SHR EQU NEQ LTE LTN GTE GTN LAND LOR NOT TILDE
%token COLON COMMA SEMI POINT ARROW
%token<int32> INTEGER
%token<Text.t> DRES
%token<KeAst.strtokens> STRING
%token<string * Text.t> LABEL IDENT GOTO
%token<int> VAR SVAR REG
%token<int> INT
%token STR
%token<string * [`Int | `Str | `None]> DWITHEXPR
%token DEOF DHALT DTARGET DVERSION DLOAD
%token DIF DELSE DELSEIF DENDIF DFOR
%token<bool> DIFDEF DINLINE
%token<[`Define | `DefineScoped | `Redefine | `Const | `Bind | `EBind]> DDEFINE
%token DSET DUNDEF DHIDING
%token OP USCORE RETURN IF ELSE WHILE REPEAT TILL FOR CASE OF OTHER ECASE BREAK CONTINUE RAW ENDRAW
%token<[`Goto | `Gosub]> GOTOCASE GOTOON
%token<string * int> SELECT
%token<[ `S | `Pause ]> SPECIAL

%right SADD SSUB SMUL SDIV SMOD SAND SOR SXOR SSHL SSHR SET
%left LOR
%left LAND
%left EQU NEQ LTE LTN GTE GTN
%left AND OR XOR
%left ADD SUB
%left MUL DIV MOD
%left SHL SHR

%start program
%type<KeAst.statement> program

%start statement
%type<KeAst.statement> statement

%start just_statements
%type<KeAst.statement DynArray.t> just_statements

%start just_expression
%type<KeAst.expression> just_expression
%start just_param_list
%type<KeAst.parameter list> just_param_list

%%

just_expression: error { error (_loc()) "expected expression" }
  | expression EOF { $1 }
just_param_list: error { error (_loc()) "expected expression" }
  | param_list EOF { $1 }

program:
  statements end_of_file { `Seq $1 }

just_statements:
  statements end_of_file { $1 }

end_of_file:
  | EOF  {}
  | DEOF {}

statements: /* empty */   { DynArray.create () }
  | statements cstatement { DynArray.add $1 $2; $1 }

label: LABEL { let str, txt = $1 in _loc(), str, txt }

labels: label          { $1 :: [] }
  | label COMMA labels { $1 :: $3 }

cstatement:
  | statement       { $1 }
  | COMMA statement { $2 }

statement:
  | error        { error (_loc()) "expected statement" }
  | DHALT        { `Halt (_loc()) }
  | BREAK        { `Break (_loc()) }
  | CONTINUE     { `Continue (_loc()) }
  | label        { `Label $1 }
  | declaration  { $1 }
  | directive    { $1 }
  | unknown_op   { $1 }
  | gotofunction { $1 }
  | structure    { $1 }
  | assignment   { $1 :> statement }
  | statement_expr { match $1 with
                       | `VarOrFn v -> `VarOrFn v
                       | `Func (l, s, t, p, d) -> `FuncCall (l, None, s, t, p, d)
                       | `SelFunc (l, s, i, w, p) -> `Select (l, `Store l, s, i, w, p)
                       | _ -> `Return (_loc(), false, $1) }
  | RETURN expression  { `Return (_loc(), true, $2) }
  | RAW rawelts ENDRAW { `RawCode (_loc(), $2) }

rawelts: /* empty */     { [] }
  | rawelt rawelts { $1 :: $2 }

rawelt:
  | IDENT { `Ident $1  } /* #hex = insert hex bytes; ?text = insert to_sjs(text) directly */
  | raw_v { `Bytes (sprintf "$%c" (char_of_int $1)) }
  | COMMA { `Bytes "," }
  | LPAR  { `Bytes "(" } | RPAR  { `Bytes ")" }
  | LCUR  { `Bytes "{" } | RCUR  { `Bytes "}" }
  | LSQU  { `Bytes "[" } | RSQU  { `Bytes "]" }
  | INTEGER { `Int $1 }

raw_v: REG { $1 } | VAR { $1 } | SVAR { $1 }

structure:
  | COLON statements SEMI
      { `Block (_loc(), $2) }
  | IF expression cstatement
      { `If (_loc(), $2, $3, None) }
  | IF expression cstatement ELSE cstatement
      { `If (_loc(), $2, $3, Some $5) }
  | WHILE expression cstatement
      { `While (_loc(), $2, $3) }
  | REPEAT statements TILL expression
      { `Repeat (_loc(), $2, $4) }
  | FOR LPAR statements forsep expression forsep statements RPAR cstatement
      { `For (_loc(), $3, $5, $7, $9) }
  | CASE expression ofcases othercase
      { `Case (_loc(), $2, $3, $4) }
  | DIF expression statements condelseorend
      { `DIf (_loc(), $2, $3, $4) }
  | DIFDEF expression statements condelseorend
      { let cond =
          map_expr_leaves
            (function
              | `VarOrFn (l, _, _) as leaf
                  -> let f = `Func (l, "defined?", Text.ident "defined?", [`Simple (l, leaf)], None) in
                     if $1 then f else `Unary (nowhere, `Not, f)
              | leaf -> (leaf :> expression))
            $2
        in `DIf (_loc(), cond, $3, $4) }
  | DFOR IDENT SET expression POINT POINT expression cstatement
      { let str, txt = $2 in `DFor (_loc(), str, txt, $4, $7, $8) }

condelseorend:
  | DENDIF
      { `DEndif (_loc()) }
  | DELSE statements DENDIF
      { `DElse (_loc(), $2) }
  | DELSEIF expression statements condelseorend
      { `DIf (_loc(), $2, $3, $4) }

forsep: SEMI {} | RPAR LPAR {}

ofcases:
  | /* empty */ { [] }
  | COMMA ofcases { $2 } 
  | OF expression statements ofcases { ($2, $3) :: $4 }

othercase:
  | ECASE { None }
  | OTHER statements ECASE { Some $2 }


unknown_op:
  OP LTN INTEGER sep module_id sep INTEGER sep INTEGER GTN maybe_param_list
    { let op_type = Int32.to_int $3
      and op_code = Int32.to_int $7
      and op_over = Int32.to_int $9
      and op_module, module_id =
        match $5 with
          | `Integer i -> Int32.to_int i, sprintf "%03ld" i
          | `Ident (s, t)
           -> try
                Hashtbl.find modules t, s
              with Not_found ->
                kprintf (error (_rhs 5)) "undefined module `%s'" s
      in
      let id = ident_of_opcode op_type op_module op_code op_over in
      let it = Text.ident id in
      if Hashtbl.mem functions it then
        `FuncCall (_loc(), None, id, it, $11, None)
      else
        let def =
          { ident = sprintf "op<%d:%s:%05d, %d>" op_type module_id op_code op_over;
            flags = [];
            op_type = op_type;
            op_module = op_module;
            op_code = op_code;
            prototypes = [||];
            targets = []; }
        in `UnknownOp (_loc(), def, op_over, $11) }

module_id:
  | INTEGER { `Integer $1 }
  | IDENT   { `Ident $1 }
  | STR     { `Ident ("Str", Text.of_arr [| 0x73; 0x74; 0x72 |]) }

sep: COLON {} | COMMA {} | POINT {} | SEMI {} | SUB {} | DIV {}

gotofunction:
  | GOTOON expression LCUR labels RCUR  { `GotoOn (_loc(), $1, $2, $4) }
  | GOTOCASE expression LCUR cases RCUR { `GotoCase (_loc(), $1, $2, $4) }

cases:
  | case                 { $1 :: [] }
  | case SEMI            { $1 :: [] }
  | case SEMI cases      { $1 :: $3 }
case:
  | USCORE COLON label     { `Default $3 }
  | expression COLON label { `Match ($1, $3) }


functioncall:
  | IDENT LPAR param_list RPAR
      { let str, txt = $1 in `Func (_loc(), str, txt, $3, None) }
  | GOTO label
      { let str, txt = $1 in `Func (_loc(), str, txt, [], Some $2) }
  | GOTO LPAR param_list RPAR label
      { let str, txt = $1 in `Func (_loc(), str, txt, $3, Some $5) }
  | SELECT maybe_sel_param_list
      { let str, opcode = $1 in `SelFunc (_loc(), str, opcode, None, $2) }
  | SELECT LSQU expression RSQU maybe_sel_param_list
      { let str, opcode = $1 in `SelFunc (_loc(), str, opcode, Some $3, $5) }

maybe_param_list:            { [] }
  | LPAR param_list RPAR     { $2 }

param_list:                  { [] }
  | fn_params                { $1 }

fn_params:
  | fn_param                 { $1 :: [] }
  | fn_param COMMA fn_params { $1 :: $3 }

fn_param:
  | expression
      { `Simple (_loc(), $1) }
  | LCUR param_list RCUR
      { `Complex (_loc(), List.map (function `Simple (_, e) -> e | _ -> raise Parse_error) $2) }

maybe_sel_param_list:        { [] }
  | LPAR sel_param_list RPAR { $2 }

sel_param_list:              { [] }
  | sel_params               { $1 }

sel_params:
  | sel_param                  { $1 :: [] }
  | sel_param COMMA sel_params { $1 :: $3 }

sel_param:
  | expression
      { `Always (_loc(), $1) }
  | sel_conds COLON expression
      { `Special (_loc(), List.rev $1, $3) }

sel_conds:
  | sel_cond                { $1 :: [] }
  | sel_conds SEMI sel_cond { $3 :: $1 }

sel_cond:
  | expression
      { match $1 with
          | `Func (_, str, text, [`Simple (_, expr)], None) -> `NonCond (_loc(), str, text, expr)
          | `VarOrFn (_, str, text) -> `Flag (_loc(), str, text)
          | _ -> raise Parse_error }
  | expression IF expression
      { match $1 with
          | `Func (_, str, text, [`Simple (_, expr)], None) -> `Cond (_loc(), str, text, Some expr, $3)
          | `VarOrFn (_, str, text) -> `Cond (_loc(), str, text, None, $3)
          | _ -> raise Parse_error }

assignment:
  | lhs assignop expression
    { match $1 with
        | #assignable as a -> `Assign (_loc(), a, $2, $3)
        | _ -> error (_loc()) "left-hand-side of assignment must be a variable" }
   
lhs:
  | variable { $1 }
  /* permit others? */

assignop:
  | SADD { `Add } | SSUB { `Sub } | SMUL { `Mul } | SDIV { `Div }
  | SMOD { `Mod } | SAND { `And } | SOR  { `Or  } | SXOR { `Xor }
  | SSHL { `Shl } | SSHR { `Shr } | SET  { `Set }

variable:
  | REG                        { match $1 with 0xc8 -> `Store (_loc()) | _ -> assert false }
  | VAR LSQU expression RSQU   { `IVar (_loc(), $1, $3) }
  | SVAR LSQU expression RSQU  { `SVar (_loc(), $1, $3) }
  | IDENT                      { let str, txt = $1 in `VarOrFn (_loc(), str, txt) }
  | IDENT LSQU expression RSQU { let str, txt = $1 in `Deref (_loc(), str, txt, $3) }

expression: expr1    { $1 }
 expr1: expr2        { $1 }
  | expr1 LOR expr2  { `AndOr (_loc(), $1, `LOr, $3) }
 expr2: expr3        { $1 }
  | expr2 LAND expr3 { `AndOr (_loc(), $1, `LAnd, $3) }
 expr3: expr4        { $1 }
  | expr3 EQU expr4  { `LogOp (_loc(), $1, `Equ, $3) }
  | expr3 NEQ expr4  { `LogOp (_loc(), $1, `Neq, $3) }
 expr4: expr5        { $1 }
  | expr4 LTE expr5  { `LogOp (_loc(), $1, `Lte, $3) }
  | expr4 LTN expr5  { `LogOp (_loc(), $1, `Ltn, $3) }
  | expr4 GTE expr5  { `LogOp (_loc(), $1, `Gte, $3) }
  | expr4 GTN expr5  { `LogOp (_loc(), $1, `Gtn, $3) }
 expr5: expr6        { $1 }
  | expr5 ADD expr6  { `Op (_loc(), $1, `Add, $3) }
  | expr5 SUB expr6  { `Op (_loc(), $1, `Sub, $3) }
  | expr5 OR  expr6  { `Op (_loc(), $1, `Or, $3) }
  | expr5 XOR expr6  { `Op (_loc(), $1, `Xor, $3) } 
 expr6: expr7        { $1 }
  | expr6 MUL expr7  { `Op (_loc(), $1, `Mul, $3) }
  | expr6 DIV expr7  { `Op (_loc(), $1, `Div, $3) }
  | expr6 MOD expr7  { `Op (_loc(), $1, `Mod, $3) }
  | expr6 AND expr7  { `Op (_loc(), $1, `And, $3) }
 expr7: expr8        { $1 }
  | expr7 SHL expr8  { `Op (_loc(), $1, `Shl, $3) }
  | expr7 SHR expr8  { `Op (_loc(), $1, `Shr, $3) }
 expr8:   expr9      { $1 }
  | SUB   expr8      { `Unary (_loc(), `Sub, $2) }
  | NOT   expr8      { `Unary (_loc(), `Not, $2) }
  | TILDE expr8      { `Unary (_loc(), `Inv, $2) }
 expr9:
  | INTEGER              { `Int (_loc(), $1) }
  | string               { $1 }
  | variable             { $1 :> expression }
  | functioncall         { $1 }
  | LPAR expression RPAR { $2 }

  /* For debugging use: */
/*| LCUR exprseqdef statements RCUR
      { if DynArray.empty $3
        then error (_loc ()) "the empty sequence {> } is not permitted in expressions"
        else `ExprSeq (_loc(), Text.of_sjs "[anonymous sequence]", $2, $3) } 

  exprseqdef: GTN { [] }
    | LTN exprseqdefs COLON GTN { $2 }
  exprseqdefs: { [] }
    | IDENT SET expression exprseqdefs { let _, txt = $1 in (txt, $3) :: $4 }
    | IDENT SET expression COMMA exprseqdefs { let _, txt = $1 in (txt, $3) :: $5 } */

string:
  | STRING         { `Str (_loc(), $1) }
  | DRES           { `Res (_loc(), $1) }

statement_expr:
  | statement_expr ADD expr6 { `Op (_loc(), $1, `Add, $3) }
  | INTEGER                  { `Int (_loc(), $1) }
  | string                   { $1 }
  | variable                 { $1 :> expression }
  | functioncall             { $1 }

directive:
  | DDEFINE identlist
      { let f (l, s, t, eo) =
          let eo = match eo with 
            | `None -> None | `Some x -> Some x 
            | `Optional -> error l "expected identifier, found `['" 
          in
          match $1 with
            | `Define | `DefineScoped
               -> `Define (l, s, t, $1 = `DefineScoped, Option.default (`Int (nowhere, 1l)) eo)
            | (`Const | `Bind | `EBind) as tp
               -> `DConst (l, s, t, tp, try Option.get eo with Option.No_value -> error l "#const and #bind definitions must be assigned values")
            | `Redefine
               -> `DSet (l, s, t, false, try Option.get eo with Option.No_value -> error l "you cannot mutate a symbol without supplying a value")
        in
        `Seq (DynArray.of_list (List.rev_map f $2)) }
  | DSET setlist
      { let f =
          function (l, _, _, _, None) -> error l "you cannot mutate a symbol without supplying a value"
            | (l, s, t, o, Some e) ->
              let e' =
                match o with
                  | #arith_op as ao -> `Op (l, `VarOrFn (l, s, t), ao, e)
                  | `Set -> e
              in
              `DSet (l, s, t, true, e')
        in
        `Seq (DynArray.of_list (List.rev_map f $2)) }
  | DUNDEF plainidts
      { `DUndef (_loc(), $2) }
  | DWITHEXPR expression
      { let id, t = $1 in `Directive (_loc(), id, t, $2) }
  | DLOAD expression
      { `LoadFile (_loc(), $2) }
  | DTARGET IDENT
      { let _, txt = $2 in `DTarget (_loc(), Text.to_sjs txt) }
  | DINLINE IDENT LPAR identifiers RPAR statement
      { let str, txt = $2 in `DInline (_loc(), str, txt, $1, List.rev $4, $6) }
  | DHIDING IDENT cstatement
      { let _, txt = $2 in `Hiding (_loc(), txt, $3) }
  | DVERSION dversion
      { let a, b, c, d = $2 in `DVersion (_loc(), a, b, c, d) }

dversion:
  | expression  { $1, `Int (_loc(), 0l), `Int (_loc(), 0l), `Int (_loc(), 0l) }
  | expression POINT expression { $1, $3, `Int (_loc(), 0l), `Int (_loc(), 0l) }
  | expression POINT expression POINT expression { $1, $3, $5, `Int (_loc(), 0l) }
  | expression POINT expression POINT expression POINT expression { $1, $3, $5, $7 }

identifiers:   { [] }
  | identlist  { $1 }
identlist: idelt          { $1 :: [] }
  | identlist COMMA idelt { $3 :: $1 }
idelt: IDENT              { let str, txt = $1 in _loc(), str, txt, `None }
  | IDENT SET expression  { let str, txt = $1 in _loc(), str, txt, `Some $3 }
  | LSQU IDENT RSQU       { let str, txt = $2 in _loc(), str, txt, `Optional }

setlist: setelt           { $1 :: [] }
  | setlist COMMA setelt  { $3 :: $1 }
setelt: IDENT                 { let str, txt = $1 in _loc(), str, txt, `Set, None }
  | IDENT assignop expression { let str, txt = $1 in _loc(), str, txt, $2, Some $3 }

plainidts: IDENT          { let str, txt = $1 in (_loc(), str, txt) :: [] }
  | IDENT COMMA plainidts { let str, txt = $1 in (_loc(), str, txt) :: $3 }


declaration: vartype decldirs vardecls 
    { `Decl (_loc(), $1, $2, List.rev_map (fun (l, (s, t), al, (v, a)) -> l, s, t, al, v, a) $3) }
        /*`Seq 
          (DynArray.of_list 
            (List.rev_map 
              (fun (l, (s, t), al, (v, a)) -> `Decl (l, $1, $2, s, t, al, v, a)) 
             $3)) } */

vartype:
  | INT { `Int $1 }
  | STR { `Str }

vardecls: vardecl          { $1 :: [] }
  | vardecls COMMA vardecl { $3 :: $1 }

vardecl:
  | IDENT arraydecl initdecl { _loc(), $1, $2, $3 }
  
arraydecl: /*empty*/     { `None }
  | LSQU RSQU            { `Auto }
  | LSQU expression RSQU { `Some $2 }
  
initdecl: /*empty*/      { `None, None }
  | valdecl              { $1, None }
  | addrdecl             { `None, Some $1 }
  | valdecl addrdecl     { $1, Some $2 }
  | addrdecl valdecl     { $2, Some $1 }
  
addrdecl: ARROW expression POINT expression { $2, $4 }
valdecl: SET fn_param 
    { match $2 with 
        | `Simple (_, e) -> `Scalar e
        | `Complex (_, es) -> `Array es
        | `Special _ -> raise Parse_error }

decldirs: /*empty*/        { [] }
  | LPAR decldirids RPAR   { $2 }
  
decldirids: /*empty*/      { [] }
  | IDENT decldirids       { Variables.decldir_of_ident (_loc()) $1 :: $2 }
  | IDENT COMMA decldirids { Variables.decldir_of_ident (_loc()) $1 :: $3 }
