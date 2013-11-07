(*
    Rlc: AST definition and utilities
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

  My sincerest apologies for the spaghetti type declarations.  It's necessary to
  do it this way to work around limitations in recursion in polymorphic variants.
  Once this is stable, I'll try to document the types in a rather more readable
  fashion.
*)

open KeTypes

(* Type definitions *)

type label = location * string * Text.t

type arith_op =
  [ `Add | `Sub | `Mul | `Div | `Mod
  | `And | `Or  | `Xor | `Shl | `Shr ]

type unary_op =
  [ `Sub | `Not | `Inv ]

type assign_op =
  [ `Set | arith_op ]

type boolean_op =
  [ `Equ | `Neq | `Ltn  | `Lte | `Gtn | `Gte ]

type chain_op =
  [ `LAnd | `LOr ]

type cond_op =
  [ boolean_op | arith_op ]

type 'expression __assignable =
  [ `Store   of location
  | `IVar    of location * int * 'expression
  | `SVar    of location * int * 'expression
  | `Deref   of location * string * Text.t * 'expression
  | `VarOrFn of location * string * Text.t ]

and 'expression __functioncall =
  [ `Func    of location * string * Text.t * 'expression __parameter list * label option
  | `SelFunc of location * string * int * 'expression option * 'expression __sel_parameter list ]

and 'expression __parameter =
  [ `Simple  of location * 'expression
  | `Complex of location * 'expression list
  | `Special of location * int * 'expression list ]

and 'expression __sel_parameter =
  [ `Always  of location * 'expression
  | `Special of location * 'expression __sel_special list * 'expression ]
  
and 'expression __sel_special =
  [ `Flag    of location * string * Text.t
  | `NonCond of location * string * Text.t * 'expression
  | `Cond    of location * string * Text.t * 'expression option * 'expression ]

type strtoken_non_expr =
  [ `EOS
  | `DQuote   of location
  | `RCur     of location
  | `LLentic  of location
  | `RLentic  of location
  | `Asterisk of location
  | `Percent  of location
  | `Hyphen   of location
  | `Speaker  of location
  | `Add      of location * (location * Text.t)
  | `Delete   of location
  | `Rewrite  of location * int (* key to StrLexer.rewrites X_x *)
  | `Text     of location * [ `Sbcs | `Dbcs ] * Text.t
  | `Space    of location * int ]

type res = location * Text.t

type 'expression __strtoken =
  [ strtoken_non_expr
  | `Name  of location * [ `Local | `Global ] * 'expression * 'expression option
  | `Gloss of location * [ `Gloss | `Ruby ] * 'expression __strtokens 
            * [ `Closed  of location * 'expression __strtokens | `ResStr of res ] 
  | `Code  of location * Text.t * 'expression option * 'expression __parameter list ]

and 'expression __strtokens = 'expression __strtoken DynArray.t


type 'expression __atom =
  [ `Store   of location
  | `Int     of location * int32
  | `Str     of location * 'expression __strtokens
  | `Res     of location * Text.t ]

type 'statement _expression =
  [ 'statement _expression __assignable
  | 'statement _expression __atom
  | `Op      of location * 'statement _expression * arith_op * 'statement _expression
  | `LogOp   of location * 'statement _expression * boolean_op * 'statement _expression
  | `AndOr   of location * 'statement _expression * chain_op * 'statement _expression
  | `Unary   of location * unary_op * 'statement _expression
  | `Parens  of location * 'statement _expression
  | `ExprSeq of location * Text.t * (Text.t * 'statement _expression) list * 'statement DynArray.t
  | 'statement _expression __functioncall ]

type 'statement _atom = 'statement _expression __atom
and 'statement _assignable = 'statement _expression __assignable
and 'statement _parameter = 'statement _expression __parameter
and 'statement _sel_parameter = 'statement _expression __sel_parameter
and 'statement _strtoken = 'statement _expression __strtoken
and 'statement _strtokens = 'statement _expression __strtokens
  
and 'statement optexpr = 'statement _expression option

type 'statement _assignment =
  [ `Assign of location * 'statement _assignable * assign_op * 'statement _expression ]

type 'statement _normalisable_directive =
  [ `Directive of location * string * [`Int | `Str | `None] * 'statement _expression ]

type 'statement _directive =
  [ `Define   of location * string * Text.t * bool * 'statement _expression
  | `DConst   of location * string * Text.t * [`Const | `Bind | `EBind] * 'statement _expression
  | `DInline  of location * string * Text.t * bool
               * (location * string * Text.t * [ `None | `Optional | `Some of 'statement _expression ]) list 
               * 'statement
  | `DUndef   of location * (location * string * Text.t) list
  | `DTarget  of location * string
  | `DVersion of location * 'statement _expression * 'statement _expression * 'statement _expression * 'statement _expression
  | `DSet     of location * string * Text.t * bool * 'statement _expression
  | 'statement _normalisable_directive ]

type 'statement _normalisable =
  [ `GotoOn    of location * [`Goto | `Gosub] * 'statement _expression * label list
  | `Return    of location * bool * 'statement _expression
  | `LoadFile  of location * 'statement _expression
  | 'statement _normalisable_directive ]

type ('statement, 'ifcont) _ifdirective =
  [ `DIf of location * 'statement _expression * 'statement DynArray.t * 'ifcont ]

type 'statement _ifcont =
  [ ('statement, 'statement _ifcont) _ifdirective
  | `DElse  of location * 'statement DynArray.t
  | `DEndif of location ]

type 'statement _structure =
  [ `Seq       of 'statement DynArray.t
  | `Hiding    of location * Text.t * 'statement
  | `Block     of location * 'statement DynArray.t
  | `If        of location * 'statement _expression * 'statement * 'statement option
  | `While     of location * 'statement _expression * 'statement
  | `Repeat    of location * 'statement DynArray.t * 'statement _expression
  | `For       of location * 'statement DynArray.t * 'statement _expression * 'statement DynArray.t * 'statement
  | `Case      of location * 'statement _expression * ('statement _expression * 'statement DynArray.t) list 
                * 'statement DynArray.t option
  | `DFor      of location * string * Text.t * 'statement _expression * 'statement _expression * 'statement
  | ('statement, 'statement _ifcont) _ifdirective ]

type decldir = [ `Zero | `Block | `Ext | `Label ]

type funccall_type = 
  location * statement _assignable option * string * Text.t * statement _parameter list * label option

and statement =
  [ `Null
  | `Halt      of location
  | `Break     of location
  | `Continue  of location
  | `Label     of label
  | `Decl      of location * [ `Int of int | `Str ] * decldir list 
                * (location * string * Text.t 
                   * [ `None | `Auto | `Some of statement _expression ]
                   * [ `None | `Scalar of statement _expression | `Array of statement _expression list ]
                   * (statement _expression * statement _expression) option) list
  | `GotoCase  of location * [`Goto | `Gosub] * statement _expression 
                * [ `Default of label | `Match of statement _expression * label ] list
  | `UnknownOp of location * func * int * statement _parameter list
  | `Select    of location * statement _assignable * string * int * statement optexpr * statement _sel_parameter list
  | `FuncCall  of funccall_type
  | `VarOrFn   of location * string * Text.t
  | `RawCode   of location * [`Bytes of string | `Int of int32 | `Ident of string * Text.t] list
  | statement _normalisable
  | statement _assignment
  | statement _directive
  | statement _structure ]

type atom = statement _atom
and assignable = statement _assignable
and parameter = statement _parameter
and sel_parameter = statement _sel_parameter
and strtoken = statement _strtoken
and strtokens = statement _strtokens

and directive = statement _directive
and structure = statement _structure
and assignment = statement _assignment
and expression = statement _expression
and normalisable = statement _normalisable
and normalisable_directive = statement _normalisable_directive


(* Utilities *)

let memory__get_as_expression = ref
  (fun ?(allow_arrays = false) ?(recurse = false) ?(args: expression list = []) ?(loc = nowhere) ?(s = "") _ -> 
    failwith "not initialised")

let is_store : [> `Store of location ] -> bool =
  function
    | `Store _ -> true
    | _ -> false

let loc_of_expr : expression -> location =
  function
    | `Int (l, _)
    | `Str (l, _)
    | `Res (l, _)
    | `Store l
    | `VarOrFn (l, _, _)
    | `Op (l, _, _, _)
    | `LogOp (l, _, _, _)
    | `AndOr (l, _, _, _)
    | `Unary (l, _, _)
    | `Func (l, _, _, _, _)
    | `SelFunc (l, _, _, _, _)
    | `IVar (l, _, _)
    | `SVar (l, _, _)
    | `Deref (l, _, _, _)
    | `ExprSeq (l, _, _, _)
    | `Parens (l, _) -> l

let simple_of_expression e = `Simple (loc_of_expr e, e)
let expression_of_simple =
  function
    | `Simple (_, e) -> e
    | _ -> invalid_arg "expression_of_simple"

let rec type_of_normalised_expr ?(allow_invalid = true) : expression -> [`Int | `Str | `Literal | `Invalid] =
  function
    | `Int _
    | `Store _
    | `IVar _
    | `LogOp _
    | `AndOr _
    | `Unary _
        -> `Int
    | `Str _
        -> `Literal
    | `SVar _
        -> `Str
    | `Op (_, e, _, _)
    | `Parens (_, e)
        -> type_of_normalised_expr e
    | `Func (l, _, _, _, _)
    | `SelFunc (l, _, _, _, _) when allow_invalid
        -> `Invalid
    | `Func (l, _, _, _, _)
    | `SelFunc (l, _, _, _, _)
    | `Deref (l, _, _, _)
    | `VarOrFn (l, _, _)
    | `Res (l, _)
    | `ExprSeq (l, _, _, _)
        -> error l "invalid element in type_of_normalised_expr"

let rec normalised_expr_is_const =
  function
    | `Int _ | `Str _ -> true
    | `Parens (_, e) -> normalised_expr_is_const e
    | _ -> false

let forward__string_of_expr = ref (fun _ -> assert false)

let rec const_of_normalised_expr ?(abort_on_fail = true) ?(expect = `None) e : [`Integer of int32 | `String of strtokens] =
  let fail l s =
    if abort_on_fail then
      let errstr =
        if l = nowhere then (* just for debugging generated code at the moment *)
          s ^ "\nErroneous expression: " ^ !forward__string_of_expr e
        else
          s
      in
      error l errstr
    else
      raise Exit
  in
  match e with
    | `Int (l, i) -> if expect = `Str then fail l "expected string constant, found integer" else `Integer i
    | `Str (l, t) -> if expect = `Int then fail l "expected integer constant, found string" else `String t
    | `Parens (_, e) -> const_of_normalised_expr e
    | `Store l
    | `IVar (l, _, _) -> fail l "expected constant, found integer variable"
    | `SVar (l, _, _) -> fail l "expected constant, found string variable"
    | e -> fail (loc_of_expr e) "expected constant, found variable expression"

let int_of_normalised_expr e =
  match const_of_normalised_expr e ~expect:`Int with
    | `Integer i -> i
    | _ -> assert false

let str_of_normalised_expr e =
  match const_of_normalised_expr e ~expect:`Str with
    | `String s -> s
    | _ -> assert false

let rec map_expr_leaves (f : [ atom | assignable ] -> expression) =
  function
    | (#atom | #assignable) as leaf -> f leaf
    | `Op (l, a, o, b) -> `Op (l, map_expr_leaves f a, o, map_expr_leaves f b)
    | `LogOp (l, a, o, b) -> `LogOp (l, map_expr_leaves f a, o, map_expr_leaves f b)
    | `AndOr (l, a, o, b) -> `AndOr (l, map_expr_leaves f a, o, map_expr_leaves f b)
    | `Unary (l, o, e) -> `Unary (l, o, map_expr_leaves f e)
    | `Parens (l, e) -> `Parens (l, map_expr_leaves f e)
    | (`Func _ | `SelFunc _ | `ExprSeq _) as e -> e

let rec set_expr_loc l =
  function
    | `Int (_, i) -> `Int (l, i)
    | `Str (_, t) -> `Str (l, t)
    | `Res (_, t) -> `Res (l, t)
    | `Store _ -> `Store l
    | `IVar (_, i, e) -> `IVar (l, i, e)
    | `SVar (_, i, e) -> `SVar (l, i, e)
    | `Deref (_, s, t, e) -> `Deref (l, s, t, e)
    | `VarOrFn (_, s, t) -> `VarOrFn (l, s, t)
    | `Op (_, a, op, b) -> `Op (l, a, op, b)
    | `LogOp (_, a, op, b) -> `LogOp (l, a, op, b)
    | `AndOr (_, a, op, b) -> `AndOr (l, a, op, b)
    | `Unary (_, op, e) -> `Unary (l, op, e)
    | `Parens (_, e) -> `Parens (l, set_expr_loc l e)
    | `Func (_, s, t, p, d) -> `Func (l, s, t, p, d)
    | `SelFunc (_, s, t, o, p) -> `SelFunc (l, s, t, o, p)
    | `ExprSeq (_, t, s, d) -> `ExprSeq (l, t, s, d)


let loc_of_statement : statement -> location =
  function
    | `Null
    | `Hiding _
    | `Seq _
       -> nowhere
    | `Halt l
    | `Break l
    | `Continue l
    | `Label (l, _, _)
    | `Decl (l, _, _, _)
    | `GotoOn (l, _, _, _)
    | `GotoCase (l, _, _, _)
    | `UnknownOp (l, _, _, _)
    | `Select (l, _, _, _, _, _)
    | `FuncCall (l, _, _, _, _, _)
    | `VarOrFn (l, _, _)
    | `Return (l, _, _)
    | `LoadFile (l, _)
    | `Assign (l, _, _, _)
    | `Define (l, _, _, _, _)
    | `DConst (l, _, _, _, _)
    | `DUndef (l, _)
    | `DInline (l, _, _, _, _, _)
    | `DTarget (l, _)
    | `DVersion (l, _, _, _, _)
    | `DSet (l, _, _, _, _)
    | `Directive (l, _, _, _)
    | `Block (l, _)
    | `If (l, _, _, _)
    | `DIf (l, _, _, _)
    | `DFor (l, _, _, _, _, _)
    | `While (l, _, _)
    | `Repeat (l, _, _)
    | `For (l, _, _, _, _)
    | `Case (l, _, _, _)
    | `RawCode (l, _)
       -> l

let expr_of_statement : statement -> expression = (* returns a NON-NORMALISED expression *)
  function
    | `VarOrFn _ as e -> (e :> expression)
    | `Return (_, _, e) -> e
    | `Select (l, `Store _, s, i, e, p) -> `SelFunc (l, s, i, e, p)
    | `FuncCall (l, (None | Some (`Store _)), s, t, p, d) -> `Func (l, s, t, p, d)
    | s -> error (loc_of_statement s) "expected expression, found non-convertible statement"

let equal_strings s1 s2 =
  false (* can be written, but I can't be bothered now as I don't particularly need it *)

let rec equal_exprs e1 e2 = (* expressions must be normalised and contain no funtions *)
  match e1, e2 with
    | `Func _, `Func _ | `SelFunc _, `SelFunc _ -> assert false
    | `Res _, _ | _, `Res _ 
    | `Deref _, _ | _, `Deref _
    | `VarOrFn _, _ | _, `VarOrFn _
    | `ExprSeq _, _ | _, `ExprSeq _
       -> failwith "expressions not normalised"
    | `Parens (_, e), `Parens (_, f)
    | `Parens (_, e), f
    | e, `Parens (_, f) 
       -> equal_exprs e f
    | `Int (_, i), `Int (_, j) -> i = j
    | `Str (_, t), `Str (_, u) -> equal_strings t u
    | `Store _, `Store _ -> true
    | `IVar (_, i, e), `IVar (_, j, f)
    | `SVar (_, i, e), `SVar (_, j, f) -> i = j && equal_exprs e f
    | `Op (_, a, op, b), `Op (_, c, pp, d) -> op = pp && equal_exprs a b && equal_exprs c d
    | `LogOp (_, a, op, b), `LogOp (_, c, pp, d) -> op = pp && equal_exprs a b && equal_exprs c d
    | `AndOr (_, a, op, b), `AndOr (_, c, pp, d) -> op = pp && equal_exprs a b && equal_exprs c d
    | `Unary (_, op, e), `Unary (_, pp, f) -> op = pp && equal_exprs e f
    | _ -> false
  

let rec exists_in_expr f (expr: [expression | assignable]) =
  if f expr then true else
  match expr with
    | #atom -> false
    | `VarOrFn (loc, _, t) -> (try f (!memory__get_as_expression t ~loc) with Not_found -> false)
    | `Deref (_, _, _, e) -> exists_in_expr f e
    | `Op (_, a, _, b)
    | `LogOp (_, a, _, b)
    | `AndOr (_, a, _, b) -> exists_in_expr f a || exists_in_expr f b
    | `Unary (_, _, e) -> exists_in_expr f e
    | `Parens (_, e) -> exists_in_expr f e
    | `IVar (_, _, e)
    | `SVar (_, _, e) -> exists_in_expr f e
    | `Func (_, _, _, p, _)
       -> List.exists
            (function
              | `Simple (_, e) -> exists_in_expr f e
              | `Complex (_, es)
              | `Special (_, _, es) -> List.exists (exists_in_expr f) es)
            p
    | `SelFunc (_, _, _, w, p)
       -> (match w with Some e -> exists_in_expr f e | None -> false)
       || List.exists
            (function
              | `Always (_, e) -> exists_in_expr f e
              | `Special (_, l, e) -> exists_in_expr f e 
                                   || List.exists 
                                        (function
                                          | `Flag _ -> false
                                          | `Cond (_, _, _, None, e)
                                          | `NonCond (_, _, _, e) -> exists_in_expr f e
                                          | `Cond (_, _, _, Some d, e) -> exists_in_expr f d || exists_in_expr f e)
                                        l)
            p
    | `ExprSeq (_, _, _, d)
       -> if DynArray.empty d
          then false
          else DynArray.length d > 1 || f (expr_of_statement (DynArray.last d))

(* Pretty-printing *)

open Printf

let string_of_op : [< cond_op | chain_op | `Not | `Inv ] -> string =
  function
    | `Add -> "+"  | `Sub -> "-"  | `Mul -> "*"  | `Div -> "/"  | `Mod -> "%"
    | `And -> "&"  | `Or  -> "|"  | `Xor -> "^"  | `Shl -> "<<" | `Shr -> ">>"
    | `LAnd-> "&&" | `LOr -> "||" | `Equ -> "==" | `Neq -> "!="
    | `Ltn -> "<"  | `Lte -> "<=" | `Gtn -> ">"  | `Gte -> ">="
    | `Not -> "!"  | `Inv -> "~"

let string_of_assign_op : assign_op -> string =
  function
    | `Set -> "="
    | `Add -> "+=" | `Sub -> "-=" | `Mul -> "*=" | `Div -> "/="  | `Mod -> "%="
    | `And -> "&=" | `Or  -> "|=" | `Xor -> "^=" | `Shl -> "<<=" | `Shr -> ">>="

let string_of_list ?(sep = ",") f p = String.concat (sep ^ " ") (List.map f p)

let variable_name ?(prefix = true) =
  let sprefix s = if prefix then Config.svar_prefix ^ s else s
  and iprefix i = if prefix then Config.ivar_prefix ^ i else i
  in
  function
    | 0x0a -> sprefix "K"   | 0x0b -> iprefix "L"
    | 0x0c -> sprefix "M"   | 0x12 -> sprefix "S"
    | 0x00 -> iprefix "A"   | 0x01 -> iprefix "B"
    | 0x02 -> iprefix "C"   | 0x03 -> iprefix "D"
    | 0x04 -> iprefix "E"   | 0x05 -> iprefix "F"
    | 0x06 -> iprefix "G"   | 0x19 -> iprefix "Z"
    | 0x1a -> iprefix "Ab"  | 0x1b -> iprefix "Bb"
    | 0x1c -> iprefix "Cb"  | 0x1d -> iprefix "Db"
    | 0x1e -> iprefix "Eb"  | 0x1f -> iprefix "Fb"
    | 0x20 -> iprefix "Gb"  | 0x33 -> iprefix "Zb"
    | 0x34 -> iprefix "A2b" | 0x35 -> iprefix "B2b"
    | 0x36 -> iprefix "C2b" | 0x37 -> iprefix "D2b"
    | 0x38 -> iprefix "E2b" | 0x39 -> iprefix "F2b"
    | 0x3a -> iprefix "G2b" | 0x4d -> iprefix "Z2b"
    | 0x4e -> iprefix "A4b" | 0x4f -> iprefix "B4b"
    | 0x50 -> iprefix "C4b" | 0x51 -> iprefix "D4b"
    | 0x52 -> iprefix "E4b" | 0x53 -> iprefix "F4b"
    | 0x54 -> iprefix "G4b" | 0x67 -> iprefix "Z4b"
    | 0x68 -> iprefix "A8b" | 0x69 -> iprefix "B8b"
    | 0x6a -> iprefix "C8b" | 0x6b -> iprefix "D8b"
    | 0x6c -> iprefix "E8b" | 0x6d -> iprefix "F8b"
    | 0x6e -> iprefix "G8b" | 0x81 -> iprefix "Z8b"
    | _ -> assert false

let string_of_decldir : decldir -> string =
  function
    | `Zero -> "zero"
    | `Block -> "block"
    | `Ext -> "ext"
    | `Label -> "labelled"

let rec string_of_expr : expression -> string =
  function
    | #assignable as a -> string_of_assignable a
    | `Int (_, i) -> Int32.to_string i
    | `Str (_, t) -> sprintf "'%s'" (string_of_strtokens t)
    | `Res (_, t) -> sprintf "#res<%s>" (Text.to_err t)
    | `Op (_, a, op, b) -> sprintf "%s %s %s" (string_of_expr a) (string_of_op op) (string_of_expr b)
    | `LogOp (_, a, op, b) -> sprintf "%s %s %s" (string_of_expr a) (string_of_op op) (string_of_expr b)
    | `AndOr (_, a, op, b) -> sprintf "%s %s %s" (string_of_expr a) (string_of_op op) (string_of_expr b)
    | `Unary (_, op, e) -> sprintf "%s%s" (string_of_op op) (string_of_expr e)
    | `Parens (_, e) -> sprintf "(%s)" (string_of_expr e)
    | `Func (_, s, _, p, d) -> sprintf "%s(%s)%s" s (string_of_list string_of_param p) (match d with None -> "" | Some lbl -> " " ^ string_of_label lbl)
    | `SelFunc (_, s, _, None, p) -> sprintf "%s(%s)" s (string_of_list string_of_sel_param p)
    | `SelFunc (_, s, _, Some e, p) -> sprintf "%s[%s](%s)" s (string_of_expr e) (string_of_list string_of_sel_param p)
    | `ExprSeq (_, _, l, d) -> sprintf "{%s> %s }" (if l = [] then "" else sprintf "< %s " (string_of_list (fun (t, e) -> sprintf "%s = %s" (Text.to_err t) (string_of_expr e)) l)) (string_of_list string_of_statement (DynArray.to_list d))

and string_of_strtokens t =
  let rv = Buffer.create 0 in
  let add = Buffer.add_string rv in
  DynArray.iter
    (function `EOS -> assert false
      | `DQuote _  -> add "\""
      | `RCur _    -> add "}"
      | `LLentic _ -> add (Text.sjs_to_enc Config.default_encoding "\x81\x79")
      | `RLentic _ -> add (Text.sjs_to_enc Config.default_encoding "\x81\x7a")
      | `Asterisk _-> add (Text.sjs_to_enc Config.default_encoding "\x81\x96")
      | `Percent _ -> add (Text.sjs_to_enc Config.default_encoding "\x81\x93")
      | `Hyphen _  -> add "-"
      | `Speaker _ -> add "\\{"
      | `Gloss (_, g, s, `Closed (_, r)) -> ksprintf add "\\%s{%s}={%s}" (match g with `Gloss -> "g" | `Ruby -> "ruby") (string_of_strtokens s) (string_of_strtokens r)
      | `Gloss (_, g, s, `ResStr (_, t)) -> ksprintf add "\\%s{%s}=<%s>" (match g with `Gloss -> "g" | `Ruby -> "ruby") (string_of_strtokens s) (Text.to_err t)
      | `Add (_, (_, t)) -> ksprintf add "\\a{%s}" (Text.to_err t)
      | `Rewrite (_, i) -> ksprintf add "\\f{<rewrite %d>}" i
      | `Name (_, lg, e, None) -> ksprintf add "\\%s{%s}" (if lg = `Local then "l" else "m") (string_of_expr e)
      | `Name (_, lg, e, Some i) -> ksprintf add "\\%s{%s, %s}" (if lg = `Local then "l" else "m") (string_of_expr e) (string_of_expr i)
      | `Space (_, i) -> add (String.make i ' ')
      | `Code (_, t, w, p) -> ksprintf add "\\%s%s{%s}" (Text.to_err t) (match w with None -> "" | Some e -> ":" ^ string_of_expr e) (string_of_list string_of_param p)
      | `Text (_, _, t) -> add (Text.to_err t)
      | `Delete _ -> add "\\d")
    t;
  Buffer.contents rv

and string_of_assignable : assignable -> string =
  function
    | `Store _ -> "store"
    | `IVar (_, i, e)
    | `SVar (_, i, e) -> sprintf "%s[%s]" (variable_name i) (string_of_expr e)
    | `Deref (_, s, _, e) -> sprintf "%s[%s]" s (string_of_expr e)
    | `VarOrFn (_, s, _) -> s

and string_of_param : parameter -> string =
  function
    | `Simple (_, e) -> string_of_expr e
    | `Complex (_, p) -> sprintf "{%s}" (string_of_list string_of_expr p)
    | `Special (_, i, p) -> sprintf "__special[%d](%s)" i (string_of_list string_of_expr p)

and string_of_sel_param : sel_parameter -> string =
  function
    | `Always (_, e) -> string_of_expr e
    | `Special (_, l, e) 
     -> let f = 
          function
            | `Flag (_, s, _) -> s
            | `NonCond (_, s, _, e) -> sprintf "%s(%s)" s (string_of_expr e)
            | `Cond (_, s, _, None, e) -> sprintf "%s if %s" s (string_of_expr e)
            | `Cond (_, s, _, Some v, e) -> sprintf "%s(%s) if %s" s (string_of_expr v) (string_of_expr e)
        in 
        sprintf "%s: %s" (String.concat "; " (List.map f l)) (string_of_expr e)

and string_of_label (_, s, _) = sprintf "@%s" s

and goto_gosub = function `Goto -> "goto" | `Gosub -> "gosub"
and string_of_case =
  function
    | `Default l -> sprintf "_: %s" (string_of_label l)
    | `Match (e, l) -> sprintf "(%s): %s" (string_of_expr e) (string_of_label l)

and string_of_statement : statement -> string =
  function
    | `Null -> "[null]"
    | `Hiding _ -> "[hiding]"
    | `RawCode _ -> "raw ... endraw"
    | `Halt _ -> "halt"
    | `Break _ -> "break"
    | `Continue _ -> "continue"
    | `Label l -> sprintf "  %s" (string_of_label l)
    | `Decl (_, t, d, vs) 
        -> sprintf "%s%s %s"
            (match t with `Str -> "str" | `Int 1 -> "bit" | `Int 8 -> "byte" | `Int 32 -> "int" | `Int i -> sprintf "bit%d" i)
            (if d = [] then "" else sprintf " (%s)" (string_of_list string_of_decldir d))
            (string_of_list 
              (fun (_, i, _, a, v, l) ->
                sprintf "%s%s%s%s" i
                  (match a with `None -> "" | `Auto -> "[]" | `Some e -> sprintf "[%s]" (string_of_expr e))
                  (match l with None -> "" | Some (d, e) -> sprintf " -> %s.%s" (string_of_expr d) (string_of_expr e))
                  (match v with `None -> "" | `Scalar e -> sprintf " = %s" (string_of_expr e) | `Array es -> sprintf " = { %s }" (string_of_list string_of_expr es)))
              vs)
    | `GotoOn (_, g, e, l) -> sprintf "%s_on (%s) { %s }" (goto_gosub g) (string_of_expr e) (string_of_list string_of_label l)
    | `GotoCase (_, g, e, c) -> sprintf "%s_case (%s) { %s }" (goto_gosub g) (string_of_expr e) (string_of_list string_of_case c ~sep:";")
    | `Select (_, `Store _, s, _, None, p) -> sprintf "%s(%s)" s (string_of_list string_of_sel_param p)
    | `Select (_, `Store _, s, _, Some e, p) -> sprintf "%s[%s](%s)" s (string_of_expr e) (string_of_list string_of_sel_param p)
    | `Select (_, d, s, _, None, p) -> sprintf "%s = %s(%s)" (string_of_assignable d) s (string_of_list string_of_sel_param p)
    | `Select (_, d, s, _, Some e, p) -> sprintf "%s = %s[%s](%s)" (string_of_assignable d) s (string_of_expr e) (string_of_list string_of_sel_param p)
    | `UnknownOp (_, { ident = s }, _, p) -> sprintf "%s(%s)" s (string_of_list string_of_param p)
    | `FuncCall (_, (None | Some (`Store _)), s, _, p, l) -> sprintf "%s(%s)%s" s (string_of_list string_of_param p) (match l with None -> "" | Some l -> " " ^ string_of_label l)
    | `FuncCall (_, Some d, s, _, p, l) -> sprintf "%s = %s(%s)%s" (string_of_assignable d) s (string_of_list string_of_param p) (match l with None -> "" | Some l -> " " ^ string_of_label l)
    | `Assign (_, d, op, e) -> sprintf "%s %s %s" (string_of_assignable d) (string_of_assign_op op) (string_of_expr e)
    | `VarOrFn (_, s, _) -> s
    | `Return (_, b, e) -> sprintf "%s%s" (if b then "return " else "") (string_of_expr e)
    | `LoadFile (_, e) -> sprintf "#load %s" (string_of_expr e)
    | `Define (_, s, _, sc, e) -> sprintf "#%sdefine %s = %s" (if sc then "s" else "") s (string_of_expr e)
    | `DConst (_, s, _, cb, e) -> sprintf "#%s %s = %s" (match cb with `Const -> "const" | `Bind -> "bind" | `EBind -> "ebind") s (string_of_expr e)
    | `DInline (_, s, _, sc, i, e) -> sprintf "#%sinline %s(%s) %s" (if sc then "s" else "") s (string_of_list (function _, s, _, `None -> s | _, s, _, `Optional -> sprintf "[%s]" s | _, s, _, `Some e -> sprintf "%s = %s" s (string_of_expr e)) i) (string_of_statement e)
    | `DUndef (_, l) -> sprintf "#undef %s" (string_of_list (fun (_, s, _) -> s) l)
    | `DSet (_, s, _, ros, e) -> sprintf "#%s %s = %s" (if ros then "set" else "redef") s (string_of_expr e)
    | `DTarget (_, s) -> sprintf "#target %s" s
    | `DVersion (_, a, b, c, d) -> sprintf "#version %s.%s.%s.%s" (string_of_expr a) (string_of_expr b) (string_of_expr c) (string_of_expr d)
    | `Directive (_, s, _, e) -> sprintf "#%s %s" s (string_of_expr e)

    | `Seq b -> string_of_list string_of_statement (DynArray.to_list b)
    | `Block (_, b) -> sprintf ":\n  %s;" (string_of_list string_of_statement (DynArray.to_list b) ~sep:"\n ")
    | `If (_, e, s, None) -> sprintf "if %s %s" (string_of_expr e) (string_of_statement s)
    | `If (_, e, s, Some t) -> sprintf "if %s %s\nelse %s" (string_of_expr e) (string_of_statement s) (string_of_statement t)
    | `While (_, e, s) -> sprintf "while %s %s" (string_of_expr e) (string_of_statement s)
    | `Repeat (_, s, e) -> sprintf "repeat\n  %s\ntill %s" (string_of_list string_of_statement (DynArray.to_list s) ~sep:"\n ") (string_of_expr e)
    | `For (_, p, c, i, s) -> sprintf "for (%s; %s; %s) %s" (string_of_statement (`Seq p)) (string_of_expr c) (string_of_statement (`Seq i)) (string_of_statement s)
    | `Case (_, e, c, None) -> sprintf "case %s\n%s\necase" (string_of_expr e) (string_of_list (fun (e, s) -> sprintf "of %s\n  %s\n" (string_of_expr e) (string_of_list string_of_statement (DynArray.to_list s) ~sep:"\n ")) c)
    | `Case (_, e, c, Some o) -> sprintf "case %s\n%s\nother\n  %s\necase" (string_of_expr e) (string_of_list (fun (e, s) -> sprintf "of %s\n  %s\n" (string_of_expr e) (string_of_list string_of_statement (DynArray.to_list s) ~sep:"\n ")) c) (string_of_list string_of_statement (DynArray.to_list o))
    | `DFor (_, s, _, f, t, d) -> sprintf "#for %s = %s .. %s %s" s (string_of_expr f) (string_of_expr t) (string_of_statement d)
    | `DIf _ as dif -> string_of_ifdir false dif

and string_of_ifdir is_cont =
  function
    | `DIf (_, e, b, c) -> sprintf "#%sif %s\n  %s\n%s" (if is_cont then "else" else "") (string_of_expr e) (string_of_list string_of_statement (DynArray.to_list b) ~sep:"\n ") (string_of_ifdir true c)
    | `DElse (_, b) -> sprintf "#else\n  %s\n#endif" (string_of_list string_of_statement (DynArray.to_list b) ~sep:"\n ")
    | `DEndif _ -> sprintf "#endif"

let () = forward__string_of_expr := string_of_expr
