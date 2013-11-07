(*
    Rlc: expression transformations
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
(*ppopt ./pa_matches.cmo *)

(* Order of evaluation of functions within an expression, and arguments within
   a function call, is undefined; it will normally be left-to-right, but this
   is NOT guaranteed. *)

open Printf
open KeTypes
open KeAst
open ExtList

type 'inferred transform_aux_struct =
  { get_id: unit -> int;
    tempvars: (int, assignable) Hashtbl.t;
    pre_code: (int * 'inferred) DynArray.t;
    mutable redef_store: assignable option;
    contains_store: bool Lazy.t }

let make_transform_aux e =
  { get_id = (let i = ref 0 in fun () -> incr i; !i);
    tempvars = Hashtbl.create 0;
    pre_code = DynArray.create ();
    redef_store = None;
    contains_store = lazy (exists_in_expr is_store e); }


(* Operator utilities *)

let prec : arith_op -> int =
  (* Actual precedences in RealLive bytecode *)
  function
    | `Add | `Sub -> 10
    | `Mul | `Div | `Mod | `And | `Or | `Xor | `Shl | `Shr -> 20

let apply_arith a b : arith_op -> int32 =
  function
    | `Add -> Int32.add a b | `Sub -> Int32.sub a b    | `Mul -> Int32.mul a b   | `Div -> Int32.div a b
    | `Mod -> Int32.rem a b | `And -> Int32.logand a b | `Or  -> Int32.logor a b | `Xor -> Int32.logxor a b
    | `Shl -> Int32.shift_left a (Int32.to_int b)      | `Shr -> Int32.shift_right a (Int32.to_int b)

let apply_unary i : unary_op -> int32 =
  function
    | `Sub -> Int32.neg i
    | `Not -> if i = 0l then 1l else 0l
    | `Inv -> Int32.logxor i Int32.minus_one

let apply_cond a b op =
  let c = match op with
    `Equ -> a = b | `Neq -> a <> b | `Ltn -> a < b | `Lte -> a <= b | `Gtn -> a > b | `Gte -> a >= b
  in
  if c then 1l else 0l

let reverse_cond : boolean_op -> boolean_op =
  function
    | `Equ -> `Neq
    | `Neq -> `Equ
    | `Ltn -> `Gte
    | `Lte -> `Gtn
    | `Gtn -> `Lte
    | `Gte -> `Ltn


(* Utility traversals *)

let expr_disambiguate =
  function
    | `VarOrFn (loc, s, t)
       -> if not (Memory.defined t) then
            if Intrinsic.is_builtin t then
              Intrinsic.eval_as_expr (loc, s, t, [], None)
            else try
              ignore (ver_fun "" (Hashtbl.find_all functions t));
              `Func (loc, s, t, [], None)
            with Not_found ->
              (* Either a user function or an undefined symbol. *)
              (* TODO: ultimately we want to implement user functions. *)
              (* TODO: do we want to look for ini variables here too? *)
              ksprintf (error loc) "undeclared identifier `%s'" s
          else
            Memory.get_as_expression ~loc ~s t
    | `Deref (loc, s, t, offset)
       -> if not (Memory.defined t) then
            ksprintf (error loc) 
              (if Intrinsic.is_builtin t || Hashtbl.mem functions t 
               then format_of_string "`%s' is not an array"
               else format_of_string "undeclared identifier `%s'") s
          else
            Memory.get_deref_as_expression ~loc ~s t offset
    | `Func (loc, s, t, parms, label)
       -> assert (Memory.defined t || Intrinsic.is_builtin t);
          assert (label == None);
          if Memory.defined t then
            let mf =
              function
                | `Simple (_, e) -> e
                | `Complex (l, _)
                | `Special (l, _, _) -> error l "expected expression as parameter to inline expansion"
            in
            Memory.get_as_expression ~loc ~s t ~args:(List.map mf parms)
          else
            Intrinsic.eval_as_expr (loc, s, t, parms, None)


let map_func_param f aux =
  function
    | `Simple (l, e) -> `Simple (l, f aux e)
    | `Complex (l, p) -> `Complex (l, List.map (f aux) p)
    | `Special (l, i, p) -> `Special (l, i, List.map (f aux) p)

let map_sel_param f aux =
  function
    | `Always (l, e) -> `Always (l, f aux e)
    | `Special (l, cl, e) 
       -> let g =
            function
              | `Flag _ as flag -> flag
              | `NonCond (l, s, t, e) -> `NonCond (l, s, t, f aux e)
              | `Cond (l, s, t, p, e) -> `Cond (l, s, t, Option.map (f aux) p, f aux e)
          in
          `Special (l, List.map g cl, f aux e)

let rec unary_to_logop l =
  function
    | `LogOp (l, a, op, b) -> `LogOp (l, a, reverse_cond op, b)
    | `Unary (l, `Not, e) -> conditional_unit e
    | e -> `LogOp (l, e, `Equ, `Int (nowhere, 0l))

and conditional_unit =
  function
    | (`LogOp _ | `AndOr _) as c -> c
    | `Unary (l, `Not, e) -> unary_to_logop l (conditional_unit e) (* TODO: check: is this correct? *)
    | `Parens (l, c) -> `Parens (l, conditional_unit c)
    | (`VarOrFn (_, _, t) | `Deref (_, _, t, _) | `Func (_, _, t, _, None)) as fc
      when Memory.defined t || Intrinsic.is_builtin t -> conditional_unit (expr_disambiguate fc)
    | c -> `LogOp (loc_of_expr c, c, `Neq, `Int (nowhere, 0l))

let rec equal_branch =
  function
    | `Int (_, i), `Int (_, j) -> i = j
    | `Store _, `Store _ -> true
    | `Op (_, a, o, b), `Op (_, c, p, d) -> o = p && equal_branch (a, c) && equal_branch (b, d)
    | `LogOp (_, a, o, b), `LogOp (_, c, p, d) -> o = p && equal_branch (a, c) && equal_branch (b, d)
    | `AndOr (_, a, o, b), `AndOr (_, c, p, d) -> o = p && equal_branch (a, c) && equal_branch (b, d)
    | `Unary (_, o, i), `Unary (_, p, j) -> o = p && equal_branch (i, j)
    | `IVar (_, i, a), `IVar (_, j, b)
    | `SVar (_, i, a), `SVar (_, j, b) -> i = j && equal_branch (a, b)
    | `Parens (_, a), b | a, `Parens (_, b) -> equal_branch (a, b)
    | _ -> false

let rec type_of_traversed =
  function
    | `Int _ | `IVar _ | `Store _ | `ExFunc _ -> `Int
    | `IStr _ | `Res _ | `SVar _ | `SChain _ -> `Str
    | `FuncCall (_, e, _, _, _, _)
    | `Parens (_, e)
    | `Unary (_, _, e)
    | `Op (_, e, _, _)
    | `LogOp (_, e, _, _)
    | `AndOr (_, e, _, _) -> type_of_traversed e
    | `Func (l, s, _, _, _) -> ksprintf (error l) "`%s' is not a function valid in expressions" s

let loc_of_traversed =
  function
    | `Int (l, _) | `IVar (l, _, _) | `Store l | `ExFunc (l, _)
    | `IStr (l, _) | `Res (l, _) | `SVar (l, _, _) | `SChain (l, _)
    | `FuncCall (l, _, _, _, _, _)
    | `Unary (l, _, _)
    | `Op (l, _, _, _)
    | `LogOp (l, _, _, _)
    | `AndOr (l, _, _, _)
    | `Parens (l, _) -> l
    | `Func (l, s, _, _, _) -> ksprintf (error l) "`%s' is not a function valid in expressions" s

(* Utilities for generated code *)

let last_store_ref { pre_code = c } =
  let rec loop n =
    if n < 0 then None
    else match DynArray.get c n with
      | _, `FuncCall (_, `Store _, _, _, _, _)
      | _, `Select (_, `Store _, _, _, _, _) -> Some n
      | _ -> loop (n - 1)
  in
  loop (DynArray.length c - 1)

