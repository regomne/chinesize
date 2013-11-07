(*
    Rlc: intrinsic functions
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
open ExtList

let builtins = Hashtbl.create 0

let is_builtin = Hashtbl.mem builtins

let buildin s f g = Hashtbl.add builtins (Text.ident s) (f, g)

let eval_as_expr (loc, _, t, parms, label) : expression = 
  assert (label == None);
  (fst (Hashtbl.find builtins t)) loc (parms: parameter list)
let eval_as_code (loc, rv, _, t, parms, label) : statement = 
  assert (label == None);
  (snd (Hashtbl.find builtins t)) loc rv parms

(* The actual functions *)

let make_unignorable_as_code fn =
  fun loc rv parms ->
    match rv with
      | None -> ksprintf (error loc) "the return value of the `%s' intrinsic cannot be ignored" fn
      | Some rv -> `Assign (loc, rv, `Set, eval_as_expr (loc, fn, Text.ident fn, parms, None))

let defined =
  buildin "defined?"
    (fun loc syms ->
      let all_defined =
        List.for_all
          (function
            | `Simple (_, `VarOrFn (_, _, t)) -> Memory.defined t
            | _ -> error loc "the `defined?' intrinsic must be passed only simple identifiers")
          syms
      in
      `Int (loc, if all_defined then 1l else 0l))
    (make_unignorable_as_code "defined?")

and default =
  buildin "default"
    (fun loc ->
      function
        | [`Simple (_, (`VarOrFn (_, _, sym) as ifdef)); `Simple (_, ifndef)]
           -> if Memory.defined sym then ifdef else ifndef
        | _ -> error loc "the `default' intrinsic must be passed a symbol and an expression")
    (make_unignorable_as_code "default")

and constant =
  buildin "constant?"
    (fun loc exprs ->
      try
        List.iter
          (function
            | `Simple (_, e) -> ignore (!Global.expr__normalise_and_get_const e ~abort_on_fail:false)
            | _ -> raise Exit)
          exprs;
        `Int (loc, 1l)
      with Exit ->
        `Int (loc, 0l))
    (make_unignorable_as_code "constant?")

and integer =
  let rec not_int =
    function
      | `Int _ | `Store _ | `IVar _ | `LogOp _ | `AndOr _ | `Unary _ | `SelFunc _
         -> false
      | `Str _ | `SVar _ | `Res _
         -> true
      | `Op (_, e, _, _) | `Parens (_, e)
         -> not_int e
      | `ExprSeq (_, _, _, d)
         -> not_int (expr_of_statement (DynArray.last d))
      | `Func (_, _, t, _, _) as elt when Memory.defined t || is_builtin t
         -> not_int (!Global.expr__disambiguate elt)
      | `VarOrFn _ | `Deref _ as elt
         -> not_int (!Global.expr__disambiguate elt)
      | `Func (l, s, t, p, label)
         -> let f = ver_fun "" (Hashtbl.find_all functions t) in
            function_type f <> `Int
  in  
  buildin "integer?"
    (fun loc exprs ->
      try
        List.iter
          (function
            | `Simple (_, e) -> if not_int e then raise Exit
            | _ -> raise Exit)
          exprs;
        `Int (loc, 1l)
      with Exit | Not_found ->
        `Int (loc, 0l))
    (make_unignorable_as_code "integer?")

