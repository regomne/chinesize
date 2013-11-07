(*
    Rlc: variable handling
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
open ExtList
open KeTypes
open KeAst

(* Code-generation auxiliaries *)

let setrng ?init fn s t ?(first = 0) len =
  Meta.call fn (`Deref (nowhere, s, t, Meta.int first)
             :: `Deref (nowhere, s, t, Meta.int (len - 1))
             :: (match init with None -> [] | Some e -> [e]))


(* Main functions *)

let decldir_of_ident =
  let decldir_table = 
    [Text.ident "zero", `Zero;
     Text.ident "block", `Block;
     Text.ident "ext", `Ext;
     Text.ident "labelled", `Label]
  in
  fun loc (str_id, txt_id) ->
    try
      List.assoc txt_id decldir_table
    with Not_found ->
      ksprintf (error loc) "unknown declaration directive `%s'" str_id



let get_elements (loc, str_id, _, array_length, init_value, _) =
  match array_length with
    | `None -> 1
    | `Auto -> 
     (match init_value with 
        | `Array [] -> ksprintf (error loc) "`%s[]' must either be given an explicit length, or the initial value array must contain at least one element" str_id
        | `Array l -> List.length l
        | _ -> ksprintf (error loc) "array `%s[]' must be given a length, either explicitly or by providing an initial value array" str_id)
    | `Some expr -> 
     (try
        Int32.to_int (Global.expr__normalise_and_get_int expr ~abort_on_fail:false)
      with Exit ->
        ksprintf (error (loc_of_expr expr)) "array length for `%s[]' must evaluate to a constant integer" str_id)


let get_address loc vartype str_id block_size =
  function
    | None 
       -> let space, f, max = if vartype = `Str then Memory.temp_str_spec () else Memory.temp_int_spec () in
          space, Memory.find_block loc space f block_size ~max
    | Some (sp, addr) 
       -> try
            Int32.to_int (Global.expr__normalise_and_get_int sp ~abort_on_fail:false),
            Int32.to_int (Global.expr__normalise_and_get_int addr ~abort_on_fail:false)
          with Exit ->
            ksprintf (error (loc_of_expr sp)) "fixed address for %s must evaluate to a pair of constant integers" str_id


let get_real_address vartype space address address_is_access =
  let typed_space, eltmod =
    match vartype with 
      | `Int 1 -> space + 26, 32
      | `Int 2 -> space + 52, 16
      | `Int 4 -> space + 78,  8
      | `Int 8 -> space + 104, 4
      | _      -> space, 1
  in
  let access_address, alloc_address =
    if address_is_access
    then address, address / eltmod
    else address * eltmod, address
  in
  typed_space, access_address, alloc_address


let allocate (loc, vartype, directives, variables) =
  assert (variables != []);
  let is_block = List.mem `Block directives 
  and is_str = vartype = `Str 
  in 
  (* Set `elements' to the number of elements required for each variable - 1 for 
     scalars, or the length for arrays. *)
  let elements = 
    List.map get_elements variables
  in
  (* Allocate and retrieve appropriate addresses to use. *)
  let spaces_addresses_counts =
    let get_block_size elt_count =
      match vartype with
        | `Str -> elt_count
        | `Int elt_bits -> (elt_count * elt_bits - 1) / 32 + 1
    in
    if is_block then
      (* Allocate one block and place individual variables within it. *)
      let elt_total = List.fold_left (+) 0 elements in
      let block_size = get_block_size elt_total
      and fixed_address =
        let all_addresses = List.map (fun (loc, _, _, _, _, rv) -> loc, rv) variables in
        let (_, rv), tail = match all_addresses with hd :: tl -> hd, tl | _ -> assert false in
        (* Direct address specifications are invalid other than for the first variable. *)
        List.iter (function _, None -> () | loc, Some _ -> error loc "when the `block' directive is specified, only the first variable in a declaration may have an address specifier") tail;
        rv
      in
      let space, base_address =
        get_address loc vartype "block allocation" block_size fixed_address
      in
      Memory.allocate_block loc space base_address block_size;
      let address =
        match vartype with 
          | `Int 1 -> base_address * 32
          | `Int 2 -> base_address * 16
          | `Int 4 -> base_address * 8
          | `Int 8 -> base_address * 4
          | _      -> base_address
      in
      let total, retval =
        List.fold_left
          (fun (elt_accum, retval) elt_count ->
            let blocks_required = 
              get_block_size elt_count 
            and typed_space, access_address, alloc_address = 
              get_real_address vartype space (address + elt_accum) true
            in
            let elt = space, typed_space, alloc_address, access_address, elt_count, blocks_required in
            elt_accum + elt_count, elt :: retval)
          (0, [])
          elements
      in
      assert (total == elt_total);
      List.rev retval
    else
      (* Allocate variables wherever they'll fit, out of order or split up if necessary. *)
      (* TODO: For now, this is done naively - each variable uses an entire block.  
         Ideally we would combine multiple sub-int variables in one block where possible. *)
      List.map2
        (fun (loc, str_id, _,_,_, fixed_addr) elt_count -> 
          let blocks_required = 
            get_block_size elt_count 
          in
          let space, address = 
            get_address loc vartype str_id blocks_required fixed_addr 
          in
          let typed_space, access_address, alloc_address =
            get_real_address vartype space address (fixed_addr <> None)
          in
          Memory.allocate_block loc space alloc_address blocks_required;
          space, typed_space, alloc_address, access_address, elt_count, blocks_required)
        variables 
        elements
  in
  (* Define the symbols. *)
  let scoped = not (List.mem `Ext directives) in
  List.iter2
    (fun (loc, _, txt_id, array_length, _, _) 
         (space, typed_space, alloc_address, access_address, elt_count, blocks_required) ->
      Memory.define txt_id ~scoped
        (`StaticVar 
          (typed_space, 
           Int32.of_int access_address, 
           (if array_length <> `None then Some elt_count else None), 
           Memory.varidx space, 
           alloc_address, 
           blocks_required)))
    variables
    spaces_addresses_counts;
  (* Write to flag.ini if required *)
  if !App.flag_labels && (List.mem `Label directives || Memory.check_def "__AllLabelled__" ()) then (
    let fn = Filename.concat (Filename.dirname !App.gameexe) "flag.ini" in
    let oc = open_out_gen [Open_wronly; Open_creat; Open_append; Open_text] 0o666 fn in
    try
      List.iter2
        (fun (_, str_id, _, array_length, _, _) (_, typed_space, _, access_address, elt_count, _) ->
          fprintf oc "%s[%d]:0:%s\n" 
            (variable_name typed_space ~prefix:false) 
            access_address
            (if array_length = `None then str_id else sprintf "%s[%d]" str_id elt_count))
        variables
        spaces_addresses_counts;
      close_out oc
    with e -> 
      close_out oc;
      raise e
  );
  (* Initialise the variables *)
  let is_zero = List.mem `Zero directives in
  if is_block && is_zero && List.length variables > 1 then begin
    (* Handle block initialisations specially when the `zero' directive was passed: 
       in this case we can sometimes optimise the initialisation fairly easily. *)
    let _, str_id, txt_id, _, _, _ = List.hd variables in
    let spc, addr = 
      match Memory.get_as_expression txt_id ~allow_arrays:true with 
        | `SVar (_, s, `Int (_, idx)) -> s, idx 
        | `IVar (_, s, `Int (_, idx)) -> s, idx
        | _ -> assert false 
    in
    (* Check whether any non-zero initial values have been provided. *)
     let elt_total, any_have_values =
      let expr_non_zero e =
        try 
          !Global.expr__normalise_and_get_const e ~abort_on_fail:false <> `Integer 0l
        with Exit -> 
          true
      in
      List.fold_left2
        (fun (ec, acc) (_, _, _, _, ival, _) ec' -> ec + ec', acc ||
          match ival with
            | `None -> false
            | `Scalar e -> expr_non_zero e
            | `Array es -> List.exists expr_non_zero es)
        (0, false)
        variables
        elements
    in
    (* If they haven't, we can just zero the entire block. *)
    if not any_have_values then
      if is_str then
        Meta.call "strclear" 
          [`SVar (nowhere, spc, `Int (nowhere, addr));
           `SVar (nowhere, spc, `Int (nowhere, Int32.add addr (Int32.of_int (elt_total - 1))))]
      else
        Meta.call "setrng" 
          [`IVar (nowhere, spc, `Int (nowhere, addr));
           `IVar (nowhere, spc, `Int (nowhere, Int32.add addr (Int32.of_int (elt_total - 1))))]
    else
      (* If they have, we must take them into account. *)
      let init_values =
        List.fold_right2
          (fun (loc, str_id, _, _, ival, _) elt_count vallist ->
            match ival with
              | `None -> List.rev_append (List.make elt_count Meta.zero) vallist
              | `Scalar e -> List.rev_append (List.make elt_count e) vallist
              | `Array es 
                 -> let len = List.length es in
                    if len > elt_count then ksprintf (error loc) "too many values supplied to initialise %s[]" str_id;
                    es @ if len < elt_count
                         then List.rev_append (List.make (elt_count - len) Meta.zero) vallist
                         else vallist)
          variables elements
          []
      in
      (* String assignments become an optional clearing followed by a series of assignments. *)
      if is_str then
        let any_empty = List.exists (function `Int _ -> true | _ -> false) init_values in
        if any_empty then 
          Meta.call "strclear" 
            [`SVar (nowhere, spc, `Int (nowhere, addr));
             `SVar (nowhere, spc, `Int (nowhere, Int32.add addr (Int32.of_int (elt_total - 1))))];
        List.iter
          (function `Int _ -> () | expr -> Meta.assign (`VarOrFn (nowhere, str_id, txt_id)) `Set expr)
          init_values
      else
        (* For ints, we just call setarray.  (This may be a Bad Thing if init_values is very long.) *)
        Meta.call "setarray" (`IVar (nowhere, spc, `Int (nowhere, addr)) :: init_values)
  end 
  else
    (* Not a block allocation, or no `zero' directive: initalise variables separately. *)
    List.iter2
      (fun (loc, str_id, txt_id, array_length, init_value, _) elt_count ->
        let is_array = array_length <> `None in
        match init_value with
          | `None 
             -> (* No initial value supplied.  If the zero directive is in effect, we
                   give the variable an empty initial value, otherwise leave it alone. *)
                if is_zero then
                  begin match is_str, is_array with
                    | false, false -> Meta.assign (`VarOrFn (nowhere, str_id, txt_id)) `Set Meta.zero
                    | true,  false -> Meta.call "strclear" [`VarOrFn (nowhere, str_id, txt_id)]
                    | false, true  -> setrng "setrng" str_id txt_id elt_count
                    | true,  true  -> setrng "strclear" str_id txt_id elt_count
                  end
          | `Scalar e
             -> if is_array then
                  if is_str then
                    let idx = Memory.get_temp_int () in
                    !Global.compilerFrame__parse (Global.dynArray (
                     `For
                       (nowhere,
                        Global.dynArray (`Assign (nowhere, idx, `Set, Meta.zero)),
                        `LogOp (nowhere, idx, `Ltn, Meta.int elt_count),
                        Global.dynArray (`Assign (nowhere, idx, `Add, Meta.int 1)),
                        `Assign (nowhere, `Deref (nowhere, str_id, txt_id, idx), `Set, e))
                    ))
                  else
                    setrng "setrng" str_id txt_id elt_count ~init:e
                else
                  Meta.assign (`VarOrFn (nowhere, str_id, txt_id)) `Set e
          | `Array es
             -> let len = List.length es in
                if len > elt_count then ksprintf (error loc) "too many values supplied to initialise %s[]" str_id;
                if is_str then
                  List.iteri (fun i e -> Meta.assign (`Deref (nowhere, str_id, txt_id, Meta.int i)) `Set e) es
                else
                  Meta.call "setarray" (`Deref (nowhere, str_id, txt_id, Meta.zero) :: es);
                if len < elt_count then
                  if is_zero 
                  then setrng (if is_str then "strclear" else "setrng") str_id txt_id ~first:len elt_count
                  else ksprintf (warning loc) "not enough values supplied for %s[]: the last %d elements will hold undefined values" str_id (elt_count - len))
      variables 
      elements