let add_store_func aux last_ref loc rv =
  begin match last_ref with
    (* If STORE is actually used in the expression and this has not been
       rectified before, give those uses temporary variables *)
    | None -> if Lazy.force aux.contains_store then
                let ti = Memory.get_temp_int () in
                assert (aux.redef_store = None);
                aux.redef_store <- Some ti
    (* Alternatively, if STORE has been used before, give the previous
       reference a temporary variable instead *)
    | Some n ->
        let tv = Memory.get_temp_int () in
        match DynArray.get aux.pre_code n with
          | id, `FuncCall (l, `Store _, s, t, p, lbl)
              -> Hashtbl.replace aux.tempvars id tv;
                 DynArray.set aux.pre_code n (id, `FuncCall (l, tv, s, t, p, lbl))
          | id, `Select (l, `Store _, s, i, w, p)
              -> Hashtbl.replace aux.tempvars id tv;
                 DynArray.set aux.pre_code n (id, `Select (l, tv, s, i, w, p))
          | _ -> assert false
  end;
  (* Either way, lift the current function out to modify STORE *)
  let id = aux.get_id () in
  DynArray.add aux.pre_code (id, rv);
  Hashtbl.add aux.tempvars id (`Store Memory.temploc);
  `ExFunc (loc, id)

let add_int_of_conditional { pre_code = c } cond =
  let tv = Memory.get_temp_int ()
  and lb = Codegen.unique_label nowhere in
  DynArray.add c (-1, `Assign (nowhere, tv, `Set, `Int (nowhere, 0l)));
  DynArray.add c (-1, `ProcCall (nowhere, "goto_unless", Text.ident "goto_unless", [`Simple (nowhere, cond)], Some lb));
  DynArray.add c (-1, `Assign (nowhere, tv, `Set, `Int (nowhere, 1l)));
  DynArray.add c (-1, `Label lb);
  tv


(* Functions to traverse and normalise an expression *)

let rec transform ?(as_cond = false) ~reject e =
  let aux = make_transform_aux e in
  let e = traverse aux (if as_cond then conditional_unit e else e) ~as_cond ~reject in
  e, aux

and traverse ?(as_cond = false) ?(keep_unknown_funcs = false) ~reject aux =
  function
   (* Return atomic elements unchanged *)
    | `Int (l, i)
       -> if reject = `Int then error l "type mismatch: integer in string expression"; `Int (l, i)
    | `Store l
       -> if reject = `Int then error l "type mismatch: integer in string expression"; `Store l
   (* Handle string literals *)
    | `Str (l, s)
       -> if reject = `Str then error l "type mismatch: string constant in integer expression";
          `IStr (l, traverse_str_tokens aux s)
   (* Expand resource strings *)
    | `Res (l, t)
       -> if reject = `Str then error l "type mismatch: #res<> in integer expression";
          let t, l = Global.get_resource l (Text.to_err t, t) in
          traverse aux (`Str (l, t)) ~as_cond ~keep_unknown_funcs ~reject
   (* Remove superfluous parentheses *)
    | `Parens (l, e)
       ->(match traverse aux e ~as_cond ~reject with
            | (`Store _ | `IStr _ | `Int _ | `IVar _ | `SVar _ | `ExFunc _) as atom -> atom
            | `Parens (_, e) | e -> if reject = `Int then e else `Parens (l, e))
   (* Nothing of great interest happens to memory references *)
    | `SVar (l, i, e)
       -> if reject = `Str then error l "type mismatch: string variable in integer expression";
          `SVar (l, i, traverse aux e ~reject:`Str)
    | `IVar (l, i, e)
       -> if reject = `Int then error l "type mismatch: integer variable in string expression";
          `IVar (l, i, traverse aux e ~reject:`Str)
   (* Disambiguate identifiers *)
    | (`Deref _ | `VarOrFn _) as vof
       -> traverse aux (expr_disambiguate vof) ~reject
   (* Move function calls out of expressions *)
    | `Func (l, s, t, parms, label as f) as fc
       -> (* TODO: ultimately we want to implement user functions. *)
          if Memory.defined t || Intrinsic.is_builtin t
          then traverse aux (expr_disambiguate fc) ~reject
          else traverse_func aux f ~reject ~keep_unknown_funcs
    | `SelFunc f
       -> traverse_select aux f ~reject
   (* Fold constants (for trivial cases only - we don't simplify things like "1 + x + 1") *)
    | `Op (l, a, op, b)
       ->(let simple_a = traverse aux a ~reject in
          if type_of_traversed simple_a = `Int then
          (* Integer operations *)
            let simple_b = traverse aux b ~reject:`Str in
            match op, simple_a, simple_b with
             (* Detect errors *)
              | (`Div | `Mod), _, `Int (_, 0l) -> error l "division by zero"
             (* Eliminate meaningless operations *)
              | `And, e, `Int (_, -1l)          | `And, `Int (_, -1l), e
              | (`Or  | `Xor), e, `Int (_, 0l)  | (`Or  | `Xor), `Int (_, 0l), e
              | (`Add | `Sub), e, `Int (_, 0l)  | `Add, `Int (_, 0l), e
              | (`Mul | `Div), e, `Int (_, 1l)  | `Mul, `Int (_, 1l), e -> e
              | (`And | `Mul), _, `Int (_, 0l)  | (`And | `Mul | `Div | `Mod), `Int (_, 0l), _ -> `Int (l, 0l)
              | (`And | `Or), i, j when equal_branch (i, j) -> i
              | (`Sub | `Xor | `Mod), i, j when equal_branch (i, j) -> `Int (l, 0l)
              | `Div, i, j when equal_branch (i, j) -> `Int (l, 1l)
             (* Fold constants *)
              | _, `Int (_, i), `Int (_, j) -> `Int (l, apply_arith i j op)
             (* Simplify expressions involving unary minus *)
              | `Add, _, `Int (lb, i) when i < 0l -> `Op (l, simple_a, `Sub, `Int (lb, Int32.neg i))
              | `Sub, _, `Int (lb, i) when i < 0l -> `Op (l, simple_a, `Add, `Int (lb, Int32.neg i))
              | `Add, _, `Unary (_, `Sub, i) -> `Op (l, simple_a, `Sub, i)
              | `Sub, _, `Unary (_, `Sub, i) -> `Op (l, simple_a, `Add, i)
              | (`Div | `Mul), `Unary (_, `Sub, i), `Unary (_, `Sub, j) -> `Op (l, i, op, j)
             (* Return ops for other cases *)
              | _ -> let a =
                       match simple_a with
                         | `Op (_, _, aop, _) when prec aop < prec op -> `Parens (l, simple_a)
                         | _ -> simple_a
                     and b =
                       match simple_b with
                         | `Op (_, _, bop, _) when prec bop <= prec op -> `Parens (l, simple_b)
                         | _ -> simple_b
                     in
                     `Op (l, a, op, b)
          else
          (* String operations *)
            let simple_b = traverse aux b ~reject:`Int in
            if op <> `Add then ksprintf (error l) "invalid operator `%s' in string expression" (string_of_op op);
            match simple_a, simple_b with
              | `IStr (_, i), `IStr (_, j) -> let n = DynArray.copy i in DynArray.append j n; `IStr (l, n)
              | `IStr (_, i), e | e, `IStr (_, i) when DynArray.length i = 0 -> e
             (* Special cases for select_* functions *)
              | `SChain (l, a), `SChain (_, b) -> `SChain (l, a @ b)
              | a, `SChain (_, b) -> `SChain (loc_of_traversed a, a :: b)
              | `SChain (l, a), b -> `SChain (l, a @ [b])
              | a, b -> `SChain (loc_of_traversed a, [a; b])
          (* end of `Op *))
    | `Unary (l, op, e)
       ->(if reject = `Int then ksprintf (error l) "invalid operator `%s' in string expression" (string_of_op op);
          match op, traverse aux e ~reject:`Str with
            | _, `Int (_, i) -> `Int (l, apply_unary i op)
            | _, `Unary (_, op', e) when op' = op -> e
            | `Sub, e
               -> let e = if (e matches `Op _) then `Parens (l, e) else e in
                  `Unary (l, `Sub, e)
            | `Inv, e 
               -> let e =
                    match e with 
                      | `Op (_, _, op, _) when prec op < prec `Xor -> `Parens (l, e)
                      | _ -> e
                  in
                  `Op (l, e, `Xor, `Int (nowhere, -1l))
            | `Not, `Unary (_, `Sub, e)
            | `Not, e -> let rv = `LogOp (l, e, `Equ, `Int (nowhere, 0l)) in
                         if as_cond then rv else add_int_of_conditional aux rv
          (* end of `Unary *))
   (* Potentially lift out logical and boolean operators *)
    | `LogOp (l, a, op, b)
       ->(if reject = `Int then  ksprintf (error l) "invalid operator `%s' in string expression" (string_of_op op);
          let simple_b = traverse aux b ~reject:`None in
          if type_of_traversed simple_b = `Int then
            let simple_a = traverse aux a ~reject:`Str in
            match op, simple_a, simple_b with
              | _, `Int (_, i), `Int (_, j) -> `Int (l, apply_cond i j op)
              | (`Equ | `Gte | `Lte), i, j when equal_branch (i, j) -> `Int (l, 1l)
              | (`Neq | `Gtn | `Ltn), i, j when equal_branch (i, j) -> `Int (l, 0l)
              | _ -> let rv = `LogOp (l, simple_a, op, simple_b) in
                     if as_cond then rv else add_int_of_conditional aux rv
          else
            (* String comparisons. *)
            let simple_a = traverse aux a ~reject:`Int in
            let v = 
              add_store_func aux (last_store_ref aux) nowhere
                (`FuncCall (nowhere, `Store nowhere, "strcmp", Text.ident "strcmp",
                   [`Simple (nowhere, simple_a); `Simple (nowhere, simple_b)], None))
            in
            if op = `Neq && not as_cond then
              v
            else
              let rv = `LogOp (l, v, op, `Int (nowhere, 0l)) in
              if as_cond then rv else add_int_of_conditional aux rv
          (* end of `LogOp *))
    | `AndOr (l, a, op, b)
       ->(if reject = `Int then ksprintf (error l) "invalid operator `%s' in string expression" (string_of_op op);
          let simple_a = traverse aux (conditional_unit a) ~as_cond:true ~reject:`Str in
          let simple_b = traverse aux (conditional_unit b) ~as_cond:true ~reject:`Str in
          let return rv =
            match rv with
              | `LogOp _ | `AndOr _ when not as_cond -> add_int_of_conditional aux rv
              | _ -> rv
          in
          match op, simple_a, simple_b with
            | `LAnd, `Int (_, i), `Int (_, j) -> if i <> 0l && j <> 0l then `Int (l, 1l) else `Int (l, 0l)
            | `LAnd, e, `Int (_, i) | `LAnd, `Int (_, i), e when i <> 0l -> return e
            | `LAnd, e, `Int (_, i) | `LAnd, `Int (_, i), e when i = 0l -> `Int (l, 0l)
            | `LOr, `Int (_, i), `Int (_, j) -> if i <> 0l || j <> 0l then `Int (l, 1l) else `Int (l, 0l)
            | `LOr, e, `Int (_, i)  | `LOr, `Int (_, i), e when i = 0l -> return e
            | `LOr, e, `Int (_, i)  | `LOr, `Int (_, i), e when i <> 0l -> `Int (l, 1l)
            | _, i, j when equal_branch (i, j) -> return i
            | _ -> (* It appears that && and || have equal precedence.  This should fix the
                      discrepancy with Kepago, I hope. *)
                   let a =
                     match simple_a with
                       | `AndOr (_, _, `LAnd, _) when op = `LOr -> `Parens (l, simple_a)
                       | _ -> simple_a
                   and b =
                     match simple_b with
                       | `AndOr _ when op = `LOr -> `Parens (l, simple_b)
                       | _ -> simple_b
                   in
                   return (`AndOr (l, a, op, b))
          (* end of `AndOr *))
    | `ExprSeq (l, id, defs, smts)
       -> (*FIXME: This evaluates the contents of the sequence: it has to do so in order that 
            constant return values can be treated as constants.  This MAY cause subtle bugs, so 
            maybe this needs rethinking... *)
          (*TODO: The "correct" behaviour would be for evaluation to halt as soon as anything is
            encountered that alters any state whatsoever outside the sequence, and everything
            then to be added to aux.pre_code instead. *)
          if DynArray.empty smts then ksprintf (error l) "inline block `%s' expands to empty sequence: this is invalid in expressions" (Text.to_sjs id);
          let last = DynArray.last smts in
          DynArray.delete_last smts;
          assert (Memory.defined id);
          let sym = Memory.pull_sym id in
          Memory.open_scope ();
          Memory.define (Text.ident "__INLINE_CALL__") (`Macro (`Int (nowhere, 1l))) ~scoped:true;
          Memory.define (Text.ident "__CALLER_FILE__") (`Macro (`Str (nowhere, Global.dynArray (`Text (nowhere, `Sbcs, Text.of_err l.file))))) ~scoped:true;
          Memory.define (Text.ident "__CALLER_LINE__") (`Macro (`Int (nowhere, Int32.of_int l.line))) ~scoped:true;
          List.iter (fun (i, e) -> Memory.define i (`Macro e) ~scoped:true) defs;
          Meta.parse smts;
          let rv = traverse aux (expr_of_statement last) ~as_cond ~reject ~keep_unknown_funcs in
          Memory.close_scope ();
          Memory.replace_sym id sym;
          rv

and traverse_str_tokens aux tkns =
  let rv = DynArray.create () in
  DynArray.iter
    begin function
      | `Code (l, i, e, p) when i = Text.ident "s"
         -> if e <> None then KeTypes.error l "length specifier invalid in \\s{}";
            let add_str_elt =
              function
                | `SVar _ as sv -> DynArray.add rv (`Code (l, Text.ident "s", None, [`Simple (l, sv)]))
                | `IStr (_, elts) -> DynArray.append elts rv
                | _ -> assert false
            in
            (match List.map (map_func_param (traverse ~reject:`None) aux) p with
              (* Expand \s{"string"} by lifting out its contents. *)
              | [`Simple (_, `SChain (_, c))] -> List.iter add_str_elt c
              | [`Simple (_, (`SVar _ | `IStr _ as elt))] -> add_str_elt elt
              | p -> DynArray.add rv (`Code (l, i, None, p)))
      | `Code (l, i, e, p)
         -> let args = List.map (map_func_param (traverse ~reject:`None) aux) p in
            if i = Text.ident "i"
            && Option.default true (Option.map normalised_expr_is_const e)
            && (args matches [`Simple (_, `Int (_, v))]) then
              (* Expand \i{const} by converting to string *)
              let s =
                Text.of_sjs
                  (Global.int32_to_string_padded
                    (Int32.to_int (Option.default 0l (Option.map int_of_normalised_expr e)))
                    (match args with [`Simple (_, `Int (_, v))] -> v | _ -> assert false))
              in
              DynArray.add rv (`Text (l, `Sbcs, s))
            else
              DynArray.add rv (`Code (l, i, Option.map (traverse aux ~reject:`Str) e, args))
      | `Gloss (l, g, s, r)
         -> let rl, rs =
              match r with
                | `Closed rl_rs -> rl_rs
                | `ResStr (kl, k) -> let a, b = Global.get_resource kl (Text.to_sjs k, k) in b, a
            in
            DynArray.add rv (`Gloss (l, g, traverse_str_tokens aux s, `Closed (rl, traverse_str_tokens aux rs)))
      | `Name (l, lg, e, w)
         -> DynArray.add rv (`Name (l, lg, traverse aux e ~reject:`Str, Option.map (traverse aux ~reject:`Str) w))
      | #strtoken_non_expr as e -> DynArray.add rv e
    end
    tkns;
  rv


and traverse_func ~keep_unknown_funcs ~reject aux (l, s, t, p, label) =
  if Intrinsic.is_builtin t then error l "internal error: intrinsic call reached Expr.traverse_func";
  match
    try
      Some (ver_fun "" (Hashtbl.find_all functions t))
    with Not_found ->
      if keep_unknown_funcs
      then None
      else ksprintf (error l) "unable to find an appropriate definition for the function `%s'" s
  with
   (* No prototype found: assume this is a special. *)
    | None -> `Func (l, s, t, List.map (map_func_param (traverse ~reject:`None) aux) p, label)
   (* Found a prototype: treat as a normal function call. *)
    | Some f ->
  (* Find out whether the function in question modifies a variable directly, or
     whether it sets STORE instead. *)
  let argc = List.length p + if List.mem KfnTypes.PushStore f.flags then 0 else 1 in
  let overload = FuncAsm.choose_overload l f argc in
  let prototype = f.prototypes.(overload) in
  let return_as =
    try
      match
        List.find
          (fun (_, pattrs) -> List.mem KfnTypes.Return pattrs)
          (Option.get prototype)
      with
        | KfnTypes.Int, _
            -> if reject = `Int
               then ksprintf (error l) "type mismatch: function `%s' returns an integer, but is here used in a string expression" s
               else `Int
        | KfnTypes.Str, _
            -> if reject = `Str
               then ksprintf (error l) "type mismatch: function `%s' returns a string, but is here used in an integer expression" s
               else `Str
        | _ -> ksprintf Optpp.sysError "error in reallive.kfn: invalid return type for function `%s'" s
    with _ ->
      if List.mem KfnTypes.PushStore f.flags
      then `Store
      else ksprintf (error l) "function `%s' has no return value: it cannot be used in expressions" s
  in
  (* Determine whether a previous function in the current expression
     has made use of STORE *)
  let last_ref = last_store_ref aux in
  (* If it has parameters, transform expressions in them *)
  let as_cond = List.mem KfnTypes.IsCond f.flags in
  let p = 
    if as_cond 
    then List.map (function `Simple (l, e) -> `Simple (l, conditional_unit e) | x -> x) p
    else p 
  in
  let p = List.map (map_func_param (traverse ~keep_unknown_funcs:true ~reject:`None ~as_cond) aux) p in
  (* Handle return value according to type *)
  match return_as with
    | `Store
     -> add_store_func aux last_ref l (`FuncCall (l, `Store nowhere, s, t, p, label))

    | `Int
     -> let id = aux.get_id () in
        let tv, rv =
          if last_ref = None && not (Lazy.force aux.contains_store) then
            (* TODO: check: is this correct? *)
            (Hashtbl.add aux.tempvars id (`Store Memory.temploc);
            `Store Memory.temploc, `ExFunc (l, id))
          else
            let tv = Memory.get_temp_int () in
            tv, tv
        in
        DynArray.add aux.pre_code (id, `FuncCall (l, tv, s, t, p, label));
        rv

    | `Str
     -> let tv = Memory.get_temp_str () in
        DynArray.add aux.pre_code (-1, `FuncCall (l, tv, s, t, p, label));
        tv

and traverse_select ~reject aux (l, s, i, w, p) =
  if reject = `Int then
    ksprintf (error l) "type mismatch: function `%s' returns an integer, but is here used in a string expression" s;
  (* Determine whether a previous function in the current expression
     has made use of STORE *)
  let last_ref = last_store_ref aux in
  (* Transform expressions in parameters *)
  let w = Option.map (traverse aux ~reject:`Str) w
  and p =
    List.map
      (function
        | `Always (ll, e)
           -> `Always (ll, traverse aux e ~reject:`Int)
        | `Special (ll, cl, e)
           -> let f =
                function
                  | `Flag _ as flag -> flag
                  | `NonCond (l, s, t, e) 
                     -> `NonCond (l, s, t, traverse aux e ~reject:`Str)
                  | `Cond (l, s, t, e, c) 
                     -> `Cond (l, s, t, Option.map (traverse aux ~reject:`Str) e,
                                        traverse aux (conditional_unit c) ~reject:`Str ~as_cond:true)
              in
              `Special (ll, List.map f cl, traverse aux e ~reject:`Int))
    p
  in
  (* Handle return value *)
  add_store_func aux last_ref l (`Select (l, `Store nowhere, s, i, w, p))

(* Function taking the results of a transformation and returning a valid expression *)
let rec finalise aux : 'inferred -> expression =
  function
    | `Store _ as s -> (match aux.redef_store with Some tempvar -> (tempvar :> expression) | None -> s)
    | `IStr (l, s) -> `Str (l, DynArray.map (finalise_str_tokens aux) s)
    | `Int _ as i -> (i :> expression)
    | `IVar (l, i, e) -> `IVar (l, i, finalise aux e)
    | `SVar (l, i, e) -> `SVar (l, i, finalise aux e)
    | `Parens (l, e) -> `Parens (l, finalise aux e)
    | `Unary (l, op, e) -> `Unary (l, op, finalise aux e)
    | `Op    (l, a, op, b) -> `Op    (l, finalise aux a, op, finalise aux b)
    | `LogOp (l, a, op, b) -> `LogOp (l, finalise aux a, op, finalise aux b)
    | `AndOr (l, a, op, b) -> `AndOr (l, finalise aux a, op, finalise aux b)
    | `Func (l, s, t, p, d) -> `Func (l, s, t, List.map (map_func_param finalise aux) p, d)
    | `ExFunc (_, id) -> (try (Hashtbl.find aux.tempvars id :> expression) with _ -> assert false)
    | `SChain (l, chain)
       -> let s = DynArray.create () in
          List.iter
            (function
              | `IStr (_, t) -> DynArray.append (DynArray.map (finalise_str_tokens aux) t) s
              | `SVar (l, i, e)
                 -> DynArray.add s (`Code (l, Text.of_sjs "s", None, [`Simple (l, `SVar (l, i, finalise aux e))]));
              | _ -> error l "expected string")
            chain;
          `Str (l, s)

and finalise_str_tokens aux =
  function
    | `Code (l, i, e, p) -> `Code (l, i, Option.map (finalise aux) e, List.map (map_func_param finalise aux) p)
    | `Gloss (l, g, s, `Closed (cl, r))  -> `Gloss (l, g, DynArray.map (finalise_str_tokens aux) s, `Closed (cl, DynArray.map (finalise_str_tokens aux) r))
    | `Gloss (_, _, _, `ResStr _) -> assert false
    | `Name (l, lg, e, cidx) -> `Name (l, lg, finalise aux e, Option.map (finalise aux) cidx)
    | #strtoken_non_expr as e -> e



let finalise_generated_code aux (_, s) : statement =
  match s with
    | `Assign (l, d, op, e)  -> `Assign (l, d, op, finalise aux e)
    | `ProcCall (l, s, t, p, lbl) -> `FuncCall (l, None, s, t, List.map (map_func_param finalise aux) p, lbl)
    | `FuncCall (l, d, s, t, p, lbl) -> `FuncCall (l, Some d, s, t, List.map (map_func_param finalise aux) p, lbl)
    | `Select (l, d, s, i, w, p) -> `Select (l, d, s, i, Option.map (finalise aux) w, List.map (map_sel_param finalise aux) p)
    | (`Label _) as a -> a


(* Function taking a potentially complex assignment statement and returning a list of statements
   representing valid RealLive code *)
let normalise_assignment (`Assign (loc, dest, op, e): assignment) =
  let dest, is_int, reject =
    let rec disambiguate =
      function
        | `Store _ as elt -> elt, true, `Str
        | `IVar _  as elt -> elt, true, `Str
        | `SVar _ as elt -> elt, false, `Int
        | `Func (l, s, t, p, d) when Intrinsic.is_builtin t -> disambiguate (Intrinsic.eval_as_expr (l, s, t, p, d))
        | `Deref (l, s, t, offset)
            -> disambiguate 
                 (try Memory.get_deref_as_expression ~loc:l ~s t offset with Not_found ->
                   ksprintf (error l) "undeclared identifier `%s'" s)
        | `VarOrFn (l, s, t)
            -> disambiguate
                 (if Intrinsic.is_builtin t then
                    Intrinsic.eval_as_expr (l, s, t, [], None)
                  else try
                    if not (Intrinsic.is_builtin t) then ignore (ver_fun "" (Hashtbl.find_all functions t));
                    ksprintf (error l) "function `%s' is not valid as the left-hand side of an assignment" s
                  with Not_found ->
                    try Memory.get_as_expression t with Not_found ->
                    (* TODO: ultimately we want to issue a different error message for user functions *)
                    ksprintf (error l) "undeclared identifier `%s'" s)
        | _ -> error loc "the left-hand side of an assignment must be a variable or memory reference"
    in
    disambiguate (dest :> expression)
  in
  let e, aux = transform e ~reject in
  let dest = traverse aux dest ~reject in
  let e = match finalise aux e with `Parens (_, e) | e -> e in
  let dest =
    match finalise aux dest with
      | #assignable as a -> a
      | _ -> error loc "internal error: LHS normalised to non-assignable"
  in
  let d = DynArray.map (finalise_generated_code aux) aux.pre_code in
  Option.may (fun ti -> DynArray.insert d 0 (`Assign (nowhere, ti, `Set, `Store nowhere))) aux.redef_store;
  let result =
    if is_int then
      match e, try DynArray.last d with _ -> `Null with
        | `Store _, `Assign (_, `Store _, `Set, expr)
          -> `Replace (`Assign (loc, dest, op, expr))
        | `Store _, `FuncCall (l, Some (`Store _), s, t, p, lbl)
          when op = `Set
          -> `Replace (`FuncCall (l, Some dest, s, t, p, lbl))
        | `Store _, `Select (l, `Store _, s, i, w, p)
          when op = `Set
          -> `Replace (`Select (l, dest, s, i, w, p))
        | `Store _, _
          when op = `Set && is_store dest
          -> `Neither
        | _ 
          when op = `Set && equal_branch (e, dest)
          -> `Neither
        | _
          -> match e with
               | `Op (_, a, op, b) when equal_branch (a, dest) -> `Append (`Assign (loc, dest, (op :> assign_op), b))
               | _ -> `Append (`Assign (loc, dest, op, e))
    else
      match e, try DynArray.last d with _ -> `Null with
        | `SVar (sl1, _, _), `FuncCall (l, Some (`SVar (sl2, _, _)), s, t, p, lbl)
          when op = `Set && sl1 = Memory.temploc && sl1 = sl2
            -> `Replace (`FuncCall (l, Some dest, s, t, p, lbl))
        | _
          -> let dest = match dest with `SVar _ as sv -> (sv :> expression) | _ -> assert false in
             (* Handle non-initial occurrences of lval on RHS *)
             let e, op =
               match e with 
                 | `Str (loc, text) when DynArray.length text > 1
                    -> let tempvar = ref None in
                       let f =
                         function
                           | `Simple (l, e) when equal_exprs e dest 
                              -> let v =
                                   match !tempvar with
                                     | Some v -> v
                                     | None -> let v = Memory.get_temp_str () ~useloc:l in 
                                               tempvar := Some v; 
                                               v
                                 in `Simple (l, v)
                           | p -> p
                       in
                       let g =
                         function 
                           | `Code (l, s, o, parms) -> `Code (l, s, o, List.map f parms)
                           | elt -> elt
                       in
                       let chop =
                         if op = `Set 
                         && (match DynArray.get text 0 with `Code (_, _, _, [`Simple (_, e)]) -> equal_exprs e dest | _ -> false) 
                         then (DynArray.delete text 0; true)
                         else false
                       in
                       let ntext = DynArray.map g text in
                       (match !tempvar with
                         | None -> ()
                         | Some v -> DynArray.insert d 0
                                       (`FuncCall (loc, None, "strcpy", Text.ident "strcpy", 
                                         [`Simple (loc, v); `Simple (loc, dest)], None)));
                       `Str (loc, ntext), if chop then `Add else op
                 | _ -> e, op
             in
             (* Replace with a function call. *)
             if op = `Set then
               if equal_branch (dest, e)
               then `Neither
               else `Append (`FuncCall (loc, None, "strcpy", Text.ident "strcpy", [`Simple (loc, dest); `Simple (loc, e)], None))
             else if op = `Add then
               `Append (`FuncCall (loc, None, "strcat", Text.ident "strcat", [`Simple (loc, dest); `Simple (loc, e)], None))
             else
               ksprintf (error loc) "assignment operator `%s' is not valid for strings" (string_of_assign_op op)
  in
  match result with
    | `Replace smt -> DynArray.set d (DynArray.length d - 1) smt; d
    | `Append smt -> DynArray.add d smt; d
    | `Neither -> d


(* As normalise_assignment, but for function calls *)
let normalise_funccall (`FuncCall (loc, dest, s_ident, t_ident, params, label)) =
  assert (dest matches None | Some (`Store _));
  let aux = make_transform_aux (`Func (loc, s_ident, t_ident, params, label)) in
  let as_cond =
    try
      List.mem KfnTypes.IsCond (ver_fun "" (Hashtbl.find_all functions t_ident)).flags
    with Not_found -> 
      false
  in
  let p = 
    if as_cond 
    then List.map (function `Simple (l, e) -> `Simple (l, conditional_unit e) | x -> x) params
    else params 
  in
  let p = List.map (map_func_param (traverse ~keep_unknown_funcs:true ~reject:`None ~as_cond) aux) p in
  DynArray.add aux.pre_code (-1, `ProcCall (loc, s_ident, t_ident, p, label));
  let d = DynArray.map (finalise_generated_code aux) aux.pre_code in
  Option.may (fun ti -> DynArray.insert d 0 (`Assign (nowhere, ti, `Set, `Store nowhere))) aux.redef_store;
  d

let normalise_unknown (`UnknownOp (loc, fn, overload, params)) =
  let aux =
    { (make_transform_aux (`Store nowhere))
      with
        contains_store = lazy (exists_in_expr is_store (`Func (loc, fn.ident, Text.of_arr [||], params, None))) }
  in
  let p = List.map (map_func_param (traverse ~keep_unknown_funcs:true ~reject:`None) aux) params in
  let p = List.map (map_func_param finalise aux) p in
  let d = DynArray.map (finalise_generated_code aux) aux.pre_code in
  Option.may (fun ti -> DynArray.insert d 0 (`Assign (nowhere, ti, `Set, `Store nowhere))) aux.redef_store;
  DynArray.add d (`UnknownOp (loc, fn, overload, p));
  d  

let normalise_select (`Select (loc, dest, s_ident, opcode, window, params)) =
  let aux = make_transform_aux (`SelFunc (loc, s_ident, opcode, window, params)) in
  let w = Option.map (traverse aux ~reject:`Str) window
  and p =
    List.map
      (function
        | `Always (ll, e)
           -> `Always (ll, traverse aux e ~reject:`Int)
        | `Special (ll, cl, e)
           -> let f =
                function
                  | `Flag _ as flag -> flag
                  | `NonCond (l, s, t, e) -> `NonCond (l, s, t, traverse aux e ~reject:`Str)
                  | `Cond (l, s, t, e, c) -> `Cond (l, s, t, Option.map (traverse aux ~reject:`Str) e,
                                                             traverse aux (conditional_unit c) ~reject:`Str ~as_cond:true)
              in
              `Special (ll, List.map f cl, traverse aux e ~reject:`Int))
    params
  in
  DynArray.add aux.pre_code (-1, `Select (loc, dest, s_ident, opcode, w, p));
  let d = DynArray.map (finalise_generated_code aux) aux.pre_code in
  Option.may (fun ti -> DynArray.insert d 0 (`Assign (nowhere, ti, `Set, `Store nowhere))) aux.redef_store;
  d


(* As normalise_assignment, but for goto_case calls *)
let normalise_gotocase (`GotoCase (loc, jump_type, e, cases)) =
  let aux =
    let all_exprs = e :: List.filter_map (function `Default _ -> None | `Match (e, _) -> Some e) cases in
    { (make_transform_aux (`Store nowhere))
      with
        contains_store = lazy (List.exists (exists_in_expr is_store) all_exprs) }
  in
  let e = traverse aux e ~reject:`Str in
  let cases =
    List.map
      (function
        | `Default _ as d -> d
        | `Match (e, l) -> `Match (traverse aux e ~reject:`Str, l))
      cases
  in
  let e = match finalise aux e with `Parens (_, e) | e -> e in
  let cases =
    List.map
      (function
        | `Default _ as d -> d
        | `Match (e, l) -> `Match ((match finalise aux e with `Parens (_, e) | e -> e), l))
      cases
  in
  let d = DynArray.map (finalise_generated_code aux) aux.pre_code in
  Option.may (fun ti -> DynArray.insert d 0 (`Assign (nowhere, ti, `Set, `Store nowhere))) aux.redef_store;
  DynArray.add d (`GotoCase (loc, jump_type, e, cases));
  d

(* As normalise_assignment, but for other statements with expressions in *)
let normalise_nonassignment s =
  let e, aux =
    match s with
      | `GotoOn (_, _, e, _) -> transform e ~reject:`Str
      | `Return (_, _, e) -> transform e ~reject:`None
      | `LoadFile (_, e) -> transform e ~reject:`Int
      | `Directive (_, _, tp, e) -> transform e ~reject:(if tp = `Str then `Int else if tp = `Int then `Str else `None);
      | `DConst (_, _, _, _, e) -> transform e ~reject:`None
  in
  let e = match finalise aux e with `Parens (_, e) | e -> e in
  let d = DynArray.map (finalise_generated_code aux) aux.pre_code in
  Option.may (fun ti -> DynArray.insert d 0 (`Assign (nowhere, ti, `Set, `Store nowhere))) aux.redef_store;
  DynArray.add d
    begin match s with
      | `GotoOn (l, jt, _, lb) -> `GotoOn (l, jt, e, lb)
      | `Return (l, b, _) -> `Return (l, b, e)
      | `LoadFile (l, _) -> `LoadFile (l, e)
      | `Directive (l, s, tp, _) -> `Directive (l, s, tp, e)
      | `DConst (l, s, t, tp, _) -> `Define (l, s, t, tp = `Bind, e)
    end;
  d


(* Generic normalisation: takes any statement and returns a normalised form.
   NOTE: If normalisation was performed, a new scope is opened but NOT
   closed; the caller must close it when finished. *)
let normalise =
  let call f e =
    Memory.open_scope ();
    let m = f e in
    if DynArray.length m = 0
    then (Memory.close_scope (); `Nothing)
    else `Multiple m
  in
  function
    | #assignment as a -> call normalise_assignment a
    | `FuncCall (_, (None | Some (`Store _)), _, _, _, _) as p -> call normalise_funccall p
    | `FuncCall (l, Some rv, s, t, p, d) -> call normalise_assignment (`Assign (l, rv, `Set, `Func (l, s, t, p, d)))
    | `UnknownOp _ as u -> call normalise_unknown u
    | `GotoCase _ as g -> call normalise_gotocase g
    | `Select   _ as s -> call normalise_select s
    | `DConst (_, _, _, (`Bind | `EBind), _)
    | #normalisable as n -> call normalise_nonassignment n
    | #statement as elt -> `Single elt


(* Expression-level normalisation, for constant expressions. *)
let normalise_and_get_const ?(abort_on_fail = true) ?(expect = `None) e =
  let fail = `Op (nowhere, `Store nowhere, `Add, `Store nowhere) in (* non-constant expression to force a failure *)
  let pre_len = DynArray.length Codegen.Output.bytecode in
  let e, aux = transform e ~reject:`None in
  let e =
    if DynArray.length aux.pre_code <> 0 then fail else finalise aux e
  in
  let e =
    if DynArray.length Codegen.Output.bytecode > pre_len then (
      DynArray.delete_range Codegen.Output.bytecode pre_len (DynArray.length Codegen.Output.bytecode - pre_len);
      fail
    ) else e    
  in
  const_of_normalised_expr e ~expect ~abort_on_fail

let normalise_and_get_int ?(abort_on_fail = true) e =
  match normalise_and_get_const e ~expect:`Int ~abort_on_fail with
    | `Integer i -> i
    | _ -> assert false

let normalise_and_get_str ?(abort_on_fail = true) e =
  match normalise_and_get_const e ~expect:`Str ~abort_on_fail with
    | `String s -> s
    | _ -> assert false