and array =
  buildin "array?"
    (fun loc syms ->
      let all_defined =
        let rec f x =
          match fst (Memory.get x) with
            | `StaticVar (_, _, Some _, _, _, _) -> true
            | `Macro (`VarOrFn (_, _, t)) -> f t
            | _ -> false
        in
        List.for_all 
          (function 
            | `Simple (_, `VarOrFn (_, _, t)) -> f t
            | _ -> error loc "the `array?' intrinsic must be passed only simple identifiers")
          syms
      in
      `Int (loc, if all_defined then 1l else 0l))
    (make_unignorable_as_code "array?")

and length =
  buildin "length"
    (fun loc ->
      function
        | [`Simple (_, `VarOrFn (_, str, sym))]
           -> let rec f x =
                match fst (Memory.get x) with
                  | `StaticVar (_, _, Some l, _, _, _) -> `Int (loc, Int32.of_int l)
                  | `Macro (`VarOrFn (_, _, t)) -> f t
                  | _ -> ksprintf (error loc) "`%s' is not an array" str
              in
              f sym
        | _ -> error loc "the `length' function must be passed a single array variable")
    (make_unignorable_as_code "length")

and __deref =
  buildin "__deref"
    (fun loc ->
      function
        | [`Simple (_, space); `Simple (_, e)]
           -> `IVar (loc, Int32.to_int (Global.expr__normalise_and_get_int space), e)
        | _ -> error loc "the `__deref' intrinsic must be passed an integer constant and an expression")
    (make_unignorable_as_code "__deref")

and __sderef  =
  buildin "__sderef"
    (fun loc ->
      function
        | [`Simple (_, space); `Simple (_, e)]
           -> `SVar (loc, Int32.to_int (Global.expr__normalise_and_get_int space), e)
        | _ -> error loc "the `__sderef' intrinsic must be passed an integer constant and an expression")
    (make_unignorable_as_code "__sderef")

and __variable =
  buildin "__variable?"
    (let rtrue = `Int (nowhere, 1l) and rfalse = `Int (nowhere, 0l) in
     fun loc ->
      function
        | [`Simple (_, (`IVar _ | `SVar _))] -> rtrue
        | [`Simple (_, `VarOrFn (loc, _, id))] 
           -> (match Memory.get_as_expression ~loc id ~allow_arrays:true ~recurse:true with
                 | `IVar _ | `SVar _ -> rtrue
                 | _ -> rfalse)
        | [`Simple (_, `Deref (loc, _, id, expr))] 
           -> (match Memory.get_deref_as_expression id expr with
                 | `IVar _ | `SVar _ -> rtrue
                 | _ -> rfalse)
        | _ -> rfalse)
    (make_unignorable_as_code "__variable?")

and __addr =
  buildin "__addr"
    (let make_addr space iexpr =
       `Op (nowhere,
            iexpr,
            `Or,
            `Op (nowhere, `Int (nowhere, Int32.of_int space), `Shl, `Int (nowhere, 16l)))      
     in
     fun loc ->
      function
        | [`Simple (_, (`IVar (_, space, indexexpr) | `SVar (_, space, indexexpr)))]
           -> make_addr space indexexpr
        | [`Simple (_, `VarOrFn (loc, _, id))] 
           -> (match Memory.get_as_expression ~loc id ~allow_arrays:true ~recurse:true with
                 | `IVar (_, space, indexexpr) | `SVar (_, space, indexexpr)
                    -> make_addr space indexexpr
                 | _ -> error loc "the `__addr' intrinsic must be passed a single variable")
        | [`Simple (_, `Deref (loc, _, id, expr))] 
           -> (match Memory.get_deref_as_expression id expr with
                 | `IVar (_, space, indexexpr) | `SVar (_, space, indexexpr)
                    -> make_addr space indexexpr
                 | _ -> error loc "the `__addr' intrinsic must be passed a single variable")
        | _ -> error loc "the `__addr' intrinsic must be passed a single variable")
    (make_unignorable_as_code "__addr")

and __ident =
  buildin "__ident"
    (fun loc ->
      function
        | [`Simple (_, id)] -> let s = StrTokens.to_string (Global.expr__normalise_and_get_str id) ~enc:"UTF-8"
                               in `VarOrFn (loc, s, Text.of_string "UTF-8" s)
        | _ -> error loc "the `__ident' intrinsic function must be passed a single string constant")
    (make_unignorable_as_code "__ident")

and at =
  buildin "at"
    (fun loc ->
      function
        | [`Simple (_, file); `Simple (_, line); `Simple (_, expr)]
           -> set_expr_loc
                { file = StrTokens.to_string (Global.expr__normalise_and_get_str file);
                  line = Int32.to_int (Global.expr__normalise_and_get_int line) }
                expr
        | _ -> error loc "the `at' intrinsic function must be passed a location and an expression (str file; int line; any expression)")
    (fun loc rv ->
      function
        | [`Simple (_, file); `Simple (_, line); `Simple (_, str)]
           -> assert (rv = None); (* Needs checking to make sure this can never happen, but... *)
              let str =
                try Global.expr__normalise_and_get_str str ~abort_on_fail:false
                with _ -> error loc "the string passed to `at' must be evaluable at compile-time"
              and atloc =
                { file = StrTokens.to_string (Global.expr__normalise_and_get_str file);
                  line = Int32.to_int (Global.expr__normalise_and_get_int line) }
              in
              KeULexer.call_parser_on_text KeAstParser.program atloc (Text.of_string "UTF8" (StrTokens.to_string str ~enc:"UTF8"))
        | _ -> error loc "the `at' intrinsic statement must be passed a location and a string to evaluate (str file; int line; str statement_string)")
  
and __empty_string =
  buildin "__empty_string?"
    (fun loc ->
      function
        | [`Simple (_, e)]
           ->(try
                let sarr = Global.expr__normalise_and_get_str e ~abort_on_fail:false in
                `Int (loc, if string_of_strtokens sarr = "" then 1l else 0l)
              with Exit ->
                error loc "the `__empty_string?' intrinsic must be passed a single string constant")
        | _ -> error loc "the `__empty_string?' intrinsic must be passed a single string constant")
    (make_unignorable_as_code "__empty_string?")

and __equal_strings =
  buildin "__equal_strings?"
    (fun loc ->
      function
        | [`Simple (_, e1); `Simple (_, e2)]
           ->(try
                let s1 = string_of_strtokens (Global.expr__normalise_and_get_str e1 ~abort_on_fail:false)
                and s2 = string_of_strtokens (Global.expr__normalise_and_get_str e2 ~abort_on_fail:false) 
                in `Int (loc, if s1 = s2 then 1l else 0l)
              with Exit ->
                error loc "the `__equal_strings?' intrinsic must be passed a single string constant")
        | _ -> error loc "the `__equal_strings?' intrinsic must be passed a single string constant")
    (make_unignorable_as_code "__equal_strings?")

and gameexe =
  buildin "gameexe"
    (fun loc params ->
      let key, idx, default =
        match params with
          | [`Simple (_, key)] -> key, `Int (nowhere, 0l), None
          | [`Simple (_, key); `Simple (_, idx)] -> key, idx, None
          | [`Simple (_, key); `Simple (_, idx); `Simple (_, default)] -> key, idx, Some default
          | [] -> error loc "the `gameexe' intrinsic requires you to specify at minimum a key"
          | _ -> error loc "too many parameters to `gameexe': this intrinsic takes a maximum of three"
      in
      let key =
        try
          let k = StrTokens.to_string (Global.expr__normalise_and_get_str key ~abort_on_fail:false) in
          if k = "" then error loc "the key passed to `gameexe' must not be empty";
          if k.[0] = '#' then ExtString.String.lchop k else k
        with _ -> error loc "the key passed to `gameexe' must evaluate to a constant"
      and idx =
        try Int32.to_int (Global.expr__normalise_and_get_int idx ~abort_on_fail:false)
        with _ -> error loc "the index passed to `gameexe' must evaluate to a constant"
      in
      try
        let value = Ini.find key in
        try
          match List.nth value idx with
            | `Enabled t -> `Int (loc, if t then 1l else 0l)
            | `Integer i -> `Int (loc, i)
            | `String s -> `Str (loc, DynArray.of_list [`Text (loc, `Sbcs, Text.of_sjs s)])
            | _ -> assert false (* other types should not be appearing *)
        with _ ->
          ksprintf (error loc) "unable to return value %d from #%s: index out of range" idx key
      with Not_found ->
        match default with
          | Some expr -> expr
          | None -> ksprintf (error loc) "unable to find #%s in GAMEEXE.INI, and no default was provided" key)
    (make_unignorable_as_code "gameexe")

and target_comparisons =
  let compver f loc args =
    let args' = 
       List.map 
         (function 
            | `Simple (_, i) -> 
               begin try
                 Int32.to_int (Global.expr__normalise_and_get_int ~abort_on_fail:false i)
               with _ ->
                 ksprintf (error loc) "the parameters to `%s' must evaluate to integer constants" f
               end
            | _ -> ksprintf (error loc) "the parameters to `%s' must be simple expressions" f)
         args
    in
    match args' with
      | [a] -> a, 0, 0, 0
      | [a; b] -> a, b, 0, 0
      | [a; b; c] -> a, b, c, 0
      | [a; b; c; d] -> a, b, c, d
      | _ -> ksprintf (error loc) "`%s' must be passed between 1 and 4 parameters" f
  in
  let target fn op =
    let fname = "target_" ^ fn in
    buildin fname 
      (fun loc args -> `Int (loc, if op (current_version ()) (compver fname loc args) then 1l else 0l))
      (make_unignorable_as_code fname)
  in
  target "lt" (<);
  target "le" (<=);
  target "gt" (>);
  target "ge" (>=)

and kinetic =
  buildin "kinetic?"
    (fun loc ->
      function
        | [] -> `Int (loc, if !global_target = `Kinetic then 1l else 0l)
        | _ -> error loc "`kinetic?' takes no parameters")
    (make_unignorable_as_code "kinetic?")

and rlc_parse_string =
  buildin "rlc_parse_string"
    (fun loc ->
      function
        | [`Simple (_, str)]
           -> let str =
                try Global.expr__normalise_and_get_str str ~abort_on_fail:false
                with _ -> error loc "the string passed to `rlc_parse_string' must be evaluable at compile-time"
              in
              KeULexer.call_parser_on_text KeAstParser.just_expression loc (Text.of_string "UTF8" (StrTokens.to_string str ~enc:"UTF8"))
        | _ -> error loc "the `rlc_parse_string' intrinsic must be passed a single string constant")
    (fun loc rv ->
      function
        | [`Simple (_, str)]
           -> assert (rv = None); (* Needs checking to make sure this can never happen, but... *)
              let str =
                try Global.expr__normalise_and_get_str str ~abort_on_fail:false
                with _ -> error loc "the string passed to `rlc_parse_string' must be evaluable at compile-time"
              in
              KeULexer.call_parser_on_text KeAstParser.program loc (Text.of_string "UTF8" (StrTokens.to_string str ~enc:"UTF8"))
        | _ -> error loc "the `rlc_parse_string' intrinsic must be passed a single string constant")
