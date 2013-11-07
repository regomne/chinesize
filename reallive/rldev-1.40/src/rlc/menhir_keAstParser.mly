/*
   Rlc: Kepago parser
   Copyright (C) 2006 Haeleth

   This parser is for use with the Menhir LR(1) parser generator, _not_ with
   ocamlyacc.

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

open Printf
open KeTypes
open KeAst
open ExtList

let _loc pos =
  { file = pos.Lexing.pos_fname;
    line = pos.Lexing.pos_lnum; }

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

//// %right SADD SSUB SMUL SDIV SMOD SAND SOR SXOR SSHL SSHR SET
%left LOR
%left LAND
%left EQU NEQ LTE LTN GTE GTN
%left AND OR XOR
%left ADD SUB
%left MUL DIV MOD
%left SHL SHR
%right UNARY

%type<KeAst.statement> program
%start program

%type<KeAst.statement DynArray.t> just_statements
%start just_statements

%type<KeAst.expression> just_expression
%start just_expression

%type<KeAst.parameter list> just_param_list
%start just_param_list

%%

%inline parenth(X): LPAR; x = X; RPAR { x }
%inline bracket(X): LSQU; x = X; RSQU { x }
%inline curlybr(X): LCUR; x = X; RCUR { x }
%inline clist(X): l = separated_list(COMMA, X) { l }

(* Start points *)

program:
  s = statements; end_of_file { `Seq s }

just_statements:
  s = statements; end_of_file { s }

end_of_file: EOF | DEOF {}

statements: /* empty */             { DynArray.create () }
  | seq = statements; s = statement { DynArray.add seq s; seq }

just_expression: 
  | error         { error (_loc $startpos) "expected expression" }
  | e = expr; EOF { e }

just_param_list:
  | error                    { error (_loc $startpos) "expected expression" }
  | p = clist(fn_param); EOF { p }


(* Statements *)

label: id = LABEL { let str, txt = id in _loc $startpos, str, txt }

statement:
  | error { error (_loc $startpos) "expected statement" }
  | s = statement_open
  | s = statement_closed { s }

statement_open:
  | IF cond = expr COMMA? 
      whentrue = statement
      { 
        `If (_loc $startpos, cond, whentrue, None) 
      }
  | s = open_structure(statement_open) { s }
  
statement_closed:
  | s = other_statement
  | s = open_structure(statement_closed) { s }

other_statement:
  | s = statement_comma COMMA? 
  | s = definesetundef
  | s = declaration { s }

statement_comma:
  | l = label       { `Label l }
  | DHALT           { `Halt (_loc $startpos) }
  | BREAK           { `Break (_loc $startpos) }
  | CONTINUE        { `Continue (_loc $startpos) }
  | RETURN e = expr { `Return (_loc $startpos, true, e) }
  | s = raw_code
  | s = unknown_op
  | s = gotofunction
  | s = directive
  | s = structure
  | s = text_or_func { s }
  | s = assignment   { s :> statement }

open_structure(S):
  | IF cond = expr COMMA? 
      whentrue = statement_closed
    ELSE
      whenfalse = S
      {
        `If (_loc $startpos, cond, whentrue, Some whenfalse)
      }
  | WHILE cond = expr COMMA?
      body = S
      {
        `While (_loc $startpos, cond, body)
      }
  | FOR 
    LPAR    init = statements
    forsep  cond = expr
    forsep  incr = statements
    RPAR COMMA?
      body = S
      {
        `For (_loc $startpos, init, cond, incr, body)
      }
  | DFOR  id   = IDENT
    SET   init = expr; POINT POINT; fini = expr
      body = S
      { 
        let str, txt = id in 
        `DFor (_loc $startpos, str, txt, init, fini, body)
      }
  | itype = DINLINE 
    ident = IDENT 
            LPAR 
    parms =   clist(inline_elt)
            RPAR 
    body  =   S
      { 
        let id_str, id_text = ident in 
        `DInline (_loc $startpos, id_str, id_text, itype, parms, body)
      }
  | DHIDING ident = IDENT COMMA?
      body = S
      { 
        `Hiding (_loc $startpos, snd ident, body) 
      }

forsep: SEMI | RPAR LPAR {}

