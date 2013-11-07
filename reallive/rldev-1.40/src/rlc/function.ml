(*
    Rlc: function typechecking and compilation
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
(*ppopt pa_macro.cmo ./pa_matches.cmo *)

(*DEFINE DEBUG*)

open Printf
open ExtList
open KfnTypes
open KeTypes
open KeAst
open Codegen


(* Error function: generic *)
let fail loc s =
  ksprintf (error loc) "unable to find a prototype for `%s' that matches these parameters" s

(* Error function: report what type we were expecting. *)
let expected loc etype msg =
  let tn =
    match etype with
      | Any -> "any type"
      | Int -> "integer variable"
      | IntC | IntV -> "integer"
      | Str -> "string variable"
      | StrC | StrV | ResStr -> "string"
      | Special _ -> "special function"
      | Complex _ -> "tuple"
  in
  ksprintf (error loc) "expected %s, %s" tn msg

(* Error function: `Func outside special context *)
let rec undeclared_func_in_expr =
  function
    | `Func (loc, s, _, _, _) -> ksprintf (error loc) "undeclared identifier `%s'" s
    | `Parens (_, e) -> undeclared_func_in_expr e
    | #expression -> invalid_arg "undeclared_func_in_expr"


(* Utilities *)
let rec get_literal_from_expr =
  function
    | `Str (_, t) -> t
    | `Parens (_, e) -> get_literal_from_expr e
    | #expression -> invalid_arg "get_literal_from_expr"

let double_quote_var =
  let sym = Text.of_sjs "__double_quote_var__\000" in
  fun ?insert_pos () ->
    try
      Memory.get_as_expression sym
    with Not_found ->
      let svar = Memory.get_temp_str () in
      Memory.define sym (`Macro svar);
      (match insert_pos with
        | None -> Output.add_code nowhere
        | Some i -> Output.insert_code i)
      (FuncAsm.compile_function_str nowhere (rlfun "zentohan")
        [`Literal "\x81\x68"] ~returnval:(code_of_var svar));
      svar


(* Retrieve an appropriate function definition based on the name and target version.
   Note that parameter type-checking is NOT performed at this point, nor is an overload
   selected: the [params] parameter is used only to disambiguate the single case in
   which two opcodes share the same name. *)
let get_func_def ?(look_in = functions) t params =
  let defs = Hashtbl.find_all look_in t in
  if defs = [] then raise Exit;
  match List.filter valid_opcode defs with
    | [] -> ksprintf failwith "the function `%s' is not supported in %s" (Text.to_sjs t) (current_version_string ())
    | [one] -> one
    | defs (* Currently inter-opcode overloading is only implemented sufficiently to handle grpMulti. *)
       -> let first_param_type =
            match
              type_of_normalised_expr ~allow_invalid:false
                (match params with `Simple (_, e) :: _ -> e | _ -> raise Not_found)
            with
              | `Int -> `Int
              | `Str | `Literal -> `Str
              | `Invalid -> assert false
          in
          List.find
            (fun { prototypes = p } ->
              let p1 = try fst (List.hd (Option.get (List.find Option.is_some (Array.to_list p)))) with _ -> raise Not_found
              in (first_param_type = `Str && List.mem p1 [Str; StrC; StrV; ResStr])
              || (first_param_type = `Int && List.mem p1 [Int; IntC; IntV]))
            defs


(* Eliminate prototype options, by comparison with a list of parameters, until
   only one remains, and return its index.  Fail if more than one remains at
   the end.

   The rules for overload distinction are based on what could easily be
   implemented that would cover existing cases in reallive.kfn.  It is possible
   that unknown functions exist which this won't handle.

   We make the following assumptions:

   1. All overloads of a function have distinct numbers of non-repeated parameters.
   2. All overloads of a function have the same number of repeated parameters.
   3. There is a maximum of one undefined overload per function. *)
let choose_overload loc options params =
  (* Get number of simple parameters in each overload:
       None = arbitrary length
       Some (total, optional, has_repeated) = length is (total - optional)..(total) *)
  let overload_lengths, _ =
    Array.fold_right
      (fun elt (acc, idx) ->
        match elt with
          | None -> (None :: acc, idx - 1)
          | Some elt
           -> let t, o, r =
                (List.fold_left
                  (fun (t, o, r) ->
                    function
                      | Special _, _ | Complex _, _ -> t, o, true
                      | _, flags when List.mem Argc flags -> t, o, true
                      | _, flags when List.mem Return flags -> t, o, r
                      | _, flags -> t + 1, (if List.mem Optional flags then o + 1 else o), r)
                  (0, 0, false))
                elt
              in
              Some (t, o, r, idx) :: acc, idx - 1)
      options
      ([], Array.length options - 1)
  in
  (* In debug builds, check that the invariants detailed above apply. *)
  IFDEF DEBUG THEN
    let module ISet = Set.Make (struct type t = int;; let compare = compare end) in
    ignore
      (List.fold_left
        (fun (used_lengths, had_nodef, has_repeated) ->
          function
            | Some (t, o, hr, _)
             -> let s = ref used_lengths in
                for i = t - o to t do
                  if ISet.mem i used_lengths then error loc "internal error: failed choose_overload invariant 1";
                  s := ISet.add i !s
                done;
                if has_repeated = Some (not hr) then error loc "internal error: failed choose_overload invariant 2";
                !s, had_nodef, Some hr
            | None
             -> if had_nodef then error loc "internal error: failed choose_overload invariant 3";
                used_lengths, true, has_repeated)
        (ISet.empty, false, None)
        overload_lengths)
  ELSE () END;
  (* Get number of simple parameters in parameter list. *)
  let param_count =
    List.fold_left
      (fun acc ->
        function
          | `Simple (_, `Func _) -> acc
          | `Simple _ -> acc + 1
          | _ -> acc)
      0 params
  in
  (* Search for an overload for which param_count falls within the defined range etc. *)
  let rv =
    try
      match
        List.find
          (function
            | None -> false
            | Some (total, optional, _, _) -> total >= param_count && total - optional <= param_count)
          overload_lengths
      with
        | Some (_, _, _, rv) -> rv
        | _ -> assert false
    with Not_found ->
      let i = ref 0 in
      while !i < Array.length options && options.(!i) <> None do incr i done;
      if !i >= Array.length options then raise Not_found;
      !i
  in
  assert (rv >= 0 && rv < Array.length options);
  rv


(* Iterate through parameters, checking types and compiling code. *)
let rec check_and_compile loc funname defs params =
  let cancel_compile_func = ref false in

  (* Main traversal function: takes a KeAst.parameter and returns a FuncAsm.parameter. *)
  let rec map_param defs argc i =

    (* Subfunction to handle special parameters. *)
    let rec handle_special ll stype lparams (ptype, pflags) =
      let s_defs =
        match ptype with
          | Special d -> d
          | _ -> expected ll ptype ("found " ^ match stype with `Index _ -> "special function" | `Named (s, _) -> sprintf "undeclared function `%s'" s | `AsComplex -> "tuple")
      in
      let s_id, s_def, s_flags =
        match stype with
          | `Index i
             -> (try List.find (fun (a, _, _) -> a == i) s_defs
                 with Not_found -> ksprintf (error ll) "special function %d is not defined for %s" i funname)
          | `Named (s, t)
             -> begin
                  match List.filter (function _, Named (s, _), _ -> Text.ident s = t | _ -> false) s_defs
                with
                  | [] -> ksprintf (expected ll ptype) "found undeclared function %s" s
                  | [single] -> single
                  | multiple ->
                    try
                      List.find
                        (function _, Named (_, d), _ -> List.length d = List.length lparams | _ -> assert false)
                        multiple
                    with Not_found ->
                      ksprintf (error ll) "unable to find a version of the %s special function `%s' that matches these parameters" funname s
                end
          | `AsComplex
             -> begin
                  match List.filter (function _, AsComplex _, _ -> true | _ -> false) s_defs
                with
                  | [] -> expected ll ptype "found tuple"
                  | [single] -> single
                  | multiple ->
                 (* If a function has more than one AsComplex special, try to determine which is
                    correct. *)
                    let lengths_and_types_match defparams =
                      let rec loop =
                        function
                          | [], [] -> true
                          | [], _ | _, [] -> false
                          | (dt, _) :: ds, l :: ls
                           -> let lt = type_of_normalised_expr l in
(*let rv =*)
                              match dt, lt with
                                | Any, (`Int | `Str)
                                | (Int | IntC | IntV), `Int
                                | (Str | StrC | StrV | ResStr), (`Str | `Literal) -> loop (ds, ls)
                                | _ -> false
(*in eprintf "  %s - %s : %s\n%!"
(match dt with Any -> "any" | Int | IntC | IntV -> "int" | Str | StrC | StrV | ResStr -> "str" | _ -> "other")
(match lt with `Int -> "int" | `Str -> "str" | _ -> "other")
(if rv then "yes" else "no"); rv*)
                      in
                      loop (defparams, lparams)                        
                    in
(*eprintf "%s:\n%!" funname;*)
                    match 
                      List.filter 
                        (function _, AsComplex d, _ -> lengths_and_types_match d | _ -> assert false) 
                        multiple
                    with
                      | [] -> ksprintf (error ll) "unable to find a version of %s that accepts tuples of this type" funname
                      | [single] -> single
                      | multiple -> ksprintf Optpp.sysError "internal error: function %s has ambiguous definition" funname
                end
      in
      let s_def = match s_def with Named (_, d) | AsComplex d -> Array.of_list d
      in
      if Array.length s_def <> List.length lparams then
        ksprintf (error ll) "expected %d parameters to %s, but found %d parameters"
          (Array.length s_def)
          (match stype with
            | `Index _ -> "special function"
            | `Named (s, _) -> sprintf "`%s'" s
            | `AsComplex  -> "tuple")
          (List.length lparams);
      if not (List.mem Uncount pflags) then incr argc;
      `Special
         (char_of_int s_id,
          s_flags,
          List.mapi
            (fun i elt -> map_param s_def (ref 0) i (simple_of_expression elt))
            lparams)
      (* End of handle_special *)

    (* Subfunction to handle non-special complex parameters *)
    and handle_complex ll lparams c_def pflags =
      let c_def = Array.of_list c_def in
      if Array.length c_def <> List.length lparams then
        ksprintf (error ll) "expected %d parameters in tuple, but found %d parameters"
          (Array.length c_def) (List.length lparams);
      if not (List.mem Uncount pflags) then incr argc;
      `List
        (List.mapi
          (fun i elt -> map_param c_def (ref 0) i (simple_of_expression elt))
          lparams)

    (* Subfunction to handle literal strings.

       Since '"' is never valid in literals in soft function calls - including
       strcpy()! - it is illegal to compile

         do_stuff('foo "bar"')

       directly; instead, we have to write the equivalent of

         do_stuff('foo ' + zentohan('h') + 'bar' + zentohan('h'))

       which is expressed adequately efficiently as

         temp1 = zentohan('h')
         strcpy(temp2, 'foo ')
         strcat(temp2, temp1)
         strcat(temp2, 'bar')
         strcat(temp2, temp1)
         do_stuff(temp2)

       We also handle expansion of codes such as \i{} and \m{} here.
    *)
    and handle_literal_in_strc ll pflags e =
      let text = get_literal_from_expr e in
      let is_basic_literal =
        try
          DynArray.iter
            (function `EOS -> assert false
              | `RCur _ | `LLentic _ | `RLentic _ | `Asterisk _ | `Percent _ | `Hyphen _ | `Text _ | `Space _ -> ()
              | _ -> raise Exit)
            text;
          true
        with
          Exit -> false
      in
      if is_basic_literal then
        `Literal (StrTokens.to_string text ~quote:true)
      else
        let invalid l s = ksprintf (error l) "`\\%s{}' invalid in string constants" s in
        (* We treat strcpy and strcat specially. *)
        let is_cpy_or_cat =
          if String.lowercase funname = "strcpy" then `Strcpy
          else if String.lowercase funname = "strcat" then `Strcat
          else `No
        in
        (* Function to return a variable containing just '"'. *)
        let doublequote () = `String (code_of_expr (double_quote_var ()))
        (* Temporary variables to use to build the result. *)
        and tempstr () = Memory.get_temp_var `Str "__handle_literal_in_strc__tempstr__\000"
        and accum =
          if is_cpy_or_cat <> `No then
            (* In the case of strcpy and strcat, we use the first parameter as an
               accumulator. *)
            match List.hd params with `Simple (_, (`SVar _ as s)) -> s | _ -> assert false
          else
            (* Otherwise, we use a temporary variable.  We can safely allocate one
               here, because we know that we are always in a scope local to the current
               statement. *)
            Memory.get_temp_str ()
        in
        (* Now we iterate through the string, adding chunks to the accumulator. *)
        let empty = ref true in
        let add, quote, not_quoted, contents, flush =
          let buffer = Buffer.create 0
          and quoted = ref false in
          (Buffer.add_string buffer),
          (fun () -> quoted := true),
          (fun () -> not !quoted),
          (fun () -> Buffer.contents buffer),
          (fun () ->
            if Buffer.length buffer > 0 then
              let fn = if is_cpy_or_cat <> `Strcat && !empty then "strcpy" else "strcat" in
              let s = Buffer.contents buffer in
              Output.add_code nowhere
                (FuncAsm.compile_function_str nowhere (rlfun fn)
                  [`String (code_of_expr accum); `Literal (if !quoted then sprintf "\"%s\"" s else s)]);
              empty := false;
              quoted := false;
              Buffer.clear buffer)
        in
        DynArray.iter
          begin function `EOS -> assert false
           (* Plain text *)
            | `RCur _     -> quote (); add "}"
            | `LLentic _  -> add "\x81\x79"
            | `RLentic _  -> add "\x81\x7a"
            | `Asterisk _ -> add "\x81\x96"
            | `Percent _  -> add "\x81\x93"
            | `Hyphen _   -> quote (); add "-"
            | `Text (l, _, s)
               ->(if not_quoted () then try
                   Text.iter (fun c -> if not (StrTokens.unquoted_char c) then raise Exit) s
                  with Exit -> quote ());
                 (try
                    add (TextTransforms.to_bytecode s)
                  with Text.Bad_char c ->
                    ksprintf (error l) "cannot represent U+%04x in RealLive bytecode" c)
            | `Space (_, i) -> quote (); add (String.make i ' ')
           (* Special cases for objOfText *)
            | `Code (l, t, _, p) when List.mem TextObject pflags && StrTokens.is_object_code t
               -> add (StrTokens.get_object_code l funname t p)
           (* Invalid codes *)
            | `Speaker l -> invalid l "{}' / `\\name"
            | `Gloss (l, `Gloss, _, _) -> invalid l "g"
            | `Gloss (l, `Ruby, _, _) -> invalid l "ruby"
            | `Add (l, _) -> invalid l "a"
            | `Delete l -> invalid l "d"
            | `Rewrite (l, _) -> invalid l "f"
            | `Code (l, t, _, _) when not (StrTokens.is_output_code t) -> invalid l (Text.to_sjs t)
           (* Elements requiring accumulation *)
            | `DQuote _
               -> flush ();
                  if is_cpy_or_cat <> `Strcat && !empty then
                    Output.add_code nowhere
                     (FuncAsm.compile_function_str nowhere (rlfun "zentohan")
                       [`Literal "\x81\x68"] ~returnval:(code_of_expr accum))
                  else
                    Output.add_code nowhere
                      (FuncAsm.compile_function_str nowhere (rlfun "strcat")
                        [`String (code_of_expr accum); doublequote ()]);
                  empty := false

            | `Name (l, locglob, i, cidx)
               -> flush ();
                  let use_accum = is_cpy_or_cat <> `Strcat && !empty in
                  let tvar = if use_accum then accum else tempstr () in
                  Output.add_code nowhere
                    (FuncAsm.compile_function_str nowhere
                      (rlfun (if locglob = `Local then "GetLocalName" else "GetName"))
                      [`Integer (code_of_expr i)] ~returnval:(code_of_expr tvar));
                  Option.may
                    (fun e ->
                      Output.add_code nowhere
                        (FuncAsm.compile_function_str nowhere (rlfun "strsub")
                          [`String (code_of_expr tvar);
                           `Integer (code_of_expr e);
                           `Integer (code_of_int32 1l)]
                          ~returnval:(code_of_expr tvar)))
                    cidx;
                  if not use_accum then
                    Output.add_code nowhere
                      (FuncAsm.compile_function_str nowhere (rlfun "strcat")
                        [`String (code_of_expr accum); `String (code_of_expr (tempstr ()))]);
                  empty := false

            | `Code (l, t, e, p)
               -> let t = Text.to_sjs t in
                  if t = "s" then
                    let parm = match p with [`Simple (_, s)] -> s | _ -> error l "the control code \\s{} must have one and only one parameter" in
                    (* By this stage, the parameter SHOULD be a string variable. *)
                    if not (parm matches `SVar _) then ksprintf (error l) "Oops, expected string variable but found %s" (string_of_expr parm);
                    if e <> None then error l "the control code \\s{} cannot have a length specifier";
                    (* Append the string. *)
                    flush ();
                    let fn = if is_cpy_or_cat <> `Strcat && !empty then "strcpy" else "strcat" in
                    Output.add_code nowhere
                      (FuncAsm.compile_function_str nowhere (rlfun fn)
                        [`String (code_of_expr accum); `String (code_of_expr parm)]);
                    empty := false
                  else
                    (* For int parameters: *)
                    let parm = match p with [`Simple (_, s)] -> s | _ -> error l "the control code \\i{} must have one and only one parameter" in
                    assert (t = "i");
                    if normalised_expr_is_const parm then
                      add (Global.int32_to_string_padded
                        (match e with None -> 0 | Some e -> Int32.to_int (int_of_normalised_expr e))
                        (int_of_normalised_expr parm))
                    else
                      let add var =
                        let length =
                          match e with
                            | None -> []
                            | Some e -> `Integer (code_of_expr e) :: []
                        in
                        Output.add_code nowhere
                          (FuncAsm.compile_function_str nowhere (rlfun "itoa")
                            (`Integer (code_of_expr parm) :: length) ~returnval:(code_of_expr var))
                      in
                      flush ();
                      if is_cpy_or_cat <> `Strcat && !empty
                      then add accum
                      else (add (tempstr ());
                            Output.add_code nowhere
                              (FuncAsm.compile_function_str nowhere (rlfun "strcat")
                                [`String (code_of_expr accum); `String (code_of_expr (tempstr ()))]));
                      empty := false
          end text;
        (* Return a suitable value. *)
        if !empty && is_cpy_or_cat = `No then
          `Literal (if not_quoted () then contents () else sprintf "\"%s\"" (contents ()))
        else (
          flush ();
          if is_cpy_or_cat = `Strcat || (is_cpy_or_cat = `Strcpy && List.length params = 2) then (
            cancel_compile_func := true;
            `Unknown ""
          )
          else `String (code_of_expr accum)
        )

    (* Subfunction to handle simple parameters in the general case *)
    and handle_simple ll e (ptype, pflags) =
      if not (List.mem Uncount pflags) then incr argc;
      match ptype, type_of_normalised_expr e with
        | _, `Invalid -> undeclared_func_in_expr e
        | Any, `Literal -> handle_literal_in_strc ll pflags e
        | Any, _ -> `Unknown (code_of_expr e)
        | IntC, `Int
                 -> `Integer (code_of_expr e)
        | Int, `Int
                 -> if (e matches `Store l | `IVar (l, _, _)) && loc_of_expr e <> Memory.temploc
                    then `Integer (code_of_expr e)
                    else expected ll ptype "found expression"
        | IntV, `Int
                 -> if (e matches `Store l | `IVar (l, _, _)) then
                      `Integer (code_of_expr e)
                    else 
                      let tv = Memory.get_temp_var `Int "__handle_simple_temp_integer_\000" in
                      let tv = match tv with #assignable as a -> a | _ -> assert false in
                      Output.add_code nowhere (code_of_assignment (nowhere, tv, `Set, e));
                      `Integer (code_of_var tv)
        | (StrC | ResStr), `Str
                 -> `String (code_of_expr e)
        | (StrC | ResStr), `Literal
                 -> handle_literal_in_strc ll pflags e
        | Str, `Str
                 -> if match e with `SVar (l, _, _) when l <> Memory.temploc -> true | _ -> false
                    then `String (code_of_expr e)
                    else expected ll ptype "found expression"
        | StrV, (`Str | `Literal)
                 -> if (e matches `SVar  (l, _, _))
                    then `String (code_of_expr e)
                    else
                      let tv = Memory.get_temp_var `Str "__handle_simple_temp_string_\000" in
                      compile (nowhere, None, "strcpy", Text.ident "strcpy", 
                        [`Simple (nowhere, tv); `Simple (nowhere, e)], None);
                      `String (code_of_expr tv)                        
        | _, `Int -> expected ll ptype "found integer"
        | _, `Str -> expected ll ptype "found string"
        | _, `Literal -> expected ll ptype "found string literal"

    (* Body of map_param *)
    in function
      | `Simple (_, `Func (ll, ls, lt, lparams, llbl))
         -> assert (llbl == None);
            handle_special ll (`Named (ls, lt)) (List.map expression_of_simple lparams) defs.(i)
      | `Special (ll, i, lparams)
         -> handle_special ll (`Index i) lparams defs.(i)
      | `Complex (ll, lparams)
         -> (match defs.(i) with
              | Special _, _ as s_def -> handle_special ll `AsComplex lparams s_def
              | Complex c_def, pflags -> handle_complex ll lparams c_def pflags
              | ptype, _ -> expected ll ptype "found tuple")
      | `Simple (ll, expr)
         -> (match defs.(i) with
              | Special _, _ as s_def -> handle_special ll `AsComplex [expr] s_def
              | _ -> handle_simple ll expr defs.(i))
  in
  (* Get definitions as an array, with the same number of elements as the parameter list. *)
  let argc = ref 0 in
  let defs =
    let plen = List.length params in
    let arr = Array.make plen (Any, []) in
    let dlen =
      List.fold_left
        (fun acc elt ->
          if acc < plen
          then (arr.(acc) <- elt; acc + 1)
          else
            (if List.exists (function Optional | Argc -> true | _ -> false) (snd elt)
             then acc
             else IFDEF DEBUG THEN failwith "arg not optional in check_and_compile" ELSE fail loc funname END))
        0
        (List.filter (function _, pflags -> not (List.mem Return pflags)) defs)
    in
    if dlen < plen then
      if List.mem Argc (snd arr.(dlen - 1)) then
        for i = dlen to plen - 1 do
         arr.(i) <- arr.(dlen - 1)
        done
      else
        (IFDEF DEBUG THEN failwith "arg not Argc in check_and_compile" ELSE fail loc funname END);
    arr
  in
  (* Traverse and return. *)
  List.mapi (map_param defs argc) params, !argc, !cancel_compile_func


(* Compile a function: general case.  The expressions in [params] must be normalised. *)
and compile ?(is_code = false) (loc, dest, s, t, params, label) =
  IFDEF DEBUG THEN ksprintf (info loc) "compiling %s" s ELSE () END;
  (* Retrieve the fndef. *)
  let def =
    try
      get_func_def t params ?look_in:(if is_code then Some ctrlcodes else None)
    with
      | Not_found -> IFDEF DEBUG THEN failwith "Not_found in get_func_def" ELSE fail loc s END
      | Failure s -> ksprintf (error loc) "caught exception Failure (%s)" s
      | Exit -> ksprintf (error loc) "expected %s, found %s `%s'"
                  (if dest = None then "expression" else "statement")
                  (Memory.describe t) s
  in
  (* Check label for sanity *)
  assert (label == None || List.mem IsGoto def.flags);
  (* Identify the particular overload to use. *)
  let overload =
    assert (Array.length def.prototypes <> 0);
    if Array.length def.prototypes = 1 then 0 else try
      choose_overload loc def.prototypes params
    with Not_found ->
      IFDEF DEBUG THEN failwith "Not_found in choose_overload" ELSE fail loc s END
  in
  (* Get the corresponding list of parameter definitions. *)
  let paramdefs =
    match def.prototypes.(overload) with
      | Some defs -> defs
      | None -> List.map (fun _ -> Any, []) params
  in
  (* If this is a special-case conditional, check for and handle optimised cases. *)
  if List.mem IsCond def.flags && Goto.special_case compile loc s def.ident params label
  then () (* Case handled, do nothing more *)
  else (* Continue compiling as standard case *)
    (* Iterate through parameters, checking types and compiling expressions. *)
    let params, argc, cancel =
      check_and_compile loc s paramdefs params
    in
    (* Build bytecode *)
    if not cancel then
      let f, a = 
        FuncAsm.compile_function loc def params
          ~overload ?returnval:(Option.map code_of_var dest)
      in
      Output.add_code loc f;
      Option.may Output.add_ref label;
      Option.may (Output.add_code loc) a


(* Compile a function: special case for unknown opcodes. *)
let compile_unknown (loc, def, overload, params) =
  let paramdefs = List.rev_map (fun _ -> Any, []) params in
  let params, argc, _ = check_and_compile loc def.ident paramdefs params in
  Output.add_code loc 
    (FuncAsm.compile_function_str loc def params ~overload)
