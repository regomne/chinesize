(*
    Rlc: code generation functions
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
open KeTypes
open KeAst

let unique_label loc =
  let s = sprintf "__auto@_%Lu__" (Global.unique ()) in
  loc, s, Text.of_sjs s

let code_of_op =
  function
    | `Add  -> '\x00' | `Sub -> '\x01' | `Mul -> '\x02' | `Div -> '\x03' | `Mod -> '\x04'
    | `And  -> '\x05' | `Or  -> '\x06' | `Xor -> '\x07' | `Shl -> '\x08' | `Shr -> '\x09'
    | `LAnd -> '\x3c' | `LOr -> '\x3d' | `Equ -> '\x28' | `Neq -> '\x29'
    | `Lte  -> '\x2a' | `Ltn -> '\x2b' | `Gte -> '\x2c' | `Gtn -> '\x2d'

let code_of_assign_op =
  function
    | `Add -> '\x14' | `Sub -> '\x15' | `Mul -> '\x16' | `Div -> '\x17' | `Mod -> '\x18'
    | `And -> '\x19' | `Or  -> '\x1a' | `Xor -> '\x1b' | `Shl -> '\x1c' | `Shr -> '\x1d'
    | `Set -> '\x1e'

let code_of_binary lhs op rhs =
  sprintf "%s\\%c%s" lhs op rhs

let code_of_int32 cval =
  sprintf "$\xff%s" (str_of_int32 cval)

let rec code_of_var =
  function
    | `Store _ -> "$\xc8"
    | `IVar (_, i, e)
    | `SVar (_, i, e) -> sprintf "$%c[%s]" (char_of_int i) (code_of_expr e)
    | `Deref (l, _, _, _)
    | `VarOrFn (l, _, _) -> error l "internal error: user variable or function should have been disambiguated in Expr.transform"

and code_of_expr : [ assignable | expression ] -> string =
  function
    | `Int (_, i) -> code_of_int32 i
    | `Op (_,  a, op, b) -> code_of_binary (code_of_expr a) (code_of_op op) (code_of_expr b)
    | `LogOp (_, a, op, b) -> code_of_binary (code_of_expr a) (code_of_op op) (code_of_expr b)
    | `AndOr (_, a, op, b) -> code_of_binary (code_of_expr a) (code_of_op op) (code_of_expr b)
    | `Unary (_, `Sub, e) -> sprintf "\\%c%s" (code_of_op `Sub) (code_of_expr e)
    | `Unary (l, _, _) -> error l "internal error: unary operators other than `-' should have been transformed to binary operations in Expr.transform"
    | `Parens (_, e) -> sprintf "(%s)" (code_of_expr e)
    | #assignable as v -> code_of_var v
    | `Func (l, _, _, _, _)
    | `SelFunc (l, _, _, _, _) -> error l "internal error: function calls should have been lifted out of expressions in Expr.transform"
    | `Str (l, _) | `Res (l, _) -> error l "internal error: type mismatches should have been discovered in Expr.transform"
    | `ExprSeq (l, _, _, _) -> error l "internal error: sequence expressions should have been lifted out of expressions in Expr.transform"

let code_of_assignment (_, dest, op, expr) =
  code_of_binary (code_of_var dest) (code_of_assign_op op) (code_of_expr expr)

let code_of_opcode op_type op_module op_code argc overload =
  sprintf
    "#%c%c%s%s%c"
    (char_of_int op_type)
    (char_of_int op_module)
    (str16_of_int op_code)
    (str16_of_int argc)
    (char_of_int overload)


module Output =
  struct
    type t =
      | Code of string
      | Label of Text.t
      | LabelRef of location * Text.t
      | Entrypoint of int
      | Kidoku of int
      | Lineref of int

    let bytecode = DynArray.create ()
    let labels = Hashtbl.create 0
    let lnum = ref ~-1

    let add_line ?(force = false) loc =
      if !App.debug_info 
      then DynArray.add bytecode (Lineref loc.line)
      else if force then DynArray.add bytecode (Lineref 0);
      lnum := loc.line

    let maybe_line loc =
      if loc <> nowhere && loc.line <> !lnum then add_line loc

    let add_code loc s =
      maybe_line loc;
      DynArray.add bytecode (Code s)

    let length () = DynArray.length bytecode
    let insert_code i s =
      DynArray.insert bytecode i (Code s)

    let add_label (loc, s, t) =
      maybe_line loc;
      if Hashtbl.mem labels t then ksprintf (error loc) "@%s already defined; label identifiers must be unique" s;
      DynArray.add bytecode (Label t);
      Hashtbl.add labels t ()

    let add_ref (loc, _, t) =
      DynArray.add bytecode (LabelRef (loc, t))

    let add_entry idx =
      DynArray.add bytecode (Entrypoint idx)

    let add_kidoku loc =
      maybe_line loc;
      DynArray.add bytecode (Kidoku loc.line)
  end
