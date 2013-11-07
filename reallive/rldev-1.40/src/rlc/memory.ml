(*
    Rlc: memory handling (mostly stubs)
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

(*DEFINE DEBUG_SCOPING*)

open Printf
open ExtList
open KeTypes
open KeAst

(* Symbol and scope handling. *)

exception MemoryError of string
let memerr s = raise (MemoryError s)

module IMap = Map.Make (struct type t = int let compare = compare end)
let staticvars = Array.make 12 IMap.empty 

let varidx i =
  if i < 7 then i
  else if i = 25 then 7
  else if i = 18 then 8
  else if i = 12 then 9
  else if (i = 10 || i = 11) && current_version () > (1, 3, 0, 0) then i
  else raise Not_found

type symbol_nodealloc =
  [ `Macro of expression
  | `Inline of (location * string * Text.t * [ `None | `Optional | `Some of expression ]) list * statement
  | `Integer of int32
  | `String of Text.t ]

type symbol_t =
  [ symbol_nodealloc
  | `StaticVar of int * int32 * int option * int * int * int ]

let symbols = Hashtbl.create 0
and scope = Stack.create ()
and defines = Hashtbl.create 0

let open_scope () = 
  IFDEF DEBUG_SCOPING THEN eprintf "Opening scope level %d\n%!" (Stack.length scope) ELSE () END;
  Stack.push (Hashtbl.create 0) scope

let () = open_scope () (* open top-level scope *)

let defined = Hashtbl.mem symbols

let pull_sym id =
  let rv = Hashtbl.find symbols id in
  Hashtbl.remove symbols id;
  rv
and replace_sym id sym =
  Hashtbl.add symbols id sym

let check_def s =
  let sym = Text.ident s in
  fun () -> defined sym (*TODO: Needs to be more sophisticated? *)

let warn_hide = check_def "__WarnHiddenSymbols__"
and warn_hide_fun () = not (check_def "__HideFunctionsSilently__" ())

let define ?(warnings = true) ?(scoped = true) ?(loc = nowhere) id (value : symbol_t) =
  if warnings then
    if warn_hide () && defined id then
      ksprintf (warning loc) "hiding existing symbol `%s'" (Text.to_sjs id)
    else
      if warn_hide_fun ()
      && (!Global.intrinsic__is_builtin id || try ignore (ver_fun "" (Hashtbl.find_all functions id)); true with _ -> false)
      then ksprintf (warning loc) "symbol is hiding API function `%s'" (Text.to_sjs id);
  Hashtbl.add symbols id (value, scoped);
  Hashtbl.add (if scoped then Stack.top scope else defines) id ()

let dealloc : symbol_t -> unit =
  function
    | #symbol_nodealloc -> ((* No deallocation required *))
    | `StaticVar (_, _, _, space, index, len) 
       -> for i = index to index + len - 1 do
            let m = staticvars.(space) in
            let nval = IMap.find i m - 1 in
            assert (nval >= 0);
            IFDEF DEBUG_SCOPING THEN eprintf "Deallocating %d.%d\n%!" space i ELSE () END;
            staticvars.(space) <- if nval = 0 then IMap.remove i m else IMap.add i nval m
          done
    (* TODO: handle other kinds of symbol? *)

let process f =
  try f symbols with Not_found -> memerr "undeclared identifier"

let undefine loc s id =
  let scoped =
    process
      (fun tbl ->
        let sym, scoped = Hashtbl.find tbl id in
        dealloc sym;
        Hashtbl.remove tbl id;
        scoped)
  in
  let tbl = if scoped then Stack.top scope else defines in
  try
    Hashtbl.find tbl id;
    Hashtbl.remove tbl id
  with Not_found ->
    ksprintf (error loc) "cannot undefine symbol `%s': not declared in current scope" s

let close_scope () =
  let closing = Stack.pop scope in
  IFDEF DEBUG_SCOPING THEN eprintf "Closing scope level %d\n%!" (Stack.length scope) ELSE () END;
  if Stack.is_empty scope then memerr "internal error: closed top-level scope";
  Hashtbl.iter
    (fun id () ->
      try
        process
          (fun tbl ->
            let sym, _ = List.find (fun (_, s) -> s = true) (Hashtbl.find_all tbl id) in
            dealloc sym;
            Hashtbl.remove tbl id)
      with
        MemoryError "undeclared identifier" -> ())
    closing

let describe t =
  try
    match fst (Hashtbl.find symbols t) with
      | `Macro _ -> "macro"
      | `Inline _ -> "inline block"
      | `Integer _ -> "integer constant"
      | `String _ -> "string constant"
      | `StaticVar (_, _, None, (8 | 9 | 10), _, _) -> "string variable"
      | `StaticVar (_, _, _, (8 | 9 | 10), _, _) -> "string array"
      | `StaticVar (_, _, None, _, _, _) -> "integer variable"
      | `StaticVar _ -> "integer array"
  with
    Not_found -> "undeclared identifier"

let get_args loc args defs s =
  let al = List.length args
  and dl = List.length defs
  and optcount = List.fold_left (fun i (_, _, _, o) -> if o <> `None then i + 1 else i) 0 defs
  in
  if al >= dl - optcount && al <= dl then
    let rec loop acc args = function [] -> acc | (_, s, t, e) :: defs ->
      match args with
        | [] -> (match e with
                  | `Some dval -> loop ((t, dval) :: acc) [] defs
                  | `Optional -> acc
                  | `None -> ksprintf (error loc) "internal error: parameter %s should have a default value" s)                
        | e :: args -> loop ((t, e) :: acc) args defs
    in
    List.rev (loop [] args defs)
  else
    if al < dl
    then ksprintf (error loc) "missing parameters to %s: %s" s (String.concat ", " (List.map (fun (_, x, _, _) -> x) (List.drop al defs)))
    else ksprintf (error loc) "%d extra parameters to %s" (al - dl) s

let rec get_as_expression ?(allow_arrays = false) ?(recurse = false) ?(args = []) ?(loc = nowhere) ?s id =
  let s = match s with None -> Text.to_sjs id | Some s -> s in
  match fst (Hashtbl.find symbols id) with
    | `Integer i -> `Int (loc, i)
    | `String s -> `Str (loc, Global.dynArray (`Text (loc, `Sbcs, s)))
    | `StaticVar (_, _, Some _, _, _, _) when not allow_arrays -> ksprintf (error loc) "expected scalar, found array `%s'" s
    | `StaticVar (s, idx, _, (8 | 9 | 10), _, _) -> `SVar (loc, s, `Int (loc, idx))
    | `StaticVar (s, idx, _, _, _, _) -> `IVar (loc, s, `Int (loc, idx))
    | `Inline (l, a)
       -> let seq =
            match a with
              | `Seq d | `Block (_, d) -> DynArray.copy d
              | a -> Global.dynArray a
          in
          `ExprSeq (loc, id, get_args loc args l s, seq)
    | `Macro (`VarOrFn (_, s, t)) when recurse && defined t
       -> get_as_expression ~allow_arrays ~recurse ~args ~loc ~s t
    | `Macro e -> e (*`ExprSeq (loc, id, [], Global.dynArray (`Return (loc, true, e)))*)

let rec get_deref_as_expression ?(loc = nowhere) ?s id offset =
  let s = match s with None -> Text.to_sjs id | Some s -> s in
  match fst (Hashtbl.find symbols id) with
    | `StaticVar (sp, idx, Some len, a, _, _)
       -> begin try 
            let offsc = Global.expr__normalise_and_get_int ~abort_on_fail:false offset in
            if offsc < 0l || offsc >= Int32.of_int len then
              ksprintf (error loc) "index %ld exceeds bounds of array `%s[%d]'" offsc s len
          with Exit ->
            if check_def "__SafeArrays__" () then
              let lhs = `LogOp (nowhere, offset, `Gte, Meta.int 0)
              and rhs = `LogOp (nowhere, offset, `Ltn, Meta.int len) in
              let parm = `AndOr (nowhere, lhs, `LAnd, rhs)
              and msg = `Str (nowhere, Global.dynArray (`Text (nowhere, `Sbcs, Text.of_sjs "array index out of bounds"))) in
              let parms = [`Simple (nowhere, parm); `Simple (nowhere, msg)] in
              Meta.parse_elt (`FuncCall (nowhere, None, "assert", Text.ident "assert", parms, None))
          end;
          let idx = `Op (nowhere, `Int (nowhere, idx), `Add, offset) in
          if a >= 8 then `SVar (loc, sp, idx) else `IVar (loc, sp, idx)
    | `Macro (`VarOrFn (_, s, t)) -> get_deref_as_expression ~loc ~s t offset
    | `Macro _
    | `Inline _ -> ksprintf (error loc) "expected array, found macro `%s'" s
    | _ -> ksprintf (error loc) "expected array, found scalar `%s'" s

let get_as_code ?(args = []) ?(loc = nowhere) ?s id =
  let s = match s with None -> Text.to_sjs id | Some s -> s in
  match fst (Hashtbl.find symbols id) with
    | `Macro e 
       -> `Return (loc, false, (*`VarOrFn (loc, s, id))*) e)
    | `Integer _ | `String _ | `StaticVar _
       -> `Return (loc, false, get_as_expression ~loc ~s id)
    | `Inline (l, c)
       -> let args' = get_args loc args l s in
          if l = [] then c else
            let rv = DynArray.create () in
            let rec loop =
              function
                | _, [] -> ()
                | [], _ -> assert false (* List.length args should always be <= List.length l *)
                | (loc, s, _, _) :: ltl, (t, e) :: argtl
                   -> DynArray.add rv (`Define (loc, s, t, true, e));
                      loop (ltl, argtl)
            in
            loop (l, args');
            DynArray.add rv c;
            `Block (nowhere, rv)

let get id = process (fun tbl -> Hashtbl.find tbl id)

let mutate loc s = Hashtbl.replace symbols


(* TODO: implement proper memory handling (even a stack?) *)

let find_unused_index loc map first last =
  let rec loop i =
    if i = last then
      error loc "failed to allocate static memory"
    else if IMap.mem i map && IMap.find i map > 0 then
      loop (i + 1)
    else
      i
  in
  loop first

let find_unused_block loc map first length =
  let maxidx = 2000 - length in
  let rec loop i =
    if i = maxidx then
      error loc "failed to allocate static block"
    else
      let avail idx = not (IMap.mem idx map) || IMap.find idx map = 0 in
      try
        if not (avail i) then raise Not_found;
        for j = i + length - 1 downto i + 1 do if not (avail j) then raise Not_found done;
        i
      with Not_found ->
        loop (i + 1)
  in
  loop first    

let temploc = { file = "temporary variable"; line = ~-2 }

let tis = Text.of_sjs "__int_alloc_space__"
and tif = Text.of_sjs "__int_alloc_first__"
and til = Text.of_sjs "__int_alloc_last__"
and tss = Text.of_sjs "__str_alloc_space__"
and tsf = Text.of_sjs "__str_alloc_first__"
and tsl = Text.of_sjs "__str_alloc_last__"

let get_const id =
  assert (defined id);
  match fst (Hashtbl.find symbols id) with
    | `Integer i -> Int32.to_int i
    | _ -> assert false

let temp_int_spc () = get_const tis
and temp_int_min () = get_const tif
and temp_int_max () = get_const til
and temp_str_spc () = get_const tss
and temp_str_min () = get_const tsf
and temp_str_max () = get_const tsl

let temp_int_spec () = temp_int_spc (), temp_int_min (), temp_int_max ()
let temp_str_spec () = temp_str_spc (), temp_str_min (), temp_str_max ()

let find_block loc space first ?max length =
  let map = 
    try
      staticvars.(varidx space)
    with Not_found -> 
      ksprintf (error loc) "cannot allocate variables in block 0x%02x" space 
  in
  let idx = find_unused_block loc map first length in
  Option.may (fun max -> if idx + length > max then error loc "unable to allocate block") max;
  idx
  
let allocate_block loc space idx length =
  let space = 
    try
      varidx space 
    with Not_found -> 
      ksprintf (error loc) "cannot allocate variables in block 0x%02x" space 
  in
  for i = idx to idx + length - 1 do
    IFDEF DEBUG_SCOPING THEN eprintf "Allocating %d.%d\n%!" space i ELSE () END;
    let currbinding = try IMap.find i staticvars.(space) with Not_found -> 0 in
    staticvars.(space) <- IMap.add i (currbinding + 1) staticvars.(space)
  done

let get_temp_int ?(space = temp_int_spc()) ?(min = temp_int_min()) ?(max = temp_int_max()) ?(useloc = temploc) () =
  let vidx = varidx space in
  let idx = find_unused_index (if useloc = temploc then nowhere else useloc) staticvars.(vidx) min max in
  IFDEF DEBUG_SCOPING THEN eprintf "Allocating %d.%d\n%!" vidx idx ELSE () END;
  staticvars.(vidx) <- IMap.add idx 1 staticvars.(vidx);
  define (ksprintf Text.of_sjs "[temp %d.%d]" space idx) (`StaticVar (space, Int32.of_int idx, None, vidx, idx, 1)) ~warnings:false;
  `IVar (useloc, space, Meta.int idx)

let get_temp_str ?(space = temp_str_spc()) ?(min = temp_str_min()) ?(max = temp_str_max()) ?(useloc = temploc) () =
  let vidx = varidx space in
  let idx = find_unused_index (if useloc = temploc then nowhere else useloc) staticvars.(vidx) min max in
  IFDEF DEBUG_SCOPING THEN eprintf "Allocating %d.%d\n%!" vidx idx ELSE () END;
  staticvars.(vidx) <- IMap.add idx 1 staticvars.(vidx);
  define (ksprintf Text.of_sjs "[temp %d.%d]" space idx) (`StaticVar (space, Int32.of_int idx, None, vidx, idx, 1)) ~warnings:false;
  `SVar (useloc, space, Meta.int idx)

let get_temp_var (vtype : [`Int | `Str]) s =
  let s = Text.ident s in
  try
    get_as_expression s
  with Not_found ->
    let tempvar = (if vtype = `Int then get_temp_int else get_temp_str) () in
    define s (`Macro tempvar);
    tempvar
