(*
    Rlc: compile-time Kepago generation utility functions
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

open KeTypes
open KeAst

let parse d = !Global.compilerFrame__parse d
let parse_elt e = parse (Global.dynArray e)

let assign (lhs : [assignable | expression]) op (rhs : [assignable | expression]) =
  match lhs, rhs with
    | #assignable as lhs, (#expression as rhs) -> parse_elt (`Assign (nowhere, lhs, op, rhs))
    | _ -> failwith "internal error"

let call ?rv ?label funname args =
  parse_elt
    (`FuncCall (nowhere, rv, funname, Text.ident funname, List.map (fun e -> `Simple (nowhere, e)) args, label))

let zero = `Int (nowhere, 0l)
let int x = `Int (nowhere, Int32.of_int x)

let goto label = call "goto" [] ~label
and gosub label = call "gosub" [] ~label
and goto_unless cond label = call "goto_unless" [cond] ~label
