(*
   Rldev: configuration-related functionality
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
(*ppopt pa_macro.cmo *)

INCLUDE "config.cfg"
INCLUDE "../VERSION"

let app_info = { Optpp.default_app_info with Optpp.version = VERSION }

let default_encoding = DEFAULT_ENCODING

IFDEF PREFIX THEN
  let prefix () = PREFIX
ELSE
  (* Prefix not specified: try to guess at runtime. *)
  let prefix =
    let (//) = Filename.concat in
    let prefix = ref "" in
    fun () ->
      if !prefix = "" then (
        let home = try Sys.getenv "HOME" with Not_found -> Filename.dirname Sys.argv.(0)
        and rldir = try Sys.getenv "RLDEV" with Not_found -> "." in
        prefix :=
          try
            List.find
              (fun s -> Sys.file_exists (s // "reallive.kfn"))
              [rldir // "lib"; rldir; home; home // "rldev"; home // ".rldev"; 
               home // "share/rldev"; home // "lib"; home // "rldev/lib"; 
               home // "share/rldev/lib"; home // ".rldev/lib"; 
               "/usr/share/rldev"; "/usr/local/share/rldev"; 
               "/usr/share/rldev/lib"; "/usr/local/share/rldev/lib"]
          with Not_found ->
            Optpp.sysError "Error: unable to locate reallive.kfn.  Try setting $RLDEV to your RLDev installation directory"
      );
      !prefix
END

let ivar_prefix = "int"
and svar_prefix = "str"
