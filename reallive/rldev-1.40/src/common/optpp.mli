(*
   Copyright (C) 2006 Haeleth; based on code copyright (C) 2000 Alain Frisch

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

   This file is a derivative of the getopt library by Alain Frisch, which is
   distributed under the GNU LGPL.  At the time of writing it was available from
   http://www.eleves.ens.fr:8080/home/frisch.
*)

(* Application information *)

type app_info =
  { name: string;
    exe_name: string;
    version: float;
    description: string;
    year: int;
    author: string;
    usage: string; }

val default_app_info : app_info


(* Option definition structures *)

type opt_srcp =
  | Opt of opt_srct
  | Message of string
  | Break

and opt_srct =
  { short : string;
    long : string;
    argname : string;
    descr : string;
    withoutarg : noarg_fun;
    witharg : arg_fun; }

and noarg_fun = (unit -> unit) option
and arg_fun = (string -> unit) option

val set_flag : 'a ref -> 'a -> noarg_fun
  (* sets the given reference to the given value *)

val arg_to_flag : (string -> 'a) -> 'a ref -> string -> arg_fun
  (* sets the given reference to the argument's value, but only once, optionally
     transforming it with op first; the other parameter is the value's name, used
     in warning messages. *)

val set_string : string ref -> string -> arg_fun

(* Option parsing *)

exception Error of string

val display_version : app_info -> unit
val display_help : app_info -> opt_srcp list -> 'a

val getopt : ?app:app_info -> opt_srcp list -> (string -> 'a) -> (unit -> unit) -> unit


(* Pretty-printing *)

val base_indent : int ref

val usageError : ?app:app_info -> string -> 'a

val cliWarning : string -> unit
val cliError : string -> 'a

val sysInfo : string -> unit
val sysWarning : string -> unit
val sysError : string -> 'a


(* Low-level pretty-printing*)

val prettily : string -> unit
val prettily_blk_ln : int -> string -> unit