%inline
structure:
  | COLON
      body = statements
    SEMI
      { 
        `Block (_loc $startpos, body)
      }
  | REPEAT
      body = statements
    TILL cond = expr
      {
        `Repeat (_loc $startpos, body, cond)
      }
  | CASE sel = expr
    ofs   = list(ofcase)
    other = othercase?
    ECASE
      {
        `Case (_loc $startpos, sel, ofs, other)
      }
  | DIF cond = expr
      body = statements 
    chain = condelseorend
      { 
        `DIf (_loc $startpos, cond, body, chain) 
      }
  | DIFDEF defs = expr
      body = statements
    chain = condelseorend
      { 
        let defs' =
          map_expr_leaves
            (function
              | `VarOrFn (l, _, _) as leaf
                  -> let f = `Func (l, "defined?", Text.ident "defined?", [`Simple (l, leaf)]) in
                     if $1 then f else `Unary (nowhere, `Not, f)
              | leaf -> (leaf :> expression))
            defs
        in 
        `DIf (_loc $startpos, defs', body, chain)
      }

condelseorend:
  | DENDIF
      { 
        `DEndif (_loc $startpos) 
      }
  | DELSE 
      body = statements 
    DENDIF
      { 
        `DElse (_loc $startpos, body)
      }
  | DELSEIF cond = expr
      body = statements 
    chain = condelseorend
      { 
        `DIf (_loc $startpos, cond, body, chain)
      }

ofcase: 
  | OF value = expr COMMA?
      body = statements { value, body }

othercase: OTHER body = statements { body }

%inline
text_or_func:
  | expr = top_expr
      { 
        match expr with
          | `VarOrFn v -> `VarOrFn v
          | `Func (l, s, t, p) -> `FuncCall (l, None, s, t, p, None)
          | `SelFunc (l, s, i, w, p) -> `Select (l, `Store l, s, i, w, p)
          | _ -> `Return (_loc $startpos, false, expr)
      }
  | func = GOTO
    args = loption(parenth(clist(fn_param)))
    dest = label
      {
        let func_str, func_text = func in
        `FuncCall (_loc $startpos, None, func_str, func_text, args, Some dest)
      }


(* Compiler directives *)

%inline
definesetundef:
  | dir = DDEFINE
    idents = separated_nonempty_list(COMMA, idelt)
      {
        let makeseq (loc, (id_str, id_text), value_opt) =
          match dir with
            | `Define | `DefineScoped
                -> let scoped = (dir = `DefineScoped) in
                   let value = Option.default (`Int (nowhere, 1l)) value_opt in
                   `Define (loc, id_str, id_text, scoped, value)
            | `Const | `Bind | `EBind as dtype
                -> let value =
                     try
                       Option.get value_opt
                     with Option.No_value ->
                       kprintf (error loc) "cannot define `%s' without an initial value" id_str
                   in
                   `DConst (loc, id_str, id_text, dtype, value)
            | `Redefine
                -> let value =
                     try
                       Option.get value_opt
                     with Option.No_value ->
                       kprintf (error loc) "cannot redefine `%s' without a new value" id_str
                   in
                   `DSet (loc, id_str, id_text, false, value)
        in
        let enm = List.enum idents in
        let seq = Enum.map makeseq enm in
        `Seq (DynArray.of_enum seq)
      }     
  | DSET idents = separated_nonempty_list(COMMA, setelt)
      {
        let makeseq (loc, (id_str, id_text), op, value) =
          let expr =
            match op with
              | #arith_op as arith_op -> `Op (loc, `VarOrFn (loc, id_str, id_text), arith_op, value)
              | `Set -> value
          in
          `DSet (loc, id_str, id_text, true, expr)
        in
        let enm = List.enum idents in
        let seq = Enum.map makeseq enm in
        `Seq (DynArray.of_enum seq)
      }
  | DUNDEF idents = separated_nonempty_list(COMMA, located_ident)
      {
        `DUndef (_loc $startpos, idents)
      }

%inline
directive:
  | dir = DWITHEXPR
    expr = expr
      { 
        let dir_id, dir_text = dir in 
        `Directive (_loc $startpos, dir_id, dir_text, expr)
      }
  | DLOAD filename = expr
      { 
        `LoadFile (_loc $startpos, filename)
      }
  | DTARGET target = IDENT
      { 
        `DTarget (_loc $startpos, Text.to_sjs (snd target))
      }
  | DVERSION ver = separated_nonempty_list(POINT, expr)
      {
        let a, b, c, d =
          let nil = `Int (nowhere, 0l) in
          match ver with
            | a :: b :: c :: d :: tl
              -> if tl <> [] then
                   warning (_loc $startpos) "version numbers in #version directives can only have four parts";
                 a, b, c, d
            | [a; b; c] -> a, b, c, nil
            | [a; b] -> a, b, nil, nil
            | [a] -> a, nil, nil, nil
            | [] -> assert false
        in  
        `DVersion (_loc $startpos, a, b, c, d)
      }
  
idelt:
  | ident = IDENT
    value = option(preceded(SET, expr))
      { 
        _loc $startpos, ident, value
      }

setelt:
  | ident = IDENT 
      { 
        kprintf (error (_loc $startpos)) "cannot mutate `%s' without a new value" (fst ident)
      }
  | ident = IDENT
    setop = assignop
    value = expr
      { 
        _loc $startpos, ident, setop, value
      }

inline_elt:
  | id = IDENT                { let s, t = id in _loc $startpos, s, t, `None }
  | id = IDENT; SET; e = expr { let s, t = id in _loc $startpos, s, t, `Some e }
  | id = bracket(IDENT)       { let s, t = id in _loc $startpos, s, t, `Optional }  

located_ident:
  | ident = IDENT
      {
        let id_str, id_text = ident in
        _loc $startpos, id_str, id_text
      }

(* Expression tree *)

%inline andor: 
  | LOR { `LOr } | LAND { `LAnd } 
%inline logop: 
  | EQU { `Equ } | NEQ { `Neq } 
  | LTE { `Lte } | LTN { `Ltn }
  | GTE { `Gte } | GTN { `Gtn }
%inline binop:
  | AND { `And } | OR  { `Or  } | XOR { `Xor }
  | ADD { `Add } | SUB { `Sub }
  | MUL { `Mul } | DIV { `Div } | MOD { `Mod }
  | SHL { `Shl } | SHR { `Shr }
%inline unary:
  | SUB { `Sub }   
  | NOT { `Not }   
  | TILDE { `Inv } 

expr:
  | e1 = expr; op = andor; e2 = expr { `AndOr (_loc $startpos, e1, op, e2) }
  | e1 = expr; op = logop; e2 = expr { `LogOp (_loc $startpos, e1, op, e2) }
  | e1 = expr; op = binop; e2 = expr { `Op    (_loc $startpos, e1, op, e2) }
  |            op = unary; ex = expr { `Unary (_loc $startpos,     op, ex) } %prec UNARY
  | leaf = string
  | leaf = variable
  | leaf = functioncall
  | leaf = parenth(expr) { leaf :> expression }
  | leaf = INTEGER       { `Int (_loc $startpos, leaf) }

top_expr:
  | e1 = top_expr; ADD; e2 = expr { `Op (_loc $startpos, e1, `Add, e2) }
  | leaf = string
  | leaf = variable
  | leaf = functioncall { leaf :> expression }
  | leaf = INTEGER      { `Int (_loc $startpos, leaf) }


(* Expression leaves *)

string:
  | s = STRING { `Str (_loc $startpos, s) }
  | s = DRES   { `Res (_loc $startpos, s) }

variable:
  | REG                          { assert ($1 == 0xc8); `Store (_loc $startpos) }
  | v = VAR;   e = bracket(expr) { `IVar (_loc $startpos, v, e) }
  | v = SVAR;  e = bracket(expr) { `SVar (_loc $startpos, v, e) }
  | i = IDENT                    { let str, txt = i in `VarOrFn (_loc $startpos, str, txt) }
  | i = IDENT; e = bracket(expr) { let str, txt = i in `Deref   (_loc $startpos, str, txt, e) }


(* Declarations *)

%inline
declaration: 
  | vtype = vartype
    directives = loption(parenth(decldirs))
    variables = separated_nonempty_list(COMMA, variable_decl)
      { 
        `Decl (_loc $startpos, vtype, directives, variables)
      }

vartype:
  | size = INT { `Int size }
  | STR        { `Str }

variable_decl:
  | ident = IDENT
    array = arraydecl
    init  = initdecl
      {
        let ident_str, ident_text = ident
        and init_val, fix_address = init
        in
        _loc $startpos, ident_str, ident_text, array, init_val, fix_address
      }

arraydecl: (* empty *) { `None }
  | LSQU RSQU          { `Auto }
  | e = bracket(expr)  { `Some e }
  
initdecl:
  | ad = addrdecl                { `None, Some ad }
  | ad = addrdecl; vd = valdecl  { vd, Some ad }
  | vd = valdecl; ad = addrdecl? { vd, ad }
  
addrdecl: 
  | ARROW vtype = expr
    POINT vaddr = expr { vtype, vaddr }

valdecl: 
  | SET param = fn_param 
      { 
        match param with 
          | `Simple (_, e) -> `Scalar e
          | `Complex (_, es) -> `Array es
          | `Special _ -> assert false
      }

decldirs: (* empty *) { [] }
  | list = decldirs; COMMA; id = IDENT
      { Variables.decldir_of_ident (_loc $startpos(id)) id :: list }

(* Assignments *)

assignment:
  | var = variable 
    op = assignop 
    value = expr
      { 
        match var with
          | #assignable as a -> `Assign (_loc $startpos, a, op, value)
          | _ -> error (_loc $startpos) "left-hand-side of assignment must be a variable" 
      }

assignop:
  | SADD { `Add } | SSUB { `Sub } | SMUL { `Mul } | SDIV { `Div }
  | SMOD { `Mod } | SAND { `And } | SOR  { `Or  } | SXOR { `Xor }
  | SSHL { `Shl } | SSHR { `Shr } | SET  { `Set }


(* Function calls *)

functioncall:
  | func = IDENT
    args = parenth(clist(fn_param))
      { 
        let fun_str, fun_txt = func in
        `Func (_loc $startpos, fun_str, fun_txt, args)
      }
  | func = SELECT
    wind = option(bracket(expr))
    args = loption(parenth(clist(sel_param)))
      {
        let fun_name, opcode = func in
        `SelFunc (_loc $startpos, fun_name, opcode, wind, args)
      }

fn_param:
  | e = expr                 { `Simple  (_loc $startpos, e) }
  | p = curlybr(clist(expr)) { `Complex (_loc $startpos, p) }

sel_param:
  | option_text = expr
      { 
        `Always (_loc $startpos, option_text) 
      }
  | func = functioncall COLON
    option_text = expr
      { 
        let func_str, func_text, func_param =
          match func with
            | `Func (_, str, text, [`Simple (_, param)]) -> str, text, param
            | _ -> $syntaxerror
        in
        `Special (_loc $startpos, func_str, func_text, func_param, option_text)
      }
  | mode = IDENT IF condition = expr COLON
    option_text = expr
      { 
        let mode_str, mode_text = mode in 
        `Unless (_loc $startpos, mode_str, mode_text, condition, option_text) 
      }


(* Special functions *)

%inline
gotofunction:
  | func = GOTOON   
    sel  = expr 
    LCUR
      labels = separated_nonempty_list(COMMA, label)
    RCUR
      { 
        `GotoOn (_loc $startpos, func, sel, labels)
      }
  | func = GOTOCASE
    sel  = expr
    LCUR
      cases = separated_nonempty_list(SEMI, case) SEMI?
    RCUR
      {
        `GotoCase (_loc $startpos, func, sel, cases)
      }

case:
  | USCORE COLON
    label = label { `Default label }
  | cond = expr COLON 
    label = label { `Match (cond, label) }


(* Unknown opcodes *)

%inline
unknown_op:
  OP LTN; op_type   = INTEGER
     sep; op_module = module_id
     sep; op_code   = INTEGER
     sep; op_over   = INTEGER
     GTN; params    = loption(parenth(clist(fn_param)))
    {
      let op_type = Int32.to_int op_type
      and op_code = Int32.to_int op_code
      and op_over = Int32.to_int op_over
      and op_module, module_id =
        match op_module with
          | `Integer i -> Int32.to_int i, sprintf "%03ld" i
          | `Ident (s, t)
           -> try
                Hashtbl.find modules t, s
              with Not_found ->
                kprintf (error (_loc $startpos(op_module))) "undefined module `%s'" s
      in
      let id = ident_of_opcode op_type op_module op_code op_over in
      let it = Text.ident id in
      if Hashtbl.mem functions it then
        `FuncCall (_loc $startpos, None, id, it, params, None)
      else
        let def =
          { ident = sprintf "op<%d:%s:%05d, %d>" op_type module_id op_code op_over;
            flags = [];
            op_type = op_type;
            op_module = op_module;
            op_code = op_code;
            prototypes = [||];
            targets = []; }
        in `UnknownOp (_loc $startpos, def, op_over, params)
    }

module_id:
  | i = INTEGER { `Integer i }
  | i = IDENT   { `Ident i }
  | STR         { `Ident ("Str", Text.ident "Str") }

sep: COLON | COMMA | POINT | SEMI | SUB | DIV {}


(* Raw bytecode *)

%inline
raw_code:
  RAW
    elts = list(rawelt)
  ENDRAW 
    { `RawCode (_loc $startpos, elts) }

rawelt:
  | i = IDENT { `Ident i } 
  | i = INTEGER { `Int i }
  | COMMA { `Bytes "," } 
  | LPAR  { `Bytes "(" } | RPAR  { `Bytes ")" }
  | LCUR  { `Bytes "{" } | RCUR  { `Bytes "}" }
  | LSQU  { `Bytes "[" } | RSQU  { `Bytes "]" }
